#!/bin/sh
testname=`basename $0 ".tst"`				# name of the test
luascript=`echo $testname | sed 's/lua_//'`.lua
docin=authorize.in					# input document name
docout=authorize.out					# output document name
testcmd="-c wolframe.conf run"				# command to execute by the test
testdata="
**file:wolframe.conf
LoadModules
{
	module `cmd/MODULE command_aamap`
	module `cmd/MODULE command_lua`
}
Processor
{
	program `cmd/PROGRAM basic.aamap`
	cmdhandler
	{
		lua
		{
			program authorize.lua
		}
	}
}
**file:basic.aamap
`cat program/basic.aamap`
**file:authorize.lua
`cat ../scripts/$luascript`"
. ./output_tst_all.sh
