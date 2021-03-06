**
**requires:LUA
**requires:CJSON
**requires:TEXTWOLF
**input
 { 
 	 " i s s u e " : 	 [ { 
 	 	 	 " d a t e " : 	 " 8 6 4 7 9 9 3 " , 
 	 	 	 " s u b t a s k " : 	 [ { 
 	 	 	 	 	 " k e y " : 	 " a 8 7 6 " , 
 	 	 	 	 	 " t i t l e " : 	 " A B C " 
 	 	 	 	 } ,   { 
 	 	 	 	 	 " k e y " : 	 " v 5 4 3 " , 
 	 	 	 	 	 " t i t l e " : 	 " X Y Z " 
 	 	 	 	 } ] , 
 	 	 	 " s y s t e m " : 	 { 
 	 	 	 	 " m a c h i n e " : 	 " o r i o n 7 " , 
 	 	 	 	 " O S " : 	 " W i n d o w s " , 
 	 	 	 	 " v e r s i o n " : 	 " 1 . 0 . 8 " 
 	 	 	 } 
 	 	 } ,   { 
 	 	 	 " d a t e " : 	 " 8 6 4 7 9 9 3 " , 
 	 	 	 " s u b t a s k " : 	 [ { 
 	 	 	 	 	 " k e y " : 	 " a 8 7 6 " , 
 	 	 	 	 	 " t i t l e " : 	 " A B C " 
 	 	 	 	 } ] , 
 	 	 	 " s y s t e m " : 	 { 
 	 	 	 	 " m a c h i n e " : 	 " o r i o n 7 " , 
 	 	 	 	 " O S " : 	 " W i n d o w s " , 
 	 	 	 	 " v e r s i o n " : 	 " 1 . 0 . 8 " 
 	 	 	 } 
 	 	 } ] 
 }**config
--input-filter cjson --output-filter cjson --module ../../src/modules/filter/cjson/mod_filter_cjson --module ../../src/modules/doctype/json/mod_doctype_json -c wolframe.conf run

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
			filter cjson
		}
	}
}
**file:script.lua
function run()
	t = input:table()

	output:as( input:doctype(), provider.filter())

	output:opentag( "issue")		-- issue
	output:opentag( "")			-- array element
		output:print( t[ "issue"][1]["date"], "date");
		output:opentag( "subtask")	-- subtask
			output:opentag( "")	-- array element
			output:print( t[ "issue"][1]["subtask"][1]["key"], "key");
			output:print( t[ "issue"][1]["subtask"][1]["title"], "title");
			output:closetag()	-- array element
			output:opentag( "")	-- array element
			output:print( t[ "issue"][1]["subtask"][2]["key"], "key");
			output:print( t[ "issue"][1]["subtask"][2]["title"], "title");
			output:closetag()	-- array element
		output:closetag()		-- end subtask
		output:opentag( "system")	-- system
			output:print( t[ "issue"][1]["system"]["machine"], "machine");
			output:print( t[ "issue"][1]["system"]["OS"], "OS");
			output:print( t[ "issue"][1]["system"]["version"], "version");
		output:closetag()		-- end system
	output:closetag()			-- end array element

	output:opentag( "")			-- array element
	output:print( t[ "issue"][2]["date"], "date");
	output:opentag( "subtask")		-- subtask
		output:opentag( "")		-- array element
		output:print( t[ "issue"][2]["subtask"][1]["key"], "key");
		output:print( t[ "issue"][2]["subtask"][1]["title"], "title");
		output:closetag()		-- end array element
	output:closetag()			-- end subtask
	output:opentag( "system")
		output:print( t[ "issue"][2]["system"]["machine"], "machine");
		output:print( t[ "issue"][2]["system"]["OS"], "OS");
		output:print( t[ "issue"][2]["system"]["version"], "version");
	output:closetag()			-- end system
	output:closetag()			-- end array element

	output:closetag()			-- end issue
end
**output
 { 
 	 " i s s u e " : 	 [ { 
 	 	 	 " d a t e " : 	 " 8 6 4 7 9 9 3 " , 
 	 	 	 " s u b t a s k " : 	 [ { 
 	 	 	 	 	 " k e y " : 	 " a 8 7 6 " , 
 	 	 	 	 	 " t i t l e " : 	 " A B C " 
 	 	 	 	 } ,   { 
 	 	 	 	 	 " k e y " : 	 " v 5 4 3 " , 
 	 	 	 	 	 " t i t l e " : 	 " X Y Z " 
 	 	 	 	 } ] , 
 	 	 	 " s y s t e m " : 	 { 
 	 	 	 	 " m a c h i n e " : 	 " o r i o n 7 " , 
 	 	 	 	 " O S " : 	 " W i n d o w s " , 
 	 	 	 	 " v e r s i o n " : 	 " 1 . 0 . 8 " 
 	 	 	 } 
 	 	 } ,   { 
 	 	 	 " d a t e " : 	 " 8 6 4 7 9 9 3 " , 
 	 	 	 " s u b t a s k " : 	 [ { 
 	 	 	 	 	 " k e y " : 	 " a 8 7 6 " , 
 	 	 	 	 	 " t i t l e " : 	 " A B C " 
 	 	 	 	 } ] , 
 	 	 	 " s y s t e m " : 	 { 
 	 	 	 	 " m a c h i n e " : 	 " o r i o n 7 " , 
 	 	 	 	 " O S " : 	 " W i n d o w s " , 
 	 	 	 	 " v e r s i o n " : 	 " 1 . 0 . 8 " 
 	 	 	 } 
 	 	 } ] 
 } 
**end
