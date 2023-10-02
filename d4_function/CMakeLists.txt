
find_package(BISON 3.3 REQUIRED)
find_package(FLEX 2.6 REQUIRED)

BISON_TARGET(d4_function_parser d4_function_parser.yy ${CMAKE_CURRENT_BINARY_DIR}/d4_function.tab.cc)
FLEX_TARGET(d4_function_scanner d4_function_parser.ll ${CMAKE_CURRENT_BINARY_DIR}/lex.d4_function.cc)
ADD_FLEX_BISON_DEPENDENCY(d4_function_scanner d4_function_parser)

add_library(libd4_function_parser OBJECT "")
target_compile_features(libd4_function_parser PUBLIC cxx_std_14)

target_sources(libd4_function_parser
		PRIVATE
		${BISON_d4_function_parser_OUTPUTS}
		D4FunctionEvaluator.cc
		PUBLIC
		D4FunctionEvaluator.h
		D4FunctionScanner.h
)
