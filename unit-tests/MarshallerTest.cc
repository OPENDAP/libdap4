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
#include <fcntl.h>
#include <pthread.h>

// #define DODS_DEBUG 1

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
#include "TestGrid.h"
#include "TestSequence.h"

#include "DataDDS.h"
#include "ConstraintEvaluator.h"
#include "TestTypeFactory.h"
#include "XDRFileMarshaller.h"
#include "XDRStreamMarshaller.h"
#include "XDRFileUnMarshaller.h"
#include "XDRStreamUnMarshaller.h"
#include "GetOpt.h"
//#include "Locker.h"
#include "debug.h"

int test_variable_sleep_interval = 0; // Used in Test* classes for testing timeouts.

static bool debug = false;

#undef DBG
#define DBG(x) do { if (debug) {x;} } while(false)

using namespace CppUnit;
using namespace std;

namespace libdap {

class MarshallerTest: public CppUnit::TestFixture {

    CPPUNIT_TEST_SUITE (MarshallerTest);

    CPPUNIT_TEST (simple_types_file_serialize_test);

    CPPUNIT_TEST (array_file_serialize_test);
    CPPUNIT_TEST (structure_file_serialize_test);
    CPPUNIT_TEST (grid_file_serialize_test);
    CPPUNIT_TEST (sequence_file_serialize_test);

    CPPUNIT_TEST (simple_types_file_deserialize_test);
    CPPUNIT_TEST (array_file_deserialize_test);
    CPPUNIT_TEST (structure_file_deserialize_test);
    CPPUNIT_TEST (grid_file_deserialize_test);
    CPPUNIT_TEST (sequence_file_deserialize_test);

    CPPUNIT_TEST (simple_types_stream_serialize_test);
    CPPUNIT_TEST (array_stream_serialize_test);
    CPPUNIT_TEST (array_stream_deserialize_test);

    CPPUNIT_TEST (array_stream_serialize_test_2);
    CPPUNIT_TEST (array_stream_serialize_test_3);
    CPPUNIT_TEST (array_stream_serialize_test_4);

    CPPUNIT_TEST (array_f32_stream_serialize_test);
    CPPUNIT_TEST (array_f32_stream_deserialize_test);
    CPPUNIT_TEST (array_f32_stream_serialize_test_2);

    CPPUNIT_TEST (array_f64_stream_serialize_test);
    CPPUNIT_TEST (array_f64_stream_deserialize_test);
    CPPUNIT_TEST (array_f64_stream_serialize_test_2);

    CPPUNIT_TEST (structure_stream_serialize_test);
    CPPUNIT_TEST (grid_stream_serialize_test);
    CPPUNIT_TEST (sequence_stream_serialize_test);

    CPPUNIT_TEST (simple_types_stream_deserialize_test);
    CPPUNIT_TEST (structure_stream_deserialize_test);
    CPPUNIT_TEST (grid_stream_deserialize_test);
    CPPUNIT_TEST (sequence_stream_deserialize_test);

    CPPUNIT_TEST (array_stream_put_vector_thread_test);
    CPPUNIT_TEST (array_stream_put_vector_thread_test_2);
    CPPUNIT_TEST (array_stream_put_vector_thread_test_3);

    CPPUNIT_TEST (array_stream_put_vector_thread_test_4);
    CPPUNIT_TEST (array_stream_put_vector_thread_test_5);

#if 1
    CPPUNIT_TEST (array_stream_serialize_part_thread_test);
    CPPUNIT_TEST (array_stream_serialize_part_thread_test_2);
    CPPUNIT_TEST (array_stream_serialize_part_thread_test_3);
#endif

    CPPUNIT_TEST_SUITE_END( );

    TestByte *b;
    TestInt16 *i16;
    TestInt32 *i32;
    TestUInt16 *ui16;
    TestUInt32 *ui32;
    TestFloat32 *f32;
    TestFloat64 *f64;
    TestStr *str;
    TestUrl *url;

    vector<dods_byte> db;
    TestByte *ab;
    TestArray *arr;

    vector<dods_float32> d_f32;
    TestFloat32 *a_f32;
    TestArray *arr_f32;

    vector<dods_float64> d_f64;
    TestFloat64 *a_f64;
    TestArray *arr_f64;

    TestStructure *s;

    ConstraintEvaluator eval;
    TestTypeFactory ttf;
    DataDDS dds;

    string str_value, str2_value;
    string url_value;

public:
    MarshallerTest() :
        b(0), i16(0), i32(0), ui16(0), ui32(0), f32(0), f64(0), str(0), url(0), ab(0), arr(0), a_f32(0), arr_f32(0), a_f64(
            0), arr_f64(0), s(0), dds(&ttf, "dds")
    {
        url_value = "http://dcz.gso.uri.edu/avhrr-archive/archive.html";
    }

    void setUp()
    {
        b = new TestByte("byte");
        b->read();

        i16 = new TestInt16("i16");
        i16->read();

        i32 = new TestInt32("i32");
        i32->read();

        ui16 = new TestUInt16("ui16");
        ui16->read();

        ui32 = new TestUInt32("ui32");
        ui32->read();

        f32 = new TestFloat32("f32");
        f32->read();

        f64 = new TestFloat64("f64");
        f64->read();

        str = new TestStr("str");
        str->read();

        url = new TestUrl("url");
        url->read();

        // Array of Byte
        ab = new TestByte("ab");
        ab->read();
        arr = new TestArray("arr", ab);
        arr->append_dim(5, "dim1");
        arr->append_dim(3, "dim2");
        arr->read();
        arr->set_read_p(true);

        db.resize(arr->length());
        for (int i = 0; i < arr->length(); ++i)
            db[i] = 126;
        arr->value(&db[0]);

        // Array of Float32
        a_f32 = new TestFloat32("a_f32");
        a_f32->read();
        arr_f32 = new TestArray("arr_f32", a_f32);
        arr_f32->append_dim(5, "dim1");
        arr_f32->append_dim(3, "dim2");
        arr_f32->read();
        arr_f32->set_read_p(true);

        d_f32.resize(arr->length());
        for (int i = 0; i < arr->length(); ++i)
            d_f32[i] = 126.126;
        arr_f32->value(&d_f32[0]);

        // Array of Float64
        a_f64 = new TestFloat64("a_f64");
        a_f64->read();
        arr_f64 = new TestArray("arr_f64", a_f64);
        arr_f64->append_dim(5, "dim1");
        arr_f64->append_dim(3, "dim2");
        arr_f64->read();
        arr_f64->set_read_p(true);

        d_f64.resize(arr->length());
        for (int i = 0; i < arr->length(); ++i)
            d_f64[i] = 1260.0126;
        arr_f64->value(&d_f64[0]);

        s = new TestStructure("s");
        s->add_var(i32);
        s->add_var(str);
        s->add_var(arr);
        s->set_send_p(true);
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
        delete a_f32;
        ab = 0;
        delete arr_f32;
        arr_f32 = 0;
        delete a_f64;
        ab = 0;
        delete arr_f64;
        arr_f64 = 0;

        delete s;
        s = 0;
    }

