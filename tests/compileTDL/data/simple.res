TRANSACTION simple
BEGIN
Code:
[0] GOTO @1
[1] RESULT_SET_INIT
[2] DBSTM_START STM (INSERT INTO tab (name) VALUES ('hugo'))
[3] DBSTM_EXEC
[4] RETURN
END

