
CREATE TABLE Customer (
 ID INTEGER NOT NULL PRIMARY KEY,
 name VARCHAR(32) ,
 CONSTRAINT tag_name_check UNIQUE( name )
);
CREATE SEQUENCE Customer_ID_Seq START WITH 1 INCREMENT BY 1;
CREATE TRIGGER Customer_Insert
BEFORE INSERT ON Customer
FOR EACH ROW
BEGIN
	SELECT Customer_ID_Seq.nextval into :new.id FROM dual;
END;
/
