**
**requires:LUA
**requires:CJSON
**requires:TEXTWOLF
**input
{
	"assignment":	[{
			"task":	[{
					"title":	"job 1",
					"key":	"A123",
					"customernumber":	"324"
				}, {
					"title":	"job 2",
					"key":	"V456",
					"customernumber":	"567"
				}],
			"employee":	{
				"firstname":	"Julia",
				"surname":	"Tegel-Sacher",
				"phone":	"098 765 43 21"
			},
			"issuedate":	"13.5.2006"
		}, {
			"task":	[{
					"title":	"job 3",
					"key":	"A456",
					"customernumber":	"567"
				}, {
					"title":	"job 4",
					"key":	"V789",
					"customernumber":	"890"
				}],
			"employee":	{
				"firstname":	"Jakob",
				"surname":	"Stegelin",
				"phone":	"012 345 67 89"
			},
			"issuedate":	"13.5.2006"
		}]
}**config
--input-filter cjson --output-filter cjson --module ../../src/modules/filter/cjson/mod_filter_cjson --module ../../src/modules/doctype/json/mod_doctype_json -c wolframe.conf test_transaction

**file:wolframe.conf
LoadModules
{
	module ../wolfilter/modules/database/testtrace/mod_db_testtrace
	module ../../src/modules/cmdbind/tdl/mod_command_tdl
	module ../../src/modules/cmdbind/lua/mod_command_lua
}
Database
{
	test
	{
		identifier testdb
		outfile DBOUT
		file DBRES
	}
}
Processor
{
	program		DBIN.tdl
	program		script.lua
	database	testdb
}
**file:script.lua

function run()
	r = provider.formfunction("test_transaction")( input:get())
	output:print( r:get())
end
**file: DBRES
#name#job1
#name#job2
#name#job3
#name#job4
**file:DBIN.tdl
TRANSACTION test_transaction
BEGIN
	INTO title FOREACH //task DO SELECT run( title);
END
**output
{
	"title":	[{
			"name":	"job1"
		}, {
			"name":	"job2"
		}, {
			"name":	"job3"
		}, {
			"name":	"job4"
		}]
}
**end
