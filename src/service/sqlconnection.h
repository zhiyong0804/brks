#ifndef DATASTORE_MYSQL_CONNECTION_H
#define DATASTORE_MYSQL_CONNECTION_H

#include <mysql/mysql.h>
#include <string>
#include <mysql/errmsg.h>
#include <assert.h>

#include "glo_def.h"
#include "Logger.h"



class SqlRecordSet
{
public:
	SqlRecordSet() : m_pRes(NULL)
	{

	}
	explicit SqlRecordSet(MYSQL_RES* pRes)
	{
		m_pRes = pRes;
	}
	MYSQL_RES* MysqlRes()
	{
		return m_pRes;
	}
	~SqlRecordSet()
	{
		mysql_free_result(m_pRes);
	}
	inline void SetResult(MYSQL_RES* pRes)
	{
		assert(m_pRes == NULL);		// 如果此时已经保存了结果集了，那么应该让程序报错，防止内存泄露
		if (m_pRes)					
		{
			LOG_WARN("the MYSQL_RES has already stored result, maybe will cause memory leak");
		}
		m_pRes = pRes;
	}
	inline MYSQL_RES* GetResult()
	{
		return m_pRes;
	}
	void FetchRow(MYSQL_ROW& row)
	{
		row = mysql_fetch_row(m_pRes);
	}
    
    inline i32 GetRowCount()
    {
        return m_pRes->row_count;
    }
private:
	MYSQL_RES* m_pRes;
};

class MysqlConnection
{
public:
	MysqlConnection();
	~MysqlConnection();
	MYSQL* Mysql()
	{
		return mysql_;
	}
	bool Init(const char* szHost, int nPort, const char* szUser, const char* szPasswd, const char* szDb);
	bool tables_init();
	bool Execute(const char* szSql);
	bool Execute(const char* szSql, SqlRecordSet& recordSet);
	int EscapeString(const char* pSrc, int nSrcLen, char* pDest);
	int EscapeString(const std::string& strSrc, std::string& strDest);
	void Close();
	const char* GetErrInfo();
	void Reconnect();
private:
	MYSQL* mysql_;

	//dongnaobike 数据库
	const char * table_accountrecords  = "create table accountrecords ( \
	id  INT(16)    AUTO_INCREMENT primary key, \
	mobile varchar(16) not null, \
	stmp timestamp not null default '1970-01-01 08:00:01', \
	type  tinyint(1)  not null default 0, \
	money int(11) not null default 0 \
	)DEFAULT CHARSET=utf8;";
	
	//bikeinfo表
	const char * table_bikeinfo = "create table bikeinfo ( \
	id int(11) AUTO_INCREMENT primary key, \
	status tinyint(1) not null  default 0, \
	trouble int(11) not null default 0, \
	tmsg  varchar(16), \
	latitude double(10,6) not null default 0.000000, \
	longitude  double(10,6) not null default 0.000000, \
	DEVNO INT(16) UNIQUE DEFAULT NULL, \
	type int(11) not null default 0, \
	mobile  varchar(16) default null, \
	sstmp timestamp not null DEFAULT CURRENT_TIMESTAMP \
	)DEFAULT CHARSET=utf8;";

	//travelinfo 表
	const char * table_travelinfo = "create table travelinfo ( \
	id int(16)  AUTO_INCREMENT primary key, \
	mobile varchar(16)  not null UNIQUE, \
	type tinyint(1)  not null default 0, \
	mileage double default null, \
	discharge double default null, \
	calorie double default null, \
	stmp bigint(20) not null default 0, \
	duration int(4) not null default 0, \
	amount int(1) not null default 0 \
	)DEFAULT CHARSET=utf8;";

	//userinfo 表
	const char * table_userinfo = "create table userinfo ( \
	id int(16)  AUTO_INCREMENT primary key, \
	mobile varchar(16)  not null UNIQUE default 1300000000, \
	username varchar(128) not null default 0, \
	verify int(4) not null default 0, \
	registertm timestamp  not null  DEFAULT CURRENT_TIMESTAMP on update CURRENT_TIMESTAMP, \
	MONEY INT(4) not null default 0 \
	)DEFAULT CHARSET=utf8;";

};

#endif
