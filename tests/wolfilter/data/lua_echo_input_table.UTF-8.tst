**
**requires:LUA
**input
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<assignmentlist><assignment><task><title>job 1</title><key>A123</key><customernumber>324</customernumber></task><task><title>job 2</title><key>V456</key><customernumber>567</customernumber></task><employee><firstname>Julia</firstname><surname>Tegel-Sacher</surname><phone>098 765 43 21</phone></employee><issuedate>13.5.2006</issuedate></assignment><assignment><task><title>job 4</title><key>V789</key><customernumber>890</customernumber></task><employee><firstname>Jakob</firstname><surname>Stegelin</surname><phone>012 345 67 89</phone></employee><issuedate>13.5.2006</issuedate></assignment></assignmentlist>**config
--input-filter xml:textwolf --output-filter xml:textwolf --script echo_input_table.lua run
**file: echo_input_table.lua
function run()
	t = input:table()
	output:print( t)
end
**output
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<assignmentlist><assignment><issuedate>13.5.2006</issuedate><employee><firstname>Julia</firstname><phone>098 765 43 21</phone><surname>Tegel-Sacher</surname></employee><task><customernumber>324</customernumber><key>A123</key><title>job 1</title></task><task><customernumber>567</customernumber><key>V456</key><title>job 2</title></task></assignment><assignment><issuedate>13.5.2006</issuedate><employee><firstname>Jakob</firstname><phone>012 345 67 89</phone><surname>Stegelin</surname></employee><task><customernumber>890</customernumber><key>V789</key><title>job 4</title></task></assignment></assignmentlist>
**end