    void simple_types_file_serialize_test()
    {
        try {
            FILE *f = fopen("st_test.file", "w");
            XDRFileMarshaller fm(f);

            b->serialize(eval, dds, fm, false);
            i16->serialize(eval, dds, fm, false);
            i32->serialize(eval, dds, fm, false);
            ui16->serialize(eval, dds, fm, false);
            ui32->serialize(eval, dds, fm, false);
            f32->serialize(eval, dds, fm, false);
            f64->serialize(eval, dds, fm, false);
            str->serialize(eval, dds, fm, false);
            url->serialize(eval, dds, fm, false);
        }
        catch (Error &e) {
            string err = "failed:" + e.get_error_message();
            CPPUNIT_FAIL(err.c_str());
        }
    }

    void simple_types_file_deserialize_test()
    {
        try {
            FILE *ff = fopen("st_test.file", "r");
            XDRFileUnMarshaller um(ff);

            Byte fb("fb");
            fb.deserialize(um, &dds, false);
            CPPUNIT_ASSERT(fb.value() == b->value());

            Int16 fi16("i16");
            fi16.deserialize(um, &dds, false);
            CPPUNIT_ASSERT(fi16.value() == i16->value());

            Int32 fi32("i32");
            fi32.deserialize(um, &dds, false);
            CPPUNIT_ASSERT(fi32.value() == i32->value());

            UInt16 fui16("ui16");
            fui16.deserialize(um, &dds, false);
            CPPUNIT_ASSERT(fui16.value() == ui16->value());

            UInt32 fui32("ui32");
            fui32.deserialize(um, &dds, false);
            CPPUNIT_ASSERT(fui32.value() == ui32->value());

            Float32 ff32("f32");
            ff32.deserialize(um, &dds, false);
            CPPUNIT_ASSERT(ff32.value() == f32->value());

            Float64 ff64("f64");
            ff64.deserialize(um, &dds, false);
            CPPUNIT_ASSERT(ff64.value() == f64->value());

            Str fstr("str");
            fstr.deserialize(um, &dds, false);
            // Test for the string value like this because the digit after
            // the colon changes each time the read() method is called.
            CPPUNIT_ASSERT(fstr.value().find("Silly test string:") != string::npos);

            Url furl("url");
            furl.deserialize(um, &dds, false);
            CPPUNIT_ASSERT(furl.value() == url_value);
        }
        catch (Error &e) {
            string err = "failed:" + e.get_error_message();
            CPPUNIT_FAIL(err.c_str());
        }
    }

    void array_file_serialize_test()
    {
        try {
            FILE *f = fopen("a_test.file", "w");
            XDRFileMarshaller fm(f);

            arr->serialize(eval, dds, fm, false);
        }
        catch (Error &e) {
            string err = "failed:" + e.get_error_message();
            CPPUNIT_FAIL(err.c_str());
        }
    }

    void array_file_deserialize_test()
    {
        try {
            FILE *ff = fopen("a_test.file", "r");
            XDRFileUnMarshaller um(ff);

            TestByte fab("ab");
            TestArray farr("arr", &fab);
            farr.append_dim(5, "dim1");
            farr.append_dim(3, "dim2");
            farr.deserialize(um, &dds, false);

            CPPUNIT_ASSERT(farr.length() == arr->length());

            dods_byte fdb[farr.length() * sizeof(dods_byte)];
            farr.value(fdb);
            CPPUNIT_ASSERT(!memcmp((void *) fdb, (void *) &db[0], farr.length() * sizeof(dods_byte)));
        }
        catch (Error &e) {
            string err = "failed:" + e.get_error_message();
            CPPUNIT_FAIL(err.c_str());
        }
    }

    void structure_file_serialize_test()
    {
        try {
            FILE *f = fopen("struct_test.file", "w");
            XDRFileMarshaller fm(f);

            s->serialize(eval, dds, fm, false);
        }
        catch (Error &e) {
            string err = "failed:" + e.get_error_message();
            CPPUNIT_FAIL(err.c_str());
        }
    }

    void structure_file_deserialize_test()
    {
        try {
            FILE *ff = fopen("struct_test.file", "r");
            XDRFileUnMarshaller um(ff);

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
            CPPUNIT_ASSERT(fsi32_p->value() == i32->value());

            Str *fsstr_p = dynamic_cast<Str *>(fs.var("fsstr"));
            CPPUNIT_ASSERT(fsstr_p);
            DBG2(cerr << "fsstr_p->value(): " << fsstr_p->value() << endl);
            CPPUNIT_ASSERT(fsstr_p->value().find("Silly test string:") != string::npos);

            BaseType *bt = fs.var("fsab");
            CPPUNIT_ASSERT(bt);
            Array *fsarr_p = dynamic_cast<Array *>(bt);
            CPPUNIT_ASSERT(fsarr_p);
            dods_byte fdb[fsarr_p->length() * sizeof(dods_byte)];
            fsarr_p->value(fdb);

            CPPUNIT_ASSERT(fsarr_p->length() == arr->length());
            CPPUNIT_ASSERT(!memcmp((void *) fdb, (void *) &db[0], fsarr_p->length() * sizeof(dods_byte)));
        }
        catch (Error &e) {
            string err = "failed:" + e.get_error_message();
            CPPUNIT_FAIL(err.c_str());
        }
    }

    void grid_file_serialize_test()
    {
        try {
            FILE *f = fopen("g_test.file", "w");
            XDRFileMarshaller fm(f);

            TestGrid tg("grid1");
            TestArray arr2("arr2", ab);
            arr2.append_dim(5, "dim1");
            arr2.append_dim(3, "dim2");
            tg.add_var(&arr2, array);

            TestArray map1("map1", f32);
            map1.append_dim(5, "dim1");
            tg.add_var(&map1, maps);

            TestArray map2("map2", f32);
            map2.append_dim(3, "dim2");
            tg.add_var(&map2, maps);

            tg.set_send_p(true);
            tg.read();
            tg.set_read_p(true);

            tg.serialize(eval, dds, fm, false);
        }
        catch (Error &e) {
            string err = "failed:" + e.get_error_message();
            CPPUNIT_FAIL(err.c_str());
        }
    }

