#!/bin/sh
testname=`basename $0 ".tst"`					# name of the test
opt=""
testcmd="--config wolframe.conf schema_select_task_by_id"	# command to execute by the test
docin=schema_select_task_by_id					# input document name
docout=output_schema_select_task_by_id				# output document name
dumpout="program/schema_select_task_by_id.dbdump.txt"		# resource dump to add to expected test output
testdata="
**requires:TEXTWOLF
**requires:DISABLED WIN32
**file: schema_select_task_by_id.sfrm
`cat ../scripts/schema_select_task_by_id.sfrm`
**file: test.dmap
`cat program/schema_select_task_by_id.dmap`
**file:wolframe.conf
`cat ../config/wolframe_unix_include.conf`
**file:database.conf
`cat ../config/database.conf`
**file:daemon.conf
`cat ../config/daemon.conf`
**file:processor.conf
`cat ../config/processor.conf`
**file:pattern1.conf
`cat ../config/pattern1.conf`
**file:pattern2.conf
`cat ../config/pattern2.conf`
**file:pattern3.conf
`cat ../config/pattern3.conf`
**file:logging.conf
`cat ../config/logging.conf`
**file:simpleform.wnmp
`cat program/simpleform.wnmp`
**file: DBDATA
`cat program/schema_select_task_by_id.sql`
**file:DBPRG.tdl
`cat program/schema_select_task_by_id.tdl`
**outputfile:DBDUMP"
csetlist="UTF-8"
. ./output_tst_all.sh
