#!/bin/sh
#
# Upload the results of tests after running a build on Travis

LOG_FILE_TGZ=libdap4-autotest-${TRAVIS_JOB_NUMBER}-logs.tar.gz
if test xLIBDAP_BUILD = xmain -o xLIBDAP_BUILD = xasan -o xLIBDAP_BUILD = xdistcheck
then
	# shellcheck disable=SC2046
	tar -czf /tmp/${LOG_FILE_TGZ} `find . -name '*.log' -print`
	
	# using: 'test -z "$AWS_ACCESS_KEY_ID" || ...' keeps after_script from running
	# the aws cli for forked PRs (where secure env vars are null). jhrg 1/30/23
	
	test -z "$AWS_ACCESS_KEY_ID" || aws s3 cp /tmp/${LOG_FILE_TGZ} s3://opendap.travis.tests/
fi
