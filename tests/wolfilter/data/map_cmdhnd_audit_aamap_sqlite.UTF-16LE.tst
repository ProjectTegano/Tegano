**
**requires:SQLITE3
**input
< ? x m l   v e r s i o n = " 1 . 0 "   e n c o d i n g = " U T F - 1 6 L E "   s t a n d a l o n e = " n o " ? > 
 < ! D O C T Y P E   p e r s o n   S Y S T E M   ' P e r s o n ' > 
 < p e r s o n > < n a m e > H u g i   T u r b o < / n a m e > < a d d r e s s > G u r k e n s t r a s s e   7 a   3 1 4 5   G u m l i g e n < / a d d r e s s > < / p e r s o n > **config
--input-filter textwolf --output-filter textwolf --module ../../src/modules/filter/textwolf/mod_filter_textwolf --module ../../src/modules/doctype/xml/mod_doctype_xml -c wolframe.conf InsertPerson
**requires:TEXTWOLF
**file:wolframe.conf
LoadModules
{
	module ./../wolfilter/modules/database/sqlite3/mod_db_sqlite3test
	module ./../../src/modules/cmdbind/directmap/mod_command_directmap
	module ./../../src/modules/cmdbind/aamap/mod_command_aamap
	module ./../../src/modules/cmdbind/tdl/mod_command_tdl
	module ./../wolfilter/modules/functions/audit/mod_audit
}
Database
{
	SQliteTest
	{
		identifier testdb
		file test.db
		connections 3
		dumpfile DBDUMP
		inputfile DBDATA
	}
}
Processor
{
	database testdb
	program audit.tdl
	program audit.aamap

	cmdhandler
	{
		directmap
		{
			program audit.dmap
			filter textwolf
		}
	}
}
**file:DBDATA
CREATE TABLE Person
(
 ID INTEGER PRIMARY KEY AUTOINCREMENT,
 name TEXT,
 address TEXT
);

INSERT INTO Person (name,address) VALUES ('Aufru','Amselstrasse 12 Aulach');
INSERT INTO Person (name,address) VALUES ('Beno','Butterweg 23 Bendorf');
INSERT INTO Person (name,address) VALUES ('Carla','Camelstreet 34 Carassa');
INSERT INTO Person (name,address) VALUES ('Dorothe','Demotastrasse 45 Durnfo');
INSERT INTO Person (name,address) VALUES ('Erik','Erakimolstrasse 56 Enden');
**file:audit.dmap
COMMAND (Insert Person) SKIP CALL insertPerson;
**file:audit.aamap

AUDIT auditMutation1 auditMutation1( user=username );
**file:audit.tdl

TRANSACTION insertPerson
BEGIN
	DO INSERT INTO Person (name, address) VALUES ($(name), $(address));
	DO UNIQUE SELECT id from Person WHERE name = $(name);
END
AUDIT CRITICAL auditMutation1( operation="insert", $RESULT.id, newvalue=$(name))
**outputfile:DBDUMP
**outputfile:audit.log
**output
Person:
'1', 'Aufru', 'Amselstrasse 12 Aulach'
'2', 'Beno', 'Butterweg 23 Bendorf'
'3', 'Carla', 'Camelstreet 34 Carassa'
'4', 'Dorothe', 'Demotastrasse 45 Durnfo'
'5', 'Erik', 'Erakimolstrasse 56 Enden'
'6', 'Hugi Turbo', 'Gurkenstrasse 7a 3145 Gumligen'

sqlite_sequence:
'Person', '6'

audit insert user 'wolfilter' id 6 old '' new 'Hugi Turbo'
**end
