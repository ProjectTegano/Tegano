**
**requires:LIBXML2
**input
<?xml version="1.0" encoding="UTF-8" standalone="no"?>
<!DOCTYPE assignmentlist PUBLIC "-//ABC//DTD XML 1.0 plus Software 2.0 plus ABC 1.1//EN" "http://www.wolframe.org/example-doc/employee_assignment_print.dtd"><assignmentlist><assignment><task><title>job 1</title><key>A123</key><customernumber>324</customernumber></task><task><title>job 2</title><key>V456</key><customernumber>567</customernumber></task><employee><firstname>Julia</firstname><surname>Tegel-Sacher</surname><phone>098 765 43 21</phone></employee><issuedate>13.5.2006</issuedate></assignment><assignment><task><title>job 3</title><key>A456</key><customernumber>567</customernumber></task><task><title>job 4</title><key>V789</key><customernumber>890</customernumber></task><employee><firstname>Jakob</firstname><surname>Stegelin</surname><phone>012 345 67 89</phone></employee><issuedate>13.5.2006</issuedate></assignment></assignmentlist>**config
--input-filter xml:libxml2 --output-filter xml:libxml2 --module ../../src/modules/filter/libxml2/mod_filter_libxml2  --program employee_assignment_print.simpleform --module ../../src/modules/ddlcompiler//simpleform/mod_ddlcompiler_simpleform --module ../../src/modules/normalize//number/mod_normalize_number --program simpleform_range_trim.normalize --module ../../src/modules/cmdbind/directmap/mod_command_directmap --module ../wolfilter/modules/database/testtrace/mod_db_testtrace --database 'identifier=testdb,outfile=DBOUT,file=DBRES' --program=DBIN.tdl --directmap 'f=xml,c=test_transaction,n=run' run

**file:simpleform_range_trim.normalize
int=number:trim,integer(10);
uint=number:trim,unsigned(10);
float=number:trim,float(10,10);
**file: DBRES
#id task start end#11 'bla bla' '12:04:19 1/3/2012' '12:41:34 1/3/2012'#12 'bli blu' '07:14:23 1/3/2012' '08:01:51 1/3/2012'
#id task start end#21 'gardening' '09:24:28 1/3/2012' '11:11:07 1/3/2012'#22 'helo' '11:31:01 1/3/2012' '12:07:55 1/3/2012'
#id task start end#31 'hula hop' '19:14:38 1/4/2012' '20:01:12 1/4/2012'#32 'hula hip' '11:31:01 1/3/2012' '12:07:55 1/3/2012'#33 'hula hup' '11:31:01 1/3/2012' '12:07:55 1/3/2012'
**file:DBIN.tdl
TRANSACTION test_transaction
BEGIN
	INTO doc/task WITH //task DO run( title);
END
**file: employee_assignment_print.simpleform
!DOCTYPE "assignmentlist 'http://www.wolframe.org/example-doc/employee_assignment_print.dtd'"
assignmentlist
{
	assignment []
	{
		task []
		{
			title string
			key string
			customernumber int
		}
		employee
		{
			firstname string
			surname string
			phone string
		}
		issuedate string
	}
}
**output
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<doc><task><id>11</id><task>bla bla</task><start>12:04:19 1/3/2012</start><end>12:41:34 1/3/2012</end></task><task><id>12</id><task>bli blu</task><start>07:14:23 1/3/2012</start><end>08:01:51 1/3/2012</end></task><task><id>21</id><task>gardening</task><start>09:24:28 1/3/2012</start><end>11:11:07 1/3/2012</end></task><task><id>22</id><task>helo</task><start>11:31:01 1/3/2012</start><end>12:07:55 1/3/2012</end></task><task><id>31</id><task>hula hop</task><start>19:14:38 1/4/2012</start><end>20:01:12 1/4/2012</end></task><task><id>32</id><task>hula hip</task><start>11:31:01 1/3/2012</start><end>12:07:55 1/3/2012</end></task><task><id>33</id><task>hula hup</task><start>11:31:01 1/3/2012</start><end>12:07:55 1/3/2012</end></task></doc>
**end