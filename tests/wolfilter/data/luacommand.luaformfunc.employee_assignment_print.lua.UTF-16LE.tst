**
**input
< ? x m l   v e r s i o n = " 1 . 0 "   e n c o d i n g = " U T F - 1 6 L E "   s t a n d a l o n e = " y e s " ? > 
 < a s s i g n m e n t l i s t > < a s s i g n m e n t > < i s s u e d a t e > 1 3 . 5 . 2 0 0 6 < / i s s u e d a t e > < e m p l o y e e > < f i r s t n a m e > J u l i a < / f i r s t n a m e > < p h o n e > 0 9 8   7 6 5   4 3   2 1 < / p h o n e > < s u r n a m e > T e g e l - S a c h e r < / s u r n a m e > < / e m p l o y e e > < t a s k > < c u s t o m e r n u m b e r > 3 2 4 < / c u s t o m e r n u m b e r > < t i t l e > j o b   1 < / t i t l e > < k e y > A 1 2 3 < / k e y > < / t a s k > < t a s k > < c u s t o m e r n u m b e r > 5 6 7 < / c u s t o m e r n u m b e r > < t i t l e > j o b   2 < / t i t l e > < k e y > V 4 5 6 < / k e y > < / t a s k > < / a s s i g n m e n t > < a s s i g n m e n t > < i s s u e d a t e > 1 3 . 5 . 2 0 0 6 < / i s s u e d a t e > < e m p l o y e e > < f i r s t n a m e > J a k o b < / f i r s t n a m e > < s u r n a m e > S t e g e l i n < / s u r n a m e > < p h o n e > 0 1 2   3 4 5   6 7   8 9 < / p h o n e > < / e m p l o y e e > < t a s k > < c u s t o m e r n u m b e r > 5 6 7 < / c u s t o m e r n u m b e r > < t i t l e > j o b   3 < / t i t l e > < k e y > A 4 5 6 < / k e y > < / t a s k > < t a s k > < c u s t o m e r n u m b e r > 8 9 0 < / c u s t o m e r n u m b e r > < t i t l e > j o b   4 < / t i t l e > < k e y > V 7 8 9 < / k e y > < / t a s k > < / a s s i g n m e n t > < / a s s i g n m e n t l i s t > 
 **file: formfunc_luatable_luaparse.lua

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
	--... iscontent=false -> print all values as tag content
	for t,v in pairs(tab) do
		local tagname = t
		if type(v) == "table" then
			iscontent = true
			if v[#v] then
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
	f = filter( "xml:textwolf")

	-- a W3C conform XML parser returns by default also tokens containing only spaces.
	-- these we eliminate to be able to create a table:
	f.empty = false

	input:as(f)
	output:as(f)

	t = readTable( input:get())
	r = formfunction("employee_assignment_convert")( t)
	printTable( r)
end

**config
-s formfunc_luatable_luaparse.lua run xml:textwolf
**output
< ? x m l   v e r s i o n = " 1 . 0 "   e n c o d i n g = " U T F - 1 6 L E "   s t a n d a l o n e = " y e s " ? > 
 < a s s i g n m e n t l i s t > < a s s i g n m e n t > < i s s u e d a t e > 8 6 4 7 9 9 3 < / i s s u e d a t e > < e m p l o y e e > < f i r s t n a m e > j u u l l i i a a < / f i r s t n a m e > < p h o n e > 9 0 1   2 3 4   5 6   7 8 < / p h o n e > < s u r n a m e > t e e g g e e l l - s a a c c h h e e r r < / s u r n a m e > < / e m p l o y e e > < t a s k > < c u s t o m e r n u m b e r > 3 2 5 < / c u s t o m e r n u m b e r > < t i t l e > j j o o b b   8 < / t i t l e > < k e y > a 8 7 6 < / k e y > < / t a s k > < t a s k > < c u s t o m e r n u m b e r > 5 6 8 < / c u s t o m e r n u m b e r > < t i t l e > j j o o b b   7 < / t i t l e > < k e y > v 5 4 3 < / k e y > < / t a s k > < / a s s i g n m e n t > < a s s i g n m e n t > < i s s u e d a t e > 8 6 4 7 9 9 3 < / i s s u e d a t e > < e m p l o y e e > < f i r s t n a m e > j a a k k o o b b < / f i r s t n a m e > < p h o n e > 9 8 7   6 5 4   3 2   1 0 < / p h o n e > < s u r n a m e > s t t e e g g e e l l i i n n < / s u r n a m e > < / e m p l o y e e > < t a s k > < c u s t o m e r n u m b e r > 5 6 8 < / c u s t o m e r n u m b e r > < t i t l e > j j o o b b   6 < / t i t l e > < k e y > a 5 4 3 < / k e y > < / t a s k > < t a s k > < c u s t o m e r n u m b e r > 8 9 1 < / c u s t o m e r n u m b e r > < t i t l e > j j o o b b   5 < / t i t l e > < k e y > v 2 1 0 < / k e y > < / t a s k > < / a s s i g n m e n t > < / a s s i g n m e n t l i s t > 
 **end
