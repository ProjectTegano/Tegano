#!/bin/sh
testname=`basename $0 ".tst"`				# name of the test
opt=""
modpath="../../src/modules"				# module directory relative from tests/temp
opt="$opt --module $modpath/cmdbind/lua/mod_command_lua"
luascript=`echo $testname | sed 's/lua_//'`.lua
testscripts=$luascript					# list of scripts of the test
testcmd="$opt --program $luascript run"			# command to execute by the test
docin=employee_assignment_print_2			# input document name
docout=$testname					# output document name
. ./output_tst_all.sh
