**
**requires:LUA
**requires:TEXTWOLF
**input
HELLO
RUN
Y�  f 
 y a o   m i n g 

.
QUIT
**file:example_1.lua

function run( )
        local f = provider.filter( "char", "UTF-16BE")
        input:as( f)
        output:as( f)
        for c in input:get() do
                output:print( c)
        end
end
**config
provider
{
	cmdhandler
	{
		lua
		{
			program example_1.lua
			filter char
		}
	}
}
proc
{
	cmd run
}
**output
OK enter cmd
Y�  f 
 y a o   m i n g 

.
OK
BYE
**end

