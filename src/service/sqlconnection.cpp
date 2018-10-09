
#include "sqlconnection.h"
#include <string.h>
#include "Logger.h"

//////////////////////////////////MySqlConnection/////////////////////////////

MysqlConnection::MysqlConnection() : mysql_(NULL)
{
    mysql_ = (MYSQL*)malloc(sizeof(MYSQL));
}

MysqlConnection::~MysqlConnection()
{
    if (mysql_ != NULL)
    {
        mysql_close(mysql_);
        free (mysql_);
        mysql_ = NULL;
    }
}

bool MysqlConnection::Init(const char* szHost, int nPort, const char* szUser, const char* szPasswd, const char* szDb)
{
	LOG_INFO("enter Init \n");
    if ((mysql_ = mysql_init(mysql_)) == NULL)
    {
        LOG_ERROR("init mysql failed %s, %d", this->GetErrInfo(), errno);
        return false;
    }
#if 0
	char cAuto = 1;
	if (!mysql_options(mysql_, MYSQL_OPT_RECONNECT, &cAuto))
	{
		LOG_INFO("mysql_options MYSQL_OPT_RECONNECT failed.");
	}

	unsigned int uTimeout = 3;
	if (!mysql_options(mysql_, MYSQL_OPT_CONNECT_TIMEOUT, &uTimeout))
	{
		TRACE("mysql_options MYSQL_OPT_CONNECT_TIMEOUT failed.");
	}
#endif

    if (mysql_real_connect(mysql_, szHost, szUser, szPasswd, szDb, nPort, NULL, 0) == NULL)
    {			
        LOG_ERROR("connect mysql failed %s", this->GetErrInfo());
		return false;
    }

    char cAuto = 1;
    if (!mysql_options(mysql_, MYSQL_OPT_RECONNECT, &cAuto))
    {
//        LOG_INFO("mysql_options MYSQL_OPT_RECONNECT failed.");
    }

    return true;
}


//初始化数据库表
bool MysqlConnection::tables_init()
{
	const char * select_table_accountrecords = "select * from accountrecords";
	const char * select_table_bikeinfo = "select * from bikeinfo";
	const char * select_table_travelinfo = "select * from travelinfo";
	const char * select_table_userinfo = "select * from userinfo";

	//accountrecords
	if(mysql_real_query(mysql_, select_table_accountrecords,strlen(select_table_accountrecords)) != 0)
	{
		int ret = mysql_real_query(mysql_,this->table_accountrecords,strlen(this->table_accountrecords));
		if(ret != 0) 
		{
			LOG_DEBUG("创建表 accountrecords 失败, accountrecords已创建好!");
			return false;
		}
		else 
		{
			LOG_DEBUG("创建表 accountrecords 成功!");
		}
	}

	//bikeinfo
	if( mysql_real_query(mysql_, select_table_bikeinfo,strlen(select_table_bikeinfo)) != 0)
	{
		int ret = mysql_real_query(mysql_,  this->table_bikeinfo ,strlen(this->table_bikeinfo));
		if(ret != 0) 
		{
			LOG_DEBUG("创建表 bikeinfo 失败, bikeinfo 已存在!");
			return false;
		}
		else 
		{
			LOG_DEBUG("创建表 bikeinfo 成功!");
		}
	}

	//travelinfo
	if(mysql_real_query(mysql_, select_table_travelinfo ,strlen(select_table_travelinfo)) != 0)
	{
		int ret = mysql_real_query(mysql_,this->table_travelinfo,strlen(this->table_travelinfo));
		if(ret != 0) 
		{
			LOG_DEBUG("创建表 travelinfo 失败,travelinfo 已存在!");
			return false;
		}
		else 
		{
			LOG_DEBUG("创建表 travelinfo 成功!");
		}
	}

	//userinfo
	if(mysql_real_query(mysql_, select_table_userinfo,strlen(select_table_userinfo)) != 0)
	{
		int ret = mysql_real_query(mysql_,this->table_bikeinfo,strlen(this->table_userinfo));
		if(ret != 0) 
		{
			LOG_DEBUG("创建表 userinfo 失败, userinfo 已存在!");
			return false;
		}
		else 
		{
			LOG_DEBUG("创建表 userinfo 成功!");
		}
	}

	return true;
}


bool MysqlConnection::Execute(const char* szSql)
{
	if (mysql_real_query(mysql_, szSql, strlen(szSql)) != 0)
	{
		if (mysql_errno(mysql_) == CR_SERVER_GONE_ERROR)
		{
			Reconnect();
		}
		return false;
	}
	return true;
}

bool MysqlConnection::Execute(const char* szSql, SqlRecordSet& recordSet)
{
    if (mysql_real_query(mysql_, szSql, strlen(szSql)) != 0)
    {
        if (mysql_errno(mysql_) == CR_SERVER_GONE_ERROR)
        {
            Reconnect();
        }

        return false;
    }
    MYSQL_RES* pRes = mysql_store_result(mysql_);
    if (!pRes)
    {
        return NULL;
    }
    recordSet.SetResult(pRes);

    return true;
}

int MysqlConnection::EscapeString(const char* pSrc, int nSrcLen, char* pDest)
{
	if (!mysql_)
	{
		return 0;
	}
	return mysql_real_escape_string(mysql_, pDest, pSrc, nSrcLen);
}

int MysqlConnection::EscapeString(const std::string& strSrc, std::string& strDest)
{
	if (!mysql_)
	{
		return 0;
	}
	char szDest[2048] = {0};
	int nRet = mysql_real_escape_string(mysql_, szDest, strSrc.c_str(), strSrc.size());
	if (nRet <= 0)
	{
		return nRet;
	}
	strDest = std::string(szDest);
	return nRet;
}

void MysqlConnection::Close()
{
    mysql_close(mysql_);
    mysql_ = NULL;
    LOG_INFO("disconnect with mysql.");
}

const char* MysqlConnection::GetErrInfo()
{
    return mysql_error(mysql_);
}

void MysqlConnection::Reconnect()
{
    mysql_ping(mysql_);
}

//////////////////////////////////MySqlConnection END/////////////////////////////