    void grid_file_deserialize_test()
    {
        try {
            FILE *ff = fopen("g_test.file", "r");
            XDRFileUnMarshaller um(ff);

            TestGrid tg("grid1");
            TestArray arr2("arr2", ab);
            arr2.append_dim(5, "dim1");
            arr2.append_dim(3, "dim2");
            tg.add_var(&arr2, array);

            TestArray map1("map1", f32);
            map1.append_dim(5, "dim1");
            tg.add_var(&map1, maps);

            TestArray map2("map2", f32);
            map2.append_dim(3, "dim2");
            tg.add_var(&map2, maps);

            tg.deserialize(um, &dds, false);

            // Check the values in the array
            CPPUNIT_ASSERT(tg.get_array()->length() == arr->length());

            dods_byte fdb[tg.get_array()->length() * sizeof(dods_byte)];
            tg.get_array()->value(fdb);
            CPPUNIT_ASSERT(!memcmp((void *) fdb, (void *) &db[0], tg.get_array()->length() * sizeof(dods_byte)));

            // Should test the map values here, but skip that for now...
        }
        catch (Error &e) {
            string err = "failed:" + e.get_error_message();
            CPPUNIT_FAIL(err.c_str());
        }
    }

    void sequence_file_serialize_test()
    {
        try {
            FILE *f = fopen("seq_test.file", "w");
            XDRFileMarshaller fm(f);

            TestSequence seq("seq");
            seq.add_var(f64);
            seq.add_var(arr);

            TestSequence seq2("seq2");
            seq2.add_var(ui16);
            seq2.add_var(url);

            seq.add_var(&seq2);

            seq.set_send_p(true);
            seq.set_leaf_sequence();

            seq.serialize(eval, dds, fm, false);
        }
        catch (Error &e) {
            string err = "failed:" + e.get_error_message();
            CPPUNIT_FAIL(err.c_str());
        }
    }

