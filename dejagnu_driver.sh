#!/bin/sh

runtest --tool das-test --srcdir das-testsuite

runtest --tool dds-test --srcdir dds-testsuite

runtest --tool expr-test --srcdir expr-testsuite
