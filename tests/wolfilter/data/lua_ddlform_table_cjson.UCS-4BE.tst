**
**requires:LUA
**requires:CJSON
**requires:TEXTWOLF
**input
   {   
   	   "   a   s   s   i   g   n   m   e   n   t   "   :   	   [   {   
   	   	   	   "   t   a   s   k   "   :   	   [   {   
   	   	   	   	   	   "   t   i   t   l   e   "   :   	   "   j   o   b       1   "   ,   
   	   	   	   	   	   "   k   e   y   "   :   	   "   A   1   2   3   "   ,   
   	   	   	   	   	   "   c   u   s   t   o   m   e   r   n   u   m   b   e   r   "   :   	   "   3   2   4   "   
   	   	   	   	   }   ,       {   
   	   	   	   	   	   "   t   i   t   l   e   "   :   	   "   j   o   b       2   "   ,   
   	   	   	   	   	   "   k   e   y   "   :   	   "   V   4   5   6   "   ,   
   	   	   	   	   	   "   c   u   s   t   o   m   e   r   n   u   m   b   e   r   "   :   	   "   5   6   7   "   
   	   	   	   	   }   ]   ,   
   	   	   	   "   e   m   p   l   o   y   e   e   "   :   	   {   
   	   	   	   	   "   f   i   r   s   t   n   a   m   e   "   :   	   "   J   u   l   i   a   "   ,   
   	   	   	   	   "   s   u   r   n   a   m   e   "   :   	   "   T   e   g   e   l   -   S   a   c   h   e   r   "   ,   
   	   	   	   	   "   p   h   o   n   e   "   :   	   "   0   9   8       7   6   5       4   3       2   1   "   
   	   	   	   }   ,   
   	   	   	   "   i   s   s   u   e   d   a   t   e   "   :   	   "   1   3   .   5   .   2   0   0   6   "   
   	   	   }   ,       {   
   	   	   	   "   t   a   s   k   "   :   	   [   {   
   	   	   	   	   	   "   t   i   t   l   e   "   :   	   "   j   o   b       3   "   ,   
   	   	   	   	   	   "   k   e   y   "   :   	   "   A   4   5   6   "   ,   
   	   	   	   	   	   "   c   u   s   t   o   m   e   r   n   u   m   b   e   r   "   :   	   "   5   6   7   "   
   	   	   	   	   }   ,       {   
   	   	   	   	   	   "   t   i   t   l   e   "   :   	   "   j   o   b       4   "   ,   
   	   	   	   	   	   "   k   e   y   "   :   	   "   V   7   8   9   "   ,   
   	   	   	   	   	   "   c   u   s   t   o   m   e   r   n   u   m   b   e   r   "   :   	   "   8   9   0   "   
   	   	   	   	   }   ]   ,   
   	   	   	   "   e   m   p   l   o   y   e   e   "   :   	   {   
   	   	   	   	   "   f   i   r   s   t   n   a   m   e   "   :   	   "   J   a   k   o   b   "   ,   
   	   	   	   	   "   s   u   r   n   a   m   e   "   :   	   "   S   t   e   g   e   l   i   n   "   ,   
   	   	   	   	   "   p   h   o   n   e   "   :   	   "   0   1   2       3   4   5       6   7       8   9   "   
   	   	   	   }   ,   
   	   	   	   "   i   s   s   u   e   d   a   t   e   "   :   	   "   1   3   .   5   .   2   0   0   6   "   
   	   	   }   ]   
   }**config
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
iNt=integer( 10);
uint=unsigneD(10 );
float=fLoatingpoint(10,  10);
currency=bIgfXp( 2);
percent_1=Bigfxp( 2);
**file:form.sfrm
FORM Employee
{
	firstname string
	surname string
	phone string
}

FORM employee_assignment_print
	-root assignmentlist
{
	assignment []
	{
		task []
		{
			title string
			key string
			customernumber int
		}
		employee Employee
		issuedate string
	}
}
**file:script.lua

