**
**requires:LIBXML2
**requires:ORACLE
**input
 < ? x m l   v e r s i o n = " 1 . 0 "   e n c o d i n g = " U T F - 1 6 B E "   s t a n d a l o n e = " n o " ? > 
 < ! D O C T Y P E   p e r s o n   S Y S T E M   ' P e r s o n ' > 
 < p e r s o n > < n a m e > H u g i   T u r b o < / n a m e > < a d d r e s s > G u r k e n s t r a s s e   7 a   3 1 4 5   G u m l i g e n < / a d d r e s s > < / p e r s o n >**config
--input-filter libxml2 --output-filter libxml2 --module ../../src/modules/filter/libxml2/mod_filter_libxml2 -c wolframe.conf InsertPerson

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
			filter libxml2
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
