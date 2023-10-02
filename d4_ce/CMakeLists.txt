
find_package(BISON 3.3 REQUIRED)
find_package(FLEX 2.6 REQUIRED)

BISON_TARGET(d4_ce_parser d4_ce_parser.yy ${CMAKE_CURRENT_BINARY_DIR}/d4_function.tab.cc)
FLEX_TARGET(d4_ce_scanner d4_ce_parser.ll ${CMAKE_CURRENT_BINARY_DIR}/lex.d4_function.cc)
ADD_FLEX_BISON_DEPENDENCY(d4_ce_scanner d4_ce_parser)

add_library(libd4_ce_parser OBJECT "")
target_compile_features(libd4_ce_parser PUBLIC cxx_std_14)

target_sources(libd4_ce_parser
		PRIVATE
		${BISON_d4_ce_parser_OUTPUTS}
		D4ConstraintEvaluator.cc
		PUBLIC
		D4CEScanner.h
		D4ConstraintEvaluator.h
)
