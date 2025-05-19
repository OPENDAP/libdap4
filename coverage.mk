
# Build the code for test coverage analysis
# jhrg 11/17/20

if ENABLE_COVERAGE
AM_CXXFLAGS += --coverage -g -O0
AM_LDFLAGS += --coverage -g -O0

# This target is for a local build that writes HTML pages.
# The travis build scan job writes am XML file that sonarqube
# can read. jhrg 5/18/25
# .PHONY: coverage
coverage:
	gcovr --config gcovr.config --html --html-details --output gcovr-output/ --sort uncovered-percent

endif