function readTable( itr)
	local tab = {}
	-- function result maps a vector with one element to this element
	function result( t)
		if #t == 1 and t[1] then
			return t[1]
		else
			return t;
		end
	end

	for v,t in itr do
		if t then
			local val = v or readTable( itr)
			local e = tab[t]
			if e then
				if type(e) == "table" and e[#e] then
					table.insert( tab[t], val)
				else
					v = { tab[t], val }
					tab[t] = v
				end
			else
				tab[t] = val
			end
		elseif v then
			table.insert( tab, v)
		else
			return result( tab)
		end
	end
	return result( tab)
end

function printArray( t, a)
	for i,v in ipairs(a) do
		if type(v) == "table" then
			output:print( false, t)
			printTable(v)
			output:print()
		else
			output:print( v, t)
		end
	end
end

function printTable( tab)
	local iscontent = true
	--... we do not print attributes here
	--... iscontent=false -> print atomic values as attributes until the first non atomic value
	--... iscontent=true -> print all values as tag content
	keys = {}
	for key,val in pairs( tab) do
		table.insert( keys, key)
	end
	table.sort( keys)

	for i,t in ipairs( keys) do
		local v = tab[ t]

		if type(v) == "table" then
			iscontent = true
			if v[ #v] then
				printArray( t, v)
			else
				output:print( false, t)
				printTable( v)
				output:print()
			end
		elseif type(t) == "number" then
			output:print( v)
		elseif iscontent then
			output:print( false, t)
			output:print( v)
			output:print()
		else
			output:print( v, t)
		end
	end
end

function run()
	t = readTable( input:get())
	r = provider.form("employee_assignment_print")
	r:fill( t)
	printTable( r:table())
end
**output
   {   
   	   "   a   s   s   i   g   n   m   e   n   t   "   :   	   [   {   
   	   	   	   "   e   m   p   l   o   y   e   e   "   :   	   {   
   	   	   	   	   "   f   i   r   s   t   n   a   m   e   "   :   	   "   J   u   l   i   a   "   ,   
   	   	   	   	   "   p   h   o   n   e   "   :   	   "   0   9   8       7   6   5       4   3       2   1   "   ,   
   	   	   	   	   "   s   u   r   n   a   m   e   "   :   	   "   T   e   g   e   l   -   S   a   c   h   e   r   "   
   	   	   	   }   ,   
   	   	   	   "   i   s   s   u   e   d   a   t   e   "   :   	   "   1   3   .   5   .   2   0   0   6   "   ,   
   	   	   	   "   t   a   s   k   "   :   	   [   {   
   	   	   	   	   	   "   c   u   s   t   o   m   e   r   n   u   m   b   e   r   "   :   	   "   3   2   4   "   ,   
   	   	   	   	   	   "   k   e   y   "   :   	   "   A   1   2   3   "   ,   
   	   	   	   	   	   "   t   i   t   l   e   "   :   	   "   j   o   b       1   "   
   	   	   	   	   }   ,       {   
   	   	   	   	   	   "   c   u   s   t   o   m   e   r   n   u   m   b   e   r   "   :   	   "   5   6   7   "   ,   
   	   	   	   	   	   "   k   e   y   "   :   	   "   V   4   5   6   "   ,   
   	   	   	   	   	   "   t   i   t   l   e   "   :   	   "   j   o   b       2   "   
   	   	   	   	   }   ]   
   	   	   }   ,       {   
   	   	   	   "   e   m   p   l   o   y   e   e   "   :   	   {   
   	   	   	   	   "   f   i   r   s   t   n   a   m   e   "   :   	   "   J   a   k   o   b   "   ,   
   	   	   	   	   "   p   h   o   n   e   "   :   	   "   0   1   2       3   4   5       6   7       8   9   "   ,   
   	   	   	   	   "   s   u   r   n   a   m   e   "   :   	   "   S   t   e   g   e   l   i   n   "   
   	   	   	   }   ,   
   	   	   	   "   i   s   s   u   e   d   a   t   e   "   :   	   "   1   3   .   5   .   2   0   0   6   "   ,   
   	   	   	   "   t   a   s   k   "   :   	   [   {   
   	   	   	   	   	   "   c   u   s   t   o   m   e   r   n   u   m   b   e   r   "   :   	   "   5   6   7   "   ,   
   	   	   	   	   	   "   k   e   y   "   :   	   "   A   4   5   6   "   ,   
   	   	   	   	   	   "   t   i   t   l   e   "   :   	   "   j   o   b       3   "   
   	   	   	   	   }   ,       {   
   	   	   	   	   	   "   c   u   s   t   o   m   e   r   n   u   m   b   e   r   "   :   	   "   8   9   0   "   ,   
   	   	   	   	   	   "   k   e   y   "   :   	   "   V   7   8   9   "   ,   
   	   	   	   	   	   "   t   i   t   l   e   "   :   	   "   j   o   b       4   "   
   	   	   	   	   }   ]   
   	   	   }   ]   
   }   
**end
