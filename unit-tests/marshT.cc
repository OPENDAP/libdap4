#include <cppunit/TestFixture.h>
#include <cppunit/TestAssert.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/CompilerOutputter.h>

#include "config.h"
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#include <iostream>
#include <fstream>
#include <cstring>

#include "TestByte.h"
#include "TestInt16.h"
#include "TestInt32.h"
#include "TestUInt16.h"
#include "TestUInt32.h"
#include "TestFloat32.h"
#include "TestFloat64.h"
#include "TestStr.h"
#include "TestUrl.h"
#include "TestArray.h"
#include "TestStructure.h"
#include "TestSequence.h"
#include "DataDDS.h"
#include "ConstraintEvaluator.h"
#include "TestTypeFactory.h"
#include "XDRFileMarshaller.h"
#include "XDRStreamMarshaller.h"
#include "XDRFileUnMarshaller.h"

#include "GetOpt.h"
#include "debug.h"

using std::cerr;
using std::cout;
using std::endl;
using std::ofstream;
using namespace CppUnit;

int test_variable_sleep_interval = 0; // Used in Test* classes for testing timeouts.

static bool debug = false;

#undef DBG
#define DBG(x) do { if (debug) {x;} } while(false)

/**
 * This test has been rewritten so that it no longer depends on the values
 * of Array remaining in place (in the Array object) once serialize() is run.
 * This is part of a change in libdap that will reduce memory consumption
 * when serializing datasets with large numbers of (large) variables.
 * jhrg 8/4/15
 */
class marshT: public CppUnit::TestFixture {
private:
    TestByte *b;
    TestInt16 *i16;
    TestInt32 *i32;
    TestUInt16 *ui16;
    TestUInt32 *ui32;
    TestFloat32 *f32;
    TestFloat64 *f64;
    TestStr *str;
    TestUrl *url;
    TestByte *ab;

    TestArray *arr;

    TestStructure *s;

    TestSequence *seq, *seq2;

public:
    marshT() :
        b(0), i16(0), i32(0), ui16(0), ui32(0), f32(0), f64(0), str(0), url(0), ab(0), arr(0), s(0), seq(0), seq2(0)
    {

    }

    void setUp()
    {
        b = new TestByte("b");
        i16 = new TestInt16("i16");
        i32 = new TestInt32("i32");
        ui16 = new TestUInt16("ui16");
        ui32 = new TestUInt32("ui32");
        f32 = new TestFloat32("f32");
        f64 = new TestFloat64("f64");
        str = new TestStr("str");
        url = new TestUrl("url");

        ab = new TestByte("ab");
        arr = new TestArray("arr", ab);
        arr->append_dim(5, "dim1");
        arr->append_dim(3, "dim2");

        s = new TestStructure("s");
        s->add_var(i32);
        s->add_var(str);
        s->add_var(arr);
        s->set_send_p(true);

        seq = new TestSequence("seq");
        seq->add_var(f64);
        seq->add_var(arr);

        seq2 = new TestSequence("seq2");
        seq2->add_var(ui16);
        seq2->add_var(url);
        seq2->set_send_p(true);

        seq->add_var(seq2);
        seq->set_send_p(true);
        seq->set_leaf_sequence();

        // Need to call read so that the value() method calls in the ..read_test()
        // will work.
        b->read();
        i16->read();
        i32->read();
        ui16->read();
        ui32->read();
        f32->read();
        f64->read();

        str->read();
        url->read();
        arr->read();
        s->read();
    }

    void tearDown()
    {
        delete b;
        b = 0;
        delete i16;
        i16 = 0;
        delete i32;
        i32 = 0;
        delete ui16;
        ui16 = 0;
        delete ui32;
        ui32 = 0;
        delete f32;
        f32 = 0;
        delete f64;
        f64 = 0;

        delete str;
        str = 0;
        delete url;
        url = 0;

        delete ab;
        ab = 0;
        delete arr;
        arr = 0;
        delete s;
        s = 0;
    }

    void marshT_test_write(Marshaller &fm)
    {
        ConstraintEvaluator eval;
        TestTypeFactory ttf;
        DataDDS dds(&ttf, "dds");

        try {
            DBG(cerr << "serializing using XDRFileMarshaller" << endl);

            b->serialize(eval, dds, fm, false);
            i16->serialize(eval, dds, fm, false);
            i32->serialize(eval, dds, fm, false);
            ui16->serialize(eval, dds, fm, false);
            ui32->serialize(eval, dds, fm, false);
            f32->serialize(eval, dds, fm, false);
            f64->serialize(eval, dds, fm, false);
            str->serialize(eval, dds, fm, false);
            url->serialize(eval, dds, fm, false);
            s->serialize(eval, dds, fm, false);
            arr->serialize(eval, dds, fm, false);
            seq->serialize(eval, dds, fm, false);

            DBG(cerr << "done serializing using XDRFileMarshaller" << endl);
        }
        catch (Error &e) {
            string err = "failed:" + e.get_error_message();
            CPPUNIT_FAIL(err.c_str());
        }
    }

