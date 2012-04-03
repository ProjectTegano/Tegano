**
**requires:LIBXML2
**requires:LUA
**input
HELLO
RUN
< ? x m l   v e r s i o n = " 1 . 0 "   e n c o d i n g = " U C S - 2 L E "   s t a n d a l o n e = " y e s " ? > 
 < i t e m > < n a m e > e x a m p l e < / n a m e > < c o n t e n t > < c o l o r > b l u e < / c o l o r > < m a t e r i a l > m e t a l < / m a t e r i a l > < / c o n t e n t > < p r i c e > 2 3 . 4 5 < / p r i c e > < / i t e m > 
.
QUIT
**file:example_4.lua

function process_content( itr)
        for c,t in itr do
                output:print( c, t)
        end
end

function run( )
        f = filter( "XML:libxml2")
        input:as(f)
        output:as(f)
        for c,t in input:get() do
                output:print( c, t)
                if t == "content" then
                        process_content( input:get())
                        output:print( false, false)
                end
        end
end
**config
proc {
script {
	cmd RUN
	path example_4.lua
	main run
}}
**output
OK enter cmd
< ? x m l   v e r s i o n = " 1 . 0 "   e n c o d i n g = " U C S - 2 L E "   s t a n d a l o n e = " y e s " ? > 
 < i t e m > < n a m e > e x a m p l e < / n a m e > < c o n t e n t > < c o l o r > b l u e < / c o l o r > < m a t e r i a l > m e t a l < / m a t e r i a l > < / c o n t e n t > < p r i c e > 2 3 . 4 5 < / p r i c e > < / i t e m > 
 
.
OK
BYE
**end

