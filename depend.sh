#!/bin/sh
#
# depend is a script replacement for makedepend. It requires gcc.
# -s causes system includes to be added to the dependencies.
# -m <makefile name> causes <makefile name> to be used instead of `Makefile'.
# 
# jhrg 4/28/95
#
# $Id: depend.sh,v 1.2 1995/06/12 19:05:11 jimg Exp $

usage="depend [-s][-m <makefile name>] -- <compiler options> -- <files>"
CFLAGS=-MM
tmp=/usr/tmp/depend$$
makefile=Makefile

# read the command options

while getopts sm: c
do
    case $c in
	s) CFLAGS=-M;;
	m) makefile=$OPTARG;;
        \?) echo $usage; exit 2;;
    esac
done
shift `expr $OPTIND - 1`	# shift past the first options and `--'

# accumulate the C compiler options into CFLAGS

while [ $1 != '--' ]
do
    CFLAGS="$CFLAGS $1"
    shift 1
done
shift 1				# shift past the second `--' to the files

# build the new Makefile in a tmp directory using the existing makefile:
# first copy everything upto the dependencies using awk, then compoute and
# append the dependencies of the files (which are the remaining arguments).

awk 'BEGIN {found = 0}
/DO NOT DELETE/ {found = 1; print $0}
found != 1 {print $0}
found == 1 {exit}' $makefile > $tmp

gcc $CFLAGS $* >> $tmp

mv $makefile makefile.bak	# backup the current Makefile 

mv $tmp $makefile

exit 0