TRANSACTION main
BEGIN
Code:
[0] GOTO @3
[1] GOTO @2
[2] RETURN
[3] RESULT_SET_INIT
[4] SUB_FRAME_OPEN SIGNATURE 0
[5] SUB_FRAME_CLOSE
[6] GOTO @1
[7] RETURN
Subroutine Signatures:
SUB 0 (  )
END

