**
**requires:LUA
**requires:CJSON
**requires:TEXTWOLF
**input
{ 
 	 " - d o c t y p e "   :   " t r e e s " , 
 	 " m t r e e " :   { 
 	 	 " - i d " :   " 1 " , 
 	 	 " n a m e " :   " e i n s " , 
 	 	 " n o d e " :   [ 
 	 	 	 { 
 	 	 	 	 " - i d " :   " 1 1 " , 
 	 	 	 	 " n a m e " :   " E i n s   e i n s " 
 	 	 	 } , 
 	 	 	 { 
 	 	 	 	 " - i d " :   " 1 2 " , 
 	 	 	 	 " n a m e " :   " E i n s   z w e i " , 
 	 	 	 	 " n o d e " :   [ { 
 	 	 	 	 	 " - i d " :   " 1 2 1 " , 
 	 	 	 	 	 " n a m e " :   " E i n s   z w e i   e i n s " 
 	 	 	 	 } ] 
 	 	 	 } , 
 	 	 	 { 
 	 	 	 	 " - i d " :   " 1 3 " , 
 	 	 	 	 " n a m e " :   " E i n s   d r e i " 
 	 	 	 } 
 	 	 ] 
 	 } , 
 	 " b t r e e " :   { 
 	 	 " - i d " :   " 1 " , 
 	 	 " n a m e " :   " e i n s " , 
 	 	 " l e f t " :   { 
 	 	 	 " - i d " :   " 1 1 " , 
 	 	 	 " n a m e " :   " E i n s   e i n s " 
 	 	 } , 
 	 	 " r i g h t " :   { 
 	 	 	 " - i d " :   " 1 2 " , 
 	 	 	 " n a m e " :   " E i n s   z w e i " , 
 	 	 	 " l e f t " :   { 
 	 	 	 	 " - i d " :   " 1 2 1 " , 
 	 	 	 	 " n a m e " :   " E i n s   z w e i   e i n s " 
 	 	 	 } 
 	 	 } 
 	 } 
 } **config
--input-filter cjson --output-filter cjson --module ../../src/modules/filter/cjson/mod_filter_cjson --module ../../src/modules/doctype/json/mod_doctype_json -c wolframe.conf run

**file:wolframe.conf
LoadModules
{
	module ../../src/modules/cmdbind/lua/mod_command_lua
	module ../../src/modules/ddlcompiler/simpleform/mod_ddlcompiler_simpleform
	module ../../src/modules/normalize/number/mod_normalize_number
	module ../../src/modules/normalize/string/mod_normalize_string
	module ../../src/modules/datatype/bcdnumber/mod_datatype_bcdnumber
}
Processor
{
	program		normalize.wnmp
	program		form.sfrm
	cmdhandler
	{
		lua
		{
			program script.lua
			filter cjson
		}
	}
}
**file:normalize.wnmp
int=integer;
uint=unsigned;
float=floatingpoint ;
currency= bigfxp(   2  );
percent_1=bigfxp(  2  );
normname =convdia,ucname;
**file:form.sfrm
STRUCT MulTreeNode
{
	id		@int
	name		normname
	node		^MulTreeNode[]
}

STRUCT BinTreeNode
{
	id		@int
	name		normname
	left		^BinTreeNode
	right		^BinTreeNode
}

FORM trees
	-root root
{
	mtree	MulTreeNode
	btree	BinTreeNode
}
**file:script.lua
function printTable( tab)
	-- deterministic print of a table (since lua 5.2.1 table keys order is non deterministic)

	keys = {}
	for key,val in pairs( tab) do
		table.insert( keys, key)
	end
	table.sort( keys)

	for i,t in ipairs( keys) do
		local v = tab[ t]

		if type(v) == "table" then
			if v[ #v] then
				-- print array (keys are indices)
				for eidx,elem in ipairs( v) do
					output:opentag( t)
					if type(elem) == "table" then
						printTable( elem)
					else
						output:print( elem)
					end
					output:closetag()
				end
			else
				-- print table (keys are values)
				output:opentag( t)
				printTable( v)
				output:closetag()
			end
		else
			output:opentag( t)
			output:print( v)
			output:closetag()
		end
	end
end

function run()
	local doctype = input:doctype()
	output:as( provider.filter(), doctype)
	printTable( input:table())
end
**output
{ 
 	 " - d o c t y p e " : 	 " t r e e s " , 
 	 " b t r e e " : 	 { 
 	 	 " i d " : 	 " 1 " , 
 	 	 " l e f t " : 	 { 
 	 	 	 " i d " : 	 " 1 1 " , 
 	 	 	 " n a m e " : 	 " E I N S   E I N S " 
 	 	 } , 
 	 	 " n a m e " : 	 " E I N S " , 
 	 	 " r i g h t " : 	 { 
 	 	 	 " i d " : 	 " 1 2 " , 
 	 	 	 " l e f t " : 	 { 
 	 	 	 	 " i d " : 	 " 1 2 1 " , 
 	 	 	 	 " n a m e " : 	 " E I N S   Z W E I   E I N S " 
 	 	 	 } , 
 	 	 	 " n a m e " : 	 " E I N S   Z W E I " 
 	 	 } 
 	 } , 
 	 " m t r e e " : 	 { 
 	 	 " i d " : 	 " 1 " , 
 	 	 " n a m e " : 	 " E I N S " , 
 	 	 " n o d e " : 	 [ { 
 	 	 	 	 " i d " : 	 " 1 1 " , 
 	 	 	 	 " n a m e " : 	 " E I N S   E I N S " 
 	 	 	 } ,   { 
 	 	 	 	 " i d " : 	 " 1 2 " , 
 	 	 	 	 " n a m e " : 	 " E I N S   Z W E I " , 
 	 	 	 	 " n o d e " : 	 { 
 	 	 	 	 	 " i d " : 	 " 1 2 1 " , 
 	 	 	 	 	 " n a m e " : 	 " E I N S   Z W E I   E I N S " 
 	 	 	 	 } 
 	 	 	 } ,   { 
 	 	 	 	 " i d " : 	 " 1 3 " , 
 	 	 	 	 " n a m e " : 	 " E I N S   D R E I " 
 	 	 	 } ] 
 	 } 
 } 
 **end