    // This test depends on the file written by the .._write_file() test,
    // so it must be run after that test.
    void marshT_test_read(UnMarshaller &um)
    {
        TestTypeFactory ttf;
        DataDDS dds(&ttf, "dds");

        // now read the values in and compare the with each other and the original values
        try {
            DBG(cerr << "deserializing XDRFileMarshaller built file" << endl);

            Byte fb("fb");
            fb.deserialize(um, &dds, false);
            CPPUNIT_ASSERT(fb.value() == b->value());

            Int16 fi16("i16");
            fi16.deserialize(um, &dds, false);
            DBG(cerr << "fi16.value(): " << fi16.value() << ", i16.value(): " << i16->value());
            CPPUNIT_ASSERT(fi16.value() == 32000);

            DBG(cerr << " file int32" << endl);
            Int32 fi32("i32");
            fi32.deserialize(um, &dds, false);
            DBG(cerr << "fi32.value(): " << fi32.value() << ", i32.value(): " << i32->value());
            CPPUNIT_ASSERT(fi32.value() == i32->value());

            UInt16 fui16("ui16");
            fui16.deserialize(um, &dds, false);
            DBG(cerr << "fui16.value(): " << fui16.value() << ", ui16.value(): " << ui16->value());
            CPPUNIT_ASSERT(fui16.value() == ui16->value());

            UInt32 fui32("ui32");
            fui32.deserialize(um, &dds, false);
            DBG(cerr << "fui32.value(): " << fui32.value() << ", ui32.value(): " << ui32->value());
            CPPUNIT_ASSERT(fui32.value() == ui32->value());

            Float32 ff32("f32");
            ff32.deserialize(um, &dds, false);
            DBG(cerr << "ff32.value(): " << ff32.value() << ", f32.value(): " << f32->value());
            CPPUNIT_ASSERT(ff32.value() == f32->value());

            Float64 ff64("f64");
            ff64.deserialize(um, &dds, false);
            DBG(cerr << "ff64.value(): " << ff64.value() << ", f64.value(): " << f64->value());
            CPPUNIT_ASSERT(ff64.value() == f64->value());

            Str fstr("str");
            fstr.deserialize(um, &dds, false);
            DBG(cerr << "fstr.value(): " << fstr.value() << ", str.value(): " << str->value());
            CPPUNIT_ASSERT(fstr.value() == str->value());

            Url furl("url");
            furl.deserialize(um, &dds, false);
            DBG(cerr << "furl.value(): " << furl.value() << ", url.value(): " << url->value());
            CPPUNIT_ASSERT(furl.value() == url->value());

            TestStructure fs("fs");
            TestInt32 fsi32("fsi32");
            fs.add_var(&fsi32);
            TestStr fsstr("fsstr");
            fs.add_var(&fsstr);
            TestByte fsab("fsab");
            TestArray fsarr("fsarr", &fsab);
            fsarr.append_dim(5, "dim1");
            fsarr.append_dim(3, "dim2");
            fs.add_var(&fsarr);
            fs.deserialize(um, &dds, false);

            Int32 *fsi32_p = dynamic_cast<Int32 *>(fs.var("fsi32"));
            CPPUNIT_ASSERT(fsi32_p);
            DBG(cerr << "fsi32.value(): " << fsi32_p->value() << ", i32.value(): " << i32->value());
            CPPUNIT_ASSERT(fsi32_p->value() == i32->value());

            Str *fsstr_p = dynamic_cast<Str *>(fs.var("fsstr"));
            CPPUNIT_ASSERT(fsstr_p);
            DBG(
                cerr << "fstr.value(): " << fsstr_p->value() << ", str.value(): "
                    << dynamic_cast<Str *>(s->var("str"))->value());
            CPPUNIT_ASSERT(fsstr_p->value() == dynamic_cast<Str *>(s->var("str"))->value());

            BaseType *bt = fs.var("fsab");
            CPPUNIT_ASSERT(bt);
            Array *fsarr_p = dynamic_cast<Array *>(bt);
            CPPUNIT_ASSERT(fsarr_p);
            dods_byte fdb[fsarr_p->length() * sizeof(dods_byte)];
            dods_byte db[arr->length() * sizeof(dods_byte)];
            fsarr_p->value(fdb);
            arr->value(db);
            CPPUNIT_ASSERT(fsarr_p->length() == arr->length());
            CPPUNIT_ASSERT(!memcmp((void *) fdb, (void *) db, fsarr_p->length() * sizeof(dods_byte)));

            DBG(cerr << " file array" << endl);
            TestByte fab("ab");
            TestArray farr("arr", &fab);
            farr.append_dim(5, "dim1");
            farr.append_dim(3, "dim2");
            farr.deserialize(um, &dds, false);
            farr.value(fdb);
            CPPUNIT_ASSERT(farr.length() == arr->length());
            CPPUNIT_ASSERT(!memcmp((void *) fdb, (void *) db, farr.length() * sizeof(dods_byte)));

            TestSequence fseq("fseq");
            fseq.add_var(f64);
            fseq.add_var(arr);
            TestSequence fseq2("fseq2");
            fseq2.add_var(ui16);
            fseq2.add_var(url);
            fseq2.set_send_p(true);
            fseq.add_var(&fseq2);
            fseq.set_leaf_sequence();

            fseq.deserialize(um, &dds, false);
            unsigned int num_rows = fseq.number_of_rows();
            CPPUNIT_ASSERT(num_rows == 4);
            for (unsigned int i = 0; i < num_rows; i++) {
                BaseTypeRow *row = fseq.row_value(i);
                CPPUNIT_ASSERT(row);
                CPPUNIT_ASSERT(row->size() == 3);
                Float64 *f64_p = dynamic_cast<Float64 *>((*row)[0]);
                CPPUNIT_ASSERT(f64_p);
                CPPUNIT_ASSERT(f64_p->value() == f64->value());
                Array *arr_p = dynamic_cast<Array *>((*row)[1]);
                CPPUNIT_ASSERT(arr_p);
                arr_p->value(fdb);
                CPPUNIT_ASSERT(arr_p->length() == arr->length());
                CPPUNIT_ASSERT(!memcmp((void *) fdb, (void *) db, arr_p->length() * sizeof(dods_byte)));
                Sequence *seq_p = dynamic_cast<Sequence *>((*row)[2]);
                CPPUNIT_ASSERT(seq_p);
                unsigned int num_rows_sub = seq_p->number_of_rows();
                CPPUNIT_ASSERT(num_rows == 4);
                for (unsigned int j = 0; j < num_rows_sub; j++) {
                    BaseTypeRow *row_sub = seq_p->row_value(j);
                    CPPUNIT_ASSERT(row_sub);
                    CPPUNIT_ASSERT(row_sub->size() == 2);
                    UInt16 *ui16_p = dynamic_cast<UInt16 *>((*row_sub)[0]);
                    CPPUNIT_ASSERT(ui16_p);
                    CPPUNIT_ASSERT(ui16_p->value() == ui16->value());
                    Url *url_p = dynamic_cast<Url *>((*row_sub)[1]);
                    CPPUNIT_ASSERT(url_p);
                    CPPUNIT_ASSERT(url_p->value() == url->value());
                }
            }

            DBG(cerr << "done deserializing XDRFileMarshaller built file" << endl);
        }
        catch (Error &e) {
            string err = "failed:" + e.get_error_message();
            CPPUNIT_FAIL(err.c_str());
        }
    }

