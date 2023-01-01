#include "sqlite.h"

SQLITE::SQLITE(void)
{
	InitSqlite();
}

void SQLITE::InitSqlite(void)
{
	wchar_t path[MAX_PATH] = { 0 };
	std::string str;
	USES_CONVERSION;

	GetModuleFileName(NULL, path, MAX_PATH);
		
	str = W2A(path);
	str = str.substr(0, str.find_last_of("\\/"));
	str.append("\\");
	str.append(SQLITE_DATABASE_NAME);

	m_strDBName = str;

	if (_access(str.c_str(), 0) != -1)
	{
		return;
	}

	CreateTableSqlite(&this->m_db, str.c_str(), SQLITE_TABLE_NAME_INSTANCE_ID, SQLITE_CREATE_TABLE_INSTANCE_ID);
	CreateTableSqlite(&this->m_db, str.c_str(), SQLITE_TABLE_NAME_POLICY_LIST, SQLITE_CREATE_TABLE_POLICY_LIST);
	CreateTableSqlite(&this->m_db, str.c_str(), SQLITE_TABLE_NAME_RESULT, SQLITE_CREATE_TABLE_RESULT);

	std::cout << "Table creation succeeded. Please restart when you are ready." << std::endl;
	exit(0);
}

int SQLITE::CreateTableSqlite(sqlite3** db, const char* strDBName, const char* strTBName, const char* strQuery)
{
	int err = 0;
	char* err_msg = NULL;

	if ((err = OpenSqlite(db, strDBName)) != SQLITE_OK)
	{
		std::cout << "Cannot open database: " << strDBName << "(" << sqlite3_errmsg(*db) << ")" << std::endl;
		goto END;
	}

	if ((err = sqlite3_exec(*db, strQuery, 0, 0, &err_msg)) != SQLITE_OK)
	{
		std::cout << "Failed to create table: " << strTBName << "(" << sqlite3_errmsg(*db) << ")" << std::endl;
		sqlite3_free(err_msg);
	}

END:
	CloseSqlite(db);
	return err;
}

int SQLITE::OpenSqlite(sqlite3 **db, const char* strDBName)
{
	return sqlite3_open(strDBName, db);
}

void SQLITE::CloseSqlite(sqlite3** db)
{
	sqlite3_close(*db);
}

SQLITE::~SQLITE(void)
{

}

Aws::Vector<Aws::String> SQLITE::SelectSqlite(sqlite3** db, std::string& sql)
{
	int err = 0;
	Aws::Vector<Aws::String> vector;
	sqlite3_stmt* stmt;

	if ((err = OpenSqlite(db, m_strDBName.c_str())) != SQLITE_OK)
	{
		std::cout << "Cannot open database: " << m_strDBName.c_str() << "(" << sqlite3_errmsg(*db) << ")" << std::endl;
		return vector;
	}

	err = sqlite3_prepare_v2(*db, sql.c_str(), -1, &stmt, 0);

	if (err == SQLITE_OK)
	{
		while(true)
		{
			err = sqlite3_step(stmt);

			if (err == SQLITE_ROW)
			{
				if (sqlite3_column_text(stmt, 0) == NULL)
				{
					break;
				}

				vector.push_back((const char*)sqlite3_column_text(stmt, 0));
			}
			else
			{
				break;
			}
		}
	}

	CloseSqlite(db);
	return vector;
}

Aws::Vector<Aws::Vector<Aws::String>> SQLITE::SelectsSqlite(sqlite3** db, std::string& sql)
{
	Aws::Vector<Aws::Vector<Aws::String>> vector;
	sqlite3_stmt* stmt;

	if (OpenSqlite(db, m_strDBName.c_str()) != SQLITE_OK)
	{
		std::cout << "Cannot open database: " << m_strDBName.c_str() << "(" << sqlite3_errmsg(*db) << ")" << std::endl;
		return vector;
	}

	if (sqlite3_prepare_v2(*db, sql.c_str(), -1, &stmt, 0) == SQLITE_OK)
	{
		while (true)
		{
			if (sqlite3_step(stmt) == SQLITE_ROW)
			{
				Aws::Vector<Aws::String> row;

				for (int i = 0;; i++)
				{
					if (sqlite3_column_text(stmt, i) == NULL)
					{
						break;
					}

					row.push_back((const char*)sqlite3_column_text(stmt, i));
				}

				vector.push_back(row);
			}
			else
			{
				break;
			}
		}
	}

	CloseSqlite(db);
	return vector;
}

Aws::Vector<Aws::String> SQLITE::SelectInstanceIds(void)
{
	std::string sql = "SELECT INSTANCE_ID FROM INSTANCE_ID";
	return SelectSqlite(&this->m_db, sql);
}

Aws::Vector<Aws::String> SQLITE::SelectInstanceIdSeq(Aws::String strInstanceId)
{
	std::string sql = "SELECT SEQ FROM INSTANCE_ID WHERE INSTANCE_ID = '";
	sql.append(strInstanceId + "'");
	return SelectSqlite(&this->m_db, sql);
}

Aws::Vector<Aws::String> SQLITE::SelectCommands(void)
{
	std::string sql = "SELECT COMMAND_REQUEST FROM POLICY_LIST";
	return SelectSqlite(&this->m_db, sql);
}

Aws::Vector<Aws::Vector<Aws::String>> SQLITE::SelectsCommands(void)
{
	std::string sql = "SELECT * FROM POLICY_LIST";
	return SelectsSqlite(&this->m_db, sql);
}

int SQLITE::InsertSqlite(sqlite3** db, std::string& sql)
{
	int err = 0;
	char* err_msg = NULL;

	if (OpenSqlite(db, m_strDBName.c_str()) != SQLITE_OK)
	{
		std::cout << "Cannot open database: " << m_strDBName.c_str() << "(" << sqlite3_errmsg(*db) << ")" << std::endl;
		goto END;
	}

	if (sqlite3_exec(*db, sql.c_str(), 0, 0, &err_msg) != SQLITE_OK)
	{
		std::cout << "Sql insert query failed: " << sql << "(" << sqlite3_errmsg(*db) << ")" << std::endl;
		sqlite3_free(err_msg);
	}

END:
	CloseSqlite(db);
	return err;
}

int SQLITE::InsertResult(std::string& sql)
{
	return InsertSqlite(&this->m_db, sql);
}