**
**requires:LUA
**input
< ? x m l   v e r s i o n = " 1 . 0 "   e n c o d i n g = " U C S - 2 L E "   s t a n d a l o n e = " y e s " ? > 
 < a s s i g n m e n t l i s t > < a s s i g n m e n t > < t a s k > < t i t l e > j o b   1 < / t i t l e > < k e y > A 1 2 3 < / k e y > < c u s t o m e r n u m b e r > 3 2 4 < / c u s t o m e r n u m b e r > < / t a s k > < t a s k > < t i t l e > j o b   2 < / t i t l e > < k e y > V 4 5 6 < / k e y > < c u s t o m e r n u m b e r > 5 6 7 < / c u s t o m e r n u m b e r > < / t a s k > < e m p l o y e e > < f i r s t n a m e > J u l i a < / f i r s t n a m e > < s u r n a m e > T e g e l - S a c h e r < / s u r n a m e > < p h o n e > 0 9 8   7 6 5   4 3   2 1 < / p h o n e > < / e m p l o y e e > < i s s u e d a t e > 1 3 . 5 . 2 0 0 6 < / i s s u e d a t e > < / a s s i g n m e n t > < a s s i g n m e n t > < t a s k > < t i t l e > j o b   3 < / t i t l e > < k e y > A 4 5 6 < / k e y > < c u s t o m e r n u m b e r > 5 6 7 < / c u s t o m e r n u m b e r > < / t a s k > < t a s k > < t i t l e > j o b   4 < / t i t l e > < k e y > V 7 8 9 < / k e y > < c u s t o m e r n u m b e r > 8 9 0 < / c u s t o m e r n u m b e r > < / t a s k > < e m p l o y e e > < f i r s t n a m e > J a k o b < / f i r s t n a m e > < s u r n a m e > S t e g e l i n < / s u r n a m e > < p h o n e > 0 1 2   3 4 5   6 7   8 9 < / p h o n e > < / e m p l o y e e > < i s s u e d a t e > 1 3 . 5 . 2 0 0 6 < / i s s u e d a t e > < / a s s i g n m e n t > < / a s s i g n m e n t l i s t > **config
--input-filter xml:textwolf --output-filter xml:textwolf --module ../../src/modules/filter/textwolf/mod_filter_textwolf  --module ../../src/modules/cmdbind/lua/mod_command_lua --program program_formfunc.lua --module ../wolfilter/modules//employee_assignment_convert//mod_employee_assignment_convert --cmdprogram formfunc_lua_table.lua run

**file:program_formfunc.lua
function toupper_table( tb)
	for i,v in pairs(tb)
	do
		if type(v) == "table" then
			tb[ i] = toupper_table(v)
		else
			tb[ i] = string.upper(v)
		end
	end
	return tb
end

function toupper_formfunc( inp)
	return toupper_table( inp:table())
end
**file: formfunc_lua_table.lua

function run()
	res = formfunction( "toupper_formfunc")( input:table())
	output:print( res:table())
end
**output
< ? x m l   v e r s i o n = " 1 . 0 "   e n c o d i n g = " U C S - 2 L E "   s t a n d a l o n e = " y e s " ? > 
 < a s s i g n m e n t l i s t > < a s s i g n m e n t > < i s s u e d a t e > 1 3 . 5 . 2 0 0 6 < / i s s u e d a t e > < e m p l o y e e > < f i r s t n a m e > J U L I A < / f i r s t n a m e > < p h o n e > 0 9 8   7 6 5   4 3   2 1 < / p h o n e > < s u r n a m e > T E G E L - S A C H E R < / s u r n a m e > < / e m p l o y e e > < t a s k > < c u s t o m e r n u m b e r > 3 2 4 < / c u s t o m e r n u m b e r > < k e y > A 1 2 3 < / k e y > < t i t l e > J O B   1 < / t i t l e > < / t a s k > < t a s k > < c u s t o m e r n u m b e r > 5 6 7 < / c u s t o m e r n u m b e r > < k e y > V 4 5 6 < / k e y > < t i t l e > J O B   2 < / t i t l e > < / t a s k > < / a s s i g n m e n t > < a s s i g n m e n t > < i s s u e d a t e > 1 3 . 5 . 2 0 0 6 < / i s s u e d a t e > < e m p l o y e e > < f i r s t n a m e > J A K O B < / f i r s t n a m e > < p h o n e > 0 1 2   3 4 5   6 7   8 9 < / p h o n e > < s u r n a m e > S T E G E L I N < / s u r n a m e > < / e m p l o y e e > < t a s k > < c u s t o m e r n u m b e r > 5 6 7 < / c u s t o m e r n u m b e r > < k e y > A 4 5 6 < / k e y > < t i t l e > J O B   3 < / t i t l e > < / t a s k > < t a s k > < c u s t o m e r n u m b e r > 8 9 0 < / c u s t o m e r n u m b e r > < k e y > V 7 8 9 < / k e y > < t i t l e > J O B   4 < / t i t l e > < / t a s k > < / a s s i g n m e n t > < / a s s i g n m e n t l i s t > 
 **end
