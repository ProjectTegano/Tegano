#!/bin/sh
testname=`basename $0 ".tst"`				# name of the test
luascript=`echo $testname | sed 's/lua_//'`.lua
docin=multilingual					# input document name
docout=$docin						# output document name
testcmd="-c wolframe.conf run"				# command to execute by the test
testdata="
**file:wolframe.conf
LoadModules
{
	module ../../src/modules/cmdbind/lua/mod_command_lua
}
Processor
{
	cmdhandler
	{
		lua
		{
			program script.lua
			filter #FILTER#
		}
	}
}
**file:script.lua
`cat ../scripts/$luascript`"

csetsize=7
csetlist="UTF-8 UTF-16LE UTF-16BE UCS-2LE UCS-2BE UCS-4LE UCS-4BE"
. ./output_tst_textwolf.sh
csetsize=6
csetlist="UTF-8 UTF-16LE UTF-16BE UCS-2LE UCS-2BE UCS-4BE"
. ./output_tst_libxml2.sh
