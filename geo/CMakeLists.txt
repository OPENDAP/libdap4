
find_package(BISON 3.3 REQUIRED)
find_package(FLEX 2.6 REQUIRED)

BISON_TARGET(gse_parser gse.yy ${CMAKE_CURRENT_BINARY_DIR}/gse.tab.cc)
FLEX_TARGET(gse_scanner gse.ll ${CMAKE_CURRENT_BINARY_DIR}/lex.gse.cc)
ADD_FLEX_BISON_DEPENDENCY(gse_scanner gse_parser)

add_library(libgeodap OBJECT "")
target_compile_features(libgeodap PUBLIC cxx_std_14)

target_sources(libgeodap
		PRIVATE
		${BISON_gse_parser_OUTPUTS}
		GSEClause.cc
		grid_utils.cc
		gse_parser.h
		PUBLIC
		GSEClause.h
		grid_utils.h
)
