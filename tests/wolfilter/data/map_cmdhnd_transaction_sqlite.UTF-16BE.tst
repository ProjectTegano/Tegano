**
**requires:SQLITE3
**input
 < ? x m l   v e r s i o n = " 1 . 0 "   e n c o d i n g = " U T F - 1 6 B E "   s t a n d a l o n e = " y e s " ? > 
 < a s s i g n m e n t l i s t > < a s s i g n m e n t > < t a s k > < t i t l e > j o b   1 < / t i t l e > < i d > 1 < / i d > < c u s t o m e r n u m b e r > 3 2 4 < / c u s t o m e r n u m b e r > < / t a s k > < t a s k > < t i t l e > j o b   2 < / t i t l e > < i d > 2 < / i d > < c u s t o m e r n u m b e r > 5 6 7 < / c u s t o m e r n u m b e r > < / t a s k > < e m p l o y e e > < f i r s t n a m e > J u l i a < / f i r s t n a m e > < s u r n a m e > T e g e l - S a c h e r < / s u r n a m e > < p h o n e > 0 9 8   7 6 5   4 3   2 1 < / p h o n e > < / e m p l o y e e > < i s s u e d a t e > 1 3 . 5 . 2 0 0 6 < / i s s u e d a t e > < / a s s i g n m e n t > < a s s i g n m e n t > < t a s k > < t i t l e > j o b   3 < / t i t l e > < i d > 3 < / i d > < c u s t o m e r n u m b e r > 5 6 7 < / c u s t o m e r n u m b e r > < / t a s k > < t a s k > < t i t l e > j o b   4 < / t i t l e > < i d > 4 < / i d > < c u s t o m e r n u m b e r > 8 9 0 < / c u s t o m e r n u m b e r > < / t a s k > < e m p l o y e e > < f i r s t n a m e > J a k o b < / f i r s t n a m e > < s u r n a m e > S t e g e l i n < / s u r n a m e > < p h o n e > 0 1 2   3 4 5   6 7   8 9 < / p h o n e > < / e m p l o y e e > < i s s u e d a t e > 1 3 . 5 . 2 0 0 6 < / i s s u e d a t e > < / a s s i g n m e n t > < / a s s i g n m e n t l i s t >**config
--input-filter xml:textwolf --output-filter xml:textwolf --module ../../src/modules/filter/textwolf/mod_filter_textwolf  --program schema_select_task_by_id.simpleform --program simpleform.normalize --module ../../src/modules/ddlcompiler//simpleform/mod_ddlcompiler_simpleform --module ../../src/modules/normalize//number/mod_normalize_number --module ../../src/modules/normalize//string/mod_normalize_string --module ../../src/modules/cmdbind/directmap/mod_command_directmap --module ../wolfilter/modules/database/sqlite3/mod_db_sqlite3test --database 'identifier=testdb,file=test.db,dumpfile=DBDUMP,inputfile=DBDATA' --program=DBPRG.tdl --cmdprogram=test.directmap run

**file: test.directmap
run = test_transaction( xml :schema_select_task_by_id);
**file:simpleform.normalize
int=number:integer;
uint=number:unsigned;
float=number:float;
**file: DBDATA
CREATE TABLE task
(
	title STRING,
	id INTEGER,
	start DATE,
	end DATE
);

INSERT INTO task (title,id,start,end) VALUES ('bla bla', '1', '1/4/2012 12:04:19', '1/4/2012 12:41:14');
INSERT INTO task (title,id,start,end) VALUES ('bli blu', '2', '2/4/2012 11:14:29', '2/4/2012 12:11:34');
INSERT INTO task (title,id,start,end) VALUES ('blu bli', '3', '3/4/2012 17:11:13', '3/4/2012 18:19:31');
INSERT INTO task (title,id,start,end) VALUES ('ble ble', '4', '4/4/2012 19:14:29', '4/4/2012 19:58:44');
**file:DBPRG.tdl
PREPARE get_task AS SELECT * FROM task WHERE id=? ORDER BY id ASC;

TRANSACTION test_transaction
RESULT INTO doc
BEGIN
	INTO task FOREACH //task DO get_task( id);
END
**outputfile:DBDUMP
**file: schema_select_task_by_id.simpleform
FORM Employee
{
	firstname string
	surname string
	phone string
}

FORM schema_select_task_by_id
{
	assignmentlist
	{
		assignment []
		{
			task []
			{
				id int
				title string
				customernumber int
			}
			employee Employee
			issuedate string
		}
	}
}

**output
 < ? x m l   v e r s i o n = " 1 . 0 "   e n c o d i n g = " U T F - 1 6 B E "   s t a n d a l o n e = " y e s " ? > 
 < d o c > < t a s k > < t i t l e > b l a   b l a < / t i t l e > < i d > 1 < / i d > < s t a r t > 1 / 4 / 2 0 1 2   1 2 : 0 4 : 1 9 < / s t a r t > < e n d > 1 / 4 / 2 0 1 2   1 2 : 4 1 : 1 4 < / e n d > < / t a s k > < t a s k > < t i t l e > b l i   b l u < / t i t l e > < i d > 2 < / i d > < s t a r t > 2 / 4 / 2 0 1 2   1 1 : 1 4 : 2 9 < / s t a r t > < e n d > 2 / 4 / 2 0 1 2   1 2 : 1 1 : 3 4 < / e n d > < / t a s k > < t a s k > < t i t l e > b l u   b l i < / t i t l e > < i d > 3 < / i d > < s t a r t > 3 / 4 / 2 0 1 2   1 7 : 1 1 : 1 3 < / s t a r t > < e n d > 3 / 4 / 2 0 1 2   1 8 : 1 9 : 3 1 < / e n d > < / t a s k > < t a s k > < t i t l e > b l e   b l e < / t i t l e > < i d > 4 < / i d > < s t a r t > 4 / 4 / 2 0 1 2   1 9 : 1 4 : 2 9 < / s t a r t > < e n d > 4 / 4 / 2 0 1 2   1 9 : 5 8 : 4 4 < / e n d > < / t a s k > < / d o c > 
task:
'bla bla', '1', '1/4/2012 12:04:19', '1/4/2012 12:41:14'
'bli blu', '2', '2/4/2012 11:14:29', '2/4/2012 12:11:34'
'blu bli', '3', '3/4/2012 17:11:13', '3/4/2012 18:19:31'
'ble ble', '4', '4/4/2012 19:14:29', '4/4/2012 19:58:44'

**end
