
# Build the code for test coverage analysis
# jhrg 11/17/20

.PHONY: coverage

if ENABLE_COVERAGE
AM_CXXFLAGS += --coverage -g -O0
AM_LDFLAGS += --coverage -g -O0

# gcov options for gcc/++
# -i (write .gcov files), -f (function summaries), -r (relative paths
# only - i.e., elide system functions), -m (demangle names)
#
# for the llvm compiler, gcov is called without any options

# GCOV_FLAGS is set by configure based on the kind of compiler

# Removed: $(GCOV_FLAGS) .libs/*.o
# @if test -n "$(coverage_subdirs)"; then \
# 	for d in $(coverage_subdirs); do \
# 		(cd $$d && $(MAKE) $(MFLAGS) coverage); \
# 	done; \
# fi

# coverage: 
#	-gcovr -r . $(GCOVR_FLAGS) > gcovr_report.txt

endif
