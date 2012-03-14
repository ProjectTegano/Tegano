--
--requires:DISABLED
--requires:LUA
--requires:XMLLITE
--input
HELLO
RUN
<?xml version="1.0" encoding="ISO-8859-1" standalone="yes"?>
<employee><firstname>Sara</firstname><surname>Sample</surname><phone>0123456789</phone><email>sara.sample@gmail.com</email><jobtitle>engineer</jobtitle></employee>
.
QUIT
--file:echo.lua
function run( )
	f = filter( "xml:xmllite")
	f.empty = false

	input:as( f)
	output:as( f)

	for c,t in input:get() do
		output:print( c, t)
	end
end
--config
script {
	name RUN
	path echo.lua
	main run
}
--output
OK enter cmd
<?xml version="1.0" encoding="ISO-8859-1" standalone="yes"?>
<employee><firstname>Sara</firstname><surname>Sample</surname><phone>0123456789</phone><email>sara.sample@gmail.com</email><jobtitle>engineer</jobtitle></employee>
.
OK
BYE
--end

