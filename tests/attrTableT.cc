#ifdef __GNUG__
#pragma implementation
#endif

#include <iostream>
#include "AttrTable.h"
#include "Pix.h"

int
main( int argc, char **argv )
{
    AttrTable at ;

    cout << endl << __LINE__ << " **** get the size, should be 0" << endl ;
    unsigned int at_size = at.get_size() ;
    if( at_size == 0 )
    {
	cout << "size = " << at_size << " - PASS" << endl ;
    } else {
	cerr << "size = " << at_size << " - FAIL" << endl ;
    }

    cout << endl << __LINE__ << " **** get the name, should be empty" << endl ;
    string at_name = at.get_name() ;
    if( at_name == "" )
    {
	cout << "name = " << at_name << " - PASS" << endl ;
    } else {
	cerr << "name = " << at_name << " - FAIL" << endl ;
    }

    cout << endl << __LINE__ << " **** set and get the name, should be set" << endl ;
    at.set_name( "My Attributes" ) ;
    at_name = at.get_name() ;
    if( at_name == "My Attributes" )
    {
	cout << "name = " << at_name << " - PASS" << endl ;
    } else {
	cerr << "name = " << at_name << " - FAIL" << endl ;
    }

    cout << endl << __LINE__ << " **** find container dummy_container" << endl ;
    AttrTable *container = at.find_container( "dummy_container" ) ;
    if( container )
    {
	cerr << "found container \"dummy_container\" - FAIL" << endl ;
    } else {
	cout << "did not find container \"dummy_container\" - PASS" << endl ;
    }

    cout << endl << __LINE__ << " **** find dummy_attr using Pix" << endl ;
    AttrTable *dummy_at = 0 ;
    Pix p = at.find( "dummy_attr", &dummy_at ) ;
    if( p )
    {
	cerr << "found attribute \"dummy_attr\" - FAIL" << endl ;
    } else {
	cout << "did not find attribute \"dummy_attr\" - PASS" << endl ;
    }

    cout << endl << __LINE__ << " **** find dummy_attr using iter" << endl ;
    AttrTable::Attr_citer iter ;
    at.find( "dummy_attr", &dummy_at, iter ) ;
    if( iter != at.attr_end() )
    {
	cerr << "found attribute \"dummy_attr\" - FAIL" << endl ;
    } else {
	cout << "did not find attribute \"dummy_attr\" - PASS" << endl ;
    }

    cout << endl << __LINE__ << " **** add some attributes, one value each" << endl ;
    string attr_name = "attr1" ;
    string attr_type = "string" ;
    string attr_value = "attr1Value1" ;
    at.append_attr( attr_name, attr_type, attr_value ) ;

    attr_name = "attr2" ;
    attr_type = "string" ;
    attr_value = "attr2Value1" ;
    at.append_attr( attr_name, attr_type, attr_value ) ;

    attr_name = "attr3" ;
    attr_type = "string" ;
    attr_value = "attr3Value1" ;
    at.append_attr( attr_name, attr_type, attr_value ) ;

    at.append_attr( "attr4", "string", "attr4Value1" ) ;

    cout << endl << __LINE__ << " **** get the size, should be 4" << endl ;
    at_size = at.get_size() ;
    if( at_size == 4 )
    {
	cout << "size = " << at_size << " - PASS" << endl ;
    } else {
	cerr << "size = " << at_size << " - FAIL" << endl ;
    }

    cout << endl << __LINE__ << " **** print the AttrTable" << endl ;
    at.print( cout ) ;

    cout << endl << __LINE__ << " **** find attr2 using Pix" << endl ;
    p = at.find( "attr2", &dummy_at ) ;
    if( p )
    {
	cout << "found attribute \"attr2\" - PASS" << endl ;
    } else {
	cerr << "did not find attribute \"attr2\" - FAIL" << endl ;
    }

    cout << endl << __LINE__ << " **** find dummy_attr using Pix" << endl ;
    p = at.find( "dummy_attr", &dummy_at ) ;
    if( p )
    {
	cerr << "found attribute \"dummy_attr\" - FAIL" << endl ;
    } else {
	cout << "did not find attribute \"dummy_attr\" - PASS" << endl ;
    }

    cout << endl << __LINE__ << " **** find attr3 using iterator" << endl ;
    iter = at.attr_end() ;
    at.find( "attr3", &dummy_at, iter ) ;
    if( iter != at.attr_end() )
    {
	cout << "found attribute " << (*iter)->name << " - PASS" << endl ;
    } else {
	cerr << "did not find attribute \"attr3\" - FAIL" << endl ;
    }

    cout << endl << __LINE__ << " **** find dummy_attr using iterator" << endl ;
    iter = at.attr_end() ;
    at.find( "dummy_attr", &dummy_at, iter ) ;
    if( iter != at.attr_end() )
    {
	cerr << "found attribute " << (*iter)->name << " - FAIL" << endl ;
    } else {
	cout << "did not find attribute \"dummy_attr\" - PASS" << endl ;
    }

    cout << endl << __LINE__ << " **** get type for attribute attr3" << endl ;
    attr_type = at.get_type( "attr3" ) ;
    if( attr_type == "String" )
    {
	cout << "type of attr3 is " << attr_type << " - PASS" << endl ;
    } else {
	cerr << "type of attr3 is " << attr_type << " - FAIL" << endl ;
    }

    cout << endl << __LINE__ << " **** get type enum for attribute attr3" << endl ;
    AttrType attr_type_enum = at.get_attr_type( "attr3" ) ;
    if( attr_type_enum == Attr_string )
    {
	cout << "type of attr3 is " << attr_type_enum << " - PASS" << endl ;
    } else {
	cerr << "type of attr3 is " << attr_type_enum << " - FAIL" << endl ;
    }

    cout << endl << __LINE__ << " **** get number of values for attr3" << endl ;
    unsigned int num_attrs = at.get_attr_num( "attr3" ) ;
    if( num_attrs == 1 )
    {
	cout << "number of values for attr3 is " << num_attrs << " - PASS"
	     << endl ;
    } else {
	cerr << "number of values for attr3 is " << num_attrs << " - FAIL"
	     << endl ;
    }

    cout << endl << __LINE__ << " **** get the value for attr3" << endl ;
    attr_value = at.get_attr( "attr3" ) ;
    if( attr_value == "attr3Value1" )
    {
	cout << "found attr3 value " << attr_value << " - PASS" << endl ;
    } else {
	cerr << "found attr3 value " << attr_value << " - FAIL" << endl ;
    }

    cout << endl << __LINE__ << " **** add 3 more values for attr3" << endl ;
    at.append_attr( "attr3", "string", "attr3Value2" ) ;
    at.append_attr( "attr3", "string", "attr3Value3" ) ;
    at.append_attr( "attr3", "string", "attr3Value4" ) ;

    cout << endl << __LINE__ << " **** get the first value for attr3" << endl ;
    attr_value = at.get_attr( "attr3" ) ;
    if( attr_value == "attr3Value1" )
    {
	cout << "found attr3 value " << attr_value << " - PASS" << endl ;
    } else {
	cerr << "found attr3 value " << attr_value << " - FAIL" << endl ;
    }

    cout << endl << __LINE__ << " **** get the value vector for attt3" << endl ;
    vector<string> sb ;
    sb.push_back( "attr3Value1" ) ;
    sb.push_back( "attr3Value2" ) ;
    sb.push_back( "attr3Value3" ) ;
    sb.push_back( "attr3Value4" ) ;

    typedef vector<string>::const_iterator str_citer ;
    typedef vector<string>::iterator str_iter ;

    vector<string> *values = at.get_attr_vector( "attr3" ) ;
    if( values )
    {
	str_citer vi = values->begin() ;
	str_citer sbi = sb.begin() ;
	for( ; vi != values->end() && sbi != sb.end(); vi++, sbi++ )
	{
	    if( (*vi) == (*sbi) )
	    {
		cout << "comparing " << (*vi) << " to " << (*sbi) << " - PASS"
		     << endl ;
	    } else {
		cerr << "comparing " << (*vi) << " to " << (*sbi) << " - FAIL"
		     << endl ;
	    }
	}
	if( vi == values->end() && sbi != sb.end() )
	{
	    cerr << "not enough values - FAIL" << endl ;
	}
	else if( vi != values->end() && sbi == sb.end() )
	{
	    cerr << "too many values - FAIL" << endl ;
	}
	else
	{
	    cout << "equal number of values found - PASS" << endl ;
	}
    } else {
	cerr << "No values found for attr3 - FAIL" << endl ;
    }

    vector<string> attrs ;
    attrs.push_back( "attr1" ) ;
    attrs.push_back( "attr2" ) ;
    attrs.push_back( "attr3" ) ;
    attrs.push_back( "attr4" ) ;

    cout << endl << __LINE__ << " **** iterate through using Pix" << endl ;
    Pix q = at.first_attr() ;
    str_citer ai = attrs.begin() ;
    for(; q && ai != attrs.end(); at.next_attr( q ), ai++ )
    {
	if( at.attr(q)->name == (*ai) )
	{
	    cout << "comparing " << at.attr(q)->name << " to " << (*ai) 
		 << " - PASS" << endl ;
	} else {
	    cerr << "comparing " << at.attr(q)->name << " to " << (*ai) 
		 << " - FAIL" << endl ;
	}
    }
    if( q && ai == attrs.end() )
    {
	cerr << "too many attributes - FAIL" << endl ;
    }
    else if( !q && ai != attrs.end() )
    {
	cerr << "not enough attributes - FAIL" << endl ;
    }
    else {
	cout << "enough attributes found - PASS" << endl ;
    }

    cout << endl << __LINE__ << " **** iterator through using iter" << endl ;
    AttrTable::Attr_citer i = at.attr_begin() ;
    ai = attrs.begin() ;
    for( ; i != at.attr_end() && ai != attrs.end(); i++, ai++ )
    {
	if( (*i)->name == (*ai) )
	{
	    cout << "comparing " << (*i)->name << " to " << (*ai) 
		 << " - PASS" << endl ;
	} else {
	    cerr << "comparing " << (*i)->name << " to " << (*ai) 
		 << " - FAIL" << endl ;
	}
    }
    if( i != at.attr_end() && ai == attrs.end() )
    {
	cerr << "too many attributes - FAIL" << endl ;
    }
    else if( i == at.attr_end() && ai != attrs.end() )
    {
	cerr << "not enough attributes - FAIL" << endl ;
    }
    else {
	cout << "enough attributes found - PASS" << endl ;
    }

    cout << endl << __LINE__ << " **** find attr3 using Pix" << endl ;
    p = at.find( "attr3", &dummy_at ) ;
    if( p )
    {
	cout << "found attribute \"attr3\" - PASS" << endl ;
    } else {
	cerr << "did not find attribute \"attr3\" - FAIL" << endl ;
    }

    cout << endl << __LINE__ << " **** find attr3 using iterator" << endl ;
    iter = at.attr_end() ;
    at.find( "attr3", &dummy_at, iter ) ;
    if( iter != at.attr_end() )
    {
	cout << "found attribute " << (*iter)->name << " - PASS" << endl ;
    } else {
	cerr << "did not find attribute \"attr3\" - FAIL" << endl ;
    }

    cout << endl << __LINE__ << " **** get the name of attr pointed to by Pix" << endl ;
    attr_name =  at.get_name( p ) ;
    if( attr_name == "attr3" )
    {
	cout << "attribute name = " << attr_name << " - PASS" << endl ;
    } else {
	cerr << "attribute name = " << attr_name << " - FAIL" << endl ;
    }

    cout << endl << __LINE__ << " **** get the name of attr pointed to by iter" << endl ;
    attr_name = at.get_name( iter ) ;
    if( attr_name == "attr3" )
    {
	cout << "attribute name = " << attr_name << " - PASS" << endl ;
    } else {
	cerr << "attribute name = " << attr_name << " - FAIL" << endl ;
    }

    cout << endl << __LINE__ << " **** is attr pointed to by Pix a container" << endl ;
    bool isit = at.is_container( p ) ;
    if( isit == false )
    {
	cout << "attribute is not a container - PASS" << endl ;
    } else {
	cerr << "attribute is a container - FAIL" << endl ;
    }

    cout << endl << __LINE__ << " **** is attr pointed to by iter a container" << endl ;
    isit = at.is_container( iter ) ;
    if( isit == false )
    {
	cout << "attribute is not a container - PASS" << endl ;
    } else {
	cerr << "attribute is a container - FAIL" << endl ;
    }

    cout << endl << __LINE__ << " **** get attr table pointed to by Pix" << endl ;
    dummy_at = at.get_attr_table( p ) ;
    if( dummy_at )
    {
	cerr << "attribute is a table - FAIL" << endl ;
    } else {
	cout << "attribute is not a table - PASS" << endl ;
    }

    cout << endl << __LINE__ << " **** get attr table pointed to by iter" << endl ;
    dummy_at = at.get_attr_table( iter ) ;
    if( dummy_at )
    {
	cerr << "attribute is a table - FAIL" << endl ;
    } else {
	cout << "attribute is not a table - PASS" << endl ;
    }

    cout << endl << __LINE__ << " **** get type of attr pointed to by Pix - str" << endl ;
    attr_type = at.get_type( p ) ;
    if( attr_type == "String" )
    {
	cout << "attribute type is " << attr_type << " - PASS" << endl ;
    } else {
	cerr << "attribute type is " << attr_type << " - FAIL" << endl ;
    }

    cout << endl << __LINE__ << " **** get type of attr pointed to by iter - str" << endl ;
    attr_type = at.get_type( iter ) ;
    if( attr_type == "String" )
    {
	cout << "attribute type is " << attr_type << " - PASS" << endl ;
    } else {
	cerr << "attribute type is " << attr_type << " - FAIL" << endl ;
    }

    cout << endl << __LINE__ << " **** get type enum of attr pointed to by Pix" << endl ;
    attr_type_enum = at.get_attr_type( p ) ;
    if( attr_type_enum == Attr_string )
    {
	cout << "type of attr3 is " << attr_type_enum << " - PASS" << endl ;
    } else {
	cerr << "type of attr3 is " << attr_type_enum << " - FAIL" << endl ;
    }

    cout << endl << __LINE__ << " **** get type enum of attr pointed to by iter" << endl ;
    attr_type_enum = at.get_attr_type( iter ) ;
    if( attr_type_enum == Attr_string )
    {
	cout << "type of attr3 is " << attr_type_enum << " - PASS" << endl ;
    } else {
	cerr << "type of attr3 is " << attr_type_enum << " - FAIL" << endl ;
    }

    cout << endl << __LINE__ << " **** get value of attr pointed to by Pix" << endl ;
    attr_value = at.get_attr( p ) ;
    if( attr_value == "attr3Value1" )
    {
	cout << "found attr3 value " << attr_value << " - PASS" << endl ;
    } else {
	cerr << "found attr3 value " << attr_value << " - FAIL" << endl ;
    }

    cout << endl << __LINE__ << " **** get value of attr pointed to by iter" << endl ;
    attr_value = at.get_attr( iter ) ;
    if( attr_value == "attr3Value1" )
    {
	cout << "found attr3 value " << attr_value << " - PASS" << endl ;
    } else {
	cerr << "found attr3 value " << attr_value << " - FAIL" << endl ;
    }

    cout << endl << __LINE__ << " **** get value 2 of attr pointed to by Pix" << endl ;
    attr_value = at.get_attr( p, 1 ) ;
    if( attr_value == "attr3Value2" )
    {
	cout << "found attr3 value " << attr_value << " - PASS" << endl ;
    } else {
	cerr << "found attr3 value " << attr_value << " - FAIL" << endl ;
    }

    cout << endl << __LINE__ << " **** get value 2 of attr pointed to by iter" << endl ;
    attr_value = at.get_attr( iter, 1 ) ;
    if( attr_value == "attr3Value2" )
    {
	cout << "found attr3 value " << attr_value << " - PASS" << endl ;
    } else {
	cerr << "found attr3 value " << attr_value << " - FAIL" << endl ;
    }

    cout << endl << __LINE__ << " **** get values of attr pointed to by Pix" << endl ;
    values = at.get_attr_vector( p ) ;
    if( values )
    {
	str_citer vi = values->begin() ;
	str_citer sbi = sb.begin() ;
	for( ; vi != values->end() && sbi != sb.end(); vi++, sbi++ )
	{
	    if( (*vi) == (*sbi) )
	    {
		cout << "comparing " << (*vi) << " to " << (*sbi) << " - PASS"
		     << endl ;
	    } else {
		cerr << "comparing " << (*vi) << " to " << (*sbi) << " - FAIL"
		     << endl ;
	    }
	}
	if( vi == values->end() && sbi != sb.end() )
	{
	    cerr << "not enough values - FAIL" << endl ;
	}
	else if( vi != values->end() && sbi == sb.end() )
	{
	    cerr << "too many values - FAIL" << endl ;
	}
	else
	{
	    cout << "equal number of values found - PASS" << endl ;
	}
    } else {
	cerr << "No values found for attr3 - FAIL" << endl ;
    }

    cout << endl << __LINE__ << " **** get values of attr pointed to by iter" << endl ;
    values = at.get_attr_vector( iter ) ;
    if( values )
    {
	str_citer vi = values->begin() ;
	str_citer sbi = sb.begin() ;
	for( ; vi != values->end() && sbi != sb.end(); vi++, sbi++ )
	{
	    if( (*vi) == (*sbi) )
	    {
		cout << "comparing " << (*vi) << " to " << (*sbi) << " - PASS"
		     << endl ;
	    } else {
		cerr << "comparing " << (*vi) << " to " << (*sbi) << " - FAIL"
		     << endl ;
	    }
	}
	if( vi == values->end() && sbi != sb.end() )
	{
	    cerr << "not enough values - FAIL" << endl ;
	}
	else if( vi != values->end() && sbi == sb.end() )
	{
	    cerr << "too many values - FAIL" << endl ;
	}
	else
	{
	    cout << "equal number of values found - PASS" << endl ;
	}
    } else {
	cerr << "No values found for attr3 - FAIL" << endl ;
    }

    cout << endl << __LINE__ << " **** delete the second value of attr3" << endl ;

    {
	str_iter sbi = sb.begin() ;
	sbi++ ;
	sb.erase( sbi ) ;
    }

    at.del_attr( "attr3", 1 ) ;
    values = at.get_attr_vector( iter ) ;
    if( values )
    {
	str_citer vi = values->begin() ;
	str_citer sbi = sb.begin() ;
	for( ; vi != values->end() && sbi != sb.end(); vi++, sbi++ )
	{
	    if( (*vi) == (*sbi) )
	    {
		cout << "comparing " << (*vi) << " to " << (*sbi) << " - PASS"
		     << endl ;
	    } else {
		cerr << "comparing " << (*vi) << " to " << (*sbi) << " - FAIL"
		     << endl ;
	    }
	}
	if( vi == values->end() && sbi != sb.end() )
	{
	    cerr << "not enough values - FAIL" << endl ;
	}
	else if( vi != values->end() && sbi == sb.end() )
	{
	    cerr << "too many values - FAIL" << endl ;
	}
	else
	{
	    cout << "equal number of values found - PASS" << endl ;
	}
    } else {
	cerr << "No values found for attr3 - FAIL" << endl ;
    }

    cout << endl << __LINE__ << " **** delete attr3 entirely" << endl ;
    at.del_attr( "attr3" ) ;
    cout << endl << __LINE__ << " **** find dummy_attr using Pix" << endl ;
    p = at.find( "attr3", &dummy_at ) ;
    if( p )
    {
	cerr << "found attribute \"attr3\" - FAIL" << endl ;
    } else {
	cout << "did not find attribute \"attr3\" - PASS" << endl ;
    }

    container = 0 ;
    cout << endl << __LINE__ << " **** try to add attr2 as container" << endl ;
    try
    {
	container = at.append_container( "attr2" ) ;
	cerr << "added successfully - FAIL" << endl ;
    }
    catch( Error &e )
    {
	cout << "caught error - PASS" << endl ;
    }
    if( container )
    {
	cerr << "new container created - FAIL" << endl ;
    } else {
	cout << "no container created - PASS" << endl ;
    }

    cout << endl << __LINE__ << " **** try to add attr5 as container" << endl ;
    try
    {
	container = at.append_container( "attr5" ) ;
	cout << "added successfully - PASS" << endl ;
    }
    catch( Error &e )
    {
	cerr << "caught error - FAIL" << endl ;
    }
    if( container )
    {
	cout << "new container created - PASS" << endl ;
	if( container->get_name() == "attr5" )
	{
	    cout << "new container named attr5 - PASS" << endl ;
	} else {
	    cerr << "new container named " << container->get_name() << " - FAIL"
		 << endl ;
	}
    } else {
	cerr << "no container created - FAIL" << endl ;
    }

    cout << endl << __LINE__ << " **** find the container named attr5" << endl ;
    container = at.find_container( "attr5" ) ;
    if( container )
    {
	string container_name = container->get_name() ;
	if( container_name == "attr5" )
	{
	    cout << "Found container with name = " << container_name
		 << " - PASS" << endl ;
	} else {
	    cerr << "Found container with name = " << container_name
		 << " - FAIL" << endl ;
	}
    } else {
	cerr << "Did not find container named attr5 - FAIL" << endl ;
    }

    cout << endl << __LINE__ << " **** find attr5 using Pix" << endl ;
    p = at.find( "attr5", &dummy_at ) ;
    if( p )
    {
	cout << "found attribute \"attr5\" - PASS" << endl ;
    } else {
	cerr << "did not find attribute \"attr5\" - FAIL" << endl ;
    }

    cout << endl << __LINE__ << " **** find attr5 using iterator" << endl ;
    iter = at.attr_end() ;
    at.find( "attr5", &dummy_at, iter ) ;
    if( iter != at.attr_end() )
    {
	cout << "found attribute " << (*iter)->name << " - PASS" << endl ;
    } else {
	cerr << "did not find attribute \"attr5\" - FAIL" << endl ;
    }

    cout << endl << __LINE__ << " **** get the name of attr pointed to by Pix" << endl ;
    attr_name =  at.get_name( p ) ;
    if( attr_name == "attr5" )
    {
	cout << "attribute name = " << attr_name << " - PASS" << endl ;
    } else {
	cerr << "attribute name = " << attr_name << " - FAIL" << endl ;
    }

    cout << endl << __LINE__ << " **** get the name of attr pointed to by iter" << endl ;
    attr_name =  at.get_name( iter ) ;
    if( attr_name == "attr5" )
    {
	cout << "attribute name = " << attr_name << " - PASS" << endl ;
    } else {
	cerr << "attribute name = " << attr_name << " - FAIL" << endl ;
    }

    cout << endl << __LINE__ << " **** is attr pointed to by Pix a container" << endl ;
    isit = at.is_container( p ) ;
    if( isit == true )
    {
	cout << "attribute is a container - PASS" << endl ;
    } else {
	cerr << "attribute is not a container - FAIL" << endl ;
    }

    cout << endl << __LINE__ << " **** is attr pointed to by iter a container" << endl ;
    isit = at.is_container( iter ) ;
    if( isit == true )
    {
	cout << "attribute is a container - PASS" << endl ;
    } else {
	cerr << "attribute is not a container - FAIL" << endl ;
    }

    cout << endl << __LINE__ << " **** get attr table pointed to by Pix" << endl ;
    container = at.get_attr_table( p ) ;
    if( container )
    {
	cout << "attribute is a table - PASS" << endl ;
    } else {
	cerr << "attribute is not a table - FAIL" << endl ;
    }

    cout << endl << __LINE__ << " **** get attr table pointed to by iter" << endl ;
    container = at.get_attr_table( iter ) ;
    if( container )
    {
	cout << "attribute is a table - PASS" << endl ;
    } else {
	cerr << "attribute is not a table - FAIL" << endl ;
    }

    cout << endl << __LINE__ << " **** get type of attr pointed to by Pix - str" << endl ;
    attr_type = at.get_type( p ) ;
    if( attr_type == "Container" )
    {
	cout << "attribute type is " << attr_type << " - PASS" << endl ;
    } else {
	cerr << "attribute type is " << attr_type << " - FAIL" << endl ;
    }

    cout << endl << __LINE__ << " **** get type of attr pointed to by iter - str" << endl ;
    attr_type = at.get_type( iter ) ;
    if( attr_type == "Container" )
    {
	cout << "attribute type is " << attr_type << " - PASS" << endl ;
    } else {
	cerr << "attribute type is " << attr_type << " - FAIL" << endl ;
    }

    cout << endl << __LINE__ << " **** get type enum of attr pointed to by Pix" << endl ;
    attr_type_enum = at.get_attr_type( p ) ;
    if( attr_type_enum == Attr_container )
    {
	cout << "attribute type is " << attr_type_enum << " - PASS" << endl ;
    } else {
	cerr << "attribute type is " << attr_type_enum << " - FAIL" << endl ;
    }

    cout << endl << __LINE__ << " **** get type enum of attr pointed to by iter" << endl ;
    attr_type_enum = at.get_attr_type( iter ) ;
    if( attr_type_enum == Attr_container )
    {
	cout << "attribute type is " << attr_type_enum << " - PASS" << endl ;
    } else {
	cerr << "attribute type is " << attr_type_enum << " - FAIL" << endl ;
    }

    cout << endl << __LINE__ << " **** add four attributes to attr5 contanier" << endl ;
    container->append_attr( "attr5-1", "string", "attr5.1Value1" ) ;
    container->append_attr( "attr5-2", "string", "attr5.2Value1" ) ;
    container->append_attr( "attr5-3", "string", "attr5.3Value1" ) ;
    container->append_attr( "attr5-4", "string", "attr5.4Value1" ) ;

    cout << endl << __LINE__ << " **** find attr5.attr5-2 using Pix" << endl ;
    p = at.find( "attr5.attr5-2", &dummy_at ) ;
    if( p )
    {
	cout << "found attribute " << container->attr(p)->name << " - PASS"
	     << endl ;
    } else {
	cerr << "did not find attribute \"attr5.attr5-2\" - FAIL" << endl ;
    }
    if( container == dummy_at )
    {
	cout << "containers are the same - PASS" << endl ;
    } else {
	cerr << "contaienrs are different - FAIL" << endl ;
    }

    cout << endl << __LINE__ << " **** find attr5.attr5-3 using iterator" << endl ;
    iter = at.attr_end() ;
    at.find( "attr5.attr5-3", &dummy_at, iter ) ;
    if( iter != at.attr_end() )
    {
	cout << "found attribute " << (*iter)->name << " - PASS" << endl ;
    } else {
	cerr << "did not find attribute \"attr5.attr5-3\" - FAIL" << endl ;
    }
    if( container == dummy_at )
    {
	cout << "containers are the same - PASS" << endl ;
    } else {
	cerr << "contaienrs are different - FAIL" << endl ;
    }

    cout << endl << __LINE__ << " **** print the AttrTable" << endl ;
    at.print( cout ) ;

    cout << endl << __LINE__ << " **** done" << endl ;
    return 0 ;
}

