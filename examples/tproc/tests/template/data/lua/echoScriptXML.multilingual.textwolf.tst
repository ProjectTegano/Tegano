#!/bin/sh
for cset in 'UTF-8' 'UTF-16LE' 'UTF-16BE' 'UCS-2LE' 'UCS-2BE' 'UCS-4LE' 'UCS-4BE'
do
output="../`echo $0 | sed 's/template//' | sed 's/.tst$//'`.$cset.tst"
rm -f $output
echo "Writing test file $output"

recode lat1..ibmpc >> $output <<!TEST
--
--requires:LUA
--input
HELLO
RUN
!TEST
cat data/multilingual.UTF-8.xml | sed "s/UTF-8/$cset/" | recode UTF-8..$cset | ../cleanInput BOM EOLN >> $output
recode lat1..ibmpc >> $output <<!TEST

.
QUIT
--file:echo.lua
function run( )
	f = filter( "xml:textwolf")
	f.empty = false
	input:as( f)
	output:as( f)

	for c,t in input:get() do
		output:print( c, t)
	end
end
--config
script {
	cmd RUN
	path echo.lua
	main run
}
--output
OK enter cmd
!TEST
cat data/multilingual.UTF-8.xml | sed "s/UTF-8/$cset/" | recode UTF-8..$cset | ../cleanInput BOM EOLN >> $output
recode lat1..ibmpc >> $output <<!TEST

.
OK
BYE
--end

!TEST
done
