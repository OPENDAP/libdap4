#!/bin/sh
#
# depend is a script replacement for makedepend. It requires gcc.
# -s causes system includes to be added to the dependencies.
# -m <makefile name> causes <makefile name> to be used instead of `Makefile'.
# 
# jhrg 4/28/95
#
# $Id: depend.sh,v 1.4 1995/06/17 21:29:45 jimg Exp $

usage="depend [-s][-m <makefile name>] -- <compiler options> -- <files>"
CFLAGS=-MM
tmp=/usr/tmp/depend$$
makefile=Makefile

# read the command options

x=`getopt sm: "$@"`		# "$@" preserves quotes in the input

if [ $? != 0 ]			# $? is the exit status of 'getopt ...'
then
   echo "${usage}"
   exit 2
fi

# set -- $x sets the shell positional params $1, $2, ... to the tokens in $x
# (which were put there by `getopt ...`. The eval preserves any quotes in $x.

eval set -- $x

for c in "$@"
do
    case $c in
	-s) CFLAGS=-M; shift 1;;
	-m) makefile=$2; shift 2;;
        -\?) echo $usage; exit 2;;
	--) shift 1; break;;
    esac
done

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