**
**input
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<aa>1</aa><bb>2</bb>
**config
--module ../../src/modules/filter/textwolf/mod_filter_textwolf
--module ../wolfilter/modules/database/testtrace/mod_db_testtrace
--module ../../src/modules/transaction/preparedStatement/mod_transaction_pstm
--input-filter 'xml:textwolf'
--database 'id=testdb,outfile=DBOUT,file=DBRES'
--transaction 'testcall dbpstm result/item=run(: /aa, /bb)'
testcall
**file: DBRES
#id name#1 hugo
**outputfile:DBOUT
**output
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<result><item><id>1</id><name>hugo</name></item></result>
begin();
start( 'run' );
bind( 1, '1' );
bind( 2, '2' );
execute();
nofColumns(); returns 2
get( 1 ); returns 1
get( 2 ); returns hugo
next(); returns 0
getLastError(); returns 0
nofColumns(); returns 2
columnName( 1 ); returns id
columnName( 2 ); returns name
commit();
**end
