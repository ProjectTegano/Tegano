#include "appdevel/databaseModuleMacros.hpp"
#include "appdevel/moduleFrameMacros.hpp"
#include "database/database.hpp"
#include "database/transaction.hpp"
#include "types/variant.hpp"
#include "serialize/descriptiveConfiguration.hpp"
#include <string>

class MyDatabaseConfig
	:public _Wolframe::serialize::DescriptiveConfiguration
{
public:
	const std::string& id() const
	{
		// ... return the identifier of the database as declared in the configuration here
		// ... for convenience the identifier of the database should be configured as 'identifier'
		static const std::string rt( "mydb");
		return rt;
	}

	// ... define your configuration data elements here
	// ... you will find an example of how to define a configuration in a descriptive way at \ref ConfigDescription

	static const _Wolframe::serialize::StructDescriptionBase* getStructDescription()
	{
		// ... return your introspection description reference of the configuration here
		return 0;
	}
	MyDatabaseConfig( const std::string& className_, const std::string& configSection_, const std::string& configKeyword_)
		:_Wolframe::serialize::DescriptiveConfiguration( className_, configSection_, configKeyword_, getStructDescription())
	{
		setBasePtr( (void*)this); // ... mandatory to set pointer to start of configuration
	}
};


class MyDatabase;

class MyTransactionExecStatemachine
	:public _Wolframe::db::TransactionExecStatemachine
{
public:
	MyTransactionExecStatemachine( MyDatabase* /*database*/)
	{
		// ... create a statemachine for one transaction on a database of 'unit' that will start with the next call of begin()
	}

	virtual const std::string& databaseID() const
	{
		static const std::string my_databaseID("mydb");
		return my_databaseID;  // ... return the configuration identifier of your database
	}

	virtual bool begin()
	{
		// ... begin of the current transaction
		return true;
	}

	virtual bool commit()
	{
		// ... commit of the current transaction
		return true;
	}

	virtual bool rollback()
	{
		// ... rollback of the current transaction
		return true;
	}

	virtual bool start( const std::string& /*statement*/)
	{
		// ... create a new statement instance from string
		return true;
	}

	virtual bool bind( std::size_t /*idx*/, const _Wolframe::types::VariantConst& /*value*/)
	{
		// ... bind a parameter (idx >= 1) of the current statement instance
		return true;
	}

	virtual bool execute()
	{
		// ... execute the built instance of the current statement
		return true;
	}

	virtual std::size_t nofColumns()
	{
		// ... get the number of columns of the last result, 0 if there was no result
		return 0;
	}

	virtual const char* columnName( std::size_t /*idx*/)
	{
		// ... get the name of a column by index (idx >= 1) of the last result
		return 0;
	}

	virtual _Wolframe::types::VariantConst get( std::size_t /*idx*/)
	{
		// ... get the value of a column by index (idx >= 1) of the last result
		return _Wolframe::types::VariantConst();
	}

	virtual bool next()
	{
		// ... skip to the next result, return false, if there is no result left
		return true;
	}

	virtual bool hasResult()
	{
		// ... return true, if the last database statement returned at least one result row
		return true;
	}

	virtual const _Wolframe::db::DatabaseError* getLastError()
	{
		// ... return the last database error as structure here
		return 0;
	}

	virtual bool isCaseSensitive()
	{
		// ... return true, if the database language is case sensitive (SQL is case insensitive)
		return true;
	}
};

class MyDatabase
	:public _Wolframe::db::Database
{
public:
	MyDatabase( const MyDatabaseConfig* config)
		:Database("MyDatabase", config->id())
	{
		// ... define your database from configuration here
	}

	virtual _Wolframe::db::Transaction* transaction( const std::string& name_)
	{
		_Wolframe::db::TransactionExecStatemachineR stm( new MyTransactionExecStatemachine( this));
		return new _Wolframe::db::Transaction( name_, stm);
	}

	virtual const _Wolframe::db::LanguageDescription* getLanguageDescription() const
	{
		static _Wolframe::db::LanguageDescriptionSQL langdescr;
		return &langdescr;
	}
};

WF_MODULE_BEGIN( "MyDatabase", "my database module")
 WF_DATABASE( "MyDB", MyDatabase, MyDatabaseConfig)
WF_MODULE_END

