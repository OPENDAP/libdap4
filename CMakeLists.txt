# libdap4 CMakeLists.txt file for use with CLion. This is not intended to be
# a replacement for the autotools build. But it does provide information that
# CLion needs to navigate the Hyrax code. jhrg 12/5/19

cmake_minimum_required(VERSION 3.13)
project(libdap4)

add_library(libdap "")
target_compile_features(libdap PUBLIC cxx_std_14)

# FIXME add_library(gl "")

add_subdirectory(geo)

# These need their own CMakeLists.txt
# add_subdirectory(tests)
# add_subdirectory(unit-tests)

# https://cmake.org/cmake/help/latest/module/FindBISON.html
find_package(BISON 3.3 REQUIRED)
# https://cmake.org/cmake/help/latest/module/FindFLEX.html
find_package(FLEX 2.6 REQUIRED)

BISON_TARGET(dds_parser dds.yy ${CMAKE_CURRENT_BINARY_DIR}/dds.tab.cc)
FLEX_TARGET(dds_scanner dds.ll ${CMAKE_CURRENT_BINARY_DIR}/lex.dds.cc)
ADD_FLEX_BISON_DEPENDENCY(dds_scanner dds_parser)

BISON_TARGET(das_parser das.yy ${CMAKE_CURRENT_BINARY_DIR}/das.tab.cc)
FLEX_TARGET(das_scanner das.ll ${CMAKE_CURRENT_BINARY_DIR}/lex.das.cc)
ADD_FLEX_BISON_DEPENDENCY(das_scanner das_parser)

BISON_TARGET(ce_expr_parser ce_expr.yy ${CMAKE_CURRENT_BINARY_DIR}/ce_expr.tab.cc)
FLEX_TARGET(ce_expr_scanner ce_expr.ll ${CMAKE_CURRENT_BINARY_DIR}/lex.ce_expr.cc)
ADD_FLEX_BISON_DEPENDENCY(ce_expr_scanner ce_expr_parser)

BISON_TARGET(Error_parser Error.yy ${CMAKE_CURRENT_BINARY_DIR}/Error.tab.cc)
FLEX_TARGET(Error_scanner Error.ll ${CMAKE_CURRENT_BINARY_DIR}/lex.Error.cc)
ADD_FLEX_BISON_DEPENDENCY(Error_scanner Error_parser)

# add_executable(Foo main.cpp ${BISON_dds_parser_OUTPUTS})

if (EXISTS /usr/local/Cellar/openssl/1.0.2r)
include_directories(/usr/local/Cellar/openssl/1.0.2r/include/)
endif()

include_directories(/usr/local/include/)

# Where the hyrax project puts the dependencies' headers. We might be building
# standalone. jhrg 7/22/21
if (EXISTS ../build/deps/include)
include_directories(../build/deps/include)
endif()

include_directories(.)
include_directories(d4_ce)
include_directories(d4_functions)
include_directories(geo)
include_directories(gl)
include_directories(GNU)

include_directories(tests)
include_directories(unit-tests)

