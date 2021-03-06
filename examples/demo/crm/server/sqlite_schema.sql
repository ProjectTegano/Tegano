-- Picture:
CREATE TABLE Picture	(
	id		INTEGER	PRIMARY KEY AUTOINCREMENT,
	caption		TEXT,
	info		TEXT,
	width		INT,
	height		INT,
	image		TEXT,
	thumbnail	TEXT
);


-- Employee:
CREATE TABLE Employee
(
	id		INTEGER	PRIMARY KEY AUTOINCREMENT,
	name		TEXT	NOT NULL,
	status		TEXT	NOT NULL,
	addr_line1	TEXT,
	addr_line2	TEXT,
	addr_postalcode	TEXT,
	addr_district	TEXT,
	addr_country	TEXT,
	CONSTRAINT tag_name_check UNIQUE( name )
);

-- Customer:
CREATE TABLE Customer
(
	id		INTEGER	PRIMARY KEY AUTOINCREMENT,
	name		TEXT	NOT NULL,
	description	TEXT,
	addr_line1	TEXT,
	addr_line2	TEXT,
	addr_postalcode	TEXT,
	addr_district	TEXT,
	addr_country	TEXT
);

-- CustomerPicture:
CREATE TABLE CustomerPicture
(
	pictureid	INT	REFERENCES Picture( id ),
	customerid	INT	REFERENCES Customer( id ),
	UNIQUE ( pictureid, customerid )
);

-- CustomerRelation:
CREATE TABLE CustomerRelation
(
	employeeid	INT	REFERENCES Employee( id ),
	customerid	INT	REFERENCES Customer( id ),
	description	TEXT,
	UNIQUE ( employeeid, customerid )
);

-- CustomerNote:
CREATE TABLE CustomerNote
(
	id		INTEGER	PRIMARY KEY AUTOINCREMENT,
	customerid	INT	REFERENCES Customer( id ),
	content		TEXT,
	inserted	DATETIME,
	link		TEXT		-- URI of an attachment (for the time being, maybe as concept too weak)
);

-- Project:
CREATE TABLE Project
(
	id		INTEGER	PRIMARY KEY AUTOINCREMENT,
	customerid	INT	REFERENCES Customer( id ),	-- a project is always atached to a customer (also internal projects)
	name		TEXT	NOT NULL,
	description	TEXT
);

-- ProjectNote:
CREATE TABLE ProjectNote
(
	id		INTEGER	PRIMARY KEY AUTOINCREMENT,
	projectid	INT	REFERENCES Project( id ),
	content		TEXT,
	inserted	DATETIME,
	link		TEXT		-- URI of an attachment (for the time being, maybe as concept too weak)
);

-- ActivityCategory: Activity Categorization. Defines the type of the activity and determines how it is charged:
CREATE TABLE ActivityCategory
(
	id		INTEGER	PRIMARY KEY AUTOINCREMENT,
	name		TEXT	NOT NULL,
	description	TEXT
);

-- ActivityCharging: How activities of a certain category are charged for the customer
CREATE TABLE ActivityCharging
(
	id		INTEGER	PRIMARY KEY AUTOINCREMENT,
	categoryid	INT	REFERENCES ActivityCategory( id ),
	start		DATETIME,
	fixum		INT,			-- immutable part of the activity costs
	timedep		INT,			-- time dependent part of the activity costs
	timeunit	INT,			-- time of charging of the time dependent part of the activity costs
	taxes		INT			-- taxes (percent)
);

-- Activity: Activities for a project
CREATE TABLE Activity
(
	id		INTEGER	PRIMARY KEY AUTOINCREMENT,
	categoryid	INT	REFERENCES ActivityCategory( id ),
	projectid	INT	REFERENCES Project( id ),
	employeeid	INT	REFERENCES Employee( id ),
	chargingid	INT	REFERENCES ActivityCharging( id ),
	description	TEXT,
	start		DATETIME,
	duration	UINT
);

-- ActivityNote:
CREATE TABLE ActivityNote
(
	id		INTEGER	PRIMARY KEY AUTOINCREMENT,
	activityid	INT	REFERENCES Activity( id ),
	content		TEXT,
	inserted	DATETIME,
	link		TEXT		-- URI of an attachment (for the time being, maybe as concept too weak)
);


