**
**requires:LUA
**requires:LIBXML2
**input
   <   ?   x   m   l       v   e   r   s   i   o   n   =   "   1   .   0   "       e   n   c   o   d   i   n   g   =   "   U   C   S   -   4   B   E   "       s   t   a   n   d   a   l   o   n   e   =   "   n   o   "   ?   >   
   <   !   D   O   C   T   Y   P   E       p   e   r   s   o   n       S   Y   S   T   E   M       '   P   e   r   s   o   n   '   >   
   <   p   e   r   s   o   n   >   <   n   a   m   e   >   H   u   g   i       T   u   r   b   o   <   /   n   a   m   e   >   <   a   d   d   r   e   s   s   >   G   u   r   k   e   n   s   t   r   a   s   s   e       7   a       3   1   4   5       G   u   m   l   i   g   e   n   <   /   a   d   d   r   e   s   s   >   <   /   p   e   r   s   o   n   >**config
--input-filter libxml2 --output-filter libxml2 --module ../../src/modules/filter/libxml2/mod_filter_libxml2 --module ../../src/modules/doctype/xml/mod_doctype_xml -c wolframe.conf run

**file:wolframe.conf
LoadModules
{
	module ./../../src/modules/cmdbind/aamap/mod_command_aamap
	module ./../../src/modules/cmdbind/lua/mod_command_lua
}
Processor
{
	program ../wolfilter/template/program/basic.aamap
	program ../wolfilter/scripts/authorize.lua
	cmdhandler
	{
		lua
		{
			program authorize_test.lua
			filter libxml2
		}
	}
}
**file:basic.aamap

# [1] Implicit authorization check called immediately after connect:
#	-> Redirect to function checkValidConnect( from="156.217.32.32", to="PublicInterface", user="Koebi" );

AUTHORIZE CONNECT checkValidConnect( from=remotehost, to=socketid, user=username );

# [2] Implicit authorization check called from main protocol to 
#	switch ON/OFF capability to change own password:
#	-> Redirect to function checkCapabilityPasswordChange( from="156.217.32.32", type="SSL" );

AUTHORIZE PASSWD checkCapabilityPasswordChange( from=remotehost, type=connectiontype );

# [3] Explicit authorization check called in transaction
# example: AUTHORIZE (DBACCESS, Customer.WRITE)
#	-> Redirect to function checkTableAccess( table=Customer, op=WRITE, user=Fredi );

AUTHORIZE DBACCESS checkTableAccess( table=resource[.1], op=resource[.2], user=username );

# [4] 
AUTHORIZE NOARG checkNoArg();

# [5] 
AUTHORIZE AUTHENTICATOR checkAuthenticator( auth=authenticator );
**file:authorize_test.lua
function run( input )
	if not provider.authorize("NOARG") then
		error( "NOARG not authorized")
	end
	if not provider.authorize("CONNECT") then
		error( "CONNECT not authorized")
	end
	if not provider.authorize("PASSWD") then
		error( "PASSWD not authorized")
	end
	if not provider.authorize("DBACCESS", "Customer.WRITE") then
		error( "DBACCESS WRITE Customer on not authorized")
	end
	if not provider.authorize("AUTHENTICATOR") then
		error( "AUTHENTICATOR not authorized")
	end
end
**file:authorize.lua
function checkValidConnect( struct )
	st = struct:value()
	if st[ 'from' ] ~= "123.123.123.123" then
		return false
	end
	if st[ 'to' ] ~= "fakeSocketIdentifier" then
		return false
	end
	if st[ 'user' ] ~= "wolfilter" then
		return false
	end
	return true
end

function checkCapabilityPasswordChange( struct )
	st = struct:value()
	if st[ 'from' ] ~= "123.123.123.123" then
		return false
	end
	if st[ 'type' ] ~= "TCP" then
		return false
	end
	return true
end

function checkTableAccess( struct )
	st = struct:value()
	if st[ 'table' ] ~= "Customer" then
		return false
	end
	if st[ 'op' ] ~= "WRITE" then
		return false
	end
	if st[ 'user' ] ~= "wolfilter" then
		return false
	end
	return true
end

function checkNoArg( struct )
	st = struct:value()
	return true
end

function checkAuthenticator( struct )
	st = struct:value()
	return (st[ 'auth' ] == "WolfilterAuth")
end
**output
   <   ?   x   m   l       v   e   r   s   i   o   n   =   "   1   .   0   "       e   n   c   o   d   i   n   g   =   "   U   C   S   -   4   B   E   "       s   t   a   n   d   a   l   o   n   e   =   "   n   o   "   ?   >   
   <   !   D   O   C   T   Y   P   E       p   e   r   s   o   n       S   Y   S   T   E   M       "   P   e   r   s   o   n   "   >   <   p   e   r   s   o   n   /   >   
**end
