#!/bin/sh
TOP=../../../../
MAIN=$TOP/docs/examples/tutorial/
for example in 4; do

output="../`echo $0 | sed 's/template//' | sed 's/.tst$//'`.$example.tst"
rm -f $output
echo "Writing test file $output"

recode lat1..ibmpc >> $output <<!TEST
**
**requires:LIBXML2
**requires:LUA
**input
HELLO
RUN
!TEST
cat $MAIN/lua_script_$example.input.xml | ../cleanInput BOM EOLN >> $output
recode lat1..ibmpc >> $output <<!TEST

.
QUIT
**file:example_$example.lua
!TEST
cat $MAIN/lua_script_$example.lua >> $output
recode lat1..ibmpc >> $output <<!TEST
**config
proc {
script {
	cmd RUN
	path example_$example.lua
	main run
}}
**output
OK enter cmd
!TEST
cat $MAIN/lua_script_$example.output.xml | ../cleanInput BOM >> $output
recode lat1..ibmpc >> $output <<!TEST

.
OK
BYE
**end

!TEST
done
