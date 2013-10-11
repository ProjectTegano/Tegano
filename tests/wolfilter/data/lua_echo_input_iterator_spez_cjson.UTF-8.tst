**
**requires:LUA
**requires:CJSON
**requires:TEXTWOLF
**input
{
  "doc": {
    "image": [
      { "-id": "1" },
      {
        "-id": "1",
        "-name": "troll"
      },
      { "-id": "2" },
      {
        "-id": "3",
        "#text": "X"
      }
    ]
  }
}**config
--input-filter cjson --output-filter cjson --module ../../src/modules/filter/cjson/mod_filter_cjson  --module ../../src/modules/cmdbind/lua/mod_command_lua --cmdprogram echo_input_iterator_spez.lua run
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
{
	"doc":	{
		"image":	[{
				"id":	"1"
			}, {
				"id":	"1",
				"name":	"troll"
			}, {
				"id":	"2"
			}, {
				"id":	"3",
				"#text":	"X"
			}]
	}
}
**end
