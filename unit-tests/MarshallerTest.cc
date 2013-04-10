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
#if 0
#include "fdiostream.h"
#endif
#include "debug.h"

using std::cerr;
using std::cout;
using std::endl;
using std::ofstream;
using std::ifstream;

int test_variable_sleep_interval = 0; // Used in Test* classes for testing
// timeouts.

class MarshallerTest: public CppUnit::TestFixture {

CPPUNIT_TEST_SUITE( MarshallerTest );

        CPPUNIT_TEST( simple_types_file_serialize_test );

        CPPUNIT_TEST( array_file_serialize_test );
        CPPUNIT_TEST( structure_file_serialize_test );
        CPPUNIT_TEST( grid_file_serialize_test );
        CPPUNIT_TEST( sequence_file_serialize_test );

        CPPUNIT_TEST( simple_types_file_deserialize_test );
        CPPUNIT_TEST( array_file_deserialize_test );
        CPPUNIT_TEST( structure_file_deserialize_test );
        CPPUNIT_TEST( grid_file_deserialize_test );
        CPPUNIT_TEST( sequence_file_deserialize_test );

        CPPUNIT_TEST( simple_types_stream_serialize_test );
        CPPUNIT_TEST( array_stream_serialize_test );
        CPPUNIT_TEST( structure_stream_serialize_test );
        CPPUNIT_TEST( grid_stream_serialize_test );
        CPPUNIT_TEST( sequence_stream_serialize_test );

        CPPUNIT_TEST( simple_types_stream_deserialize_test );
        CPPUNIT_TEST( array_stream_deserialize_test );
        CPPUNIT_TEST( structure_stream_deserialize_test );
        CPPUNIT_TEST( grid_stream_deserialize_test );
        CPPUNIT_TEST( sequence_stream_deserialize_test );

#if CHECKSUMS
        CPPUNIT_TEST( simple_types_stream_serialize_checksum_test );
        CPPUNIT_TEST( array_stream_serialize_checksum_test );
        CPPUNIT_TEST( structure_stream_serialize_checksum_test );
        CPPUNIT_TEST( grid_stream_serialize_checksum_test );
#if 0
        CPPUNIT_TEST( sequence_stream_serialize_checksum_test );
#endif
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

    TestByte *ab;

    TestArray *arr;

    TestStructure *s;

    ConstraintEvaluator eval;
    TestTypeFactory ttf;
    DataDDS dds;

    string str_value, str2_value;
    string url_value;

    dods_byte *db;
public:
    MarshallerTest() :
        b(0), i16(0), i32(0), ui16(0), ui32(0), f32(0), f64(0), str(0), url(0), ab(0), arr(0), s(0), dds(&ttf, "dds")
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

        ab = new TestByte("ab");
        ab->read();
        arr = new TestArray("arr", ab);
        arr->append_dim(5, "dim1");
        arr->append_dim(3, "dim2");
        arr->read();
        arr->set_read_p(true);

