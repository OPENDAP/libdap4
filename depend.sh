#!/bin/sh
#
# depend is a script replacement for makedepend. It requires gcc.
# -s causes system includes to be added to the dependencies.
# -b <extension> causes <extension> to be used instead of `bak' when naming
#    the backup coy of the Makefile.
# -m <makefile name> causes depend to use <makefile name> instead of
#    `Makefile'. 
# -n Don't actually make the dependencies, just print out what would have been 
#    done. 
# jhrg 4/28/95
#
# Added -b option. Added `-E' to CFLAGS.
# jhrg 3/5/96
#
# $Id: depend.sh,v 1.7 1996/11/13 23:39:07 jimg Exp $

usage="depend [-s][-b <ext>][-m <makefile name>] -- <compiler options> -- <files>"
CFLAGS="-E -MM"
tmp=/usr/tmp/depend$$
makefile=Makefile
bak=bak
doit=1

# read the command options

x=`getopt sb:m:n "$@"`		# "$@" preserves quotes in the input

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
	-s) CFLAGS="-E -M"; shift 1;;
	-m) makefile=$2; shift 2;;
	-b) bak=$2; shift 2;;
	-n) doit=0; shift 1;;
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

if test $doit = "1"
then

# build the new Makefile in a tmp directory using the existing makefile:
# first copy everything upto the dependencies using awk, then compoute and
# append the dependencies of the files (which are the remaining arguments).

awk 'BEGIN {found = 0}
/DO NOT DELETE/ {found = 1; print $0}
found != 1 {print $0}
found == 1 {exit}' $makefile > $tmp

gcc $CFLAGS $* >> $tmp

mv $makefile ${makefile}.${bak}	# backup the current Makefile 

mv $tmp $makefile

else

echo "gcc $CFLAGS $* >> $tmp"
echo "mv $makefile ${makefile}.${bak}"
echo "mv $tmp $makefile"

fi

exit 0
