**
**requires:LUA
**requires:CJSON
**requires:TEXTWOLF
**input
 { 
 	 " a s s i g n m e n t " : 	 [ { 
 	 	 	 " t a s k " : 	 [ { 
 	 	 	 	 	 " t i t l e " : 	 " j o b   1 " , 
 	 	 	 	 	 " k e y " : 	 " A 1 2 3 " , 
 	 	 	 	 	 " c u s t o m e r n u m b e r " : 	 " 3 2 4 " 
 	 	 	 	 } ,   { 
 	 	 	 	 	 " t i t l e " : 	 " j o b   2 " , 
 	 	 	 	 	 " k e y " : 	 " V 4 5 6 " , 
 	 	 	 	 	 " c u s t o m e r n u m b e r " : 	 " 5 6 7 " 
 	 	 	 	 } ] , 
 	 	 	 " e m p l o y e e " : 	 { 
 	 	 	 	 " f i r s t n a m e " : 	 " J u l i a " , 
 	 	 	 	 " s u r n a m e " : 	 " T e g e l - S a c h e r " , 
 	 	 	 	 " p h o n e " : 	 " 0 9 8   7 6 5   4 3   2 1 " 
 	 	 	 } , 
 	 	 	 " i s s u e d a t e " : 	 " 1 3 . 5 . 2 0 0 6 " 
 	 	 } ,   { 
 	 	 	 " t a s k " : 	 [ { 
 	 	 	 	 	 " t i t l e " : 	 " j o b   3 " , 
 	 	 	 	 	 " k e y " : 	 " A 4 5 6 " , 
 	 	 	 	 	 " c u s t o m e r n u m b e r " : 	 " 5 6 7 " 
 	 	 	 	 } ,   { 
 	 	 	 	 	 " t i t l e " : 	 " j o b   4 " , 
 	 	 	 	 	 " k e y " : 	 " V 7 8 9 " , 
 	 	 	 	 	 " c u s t o m e r n u m b e r " : 	 " 8 9 0 " 
 	 	 	 	 } ] , 
 	 	 	 " e m p l o y e e " : 	 { 
 	 	 	 	 " f i r s t n a m e " : 	 " J a k o b " , 
 	 	 	 	 " s u r n a m e " : 	 " S t e g e l i n " , 
 	 	 	 	 " p h o n e " : 	 " 0 1 2   3 4 5   6 7   8 9 " 
 	 	 	 } , 
 	 	 	 " i s s u e d a t e " : 	 " 1 3 . 5 . 2 0 0 6 " 
 	 	 } ] 
 }**config
--input-filter cjson --output-filter cjson --module ../../src/modules/filter/cjson/mod_filter_cjson --module ../../src/modules/doctype/json/mod_doctype_json -c wolframe.conf run

**file:wolframe.conf
LoadModules
{
	module ../../src/modules/cmdbind/lua/mod_command_lua
	module ../wolfilter/modules/employee_assignment_convert/mod_employee_assignment_convert
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
	t = input:table()
	r = provider.formfunction("employee_assignment_convert")( t)
	printTable( r:table())
end
**output
 { 
 	 " a s s i g n m e n t " : 	 [ { 
 	 	 	 " e m p l o y e e " : 	 { 
 	 	 	 	 " f i r s t n a m e " : 	 " j u u l l i i a a " , 
 	 	 	 	 " p h o n e " : 	 " 9 0 1   2 3 4   5 6   7 8 " , 
 	 	 	 	 " s u r n a m e " : 	 " t e e g g e e l l - s a a c c h h e e r r " 
 	 	 	 } , 
 	 	 	 " i s s u e d a t e " : 	 " 8 6 4 7 9 9 3 " , 
 	 	 	 " t a s k " : 	 [ { 
 	 	 	 	 	 " c u s t o m e r n u m b e r " : 	 " 3 2 5 " , 
 	 	 	 	 	 " k e y " : 	 " a 8 7 6 " , 
 	 	 	 	 	 " t i t l e " : 	 " j j o o b b   8 " 
 	 	 	 	 } ,   { 
 	 	 	 	 	 " c u s t o m e r n u m b e r " : 	 " 5 6 8 " , 
 	 	 	 	 	 " k e y " : 	 " v 5 4 3 " , 
 	 	 	 	 	 " t i t l e " : 	 " j j o o b b   7 " 
 	 	 	 	 } ] 
 	 	 } ,   { 
 	 	 	 " e m p l o y e e " : 	 { 
 	 	 	 	 " f i r s t n a m e " : 	 " j a a k k o o b b " , 
 	 	 	 	 " p h o n e " : 	 " 9 8 7   6 5 4   3 2   1 0 " , 
 	 	 	 	 " s u r n a m e " : 	 " s t t e e g g e e l l i i n n " 
 	 	 	 } , 
 	 	 	 " i s s u e d a t e " : 	 " 8 6 4 7 9 9 3 " , 
 	 	 	 " t a s k " : 	 [ { 
 	 	 	 	 	 " c u s t o m e r n u m b e r " : 	 " 5 6 8 " , 
 	 	 	 	 	 " k e y " : 	 " a 5 4 3 " , 
 	 	 	 	 	 " t i t l e " : 	 " j j o o b b   6 " 
 	 	 	 	 } ,   { 
 	 	 	 	 	 " c u s t o m e r n u m b e r " : 	 " 8 9 1 " , 
 	 	 	 	 	 " k e y " : 	 " v 2 1 0 " , 
 	 	 	 	 	 " t i t l e " : 	 " j j o o b b   5 " 
 	 	 	 	 } ] 
 	 	 } ] 
 } 
**end
