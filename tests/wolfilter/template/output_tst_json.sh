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
# - docin		input document name
# - docout		output document name
# - dumpout		(optional) file to dump to expected output too
# - csetlist		list of character set encodings
# - csetsize		number of selected character set encodings
# - testdata		(optional) additionaly defined test data
#
TESTCMD="$testcmd"
TESTNAME="$testname"
INPUTFILTER="$inputfilter"
OUTPUTFILTER="$outputfilter"
PWD=`pwd`
topdir=`dirname $PWD/$0`"/../../../../"

filter="cjson"
if [ `echo $testcmd | grep -c -- '--config'` = 0 ]; then
	modpath="../../src/modules"
	testcmd="--module $modpath/filter/cjson/mod_filter_cjson --module $modpath/doctype/json/mod_doctype_json $testcmd"
fi

testname="$TESTNAME""_cjson"

if [ x"$csetsize" = "x" ]; then
	csetsize="2"
fi
cc=`echo $csetlist | sed 's/UTF-8//'`
if [ x"$cc" != x"$csetlist" ]; then
	csetselect="UTF-8 `cmd/SELECTS $testname $csetsize $cc`"
else
	csetselect="`cmd/SELECTS $testname $csetsize $cc`"
fi

for cset in $csetselect
do
	if test x"$inputfilter" = "x"; then
		inputfilter="$filter"
	fi
	if test x"$outputfilter" = "x"; then
		outputfilter="$filter"
	fi
	docformat="json"
	. ./output_tst.sh
done
testcmd="$TESTCMD"
testname="$TESTNAME"
inputfilter="$INPUTFILTER"
outputfilter="$OUTPUTFILTER"
