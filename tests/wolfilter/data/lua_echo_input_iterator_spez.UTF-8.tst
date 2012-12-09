**
**requires:LUA
**input
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<doc><image id="1"/><image id="1" name="troll"/><image id="2"></image><image id="3">X</image></doc>**config
--input-filter xml:textwolf --output-filter xml:textwolf --module ../../src/modules/filter/textwolf/mod_filter_textwolf  --module ../../src/modules/cmdbind/lua/mod_command_lua --program echo_input_iterator_spez.lua run
**file: echo_input_iterator_spez.lua

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
			process_image( scope( itr))
			output:closetag()
		else
			output:print( v, t)
		end
	end
end

**output
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<doc><image id="1"/><image id="1" name="troll"/><image id="2"/><image id="3">X</image></doc>
**end