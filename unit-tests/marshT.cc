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

using std::cerr ;
using std::cout ;
using std::endl ;
using std::ofstream ;

int test_variable_sleep_interval = 0; // Used in Test* classes for testing
				      // timeouts. 

class marshT : public CppUnit::TestFixture {

CPPUNIT_TEST_SUITE( marshT ) ;
CPPUNIT_TEST( marshT_test ) ;
CPPUNIT_TEST_SUITE_END( ) ;

public:
    void setUp()
    {
    }

    void tearDown() 
    {
    }

    void marshT_test()
    {
	TestByte b( "byte" ) ;
	TestInt16 i16( "i16" ) ;
	TestInt32 i32( "i32" ) ;
	TestUInt16 ui16( "ui16" ) ;
	TestUInt32 ui32( "ui32" ) ;
	TestFloat32 f32( "f32" ) ;
	TestFloat64 f64( "f64" ) ;
	TestStr str( "str" ) ;
	TestUrl url( "url" ) ;
	TestByte ab( "ab" ) ;

	TestArray arr( "arr", &ab ) ;
	arr.append_dim( 5, "dim1" ) ;
	arr.append_dim( 3, "dim2" ) ;

	TestStructure s( "s" ) ;
	s.add_var( &i32 ) ;
	s.add_var( &str ) ;
	Str *str_p = dynamic_cast<Str *>(s.var( "str" )) ;
	s.add_var( &arr ) ;
	s.set_send_p( true ) ;

	ConstraintEvaluator eval ;
	TestTypeFactory ttf ;
	DataDDS dds( &ttf, "dds" ) ;

	try
	{
	    cout << "serializing using XDRFileMarshaller" << endl ;
	    FILE *f = fopen( "test.file", "w" ) ;
	    XDRFileMarshaller fm( f ) ;
	    cout << " file byte" << endl ;
	    b.serialize( eval, dds, fm, false ) ;
	    cout << " file int16" << endl ;
	    i16.serialize( eval, dds, fm, false ) ;
	    cout << " file int32" << endl ;
	    i32.serialize( eval, dds, fm, false ) ;
	    cout << " file uint16" << endl ;
	    ui16.serialize( eval, dds, fm, false ) ;
	    cout << " file uint32" << endl ;
	    ui32.serialize( eval, dds, fm, false ) ;
	    cout << " file float32" << endl ;
	    f32.serialize( eval, dds, fm, false ) ;
	    cout << " file float64" << endl ;
	    f64.serialize( eval, dds, fm, false ) ;
	    cout << " file str" << endl ;
	    str.serialize( eval, dds, fm, false ) ;
	    cout << " file url" << endl ;
	    url.serialize( eval, dds, fm, false ) ;
	    cout << " file structure" << endl ;
	    s.serialize( eval, dds, fm, false ) ;
	    cout << " file array" << endl ;
	    arr.serialize( eval, dds, fm, false ) ;

	    cout << " file sequence" << endl ;
	    TestSequence seq( "seq" ) ;
	    seq.add_var( &f64 ) ;
	    seq.add_var( &arr ) ;
	    TestSequence seq2( "seq2" ) ;
	    seq2.add_var( &ui16 ) ;
	    seq2.add_var( &url ) ;
	    seq2.set_send_p( true ) ;
	    seq.add_var( &seq2 ) ;
	    seq.set_send_p( true ) ;
	    seq.set_leaf_sequence() ;
	    seq.serialize( eval, dds, fm, false ) ;

	    fclose( f ) ;
	    cout << "done serializing using XDRFileMarshaller" << endl ;
	}
	catch( Error &e )
	{
	    string err = "failed:" + e.get_error_message() ;
	    CPPUNIT_FAIL( err.c_str() ) ;
	}

	cout << s << endl ;

	try
	{
	    cout << "serializing using XDRStreamMarshaller" << endl ;
	    ofstream strm( "test.strm", ios::out|ios::trunc ) ;
	    XDRStreamMarshaller sm( strm ) ;
	    cout << " stream byte" << endl ;
	    b.serialize( eval, dds, sm, false ) ;
	    cout << " stream int16" << endl ;
	    i16.serialize( eval, dds, sm, false ) ;
	    cout << " stream int32" << endl ;
	    i32.serialize( eval, dds, sm, false ) ;
	    cout << " stream uint16" << endl ;
	    ui16.serialize( eval, dds, sm, false ) ;
	    cout << " stream uint32" << endl ;
	    ui32.serialize( eval, dds, sm, false ) ;
	    cout << " stream float32" << endl ;
	    f32.serialize( eval, dds, sm, false ) ;
	    cout << " stream float64" << endl ;
	    f64.serialize( eval, dds, sm, false ) ;
	    cout << " stream str" << endl ;
	    str.serialize( eval, dds, sm, false ) ;
	    cout << " stream url" << endl ;
	    url.serialize( eval, dds, sm, false ) ;
	    cout << " stream structure" << endl ;
	    s.serialize( eval, dds, sm, false ) ;
	    cout << " stream array" << endl ;
	    arr.serialize( eval, dds, sm, false ) ;

	    cout << " stream sequence" << endl ;
	    TestSequence seq( "seq" ) ;
	    seq.add_var( &f64 ) ;
	    seq.add_var( &arr ) ;
	    TestSequence seq2( "seq2" ) ;
	    seq2.add_var( &ui16 ) ;
	    seq2.add_var( &url ) ;
	    seq2.set_send_p( true ) ;
	    seq.add_var( &seq2 ) ;
	    seq.set_send_p( true ) ;
	    seq.set_leaf_sequence() ;
	    seq.serialize( eval, dds, sm, false ) ;

	    strm.close() ;
	    cout << "done serializing using XDRStreamMarshaller" << endl ;
	}
	catch( Error &e )
	{
	    string err = "failed:" + e.get_error_message() ;
	    CPPUNIT_FAIL( err.c_str() ) ;
	}

	// now read the values in and compare the with each other and the original values
	try
	{
	    cout << "deserializing XDRFileMarshaller built file" << endl ;
	    FILE *ff = fopen( "test.file", "r" ) ;
	    XDRFileUnMarshaller um( ff ) ;
	    cout << " file byte" << endl ;
	    Byte fb( "fb" ) ;
	    fb.deserialize( um, &dds, false ) ;
	    CPPUNIT_ASSERT( fb.value() == b.value() ) ;
	    cout << " file int16" << endl ;
	    Int16 fi16( "i16" ) ;
	    fi16.deserialize( um, &dds, false ) ;
	    CPPUNIT_ASSERT( fi16.value() == i16.value() ) ;
	    cout << " file int32" << endl ;
	    Int32 fi32( "i32" ) ;
	    fi32.deserialize( um, &dds, false ) ;
	    CPPUNIT_ASSERT( fi32.value() == i32.value() ) ;
	    cout << " file uint16" << endl ;
	    UInt16 fui16( "ui16" ) ;
	    fui16.deserialize( um, &dds, false ) ;
	    CPPUNIT_ASSERT( fui16.value() == ui16.value() ) ;
	    cout << " file uint32" << endl ;
	    UInt32 fui32( "ui32" ) ;
	    fui32.deserialize( um, &dds, false ) ;
	    CPPUNIT_ASSERT( fui32.value() == ui32.value() ) ;
	    cout << " file float32" << endl ;
	    Float32 ff32( "f32" ) ;
	    ff32.deserialize( um, &dds, false ) ;
	    CPPUNIT_ASSERT( ff32.value() == f32.value() ) ;
	    cout << " file float64" << endl ;
	    Float64 ff64( "f64" ) ;
	    ff64.deserialize( um, &dds, false ) ;
	    CPPUNIT_ASSERT( ff64.value() == f64.value() ) ;
	    cout << " file str" << endl ;
	    Str fstr( "str" ) ;
	    fstr.deserialize( um, &dds, false ) ;
	    CPPUNIT_ASSERT( fstr.value() == str.value() ) ;
	    cout << " file url" << endl ;
	    Url furl( "url" ) ;
	    furl.deserialize( um, &dds, false ) ;
	    CPPUNIT_ASSERT( furl.value() == url.value() ) ;

	    cout << " file structure" << endl ;
	    TestStructure fs( "fs" ) ;
	    TestInt32 fsi32( "fsi32" ) ;
	    fs.add_var( &fsi32 ) ;
	    TestStr fsstr( "fsstr" ) ;
	    fs.add_var( &fsstr ) ;
	    TestByte fsab( "fsab" ) ;
	    TestArray fsarr( "fsarr", &fsab ) ;
	    fsarr.append_dim( 5, "dim1" ) ;
	    fsarr.append_dim( 3, "dim2" ) ;
	    fs.add_var( &fsarr ) ;
	    fs.deserialize( um, &dds, false ) ;

	    Int32 *fsi32_p = dynamic_cast<Int32 *>(fs.var( "fsi32" )) ;
	    CPPUNIT_ASSERT( fsi32_p ) ;
	    CPPUNIT_ASSERT( fsi32_p->value() == i32.value() ) ;

	    Str *fsstr_p = dynamic_cast<Str *>(fs.var( "fsstr" )) ;
	    CPPUNIT_ASSERT( fsstr_p ) ;
	    CPPUNIT_ASSERT( fsstr_p->value() == str_p->value() ) ;

	    BaseType *bt = fs.var( "fsab" ) ;
	    CPPUNIT_ASSERT( bt ) ;
	    Array *fsarr_p = dynamic_cast<Array *>(bt) ;
	    CPPUNIT_ASSERT( fsarr_p ) ;
	    dods_byte fdb[fsarr_p->length() * sizeof(dods_byte)] ;
	    dods_byte db[arr.length() * sizeof(dods_byte)] ;
	    fsarr_p->value( fdb ) ;
	    arr.value( db ) ;
	    CPPUNIT_ASSERT( fsarr_p->length() == arr.length() ) ;
	    CPPUNIT_ASSERT( !memcmp( (void *)fdb, (void *)db, fsarr_p->length() * sizeof( dods_byte ) ) ) ;

	    cout << " file array" << endl ;
	    TestByte fab( "ab" ) ;
	    TestArray farr( "arr", &fab ) ;
	    farr.append_dim( 5, "dim1" ) ;
	    farr.append_dim( 3, "dim2" ) ;
	    farr.deserialize( um, &dds, false ) ;
	    farr.value( fdb ) ;
	    CPPUNIT_ASSERT( farr.length() == arr.length() ) ;
	    CPPUNIT_ASSERT( !memcmp( (void *)fdb, (void *)db, farr.length() * sizeof( dods_byte ) ) ) ;

	    cout << " file sequence" << endl ;
	    TestSequence seq( "seq" ) ;
	    seq.add_var( &f64 ) ;
	    seq.add_var( &arr ) ;
	    TestSequence seq2( "seq2" ) ;
	    seq2.add_var( &ui16 ) ;
	    seq2.add_var( &url ) ;
	    seq2.set_send_p( true ) ;
	    seq.add_var( &seq2 ) ;
	    seq.set_leaf_sequence() ;
	    seq.deserialize( um, &dds, false ) ;
	    unsigned int num_rows = seq.number_of_rows() ;
	    CPPUNIT_ASSERT( num_rows == 4 ) ;
	    for( unsigned int i = 0; i < num_rows; i++ )
	    {
		BaseTypeRow *row = seq.row_value( i ) ;
		CPPUNIT_ASSERT( row ) ;
		CPPUNIT_ASSERT( row->size() == 3 ) ;
		Float64 *f64_p = dynamic_cast<Float64 *>((*row)[0]) ;
		CPPUNIT_ASSERT( f64_p ) ;
		CPPUNIT_ASSERT( f64_p->value() == f64.value() ) ;
		Array *arr_p = dynamic_cast<Array *>((*row)[1]) ;
		CPPUNIT_ASSERT( arr_p ) ;
		arr_p->value( fdb ) ;
		CPPUNIT_ASSERT( arr_p->length() == arr.length() ) ;
		CPPUNIT_ASSERT( !memcmp( (void *)fdb, (void *)db, arr_p->length() * sizeof( dods_byte ) ) ) ;
		Sequence *seq_p = dynamic_cast<Sequence *>((*row)[2]) ;
		CPPUNIT_ASSERT( seq_p ) ;
		unsigned int num_rows_sub = seq_p->number_of_rows() ;
		CPPUNIT_ASSERT( num_rows == 4 ) ;
		for( unsigned int j = 0; j < num_rows_sub; j++ )
		{
		    BaseTypeRow *row_sub = seq_p->row_value( j ) ;
		    CPPUNIT_ASSERT( row_sub ) ;
		    CPPUNIT_ASSERT( row_sub->size() == 2 ) ;
		    UInt16 *ui16_p = dynamic_cast<UInt16 *>((*row_sub)[0]) ;
		    CPPUNIT_ASSERT( ui16_p ) ;
		    CPPUNIT_ASSERT( ui16_p->value() == ui16.value() ) ;
		    Url *url_p = dynamic_cast<Url *>((*row_sub)[1]) ;
		    CPPUNIT_ASSERT( url_p ) ;
		    CPPUNIT_ASSERT( url_p->value() == url.value() ) ;
		}
	    }

	    fclose( ff ) ;

	    cout << "done deserializing XDRFileMarshaller built file" << endl ;
	}
	catch( Error &e )
	{
	    string err = "failed:" + e.get_error_message() ;
	    CPPUNIT_FAIL( err.c_str() ) ;
	}

	try
	{
	    cout << "deserializing XDRStreamMarshaller built file" << endl ;
	    FILE *sf = fopen( "test.strm", "r" ) ;
	    XDRFileUnMarshaller um( sf ) ;
	    cout << " stream byte" << endl ;
	    Byte sb( "sb" ) ;
	    sb.deserialize( um, &dds, false ) ;
	    CPPUNIT_ASSERT( sb.value() == b.value() ) ;
	    cout << " stream int16" << endl ;
	    Int16 si16( "i16" ) ;
	    si16.deserialize( um, &dds, false ) ;
	    CPPUNIT_ASSERT( si16.value() == i16.value() ) ;
	    cout << " stream int32" << endl ;
	    Int32 si32( "i32" ) ;
	    si32.deserialize( um, &dds, false ) ;
	    CPPUNIT_ASSERT( si32.value() == i32.value() ) ;
	    cout << " stream uint16" << endl ;
	    UInt16 sui16( "ui16" ) ;
	    sui16.deserialize( um, &dds, false ) ;
	    CPPUNIT_ASSERT( sui16.value() == ui16.value() ) ;
	    cout << " stream uint32" << endl ;
	    UInt32 sui32( "ui32" ) ;
	    sui32.deserialize( um, &dds, false ) ;
	    CPPUNIT_ASSERT( sui32.value() == ui32.value() ) ;
	    cout << " stream float32" << endl ;
	    Float32 sf32( "f32" ) ;
	    sf32.deserialize( um, &dds, false ) ;
	    CPPUNIT_ASSERT( sf32.value() == f32.value() ) ;
	    cout << " stream float64" << endl ;
	    Float64 sf64( "f64" ) ;
	    sf64.deserialize( um, &dds, false ) ;
	    CPPUNIT_ASSERT( sf64.value() == f64.value() ) ;
	    cout << " stream str" << endl ;
	    Str sstr( "str" ) ;
	    sstr.deserialize( um, &dds, false ) ;
	    CPPUNIT_ASSERT( sstr.value() == str.value() ) ;
	    cout << " stream url" << endl ;
	    Url surl( "url" ) ;
	    surl.deserialize( um, &dds, false ) ;
	    CPPUNIT_ASSERT( surl.value() == url.value() ) ;

	    cout << " stream structure" << endl ;
	    TestStructure ss( "ss" ) ;
	    TestInt32 ssi32( "ssi32" ) ;
	    ss.add_var( &ssi32 ) ;
	    TestStr ssstr( "ssstr" ) ;
	    ss.add_var( &ssstr ) ;
	    TestByte ssab( "ssab" ) ;
	    TestArray ssarr( "ssarr", &ssab ) ;
	    ssarr.append_dim( 5, "dim1" ) ;
	    ssarr.append_dim( 3, "dim2" ) ;
	    ss.add_var( &ssarr ) ;
	    ss.deserialize( um, &dds, false ) ;

	    Int32 *ssi32_p = dynamic_cast<Int32 *>(ss.var( "ssi32" )) ;
	    CPPUNIT_ASSERT( ssi32_p ) ;
	    CPPUNIT_ASSERT( ssi32_p->value() == i32.value() ) ;

	    Str *ssstr_p = dynamic_cast<Str *>(ss.var( "ssstr" )) ;
	    CPPUNIT_ASSERT( ssstr_p ) ;
	    CPPUNIT_ASSERT( ssstr_p->value() == str_p->value() ) ;

	    BaseType *bt = ss.var( "ssab" ) ;
	    CPPUNIT_ASSERT( bt ) ;
	    Array *ssarr_p = dynamic_cast<Array *>(bt) ;
	    CPPUNIT_ASSERT( ssarr_p ) ;
	    dods_byte sdb[ssarr_p->length() * sizeof(dods_byte)] ;
	    dods_byte db[arr.length() * sizeof(dods_byte)] ;
	    ssarr_p->value( sdb ) ;
	    arr.value( db ) ;
	    CPPUNIT_ASSERT( ssarr_p->length() == arr.length() ) ;
	    CPPUNIT_ASSERT( !memcmp( (void *)sdb, (void *)db, ssarr_p->length() * sizeof( dods_byte ) ) ) ;

	    cout << " stream array" << endl ;
	    TestByte sab( "ab" ) ;
	    TestArray sarr( "arr", &sab ) ;
	    sarr.append_dim( 5, "dim1" ) ;
	    sarr.append_dim( 3, "dim2" ) ;
	    sarr.deserialize( um, &dds, false ) ;
	    sarr.value( sdb ) ;
	    CPPUNIT_ASSERT( sarr.length() == arr.length() ) ;
	    CPPUNIT_ASSERT( !memcmp( (void *)sdb, (void *)db, sarr.length() * sizeof( dods_byte ) ) ) ;

	    cout << " stream sequence" << endl ;
	    TestSequence seq( "seq" ) ;
	    seq.add_var( &f64 ) ;
	    seq.add_var( &arr ) ;
	    TestSequence seq2( "seq2" ) ;
	    seq2.add_var( &ui16 ) ;
	    seq2.add_var( &url ) ;
	    seq2.set_send_p( true ) ;
	    seq.add_var( &seq2 ) ;
	    seq.set_leaf_sequence() ;
	    seq.deserialize( um, &dds, false ) ;
	    unsigned int num_rows = seq.number_of_rows() ;
	    CPPUNIT_ASSERT( num_rows == 4 ) ;
	    for( unsigned int i = 0; i < num_rows; i++ )
	    {
		BaseTypeRow *row = seq.row_value( i ) ;
		CPPUNIT_ASSERT( row ) ;
		CPPUNIT_ASSERT( row->size() == 3 ) ;
		Float64 *f64_p = dynamic_cast<Float64 *>((*row)[0]) ;
		CPPUNIT_ASSERT( f64_p ) ;
		CPPUNIT_ASSERT( f64_p->value() == f64.value() ) ;
		Array *arr_p = dynamic_cast<Array *>((*row)[1]) ;
		CPPUNIT_ASSERT( arr_p ) ;
		arr_p->value( sdb ) ;
		CPPUNIT_ASSERT( arr_p->length() == arr.length() ) ;
		CPPUNIT_ASSERT( !memcmp( (void *)sdb, (void *)db, arr_p->length() * sizeof( dods_byte ) ) ) ;
		Sequence *seq_p = dynamic_cast<Sequence *>((*row)[2]) ;
		CPPUNIT_ASSERT( seq_p ) ;
		unsigned int num_rows_sub = seq_p->number_of_rows() ;
		CPPUNIT_ASSERT( num_rows == 4 ) ;
		for( unsigned int j = 0; j < num_rows_sub; j++ )
		{
		    BaseTypeRow *row_sub = seq_p->row_value( j ) ;
		    CPPUNIT_ASSERT( row_sub ) ;
		    CPPUNIT_ASSERT( row_sub->size() == 2 ) ;
		    UInt16 *ui16_p = dynamic_cast<UInt16 *>((*row_sub)[0]) ;
		    CPPUNIT_ASSERT( ui16_p ) ;
		    CPPUNIT_ASSERT( ui16_p->value() == ui16.value() ) ;
		    Url *url_p = dynamic_cast<Url *>((*row_sub)[1]) ;
		    CPPUNIT_ASSERT( url_p ) ;
		    CPPUNIT_ASSERT( url_p->value() == url.value() ) ;
		}
	    }

	    fclose( sf ) ;

	    cout << "done deserializing XDRStreamMarshaller built file" << endl ;
	}
	catch( Error &e )
	{
	    string err = "failed:" + e.get_error_message() ;
	    CPPUNIT_FAIL( err.c_str() ) ;
	}

	unlink( "test.file" ) ;
	unlink( "test.strm" ) ;
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION( marshT ) ;

int main(int, char **)
{
    CppUnit::TextUi::TestRunner runner ;
    CppUnit::TestFactoryRegistry &registry =
	CppUnit::TestFactoryRegistry::getRegistry() ;
    runner.addTest( registry.makeTest() ) ;
    runner.setOutputter( CppUnit::CompilerOutputter::defaultOutputter( 
                                                        &runner.result(),
                                                        std::cerr ) );
    bool wasSuccessful = runner.run( "", false ) ;
    return wasSuccessful ? 0 : 1;
}


