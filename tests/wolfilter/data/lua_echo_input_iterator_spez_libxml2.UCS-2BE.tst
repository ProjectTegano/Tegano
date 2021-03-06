**
**requires:LUA
**requires:LIBXML2
**input
 < ? x m l   v e r s i o n = " 1 . 0 "   e n c o d i n g = " U C S - 2 B E "   s t a n d a l o n e = " y e s " ? > 
 < d o c > < i m a g e   i d = " 1 " / > < i m a g e   i d = " 1 "   n a m e = " t r o l l " / > < i m a g e   i d = " 2 " > < / i m a g e > < i m a g e   i d = " 3 " > X < / i m a g e > < / d o c >**config
--input-filter libxml2 --output-filter libxml2 --module ../../src/modules/filter/libxml2/mod_filter_libxml2 --module ../../src/modules/doctype/xml/mod_doctype_xml -c wolframe.conf run

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
			filter libxml2
		}
	}
}
**file:script.lua

local function process_image( itr)
	for v,t in itr do
		output:print( v, t)
	end
end

function run()
	local itr = input:get()
	for v,t in itr do
		if t == "image" then
			output:opentag( t)
			process_image( iterator.scope( itr))
			output:closetag()
		else
			output:print( v, t)
		end
	end
end
**output
 < ? x m l   v e r s i o n = " 1 . 0 "   e n c o d i n g = " U C S - 2 B E "   s t a n d a l o n e = " y e s " ? > 
 < d o c > < i m a g e   i d = " 1 " / > < i m a g e   i d = " 1 "   n a m e = " t r o l l " / > < i m a g e   i d = " 2 " / > < i m a g e   i d = " 3 " > X < / i m a g e > < / d o c > 
**end
