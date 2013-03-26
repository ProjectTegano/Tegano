**
**requires:LUA
**requires:LIBXML2
**input
   <   ?   x   m   l       v   e   r   s   i   o   n   =   "   1   .   0   "       e   n   c   o   d   i   n   g   =   "   U   C   S   -   4   B   E   "       s   t   a   n   d   a   l   o   n   e   =   "   y   e   s   "   ?   >   
   <   d   o   c   >   <   i   m   a   g   e       i   d   =   "   1   "   /   >   <   i   m   a   g   e       i   d   =   "   1   "       n   a   m   e   =   "   t   r   o   l   l   "   /   >   <   i   m   a   g   e       i   d   =   "   2   "   >   <   /   i   m   a   g   e   >   <   i   m   a   g   e       i   d   =   "   3   "   >   X   <   /   i   m   a   g   e   >   <   /   d   o   c   >**config
--input-filter xml:libxml2 --output-filter xml:libxml2 --module ../../src/modules/filter/libxml2/mod_filter_libxml2  --module ../../src/modules/cmdbind/lua/mod_command_lua --cmdprogram echo_input_iterator_spez.lua run
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
   <   ?   x   m   l       v   e   r   s   i   o   n   =   "   1   .   0   "       e   n   c   o   d   i   n   g   =   "   U   C   S   -   4   B   E   "       s   t   a   n   d   a   l   o   n   e   =   "   y   e   s   "   ?   >   
   <   d   o   c   >   <   i   m   a   g   e       i   d   =   "   1   "   /   >   <   i   m   a   g   e       i   d   =   "   1   "       n   a   m   e   =   "   t   r   o   l   l   "   /   >   <   i   m   a   g   e       i   d   =   "   2   "   /   >   <   i   m   a   g   e       i   d   =   "   3   "   >   X   <   /   i   m   a   g   e   >   <   /   d   o   c   >   
**end