    void sequence_file_deserialize_test()
    {
        try {
            FILE *ff = fopen("seq_test.file", "r");
            XDRFileUnMarshaller um(ff);

            dods_byte fdb[arr->length() * sizeof(dods_byte)];

            TestSequence seq("seq");
            seq.add_var(f64);
            seq.add_var(arr);

            TestSequence seq2("seq2");
            seq2.add_var(ui16);
            seq2.add_var(url);
            seq2.set_send_p(true);
            seq.add_var(&seq2);
            seq.set_leaf_sequence();

            seq.deserialize(um, &dds, false);
            unsigned int num_rows = seq.number_of_rows();
            CPPUNIT_ASSERT(num_rows == 4);
            for (unsigned int i = 0; i < num_rows; i++) {
                BaseTypeRow *row = seq.row_value(i);
                CPPUNIT_ASSERT(row);
                CPPUNIT_ASSERT(row->size() == 3);
                Float64 *f64_p = dynamic_cast<Float64 *>((*row)[0]);
                CPPUNIT_ASSERT(f64_p);
                CPPUNIT_ASSERT(f64_p->value() == f64->value());

                Array *arr_p = dynamic_cast<Array *>((*row)[1]);
                CPPUNIT_ASSERT(arr_p);
                arr_p->value(fdb);
                CPPUNIT_ASSERT(arr_p->length() == arr->length());
                CPPUNIT_ASSERT(!memcmp((void *) fdb, (void *) &db[0], arr_p->length() * sizeof(dods_byte)));
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
        }
        catch (Error &e) {
            string err = "failed:" + e.get_error_message();
            CPPUNIT_FAIL(err.c_str());
        }
    }

    // Stream tests from here on

    void simple_types_stream_serialize_test()
    {
        try {
            ofstream strm("st_test.strm", ios::out | ios::trunc);
            XDRStreamMarshaller sm(strm);

            b->serialize(eval, dds, sm, false);

            i16->serialize(eval, dds, sm, false);

            i32->serialize(eval, dds, sm, false);

            ui16->serialize(eval, dds, sm, false);

            ui32->serialize(eval, dds, sm, false);

            f32->serialize(eval, dds, sm, false);

            f64->serialize(eval, dds, sm, false);

            str->serialize(eval, dds, sm, false);

            url->serialize(eval, dds, sm, false);
        }
        catch (Error &e) {
            string err = "failed:" + e.get_error_message();
            CPPUNIT_FAIL(err.c_str());
        }
    }

    void simple_types_stream_deserialize_test()
    {
        try {
#if 0
            ifstream strm( "st_test.strm", ios::in );
            XDRStreamUnMarshaller um( strm );
#else
            FILE *sf = fopen("st_test.strm", "r");
            XDRFileUnMarshaller um(sf);
#endif
            Byte fb("fb");
            fb.deserialize(um, &dds, false);
            CPPUNIT_ASSERT(fb.value() == b->value());

            Int16 fi16("i16");
            fi16.deserialize(um, &dds, false);
            CPPUNIT_ASSERT(fi16.value() == i16->value());

            Int32 fi32("i32");
            fi32.deserialize(um, &dds, false);
            CPPUNIT_ASSERT(fi32.value() == i32->value());

            UInt16 fui16("ui16");
            fui16.deserialize(um, &dds, false);
            CPPUNIT_ASSERT(fui16.value() == ui16->value());

            UInt32 fui32("ui32");
            fui32.deserialize(um, &dds, false);
            CPPUNIT_ASSERT(fui32.value() == ui32->value());

            Float32 ff32("f32");
            ff32.deserialize(um, &dds, false);
            CPPUNIT_ASSERT(ff32.value() == f32->value());

            Float64 ff64("f64");
            ff64.deserialize(um, &dds, false);
            CPPUNIT_ASSERT(ff64.value() == f64->value());

            Str fstr("str");
            fstr.deserialize(um, &dds, false);
            DBG(cerr << "fstr.value(): " << fstr.value() << endl);
            CPPUNIT_ASSERT(fstr.value().find("Silly test string:") != string::npos);

            Url furl("url");
            furl.deserialize(um, &dds, false);
            CPPUNIT_ASSERT(furl.value() == url_value);
        }
        catch (Error &e) {
            string err = "failed:" + e.get_error_message();
            CPPUNIT_FAIL(err.c_str());
        }
    }

    void array_stream_serialize_test()
    {
        try {
            ofstream strm("a_test.strm", ios::out | ios::trunc);
            XDRStreamMarshaller sm(strm);

            arr->serialize(eval, dds, sm, false);
        }
        catch (Error &e) {
            string err = "failed:" + e.get_error_message();
            CPPUNIT_FAIL(err.c_str());
        }
    }

    void array_stream_deserialize_test()
    {
        try {
#if 0
            ifstream strm( "a_test.strm", ios::in );
            XDRStreamUnMarshaller um( strm );
#else
            FILE *sf = fopen("a_test.strm", "r");
            XDRFileUnMarshaller um(sf);
#endif
            TestByte fab("ab");
            TestArray farr("arr", &fab);
            farr.append_dim(5, "dim1");
            farr.append_dim(3, "dim2");
            farr.deserialize(um, &dds, false);

            CPPUNIT_ASSERT(farr.length() == arr->length());

            dods_byte fdb[arr->length() * sizeof(dods_byte)];
            farr.value(fdb);
            CPPUNIT_ASSERT(!memcmp((void *) fdb, (void *) &db[0], farr.length() * sizeof(dods_byte)));
        }
        catch (Error &e) {
            string err = "failed:" + e.get_error_message();
            CPPUNIT_FAIL(err.c_str());
        }
    }

    // This version of array_file_serialize_test tests the new code in Vector and
    // Marshaller that enables an Array's serialization to be split over two or more calls.
    void array_stream_serialize_test_2()
    {
        try {
            fstream f("a_test_2.file", fstream::out);
            XDRStreamMarshaller fm(f);

            DBG(cerr << "arr->length(): " << arr->length() << endl);
            fm.put_vector_start(arr->length());

            DBG(cerr << "arr->var()->width(): " << arr->var()->width() << endl);

            switch (arr->var()->type()) {
            case dods_byte_c:
            case dods_int16_c:
            case dods_uint16_c:
            case dods_int32_c:
            case dods_uint32_c:
            case dods_float32_c:
            case dods_float64_c:
                fm.put_vector_part(arr->get_buf(), arr->length(), arr->var()->width(), arr->var()->type());
                fm.put_vector_end();
                break;

            default:
                throw InternalErr(__FILE__, __LINE__, "Implemented for numeric simple types only");
            }
        }
        catch (Error &e) {
            string err = "failed:" + e.get_error_message();
            CPPUNIT_FAIL(err.c_str());
        }

        // now test the file contents to see if the correct stuff was serialized.
        // Given that this test runs after the first array serialize test, just
        // use system("cmp ...").
        //int status = system("cmp a_test.file a_test_2.file >/dev/null 2>&1");
        CPPUNIT_ASSERT(0 == system("cmp a_test.file a_test_2.file >/dev/null 2>&1"));
    }

    void array_stream_serialize_test_3()
    {
        try {
            fstream f("a_test_3.file", fstream::out);
            XDRStreamMarshaller fm(f);

            DBG(cerr << "arr->length(): " << arr->length() << endl);
            fm.put_vector_start(arr->length());

            DBG(cerr << "arr->var()->width(): " << arr->var()->width() << endl);

            const int size_of_first_part = 4;
            switch (arr->var()->type()) {
            case dods_byte_c:
            case dods_int16_c:
            case dods_uint16_c:
            case dods_int32_c:
            case dods_uint32_c:
            case dods_float32_c:
            case dods_float64_c:
                fm.put_vector_part(arr->get_buf(), size_of_first_part, arr->var()->width(), arr->var()->type());
                fm.put_vector_part(arr->get_buf() + size_of_first_part, arr->length() - size_of_first_part,
                    arr->var()->width(), arr->var()->type());
                fm.put_vector_end();
                break;

            default:
                throw InternalErr(__FILE__, __LINE__, "Implemented for numeric simple types only");
            }
        }
        catch (Error &e) {
            string err = "failed:" + e.get_error_message();
            CPPUNIT_FAIL(err.c_str());
        }

        // now test the file contents to see if the correct stuff was serialized.
        // Given that this test runs after the first array serialize test, just
        // use system("cmp ...").
        //int status = system("cmp a_test.file a_test_2.file >/dev/null 2>&1");
        CPPUNIT_ASSERT(0 == system("cmp a_test.file a_test_3.file >/dev/null 2>&1"));
    }

    void array_stream_serialize_test_4()
    {
        try {
            fstream f("a_test_4.file", fstream::out);
            XDRStreamMarshaller fm(f);

            DBG(cerr << "arr->length(): " << arr->length() << endl);
            fm.put_vector_start(arr->length());

            DBG(cerr << "arr->var()->width(): " << arr->var()->width() << endl);

            const int size_of_first_part = 5;
            switch (arr->var()->type()) {
            case dods_byte_c:
            case dods_int16_c:
            case dods_uint16_c:
            case dods_int32_c:
            case dods_uint32_c:
            case dods_float32_c:
            case dods_float64_c:
                fm.put_vector_part(arr->get_buf(), size_of_first_part, arr->var()->width(), arr->var()->type());
                fm.put_vector_part(arr->get_buf() + size_of_first_part, arr->length() - size_of_first_part,
                    arr->var()->width(), arr->var()->type());
                fm.put_vector_end();
                break;

            default:
                throw InternalErr(__FILE__, __LINE__, "Implemented for numeric simple types only");
            }
        }
        catch (Error &e) {
            string err = "failed:" + e.get_error_message();
            CPPUNIT_FAIL(err.c_str());
        }

        // now test the file contents to see if the correct stuff was serialized.
        // Given that this test runs after the first array serialize test, just
        // use system("cmp ...").
        //int status = system("cmp a_test.file a_test_2.file >/dev/null 2>&1");
        CPPUNIT_ASSERT(0 == system("cmp a_test.file a_test_4.file >/dev/null 2>&1"));
    }

    void array_f32_stream_serialize_test()
    {
        try {
            ofstream strm("a_f32_test.file", ios::out | ios::trunc);
            XDRStreamMarshaller sm(strm);

            arr_f32->serialize(eval, dds, sm, false);
        }
        catch (Error &e) {
            string err = "failed:" + e.get_error_message();
            CPPUNIT_FAIL(err.c_str());
        }
    }

    void array_f32_stream_deserialize_test()
    {
        try {
#if 0
            ifstream strm( "a_test.strm", ios::in );
            XDRStreamUnMarshaller um( strm );
#else
            FILE *sf = fopen("a_f32_test.file", "r");
            XDRFileUnMarshaller um(sf);
#endif
            TestFloat32 fa_f32("a_f32");
            TestArray farr("arr_f32", &fa_f32);
            farr.append_dim(5, "dim1");
            farr.append_dim(3, "dim2");
            farr.deserialize(um, &dds, false);

            CPPUNIT_ASSERT(farr.length() == arr->length());

            vector<dods_float32> fd_f32(arr->length());
            farr.value(&fd_f32[0]);
            CPPUNIT_ASSERT(!memcmp((void *) &fd_f32[0], (void *) &d_f32[0], farr.length() * sizeof(dods_float32)));
        }
        catch (Error &e) {
            string err = "failed:" + e.get_error_message();
            CPPUNIT_FAIL(err.c_str());
        }
    }

    void array_f32_stream_serialize_test_2()
    {
        try {
            fstream f("a_f32_test_2.file", fstream::out);
            XDRStreamMarshaller fm(f);

            DBG(cerr << "arr_f32->length(): " << arr_f32->length() << endl);
            fm.put_vector_start(arr_f32->length());

            DBG(cerr << "&arr_f32->get_buf(): " << hex << (void * )arr_f32->get_buf() << dec << endl);
            DBG(cerr << "arr_f32->var()->width(): " << arr_f32->var()->width() << endl);

            const int size_of_first_part = 5;

            switch (arr_f32->var()->type()) {
            case dods_byte_c:
            case dods_int16_c:
            case dods_uint16_c:
            case dods_int32_c:
            case dods_uint32_c:
            case dods_float32_c:
            case dods_float64_c:
                fm.put_vector_part(arr_f32->get_buf(), size_of_first_part, arr_f32->var()->width(),
                    arr_f32->var()->type());

                // For this call, we have to pass the memory location of the rest of the array,
                // so we do a little calculation since get_buf() returns a char *. Actual code
                // would not need to do that.
                fm.put_vector_part(arr_f32->get_buf() + (size_of_first_part * arr_f32->var()->width()),
                    arr_f32->length() - size_of_first_part, arr_f32->var()->width(), arr_f32->var()->type());
                fm.put_vector_end();
                break;

            default:
                throw InternalErr(__FILE__, __LINE__, "Implemented for numeric simple types only");
            }
        }
        catch (Error &e) {
            string err = "failed:" + e.get_error_message();
            CPPUNIT_FAIL(err.c_str());
        }

        // now test the file contents to see if the correct stuff was serialized.
        // Given that this test runs after the first array serialize test, just
        // use system("cmp ...").
        CPPUNIT_ASSERT(0 == system("cmp a_f32_test.file a_f32_test_2.file >/dev/null 2>&1"));
    }

    void array_f64_stream_serialize_test()
    {
        try {
            ofstream strm("a_f64_test.file", ios::out | ios::trunc);
            XDRStreamMarshaller sm(strm);

            arr_f64->serialize(eval, dds, sm, false);
        }
        catch (Error &e) {
            string err = "failed:" + e.get_error_message();
            CPPUNIT_FAIL(err.c_str());
        }
    }

    void array_f64_stream_deserialize_test()
    {
        try {
#if 0
            ifstream strm( "a_test.strm", ios::in );
            XDRStreamUnMarshaller um( strm );
#else
            FILE *sf = fopen("a_f64_test.file", "r");
            XDRFileUnMarshaller um(sf);
#endif
            TestFloat64 fa_f64("a_f64");
            TestArray farr("arr_f64", &fa_f64);
            farr.append_dim(5, "dim1");
            farr.append_dim(3, "dim2");
            farr.deserialize(um, &dds, false);

            CPPUNIT_ASSERT(farr.length() == arr->length());

            vector<dods_float64> fd_f64(arr->length());
            farr.value(&fd_f64[0]);
            CPPUNIT_ASSERT(!memcmp((void *) &fd_f64[0], (void *) &d_f64[0], farr.length() * sizeof(dods_float64)));
        }
        catch (Error &e) {
            string err = "failed:" + e.get_error_message();
            CPPUNIT_FAIL(err.c_str());
        }
    }

    void array_f64_stream_serialize_test_2()
    {
        try {
            fstream f("a_f64_test_2.file", fstream::out);
            XDRStreamMarshaller fm(f);

            DBG(cerr << "arr_f64->length(): " << arr_f64->length() << endl);
            fm.put_vector_start(arr_f64->length());

            DBG(cerr << "&arr_f64->get_buf(): " << hex << (void * )arr_f64->get_buf() << dec << endl);
            DBG(cerr << "arr_f64->var()->width(): " << arr_f64->var()->width() << endl);

            const int size_of_first_part = 5;

            switch (arr_f64->var()->type()) {
            case dods_byte_c:
            case dods_int16_c:
            case dods_uint16_c:
            case dods_int32_c:
            case dods_uint32_c:
            case dods_float32_c:
            case dods_float64_c:
                fm.put_vector_part(arr_f64->get_buf(), size_of_first_part, arr_f64->var()->width(),
                    arr_f64->var()->type());

                // For this call, we have to pass the memory location of the rest of the array,
                // so we do a little calculation since get_buf() returns a char *. Actual code
                // would not need to do that.
                fm.put_vector_part(arr_f64->get_buf() + (size_of_first_part * arr_f64->var()->width()),
                    arr_f64->length() - size_of_first_part, arr_f64->var()->width(), arr_f64->var()->type());
                fm.put_vector_end();
                break;

            default:
                throw InternalErr(__FILE__, __LINE__, "Implemented for numeric simple types only");
            }
        }
        catch (Error &e) {
            string err = "failed:" + e.get_error_message();
            CPPUNIT_FAIL(err.c_str());
        }

        // now test the file contents to see if the correct stuff was serialized.
        // Given that this test runs after the first array serialize test, just
        // use system("cmp ...").
        CPPUNIT_ASSERT(0 == system("cmp a_f64_test.file a_f64_test_2.file >/dev/null 2>&1"));
    }

    void structure_stream_serialize_test()
    {
        try {
            ofstream strm("struct_test.strm", ios::out | ios::trunc);
            XDRStreamMarshaller sm(strm);
            s->serialize(eval, dds, sm, false);
        }
        catch (Error &e) {
            string err = "failed:" + e.get_error_message();
            CPPUNIT_FAIL(err.c_str());
        }
    }

    void structure_stream_deserialize_test()
    {
        try {
#if 0
            ifstream strm( "struct_test.strm", ios::in );
            XDRStreamUnMarshaller um( strm );
#else
            FILE *sf = fopen("struct_test.strm", "r");
            XDRFileUnMarshaller um(sf);
#endif
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
            CPPUNIT_ASSERT(fsi32_p->value() == i32->value());

            Str *fsstr_p = dynamic_cast<Str *>(fs.var("fsstr"));
            CPPUNIT_ASSERT(fsstr_p);
            DBG(cerr << "fsstr_p->value(): " << fsstr_p->value() << endl);
            CPPUNIT_ASSERT(fsstr_p->value().find("Silly test string:") != string::npos);

            BaseType *bt = fs.var("fsab");
            CPPUNIT_ASSERT(bt);
            Array *fsarr_p = dynamic_cast<Array *>(bt);
            CPPUNIT_ASSERT(fsarr_p);
            CPPUNIT_ASSERT(fsarr_p->length() == arr->length());
            dods_byte fdb[fsarr_p->length() * sizeof(dods_byte)];
            fsarr_p->value(fdb);
            CPPUNIT_ASSERT(!memcmp((void *) fdb, (void *) &db[0], fsarr_p->length() * sizeof(dods_byte)));
        }
        catch (Error &e) {
            string err = "failed:" + e.get_error_message();
            CPPUNIT_FAIL(err.c_str());
        }
    }

    void grid_stream_serialize_test()
    {
        try {
            ofstream strm("g_test.strm", ios::out | ios::trunc);
            XDRStreamMarshaller sm(strm);

            TestGrid tg("grid1");
            TestArray arr2("arr2", ab);
            arr2.append_dim(5, "dim1");
            arr2.append_dim(3, "dim2");
            tg.add_var(&arr2, array);

            TestArray map1("map1", f32);
            map1.append_dim(5, "dim1");
            tg.add_var(&map1, maps);

            TestArray map2("map2", f32);
            map2.append_dim(3, "dim2");
            tg.add_var(&map2, maps);

            tg.set_send_p(true);
            tg.read();
            tg.set_read_p(true);

            tg.serialize(eval, dds, sm, false);
        }
        catch (Error &e) {
            string err = "failed:" + e.get_error_message();
            CPPUNIT_FAIL(err.c_str());
        }
    }

    void grid_stream_deserialize_test()
    {
        try {
#if 0
            ifstream strm( "g_test.strm", ios::in );
            XDRStreamUnMarshaller um( strm );
#else
            FILE *sf = fopen("g_test.strm", "r");
            XDRFileUnMarshaller um(sf);
#endif
            TestGrid tg("grid1");
            TestArray arr2("arr2", ab);
            arr2.append_dim(5, "dim1");
            arr2.append_dim(3, "dim2");
            tg.add_var(&arr2, array);

            TestArray map1("map1", f32);
            map1.append_dim(5, "dim1");
            tg.add_var(&map1, maps);

            TestArray map2("map2", f32);
            map2.append_dim(3, "dim2");
            tg.add_var(&map2, maps);

            tg.deserialize(um, &dds, false);

            // Check the values in the array
            CPPUNIT_ASSERT(tg.get_array()->length() == arr->length());

            dods_byte fdb[tg.get_array()->length() * sizeof(dods_byte)];
            tg.get_array()->value(fdb);
            CPPUNIT_ASSERT(!memcmp((void *) fdb, (void *) &db[0], tg.get_array()->length() * sizeof(dods_byte)));
        }
        catch (Error &e) {
            string err = "failed:" + e.get_error_message();
            CPPUNIT_FAIL(err.c_str());
        }
    }

    void sequence_stream_serialize_test()
    {
        try {
            ofstream strm("seq_test.strm", ios::out | ios::trunc);
            XDRStreamMarshaller sm(strm);

            TestSequence seq("seq");
            seq.add_var(f64);
            seq.add_var(arr);

            TestSequence seq2("seq2");
            seq2.add_var(ui16);
            seq2.add_var(url);
            seq.add_var(&seq2);

            seq.set_send_p(true);
            seq.set_leaf_sequence();

            seq.serialize(eval, dds, sm, false);
        }
        catch (Error &e) {
            string err = "failed:" + e.get_error_message();
            CPPUNIT_FAIL(err.c_str());
        }
    }

    void sequence_stream_deserialize_test()
    {
        try {
#if 0
            ifstream strm( "seq_test.strm", ios::in );
            XDRStreamUnMarshaller um( strm );
#else
            FILE *sf = fopen("seq_test.strm", "r");
            XDRFileUnMarshaller um(sf);
#endif
            dods_byte fdb[arr->length() * sizeof(dods_byte)];

            TestSequence seq("seq");
            seq.add_var(f64);
            seq.add_var(arr);

            TestSequence seq2("seq2");
            seq2.add_var(ui16);
            seq2.add_var(url);

            seq.add_var(&seq2);

            seq.set_leaf_sequence();

            seq.deserialize(um, &dds, false);

            unsigned int num_rows = seq.number_of_rows();
            CPPUNIT_ASSERT(num_rows == 4);
            for (unsigned int i = 0; i < num_rows; i++) {
                BaseTypeRow *row = seq.row_value(i);
                CPPUNIT_ASSERT(row);
                CPPUNIT_ASSERT(row->size() == 3);
                Float64 *f64_p = dynamic_cast<Float64 *>((*row)[0]);
                CPPUNIT_ASSERT(f64_p);
                CPPUNIT_ASSERT(f64_p->value() == f64->value());
                Array *arr_p = dynamic_cast<Array *>((*row)[1]);
                CPPUNIT_ASSERT(arr_p);
                arr_p->value(fdb);
                CPPUNIT_ASSERT(arr_p->length() == arr->length());
                CPPUNIT_ASSERT(!memcmp((void *) fdb, (void *) &db[0], arr_p->length() * sizeof(dods_byte)));
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
        }
        catch (Error &e) {
            string err = "failed:" + e.get_error_message();
            CPPUNIT_FAIL(err.c_str());
        }
    }

    // Test put_vector and its operation, both with and without using
    // pthreads
    void array_stream_put_vector_thread_test()
    {
        try {
            fstream f("a_test_pv.file", fstream::out);
            XDRStreamMarshaller fm(f);

            switch (arr->var()->type()) {
            case dods_byte_c: {
                fm.put_vector/*_thread*/(arr->get_buf(), arr->length(), *arr);
                break;
            }
            case dods_int16_c:
            case dods_uint16_c:
            case dods_int32_c:
            case dods_uint32_c:
            case dods_float32_c:
            case dods_float64_c:
                throw InternalErr(__FILE__, __LINE__, "Unit test fail; array is a byte array.");
                break;

            default:
                throw InternalErr(__FILE__, __LINE__, "Implemented for numeric simple types only");
            }
        }
        catch (Error &e) {
            string err = "failed:" + e.get_error_message();
            CPPUNIT_FAIL(err.c_str());
        }

        //int status = system("cmp a_test.file a_test_2.file >/dev/null 2>&1");
        CPPUNIT_ASSERT(0 == system("cmp a_test.file a_test_pv.file >/dev/null 2>&1"));
    }

    // This test doesn't actually check its result - fix or replace
    void array_stream_put_vector_thread_test_2()
    {
        try {
            fstream f("a_test_pv_2.file", fstream::out);
            XDRStreamMarshaller fm(f);

            switch (arr->var()->type()) {
            case dods_byte_c: {
                DBG(cerr << "arr->get_buf(): " << hex << (void* )arr->get_buf() << dec << endl);

                fm.put_vector(arr->get_buf(), arr->length(), *arr);
                fm.put_vector(arr->get_buf(), arr->length(), *arr);
                fm.put_vector(arr->get_buf(), arr->length(), *arr);
                fm.put_vector(arr->get_buf(), arr->length(), *arr);
                break;
            }
            case dods_int16_c:
            case dods_uint16_c:
            case dods_int32_c:
            case dods_uint32_c:
            case dods_float32_c:
            case dods_float64_c:
                throw InternalErr(__FILE__, __LINE__, "Unit test fail; array is a byte array.");
                break;

            default:
                throw InternalErr(__FILE__, __LINE__, "Implemented for numeric simple types only");
            }
        }
        catch (Error &e) {
            string err = "failed:" + e.get_error_message();
            CPPUNIT_FAIL(err.c_str());
        }

        try {
            FILE *sf = fopen("a_test_pv_2.file", "r");
            XDRFileUnMarshaller um(sf);

            TestByte fab("ab");
            TestArray farr("farr", &fab);
            farr.append_dim(5, "dim1");
            farr.append_dim(3, "dim2");
            farr.deserialize(um, &dds, false);

            CPPUNIT_ASSERT(farr.length() == arr->length());

            dods_byte fdb[arr->length() * sizeof(dods_byte)];
            farr.value(fdb);
            CPPUNIT_ASSERT(!memcmp((void *) fdb, (void *) &db[0], farr.length() * sizeof(dods_byte)));

            // now get three more arrays of the same size
            farr.deserialize(um, &dds, false);
            farr.value(fdb);
            CPPUNIT_ASSERT(!memcmp((void *) fdb, (void *) &db[0], farr.length() * sizeof(dods_byte)));

            farr.deserialize(um, &dds, false);
            farr.value(fdb);
            CPPUNIT_ASSERT(!memcmp((void *) fdb, (void *) &db[0], farr.length() * sizeof(dods_byte)));

            farr.deserialize(um, &dds, false);
            farr.value(fdb);
            CPPUNIT_ASSERT(!memcmp((void *) fdb, (void *) &db[0], farr.length() * sizeof(dods_byte)));
        }
        catch (Error &e) {
            string err = "failed:" + e.get_error_message();
            CPPUNIT_FAIL(err.c_str());
        }
    }

    void array_stream_put_vector_thread_test_3()
    {
        try {
            fstream f("a_test_pv_3.file", fstream::out);
            XDRStreamMarshaller fm(f);

            switch (arr->var()->type()) {
            case dods_byte_c: {
                DBG(cerr << "arr->get_buf(): " << hex << (void* )arr->get_buf() << dec << endl);

                // test sequencing of threads and non-threaded calls. Note that for the
                // non-threaded calls, we pass a _reference_ to the object and it's an
                // ignored parameter (left over cruft...).
                fm.put_vector(arr->get_buf(), arr->length(), *arr);
                fm.put_vector(arr->get_buf(), arr->length(), *arr);
                fm.put_vector(arr->get_buf(), arr->length(), *arr);
                fm.put_vector(arr->get_buf(), arr->length(), *arr);

                // No need to wait since put_vector() should be doing that
                break;
            }
            case dods_int16_c:
            case dods_uint16_c:
            case dods_int32_c:
            case dods_uint32_c:
            case dods_float32_c:
            case dods_float64_c:
                throw InternalErr(__FILE__, __LINE__, "Unit test fail; array is a byte array.");
                break;

            default:
                throw InternalErr(__FILE__, __LINE__, "Implemented for numeric simple types only");
            }
        }
        catch (Error &e) {
            string err = "failed:" + e.get_error_message();
            CPPUNIT_FAIL(err.c_str());
        }

        // this should be identical to the output from pv_2 (the previous test).
        CPPUNIT_ASSERT(0 == system("cmp a_test_pv_2.file a_test_pv_3.file >/dev/null 2>&1"));
    }

    void array_stream_put_vector_thread_test_4()
    {
        try {
            fstream f("a_f32_test_pv.file", fstream::out);
            XDRStreamMarshaller fm(f);

            switch (arr_f32->var()->type()) {
            case dods_byte_c:
                throw InternalErr(__FILE__, __LINE__, "Unit test fail; array is not a byte array.");
                break;
            case dods_int16_c:
            case dods_uint16_c:
            case dods_int32_c:
            case dods_uint32_c:
            case dods_float32_c:
            case dods_float64_c: {
                DBG(cerr << "arr_f32->get_buf(): " << hex << (void* )arr_f32->get_buf() << dec << endl);

                fm.put_vector(arr_f32->get_buf(), arr_f32->length(), arr_f32->var()->width(), *arr_f32);

                break;
            }

            default:
                throw InternalErr(__FILE__, __LINE__, "Implemented for numeric simple types only");
            }
        }
        catch (Error &e) {
            string err = "failed:" + e.get_error_message();
            CPPUNIT_FAIL(err.c_str());
        }

        CPPUNIT_ASSERT(0 == system("cmp a_f32_test.file a_f32_test_pv.file >/dev/null 2>&1"));
    }

    void array_stream_put_vector_thread_test_5()
    {
        try {
            fstream f("a_f32_test_pv_2.file", fstream::out);
            XDRStreamMarshaller fm(f);

            switch (arr_f32->var()->type()) {
            case dods_byte_c:
                throw InternalErr(__FILE__, __LINE__, "Unit test fail; array is not a byte array.");
                break;
            case dods_int16_c:
            case dods_uint16_c:
            case dods_int32_c:
            case dods_uint32_c:
            case dods_float32_c:
            case dods_float64_c: {
                DBG(cerr << "arr_f32->get_buf(): " << hex << (void* )arr_f32->get_buf() << dec << endl);

                fm.put_vector(arr_f32->get_buf(), arr_f32->length(), arr_f32->var()->width(), *arr_f32);
                fm.put_vector(arr_f32->get_buf(), arr_f32->length(), arr_f32->var()->width(), *arr_f32);

                break;
            }

            default:
                throw InternalErr(__FILE__, __LINE__, "Implemented for numeric simple types only");
            }
        }
        catch (Error &e) {
            string err = "failed:" + e.get_error_message();
            CPPUNIT_FAIL(err.c_str());
        }

        try {
            FILE *sf = fopen("a_f32_test_pv_2.file", "r");
            XDRFileUnMarshaller um(sf);

            TestFloat32 fa_32("fa_32");
            TestArray farr("farr", &fa_32);
            farr.append_dim(5, "dim1");
            farr.append_dim(3, "dim2");
            farr.deserialize(um, &dds, false);

            CPPUNIT_ASSERT(farr.length() == arr->length());

            dods_float32 fd_32[arr->length() * sizeof(dods_float32)];
            farr.value(fd_32);
            CPPUNIT_ASSERT(!memcmp((void *) fd_32, (void *) &d_f32[0], farr.length() * sizeof(dods_float32)));

            // now get three more arrays of the same size
            farr.deserialize(um, &dds, false);
            farr.value(fd_32);
            CPPUNIT_ASSERT(!memcmp((void *) fd_32, (void *) &d_f32[0], farr.length() * sizeof(dods_byte)));
        }
        catch (Error &e) {
            string err = "failed:" + e.get_error_message();
            CPPUNIT_FAIL(err.c_str());
        }
    }

    void array_stream_serialize_part_thread_test()
    {
        try {
            fstream f("a_test_ptv.file", fstream::out);
            XDRStreamMarshaller fm(f);

            DBG(cerr << "arr->length(): " << arr->length() << endl);
            fm.put_vector_start(arr->length());

            DBG(cerr << "arr->var()->width(): " << arr->var()->width() << endl);

            const int size_of_first_part = 5;
            switch (arr->var()->type()) {
            case dods_byte_c:
            case dods_int16_c:
            case dods_uint16_c:
            case dods_int32_c:
            case dods_uint32_c:
            case dods_float32_c:
            case dods_float64_c: {
                fm.put_vector_part(arr->get_buf(), size_of_first_part, arr->var()->width(), arr->var()->type());

                fm.put_vector_part(arr->get_buf() + size_of_first_part, arr->length() - size_of_first_part,
                    arr->var()->width(), arr->var()->type());

                fm.put_vector_end();    // forces a wait on the thread
                break;
            }

            default:
                throw InternalErr(__FILE__, __LINE__, "Implemented for numeric simple types only");
            }
        }
        catch (Error &e) {
            string err = "failed:" + e.get_error_message();
            CPPUNIT_FAIL(err.c_str());
        }

        CPPUNIT_ASSERT(0 == system("cmp a_test.file a_test_ptv.file >/dev/null 2>&1"));
    }

    void array_stream_serialize_part_thread_test_2()
    {
        try {
            fstream f("a_f32_test_ptv.file", fstream::out);
            XDRStreamMarshaller fm(f);

            DBG(cerr << "arr_f32->length(): " << arr_f32->length() << endl);
            fm.put_vector_start(arr_f32->length());

            DBG(cerr << "arr_f32->var()->width(): " << arr_f32->var()->width() << endl);

            const int size_of_first_part = 5;
            switch (arr_f32->var()->type()) {
            case dods_byte_c:
            case dods_int16_c:
            case dods_uint16_c:
            case dods_int32_c:
            case dods_uint32_c:
            case dods_float32_c:
            case dods_float64_c: {
                fm.put_vector_part(arr_f32->get_buf(), size_of_first_part, arr_f32->var()->width(),
                    arr_f32->var()->type());

                fm.put_vector_part(arr_f32->get_buf() + (size_of_first_part * arr_f32->var()->width()),
                    arr_f32->length() - size_of_first_part, arr_f32->var()->width(), arr_f32->var()->type());

                fm.put_vector_end();    // forces a wait on the thread
                break;
            }

            default:
                throw InternalErr(__FILE__, __LINE__, "Implemented for numeric simple types only");
            }

        }
        catch (Error &e) {
            string err = "failed:" + e.get_error_message();
            CPPUNIT_FAIL(err.c_str());
        }

        CPPUNIT_ASSERT(0 == system("cmp a_f32_test.file a_f32_test_ptv.file >/dev/null 2>&1"));
    }

    void array_stream_serialize_part_thread_test_3()
    {
        try {
            fstream f("a_f64_test_ptv.file", fstream::out);
            XDRStreamMarshaller fm(f);

            DBG(cerr << "arr_f64->length(): " << arr_f64->length() << endl);
            fm.put_vector_start(arr_f64->length());

            DBG(cerr << "arr_f64->var()->width(): " << arr_f64->var()->width() << endl);

            const int size_of_first_part = 5;
            switch (arr_f64->var()->type()) {
            case dods_byte_c:
            case dods_int16_c:
            case dods_uint16_c:
            case dods_int32_c:
            case dods_uint32_c:
            case dods_float32_c:
            case dods_float64_c:
                fm.put_vector_part(arr_f64->get_buf(), size_of_first_part, arr_f64->var()->width(),
                    arr_f64->var()->type());

                fm.put_vector_part(arr_f64->get_buf() + (size_of_first_part * arr_f64->var()->width()),
                    arr_f64->length() - size_of_first_part, arr_f64->var()->width(), arr_f64->var()->type());

                fm.put_vector_end();    // forces a wait on the thread
                break;

            default:
                throw InternalErr(__FILE__, __LINE__, "Implemented for numeric simple types only");
            }
        }
        catch (Error &e) {
            string err = "failed:" + e.get_error_message();
            CPPUNIT_FAIL(err.c_str());
        }

        CPPUNIT_ASSERT(0 == system("cmp a_f64_test.file a_f64_test_ptv.file >/dev/null 2>&1"));
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION (MarshallerTest);

} // namepsace libdap

int main(int argc, char*argv[])
{
    GetOpt getopt(argc, argv, "dh");
    int option_char;

    while ((option_char = getopt()) != -1)
        switch (option_char) {
        case 'd':
            debug = 1;  // debug is a static global
            break;
        case 'h': {     // help - show test names
            cerr << "Usage: MarshallerTest has the following tests:" << endl;
            const std::vector<Test*> &tests = libdap::MarshallerTest::suite()->getTests();
            unsigned int prefix_len = libdap::MarshallerTest::suite()->getName().append("::").length();
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
            test = libdap::MarshallerTest::suite()->getName().append("::").append(argv[i]);
            wasSuccessful = wasSuccessful && runner.run(test);
        }
    }

    xmlMemoryDump();

    return wasSuccessful ? 0 : 1;
}
