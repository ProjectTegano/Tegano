**
**requires:CJSON
**requires:TEXTWOLF
**input
{ 
 	 " - d o c t y p e " : 	 " U s e r D a t a " , 
 	 " d a t a " : 	 " b l a " , 
 	 " c o m m a n d " : 	 " e x e c " 
 } **config
--input-filter cjson --output-filter cjson --module ../../src/modules/filter/cjson/mod_filter_cjson --module ../../src/modules/doctype/json/mod_doctype_json -c wolframe.conf echoUserData

**file:wolframe.conf
LoadModules
{
	module ./../../src/modules/cmdbind/directmap/mod_command_directmap
	module ./../../src/modules/cmdbind/lua/mod_command_lua
	module ./../../src/modules/ddlcompiler/simpleform/mod_ddlcompiler_simpleform
}
Processor
{
	program echo_input_table.lua
	program userdata.sfrm
	cmdhandler
	{
		directmap
		{
			program test.dmap
			filter cjson
		}
	}
}
**file: test.dmap
COMMAND (echo UserData) CALL run CONTEXT {uname=UserName, host=RemoteHost} RETURN UserDataWithLogin;
**file:userdata.sfrm
FORM UserData
	-root doc
{
	data string
	command string
}

FORM UserDataWithLogin
	-root doc
{
	_ UserData
	uname string
	host string
}
**file:echo_input_table.lua
function run( input)
	return input:get()
end

**output
{ 
 	 " - d o c t y p e " : 	 " U s e r D a t a W i t h L o g i n " , 
 	 " d a t a " : 	 " b l a " , 
 	 " c o m m a n d " : 	 " e x e c " , 
 	 " u n a m e " : 	 " w o l f i l t e r " , 
 	 " h o s t " : 	 " 1 2 3 . 1 2 3 . 1 2 3 . 1 2 3 " 
 } 
 **end