    void marshT_test_write_file()
    {
        FILE *f = fopen("test.file", "w");
        XDRFileMarshaller fm(f);

        marshT_test_write(fm);

        fclose(f);
    }

    void marshT_test_read_file()
    {
        FILE *ff = fopen("test.file", "r");
        XDRFileUnMarshaller um(ff);

        marshT_test_read(um);

        fclose(ff);
        unlink("test.file");
    }

    void marshT_test_write_stream()
    {
        ofstream strm("test.strm", ios::out | ios::trunc);
        XDRStreamMarshaller sm(strm);

        marshT_test_write(sm);

        strm.close();
    }

    // Not that compelling a test really... (It is the same as .._read_file())
    void marshT_test_read_stream()
    {
        FILE *sf = fopen("test.strm", "r");
        XDRFileUnMarshaller um(sf);

        marshT_test_read(um);

        fclose(sf);
        unlink("test.strm");
    }

    CPPUNIT_TEST_SUITE (marshT);

    CPPUNIT_TEST (marshT_test_write_file);
    CPPUNIT_TEST (marshT_test_read_file);
    CPPUNIT_TEST (marshT_test_write_stream);
    CPPUNIT_TEST (marshT_test_read_stream);

    CPPUNIT_TEST_SUITE_END( );

};

CPPUNIT_TEST_SUITE_REGISTRATION (marshT);

int main(int argc, char *argv[])
{
    GetOpt getopt(argc, argv, "dh");
    int option_char;

    while ((option_char = getopt()) != -1)
        switch (option_char) {
        case 'd':
            debug = 1;  // debug is a static global
            break;
        case 'h': {     // help - show test names
            cerr << "Usage: marshT has the following tests:" << endl;
            const std::vector<Test*> &tests = marshT::suite()->getTests();
            unsigned int prefix_len = marshT::suite()->getName().append("::").length();
            for (std::vector<Test*>::const_iterator i = tests.begin(), e = tests.end(); i != e; ++i) {
                cerr << (*i)->getName().replace(0, prefix_len, "") << endl;
            }
            break;
        }
        default:
            break;
        }

    CppUnit::TextTestRunner runner;
    runner.addTest(CppUnit::TestFactoryRegistry::getRegistry().makeTest());

    bool wasSuccessful = true;
    string test = "";
    int i = getopt.optind;
    if (i == argc) {
        // run them all
        wasSuccessful = runner.run("");
    }
    else {
        for (; i < argc; ++i) {
            if (debug) cerr << "Running " << argv[i] << endl;
            test = marshT::suite()->getName().append("::").append(argv[i]);
            wasSuccessful = wasSuccessful && runner.run(test);
        }
    }

    return wasSuccessful ? 0 : 1;
}

