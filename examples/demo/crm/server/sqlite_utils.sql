PREPARE getLastIdPicture AS SELECT DISTINCT last_insert_rowid() FROM Picture;
PREPARE getLastIdEmployee AS SELECT DISTINCT last_insert_rowid() FROM Employee;
PREPARE getLastIdCustomer AS SELECT DISTINCT last_insert_rowid() FROM Customer;
PREPARE getLastIdProject AS SELECT DISTINCT last_insert_rowid() FROM Project;
PREPARE getLastIdActivity AS SELECT DISTINCT last_insert_rowid() FROM Activity;
PREPARE getLastIdActivityCategory AS SELECT DISTINCT last_insert_rowid() FROM Activity;
PREPARE getLastIdActivityNote AS SELECT DISTINCT last_insert_rowid() FROM ActivityNote;
PREPARE getLastIdCustomerNote AS SELECT DISTINCT last_insert_rowid() FROM CustomerNote;
PREPARE getLastIdProjectNote AS SELECT DISTINCT last_insert_rowid() FROM ProjectNote;