        db = new dods_byte[arr->length() * sizeof(dods_byte)];
        for (int i = 0; i < arr->length(); ++i)
            db[i] = 126;
        arr->value(db);

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
        catch( Error &e ) {
            string err = "failed:" + e.get_error_message();
            CPPUNIT_FAIL( err.c_str() );
        }
    }

    void simple_types_file_deserialize_test()
    {
        try {
            FILE *ff = fopen("st_test.file", "r");
            XDRFileUnMarshaller um(ff);

            Byte fb("fb");
            fb.deserialize(um, &dds, false);
            CPPUNIT_ASSERT( fb.value() == b->value() );

            Int16 fi16("i16");
            fi16.deserialize(um, &dds, false);
            CPPUNIT_ASSERT( fi16.value() == i16->value() );

            Int32 fi32("i32");
            fi32.deserialize(um, &dds, false);
            CPPUNIT_ASSERT( fi32.value() == i32->value() );

            UInt16 fui16("ui16");
            fui16.deserialize(um, &dds, false);
            CPPUNIT_ASSERT( fui16.value() == ui16->value() );

            UInt32 fui32("ui32");
            fui32.deserialize(um, &dds, false);
            CPPUNIT_ASSERT( fui32.value() == ui32->value() );

            Float32 ff32("f32");
            ff32.deserialize(um, &dds, false);
            CPPUNIT_ASSERT( ff32.value() == f32->value() );

            Float64 ff64("f64");
            ff64.deserialize(um, &dds, false);
            CPPUNIT_ASSERT( ff64.value() == f64->value() );

            Str fstr("str");
            fstr.deserialize(um, &dds, false);
            // Test for the string value like this because the digit after
            // the colon changes each time the read() method is called.
            CPPUNIT_ASSERT( fstr.value().find("Silly test string:") != string::npos );

            Url furl("url");
            furl.deserialize(um, &dds, false);
            CPPUNIT_ASSERT( furl.value() == url_value );
        }
        catch( Error &e ) {
            string err = "failed:" + e.get_error_message();
            CPPUNIT_FAIL( err.c_str() );
        }
    }

    void array_file_serialize_test()
    {
        try {
            FILE *f = fopen("a_test.file", "w");
            XDRFileMarshaller fm(f);

            arr->serialize(eval, dds, fm, false);

        }
        catch( Error &e ) {
            string err = "failed:" + e.get_error_message();
            CPPUNIT_FAIL( err.c_str() );
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

            CPPUNIT_ASSERT( farr.length() == arr->length() );

            dods_byte fdb[farr.length() * sizeof(dods_byte)];
            farr.value(fdb);
            CPPUNIT_ASSERT( !memcmp( (void *)fdb, (void *)db, farr.length() * sizeof( dods_byte ) ) );
        }
        catch( Error &e ) {
            string err = "failed:" + e.get_error_message();
            CPPUNIT_FAIL( err.c_str() );
        }
    }

    void structure_file_serialize_test()
    {
        try {
            FILE *f = fopen("struct_test.file", "w");
            XDRFileMarshaller fm(f);
            s->serialize(eval, dds, fm, false);
        }
        catch( Error &e ) {
            string err = "failed:" + e.get_error_message();
            CPPUNIT_FAIL( err.c_str() );
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

            Int32 *fsi32_p = dynamic_cast<Int32 *> (fs.var("fsi32"));
            CPPUNIT_ASSERT( fsi32_p );
            CPPUNIT_ASSERT( fsi32_p->value() == i32->value() );

            Str *fsstr_p = dynamic_cast<Str *> (fs.var("fsstr"));
            CPPUNIT_ASSERT( fsstr_p );
            DBG2(cerr << "fsstr_p->value(): " << fsstr_p->value() << endl);
            CPPUNIT_ASSERT( fsstr_p->value().find("Silly test string:") != string::npos );

            BaseType *bt = fs.var("fsab");
            CPPUNIT_ASSERT( bt );
            Array *fsarr_p = dynamic_cast<Array *> (bt);
            CPPUNIT_ASSERT( fsarr_p );
            dods_byte fdb[fsarr_p->length() * sizeof(dods_byte)];
            fsarr_p->value(fdb);

            CPPUNIT_ASSERT( fsarr_p->length() == arr->length() );
            CPPUNIT_ASSERT( !memcmp( (void *)fdb, (void *)db, fsarr_p->length() * sizeof( dods_byte ) ) );
        }
        catch( Error &e ) {
            string err = "failed:" + e.get_error_message();
            CPPUNIT_FAIL( err.c_str() );
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
        catch( Error &e ) {
            string err = "failed:" + e.get_error_message();
            CPPUNIT_FAIL( err.c_str() );
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
            CPPUNIT_ASSERT( tg.get_array()->length() == arr->length() );

            dods_byte fdb[tg.get_array()->length() * sizeof(dods_byte)];
            tg.get_array()->value(fdb);
            CPPUNIT_ASSERT( !memcmp( (void *)fdb, (void *)db, tg.get_array()->length() * sizeof( dods_byte ) ) );

            // Should test the map values here, but skip that for now...
        }
        catch( Error &e ) {
            string err = "failed:" + e.get_error_message();
            CPPUNIT_FAIL( err.c_str() );
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
        catch( Error &e ) {
            string err = "failed:" + e.get_error_message();
            CPPUNIT_FAIL( err.c_str() );
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
            CPPUNIT_ASSERT( num_rows == 4 );
            for (unsigned int i = 0; i < num_rows; i++) {
                BaseTypeRow *row = seq.row_value(i);
                CPPUNIT_ASSERT( row );
                CPPUNIT_ASSERT( row->size() == 3 );
                Float64 *f64_p = dynamic_cast<Float64 *> ((*row)[0]);
                CPPUNIT_ASSERT( f64_p );
                CPPUNIT_ASSERT( f64_p->value() == f64->value() );

                Array *arr_p = dynamic_cast<Array *> ((*row)[1]);
                CPPUNIT_ASSERT( arr_p );
                arr_p->value(fdb);
                CPPUNIT_ASSERT( arr_p->length() == arr->length() );
                CPPUNIT_ASSERT( !memcmp( (void *)fdb, (void *)db,
                                arr_p->length() * sizeof( dods_byte ) ) );
                Sequence *seq_p = dynamic_cast<Sequence *> ((*row)[2]);
                CPPUNIT_ASSERT( seq_p );
                unsigned int num_rows_sub = seq_p->number_of_rows();
                CPPUNIT_ASSERT( num_rows == 4 );
                for (unsigned int j = 0; j < num_rows_sub; j++) {
                    BaseTypeRow *row_sub = seq_p->row_value(j);
                    CPPUNIT_ASSERT( row_sub );
                    CPPUNIT_ASSERT( row_sub->size() == 2 );
                    UInt16 *ui16_p = dynamic_cast<UInt16 *> ((*row_sub)[0]);
                    CPPUNIT_ASSERT( ui16_p );
                    CPPUNIT_ASSERT( ui16_p->value() == ui16->value() );
                    Url *url_p = dynamic_cast<Url *> ((*row_sub)[1]);
                    CPPUNIT_ASSERT( url_p );
                    CPPUNIT_ASSERT( url_p->value() == url->value() );
                }
            }
        }
        catch( Error &e ) {
            string err = "failed:" + e.get_error_message();
            CPPUNIT_FAIL( err.c_str() );
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
        catch( Error &e ) {
            string err = "failed:" + e.get_error_message();
            CPPUNIT_FAIL( err.c_str() );
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
            CPPUNIT_ASSERT( fb.value() == b->value() );

            Int16 fi16("i16");
            fi16.deserialize(um, &dds, false);
            CPPUNIT_ASSERT( fi16.value() == i16->value() );

            Int32 fi32("i32");
            fi32.deserialize(um, &dds, false);
            CPPUNIT_ASSERT( fi32.value() == i32->value() );

            UInt16 fui16("ui16");
            fui16.deserialize(um, &dds, false);
            CPPUNIT_ASSERT( fui16.value() == ui16->value() );

            UInt32 fui32("ui32");
            fui32.deserialize(um, &dds, false);
            CPPUNIT_ASSERT( fui32.value() == ui32->value() );

            Float32 ff32("f32");
            ff32.deserialize(um, &dds, false);
            CPPUNIT_ASSERT( ff32.value() == f32->value() );

            Float64 ff64("f64");
            ff64.deserialize(um, &dds, false);
            CPPUNIT_ASSERT( ff64.value() == f64->value() );

            Str fstr("str");
            fstr.deserialize(um, &dds, false);
            DBG(cerr << "fstr.value(): " << fstr.value() << endl);
            CPPUNIT_ASSERT( fstr.value().find("Silly test string:") != string::npos );

            Url furl("url");
            furl.deserialize(um, &dds, false);
            CPPUNIT_ASSERT( furl.value() == url_value );
        }
        catch( Error &e ) {
            string err = "failed:" + e.get_error_message();
            CPPUNIT_FAIL( err.c_str() );
        }
    }

#if 0
    // Not currently run...
    void simple_types_fdistream_deserialize_test() {
        try
        {
#if 1
            int in = open( "st_test.strm", O_RDONLY );
            if (in < 0)
            throw Error("Could not open file.");
            fdistream sin( in );
#else
            FILE *in = fopen("st_test.strm", "r");
            if (!in)
            throw Error("Could not open the file");
            fpistream sin(in);
#endif
            XDRStreamUnMarshaller um( sin );

            Byte fb( "fb" );
            fb.deserialize( um, &dds, false );
            DBG(std::cerr << "expected: '" << b.value() << "' ; actual: '" << fb.value() << "'" << std::endl);
            CPPUNIT_ASSERT( fb.value() == b->value() );

            Int16 fi16( "i16" );
            fi16.deserialize( um, &dds, false );
            CPPUNIT_ASSERT( fi16.value() == i16->value() );

            Int32 fi32( "i32" );
            fi32.deserialize( um, &dds, false );
            CPPUNIT_ASSERT( fi32.value() == i32->value() );

            UInt16 fui16( "ui16" );
            fui16.deserialize( um, &dds, false );
            CPPUNIT_ASSERT( fui16.value() == ui16->value() );

            UInt32 fui32( "ui32" );
            fui32.deserialize( um, &dds, false );
            CPPUNIT_ASSERT( fui32.value() == ui32->value() );

            Float32 ff32( "f32" );
            ff32.deserialize( um, &dds, false );
            CPPUNIT_ASSERT( ff32.value() == f32->value() );

            Float64 ff64( "f64" );
            ff64.deserialize( um, &dds, false );
            CPPUNIT_ASSERT( ff64.value() == f64->value() );

            Str fstr( "str" );
            fstr.deserialize( um, &dds, false );
            DBG(cerr << "fstr.value(): " << fstr.value() << endl);
            CPPUNIT_ASSERT( fstr.value().find("Silly test string:") != string::npos );

            Url furl( "url" );
            furl.deserialize( um, &dds, false );
            CPPUNIT_ASSERT( furl.value() == url_value );
        }
        catch( Error &e )
        {
            string err = "failed:" + e.get_error_message();
            CPPUNIT_FAIL( err.c_str() );
        }
    }
#endif

    void array_stream_serialize_test()
    {
        try {
            ofstream strm("a_test.strm", ios::out | ios::trunc);
            XDRStreamMarshaller sm(strm);

            arr->serialize(eval, dds, sm, false);
        }
        catch( Error &e ) {
            string err = "failed:" + e.get_error_message();
            CPPUNIT_FAIL( err.c_str() );
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

            CPPUNIT_ASSERT( farr.length() == arr->length() );

            dods_byte fdb[arr->length() * sizeof(dods_byte)];
            farr.value(fdb);
            CPPUNIT_ASSERT( !memcmp( (void *)fdb, (void *)db, farr.length() * sizeof( dods_byte ) ) );
        }
        catch( Error &e ) {
            string err = "failed:" + e.get_error_message();
            CPPUNIT_FAIL( err.c_str() );
        }
    }

    void structure_stream_serialize_test()
    {
        try {
            ofstream strm("struct_test.strm", ios::out | ios::trunc);
            XDRStreamMarshaller sm(strm);
            s->serialize(eval, dds, sm, false);
        }
        catch( Error &e ) {
            string err = "failed:" + e.get_error_message();
            CPPUNIT_FAIL( err.c_str() );
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

            Int32 *fsi32_p = dynamic_cast<Int32 *> (fs.var("fsi32"));
            CPPUNIT_ASSERT( fsi32_p );
            CPPUNIT_ASSERT( fsi32_p->value() == i32->value() );

            Str *fsstr_p = dynamic_cast<Str *> (fs.var("fsstr"));
            CPPUNIT_ASSERT( fsstr_p );
            DBG(cerr << "fsstr_p->value(): " << fsstr_p->value() << endl);
            CPPUNIT_ASSERT( fsstr_p->value().find("Silly test string:") != string::npos );

            BaseType *bt = fs.var("fsab");
            CPPUNIT_ASSERT( bt );
            Array *fsarr_p = dynamic_cast<Array *> (bt);
            CPPUNIT_ASSERT( fsarr_p );
            CPPUNIT_ASSERT( fsarr_p->length() == arr->length() );
            dods_byte fdb[fsarr_p->length() * sizeof(dods_byte)];
            fsarr_p->value(fdb);
            CPPUNIT_ASSERT( !memcmp( (void *)fdb, (void *)db, fsarr_p->length() * sizeof( dods_byte ) ) );
        }
        catch( Error &e ) {
            string err = "failed:" + e.get_error_message();
            CPPUNIT_FAIL( err.c_str() );
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

            // strm.close() ;
        }
        catch( Error &e ) {
            string err = "failed:" + e.get_error_message();
            CPPUNIT_FAIL( err.c_str() );
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
            CPPUNIT_ASSERT( tg.get_array()->length() == arr->length() );

            dods_byte fdb[tg.get_array()->length() * sizeof(dods_byte)];
            tg.get_array()->value(fdb);
            CPPUNIT_ASSERT( !memcmp( (void *)fdb, (void *)db, tg.get_array()->length() * sizeof( dods_byte ) ) );

            // Should test the map values here, but skip that for now...

            // fclose( sf ) ;
        }
        catch( Error &e ) {
            string err = "failed:" + e.get_error_message();
            CPPUNIT_FAIL( err.c_str() );
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

            // strm.close() ;
        }
        catch( Error &e ) {
            string err = "failed:" + e.get_error_message();
            CPPUNIT_FAIL( err.c_str() );
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
            CPPUNIT_ASSERT( num_rows == 4 );
            for (unsigned int i = 0; i < num_rows; i++) {
                BaseTypeRow *row = seq.row_value(i);
                CPPUNIT_ASSERT( row );
                CPPUNIT_ASSERT( row->size() == 3 );
                Float64 *f64_p = dynamic_cast<Float64 *> ((*row)[0]);
                CPPUNIT_ASSERT( f64_p );
                CPPUNIT_ASSERT( f64_p->value() == f64->value() );
                Array *arr_p = dynamic_cast<Array *> ((*row)[1]);
                CPPUNIT_ASSERT( arr_p );
                arr_p->value(fdb);
                CPPUNIT_ASSERT( arr_p->length() == arr->length() );
                CPPUNIT_ASSERT( !memcmp( (void *)fdb, (void *)db,
                                arr_p->length() * sizeof( dods_byte ) ) );
                Sequence *seq_p = dynamic_cast<Sequence *> ((*row)[2]);
                CPPUNIT_ASSERT( seq_p );
                unsigned int num_rows_sub = seq_p->number_of_rows();
                CPPUNIT_ASSERT( num_rows == 4 );
                for (unsigned int j = 0; j < num_rows_sub; j++) {
                    BaseTypeRow *row_sub = seq_p->row_value(j);
                    CPPUNIT_ASSERT( row_sub );
                    CPPUNIT_ASSERT( row_sub->size() == 2 );
                    UInt16 *ui16_p = dynamic_cast<UInt16 *> ((*row_sub)[0]);
                    CPPUNIT_ASSERT( ui16_p );
                    CPPUNIT_ASSERT( ui16_p->value() == ui16->value() );
                    Url *url_p = dynamic_cast<Url *> ((*row_sub)[1]);
                    CPPUNIT_ASSERT( url_p );
                    CPPUNIT_ASSERT( url_p->value() == url->value() );
                }
            }

            // fclose( sf ) ;
        }
        catch( Error &e ) {
            string err = "failed:" + e.get_error_message();
            CPPUNIT_FAIL( err.c_str() );
        }
    }

#if CHECKSUMS
    // Check sum tests

    void simple_types_stream_serialize_checksum_test()
    {
        try {
            ofstream strm("st_test.strm", ios::out | ios::trunc);
            XDRStreamMarshaller sm(strm, true);

            sm.reset_checksum();
            b->serialize(eval, dds, sm, false);
            DBG(cerr << sm.get_checksum() << endl);
            CPPUNIT_ASSERT(sm.get_checksum() == "85e53271e14006f0265921d02d4d736cdc580b0b");

            sm.reset_checksum();
            i16->serialize(eval, dds, sm, false);
            DBG(cerr << sm.get_checksum() << endl);
            CPPUNIT_ASSERT(sm.get_checksum() == "fb7cc6f64453ad5a9926a1ba40955198004f6b31");

            sm.reset_checksum();
            i32->serialize(eval, dds, sm, false);
            DBG(cerr << sm.get_checksum() << endl);
            CPPUNIT_ASSERT(sm.get_checksum() == "d245351a7b5cf9244f146fa0763b4dd036245666");

            sm.reset_checksum();
            ui16->serialize(eval, dds, sm, false);
            DBG(cerr << sm.get_checksum() << endl);
            CPPUNIT_ASSERT(sm.get_checksum() == "f1e39479b3f84f40a6dca061ace8c910036cb867");

            sm.reset_checksum();
            ui32->serialize(eval, dds, sm, false);
            DBG(cerr << sm.get_checksum() << endl);
            CPPUNIT_ASSERT(sm.get_checksum() == "0d75307097b3f51d5b327f59e775165d4b1bfefa");

            sm.reset_checksum();
            f32->serialize(eval, dds, sm, false);
            DBG(cerr << sm.get_checksum() << endl);
            CPPUNIT_ASSERT(sm.get_checksum() == "16b84e7d293b3a53ceb97b9e50999b7ca2d17204");

            sm.reset_checksum();
            f64->serialize(eval, dds, sm, false);
            DBG(cerr << sm.get_checksum() << endl);
            CPPUNIT_ASSERT(sm.get_checksum() == "e8f339d9807f4998d8dc11e4c9d6f2ed05ca50cb");

            sm.reset_checksum();
            str->serialize(eval, dds, sm, false);
            string cs = sm.get_checksum();
            DBG(cerr << "cs: " << cs << endl);
            // This value changes with the number of times str is serialized
            // since the TestStr class returns different values for each call
            // to read().
            CPPUNIT_ASSERT(cs == "77b52cf559aec21b5bb06785693c915cdd7983c3");

            sm.reset_checksum();
            url->serialize(eval, dds, sm, false);
            cs = sm.get_checksum();
            DBG(cerr << "cs: " << cs << endl);
            CPPUNIT_ASSERT(cs == "18c61893206349dfc1ee4d030cfa18f924d44571");
        }
        catch( Error &e ) {
            string err = "failed:" + e.get_error_message();
            CPPUNIT_FAIL( err.c_str() );
        }
    }

    void array_stream_serialize_checksum_test()
    {
        try {
            ofstream strm("a_test.strm", ios::out | ios::trunc);
            XDRStreamMarshaller sm(strm, true);

            sm.reset_checksum();
            arr->serialize(eval, dds, sm, false);
            string cs = sm.get_checksum();

            DBG(cerr << cs << endl);
            CPPUNIT_ASSERT(cs == "9f39fdfeaf3d34181b346e2eec26abe9d9cdde3a");
        }
        catch( Error &e ) {
            string err = "failed:" + e.get_error_message();
            CPPUNIT_FAIL( err.c_str() );
        }
    }

    void structure_stream_serialize_checksum_test()
    {
        try {
            ofstream strm("struct_test.strm", ios::out | ios::trunc);
            XDRStreamMarshaller sm(strm, true);
            sm.reset_checksum();
            s->serialize(eval, dds, sm, false);
            string cs = sm.get_checksum();

            DBG(cerr << cs << endl);
            CPPUNIT_ASSERT(cs == "9f39fdfeaf3d34181b346e2eec26abe9d9cdde3a");
        }
        catch( Error &e ) {
            string err = "failed:" + e.get_error_message();
            CPPUNIT_FAIL( err.c_str() );
        }
    }

    void grid_stream_serialize_checksum_test()
    {
        try {
            ofstream strm("g_test.strm", ios::out | ios::trunc);
            XDRStreamMarshaller sm(strm, true);

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

            sm.reset_checksum();
            tg.serialize(eval, dds, sm, false);
            string cs = sm.get_checksum();

            DBG(cerr << cs << endl);
            CPPUNIT_ASSERT(cs == "ed67de94237ec33d220d8fb75734c195d64d4794");
        }
        catch( Error &e ) {
            string err = "failed:" + e.get_error_message();
            CPPUNIT_FAIL( err.c_str() );
        }
    }

    // This test is broken because Sequence::serialize() does not
    // properly call the checksum methods.
    void sequence_stream_serialize_checksum_test()
    {
        try {
            ofstream strm("seq_test.strm", ios::out | ios::trunc);
            XDRStreamMarshaller sm(strm, true);

            TestSequence seq("seq");
            seq.add_var(f64);
            seq.add_var(arr);

            TestSequence seq2("seq2");
            seq2.add_var(ui16);
            seq2.add_var(url);
            seq.add_var(&seq2);

            seq.set_send_p(true);
            seq.set_leaf_sequence();

            sm.reset_checksum();
            seq.serialize(eval, dds, sm, false);
            string cs = sm.get_checksum();

            DBG(cerr << cs << endl);
            CPPUNIT_ASSERT(cs == "7b99e35c2fb361eb27f51aec30fc2a17ac8cda50");
        }
        catch( Error &e ) {
            string err = "failed:" + e.get_error_message();
            CPPUNIT_FAIL( err.c_str() );
        }
    }
#endif
};

CPPUNIT_TEST_SUITE_REGISTRATION( MarshallerTest ) ;

int main(int, char **)
{
    CppUnit::TextUi::TestRunner runner;
    CppUnit::TestFactoryRegistry &registry = CppUnit::TestFactoryRegistry::getRegistry();
    runner.addTest(registry.makeTest());
    runner.setOutputter(CppUnit::CompilerOutputter::defaultOutputter(&runner.result(), std::cerr));
    bool wasSuccessful = runner.run("", false);
    return wasSuccessful ? 0 : 1;
}

