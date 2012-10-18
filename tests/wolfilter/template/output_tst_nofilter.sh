#!/bin/sh

#
# THIS FILE IS NOT PART OF WOLFRAME !
#
# It is just used to generate test documents out of a description.
# It is only aimed to be used on our development platforms.
# Please do not execute this script unless you know what you do !
# Executing this script in the wrong context might invalidate your test collection.
#
# DESCRIPTION
# - Script called in the context of a test script (as ". output_tst_all.sh") with the following variables set
#
# VARIABLES
# - testname		name of the test
# - testcmd		command to execute by the test
# - testscripts		list of scripts of the test
# - docin		input document name
# - docout		output document name
# - dumpout		(optional) file to dump to expected output too
# - csetlist		list of character set encodings
# - testdata		(optional) additionaly defined test data
#
TESTCMD="$testcmd"
TESTNAME="$testname"
PWD=`pwd`
topdir=`dirname $PWD/$0`"/../../../../"

for cset in $csetlist
do
	inputfilter="xml"
	outputfilter="xml"
	. ./output_tst.sh
	echo "echo executing test $testname $cset" >> ../../testWolfilter.sh
	echo "cat $topdir/tests/wolfilter/template/doc/$docin.UTF-8.xml | sed 's/UTF-8/$cset/' | recode UTF-8..$cset | $topdir/wtest/cleanInput BOM EOLN | $topdir/src/wolfilter `echo --input-filter $inputfilter --output-filter $outputfilter $testcmd | sed "s@--form @--form $topdir/tests/wolfilter/scripts/@" | sed "s@--script @--script $topdir/tests/wolfilter/scripts/@" | sed "s@--module @--module $topdir/tests/wolfilter/modules/../@"` > $topdir/tests/temp/$docout.$cset.xml" >> ../../testWolfilter.sh
done
testcmd="$TESTCMD"
testname="$TESTNAME"
