#pragma once
#ifndef SQLITE_H
#define SQLITE_H

#include "CloudAws.h"
#include "sqlite3.h"
#include "stdafx.h"

#define SQLITE_DATABASE_NAME "aws.db"

#define SQLITE_TABLE_NAME_INSTANCE_ID "INSTANCE_ID"
#define SQLITE_TABLE_NAME_POLICY_LIST "POLICY_LIST"
#define SQLITE_TABLE_NAME_RESULT "RESULT"

#define SQLITE_CREATE_TABLE_INSTANCE_ID " \
	CREATE TABLE INSTANCE_ID \
	( \
		SEQ INTEGER primary key AUTOINCREMENT, \
		INSTANCE_ID TEXT \
	); "
#define SQLITE_CREATE_TABLE_POLICY_LIST " \
	CREATE TABLE POLICY_LIST \
	( \
		SEQ INTEGER primary key AUTOINCREMENT, \
		POLICY_NM TEXT, \
		COMMAND_REQUEST TEXT, \
		PROBE_SEQ INTEGER, \
		VALUE_EXISTS INTEGER, \
		ARGS TEXT, \
		DESCRIPTION TEXT, \
		MEASURE TEXT \
	); "
#define SQLITE_CREATE_TABLE_RESULT " \
	CREATE TABLE RESULT \
	( \
		SEQ INTEGER primary key AUTOINCREMENT, \
		INSTANCE_ID_SEQ INTEGER, \
		POLICY_SEQ INTEGER, \
		COMMAND_RESULT TEXT, \
		INSPECTION_RESULT TEXT, \
		FOREIGN KEY(INSTANCE_ID_SEQ) REFERENCES INSTANCE_ID(SEQ), \
		FOREIGN KEY(POLICY_SEQ) REFERENCES POLICY_LIST(SEQ) \
	); "

enum POLICY_LIST_COLUMN
{
	POLICY_LIST_COLUMN_SEQ = 0,
	POLICY_LIST_COLUMN_POLICY_NM,
	POLICY_LIST_COLUMN_COMMAND_REQUEST,
	POLICY_LIST_COLUMN_PROBE_SEQ,
	POLICY_LIST_COLUMN_VALUE_EXISTS,
	POLICY_LIST_COLUMN_ARGS,
	POLICY_LIST_COLUMN_DESCRIPTION,
	POLICY_LIST_COLUMN_MEASURE,
	POLICY_LIST_COLUMN_LAST // PROBE.H INSTANCE_ID
};

enum VALUE_EXISTS
{
	VALUE_NOT_EXISTS = 0,
	VALUE_EXISTS
};

class SQLITE
{
	/* function */
public:
	SQLITE(void);
	~SQLITE(void);

	int CreateTableSqlite(sqlite3** db, const char* strDBName, const char* strTBName, const char* strQuery);
	int OpenSqlite(sqlite3** db, const char* strDBName);
	void CloseSqlite(sqlite3** db);
	Aws::Vector<Aws::Vector<Aws::String>> SelectsSqlite(sqlite3** db, std::string& sql);
	int InsertResult(std::string& sql);

	Aws::Vector<Aws::String> SelectInstanceIds(void);
	Aws::Vector<Aws::String> SelectInstanceIdSeq(Aws::String strInstanceId);
	Aws::Vector<Aws::String> SelectCommands(void);
	Aws::Vector<Aws::Vector<Aws::String>> SelectsCommands(void);

private:
	void InitSqlite(void);
	Aws::Vector<Aws::String> SelectSqlite(sqlite3** db, std::string& sql);
	int InsertSqlite(sqlite3** db, std::string& sql);

	/* variable */
private:
	sqlite3* m_db;
	std::string m_strDBName;
};

#endif /* SQLITE_H */