# target_link_libraries(Collector    PUBLIC  Ui    PRIVATE Algo Engine )
# target_link_libraries(MyApp PRIVATE Collector)
target_sources(libdap
		PRIVATE
		${BISON_dds_parser_OUTPUTS}
		${FLEX_dds_parser_OUTPUTS}
		${BISON_das_parser_OUTPUTS}
		${FLEX_das_parser_OUTPUTS}
		${BISON_ce_parser_parser_OUTPUTS}
		${FLEX_ce_parser_parser_OUTPUTS}
		${BISON_Error_parser_OUTPUTS}
		${FLEX_Error_parser_OUTPUTS}
		GNU/GNURegex.cc
		GNU/GetOpt.cc
		AISConnect.cc
		AISDatabaseParser.cc
		AISMerge.cc
		AISResources.cc
		Ancillary.cc
		Array.cc
		AttrTable.cc
		BaseType.cc
		BaseTypeFactory.cc
		Byte.cc
		Clause.cc
		Connect.cc
		ConstraintEvaluator.cc
		Constructor.cc
		D4AsyncUtil.cc
		D4Attributes.cc
		D4BaseTypeFactory.cc
		D4Connect.cc
		D4Dimensions.cc
		D4Enum.cc
		D4EnumDefs.cc
		D4FilterClause.cc
		D4Group.cc
		D4Maps.cc
		D4Opaque.cc
		D4ParserSax2.cc
		D4RValue.cc
		D4Sequence.cc
		D4StreamMarshaller.cc
		D4StreamUnMarshaller.cc
		DAPCache3.cc
		DAS.cc
		DDS.cc
		DDXParserSAX2.cc
		DMR.cc
		DODSFilter.cc
		DapIndent.cc
		DapXmlNamespaces.cc
		DataDDS.cc
		Error.cc
		Float32.cc
		Float64.cc
		Grid.cc
		HTTPCache.cc
		HTTPCacheTable.cc
		HTTPConnect.cc
		Int16.cc
		Int32.cc
		Int64.cc
		Int8.cc
		InternalErr.cc
		Keywords2.cc
		MarshallerThread.cc
		RCReader.cc
		RValue.cc
		ResponseTooBigErr.cc
		Sequence.cc
		ServerFunction.cc
		ServerFunctionsList.cc
		SignalHandler.cc
		Str.cc
		Structure.cc
		UInt16.cc
		UInt32.cc
		UInt64.cc
		Url.cc
		Vector.cc
		XDRFileMarshaller.cc
		XDRFileUnMarshaller.cc
		XDRStreamMarshaller.cc
		XDRStreamUnMarshaller.cc
		XDRUtils.cc
		XMLWriter.cc
		chunked_istream.cc
		chunked_ostream.cc
		error-test.cc
		escaping.cc
		fdiostream.cc
		getdap.cc
		getdap4.cc
		mime_util.cc
		parser-util.cc
		regex_test.cc
		util.cc
		util_mit.cc

		PUBLIC
		GNU/GNURegex.h
		GNU/GetOpt.h
		conf/arg-nonnull.h
		conf/c++defs.h
		conf/warn-on-use.h
		AISConnect.h
		AISDatabaseParser.h
		AISExceptions.h
		AISMerge.h
		AISResources.h
		AlarmHandler.h
		Ancillary.h
		Array.h
		AttrTable.h
		BaseType.h
		BaseTypeFactory.h
		Byte.h
		Clause.h
		Connect.h
		ConstraintEvaluator.h
		Constructor.h
		D4AsyncUtil.h
		D4AttributeType.h
		D4Attributes.h
		D4BaseTypeFactory.h
		D4Connect.h
		D4Dimensions.h
		D4Enum.h
		D4EnumDefs.h
		D4FilterClause.h
		D4Function.h
		D4Group.h
		D4Maps.h
		D4Opaque.h
		D4ParseError.h
		D4ParserSax2.h
		D4RValue.h
		D4Sequence.h
		D4StreamMarshaller.h
		D4StreamUnMarshaller.h
		DAPCache3.h
		DAS.h
		DDS.h
		DDXExceptions.h
		DDXParserSAX2.h
		DMR.h
		DODSFilter.h
		DapIndent.h
		DapObj.h
		DapXmlNamespaces.h
		DataDDS.h
		EncodingType.h
		Error.h
		EventHandler.h
		Float32.h
		Float64.h
		Grid.h
		HTTPCache.h
		HTTPCacheDisconnectedMode.h
		HTTPCacheInterruptHandler.h
		HTTPCacheMacros.h
		HTTPCacheResponse.h
		HTTPCacheTable.h
		HTTPConnect.h
		HTTPResponse.h
		Int16.h
		Int32.h
		Int64.h
		Int8.h
		InternalErr.h
		Keywords2.h
		Marshaller.h
		MarshallerThread.h
		ObjectType.h
		Operators.h
		PipeResponse.h
		RCReader.h
		RValue.h
		Resource.h
		Response.h
		ResponseTooBigErr.h
		Sequence.h
		ServerFunction.h
		ServerFunctionsList.h
		SignalHandler.h
		SignalHandlerRegisteredErr.h
		StdinResponse.h
		Str.h
		Structure.h
		Type.h
		UInt16.h
		UInt32.h
		UInt64.h
		UnMarshaller.h
		Url.h
		Vector.h
		XDRFileMarshaller.h
		XDRFileUnMarshaller.h
		XDRStreamMarshaller.h
		XDRStreamUnMarshaller.h
		XDRUtils.h
		XMLWriter.h
		ce_parser.h
		cgi_util.h
		chunked_istream.h
		chunked_ostream.h
		chunked_stream.h
		config.h
		config_dap.h
		crc.h
		debug.h
		dods-datatypes-config.h
		dods-datatypes-static.h
		dods-datatypes.h
		dods-limits.h
		escaping.h
		expr.h
		fdiostream.h
		media_types.h
		mime_util.h
		parser-util.h
		parser.h
		util.h
		util_mit.h
		xdr-datatypes-config.h
		xdr-datatypes-static.h
		xdr-datatypes.h
)
