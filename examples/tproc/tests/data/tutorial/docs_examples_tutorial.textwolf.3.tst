**
**requires:LUA
**input
HELLO
RUN
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<recipe><item amount="100g">chocolate</item><item amount="0.5l">milk</item></recipe>

.
QUIT
**file:example_3.lua

function run( )
        f = filter( "XML:textwolf")
        input:as(f)
        output:as(f)
        for c,t in input:get() do
                output:print( c, t)
        end
end
**config
provider
{
	cmdhandler
	{
		lua
		{
			script
			{
				name run
				file example_3.lua
			}
		}
	}
}
proc
{
	cmd run
}
**output
OK enter cmd
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<recipe><item amount="100g">chocolate</item><item amount="0.5l">milk</item></recipe>


.
OK
BYE
**end

