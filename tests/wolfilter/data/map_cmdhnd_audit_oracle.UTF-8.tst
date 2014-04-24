**
**requires:ORACLE
**input
<?xml version="1.0" encoding="UTF-8" standalone="no"?>
<!DOCTYPE person SYSTEM 'Person'>
<person><name>Hugi Turbo</name><address>Gurkenstrasse 7a 3145 Gumligen</address></person>**config
--input-filter textwolf --output-filter textwolf --module ../../src/modules/filter/textwolf/mod_filter_textwolf -c wolframe.conf InsertPerson
**requires:TEXTWOLF
**file:wolframe.conf
LoadModules
{
	module ./../wolfilter/modules/database/oracle/mod_db_oracletest
	module ./../../src/modules/cmdbind/directmap/mod_command_directmap
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
 ID INTEGER NOT NULL PRIMARY KEY,
 name VARCHAR2(30),
 address VARCHAR2(40)
);
CREATE SEQUENCE Person_ID_Seq START WITH 1 INCREMENT BY 1;
CREATE TRIGGER Person_Insert
BEFORE INSERT ON Person
FOR EACH ROW
BEGIN
	SELECT Person_ID_Seq.nextval into :new.id FROM dual;
END;
/

INSERT INTO Person (name,address) VALUES ('Aufru','Amselstrasse 12 Aulach');
INSERT INTO Person (name,address) VALUES ('Beno','Butterweg 23 Bendorf');
INSERT INTO Person (name,address) VALUES ('Carla','Camelstreet 34 Carassa');
INSERT INTO Person (name,address) VALUES ('Dorothe','Demotastrasse 45 Durnfo');
INSERT INTO Person (name,address) VALUES ('Erik','Erakimolstrasse 56 Enden');
**file:audit.dmap
COMMAND (Insert Person) SKIP CALL(insertPerson);
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
**end
