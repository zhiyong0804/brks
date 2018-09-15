#ifndef	BRKS_COMMON_DATASEVER_SQLTABLES_H
#define BRKS_COMMON_DATASEVER_SQLTABLES_H

#include "sqlconnection.h"
#include "Logger.h"
#include <iostream>

using namespace std;

class SqlTables
{
public:
	SqlTables(std::shared_ptr<MysqlConnection> sqlConn) : sqlconn_(sqlConn)
	{
	}

	bool CreateUserInfo()
	{
		const char* pUserInfoTable = " \
								 CREATE TABLE IF NOT EXISTS userinfo( \
								 id			  int(16)		     NOT NULL primary key auto_increment, \
								 mobile		  varchar(16)		 NOT NULL default '1300000000', \
								 username     varchar(128)	     NOT NULL default '', \
								 verify		  int(4)             NOT NULL default '0', \
								 registertm	  timestamp          NOT NULL default CURRENT_TIMESTAMP, \
                                 money        int(4)             NOT NULL default 0, \
								 INDEX        mobile_index(mobile) \
								 )";

		if (!sqlconn_->Execute(pUserInfoTable))
		{
			LOG_ERROR("create table userinfo table failed. error msg:%s", sqlconn_->GetErrInfo());
			return false;
		}

		return true;
	}

	bool CreateBikeTable()
	{
		const char* pBikeInfoTable = "\
				          CREATE TABLE IF NOT EXISTS bikeinfo( \
				          id		   int               NOT NULL primary key auto_increment, \
				          devno        int               NOT NULL, \
				          status       tinyint(1)        NOT NULL default 0, \
                          trouble      int               NOT NULL default 0, \
                          tmsg         varchar(256)      NOT NULL default '',\
                          latitude     double(10,6)      NOT NULL default 0, \
                          longitude    double(10,6)      NOT NULL default 0, \
                          unique(devno)  \
                          )";

		if (!sqlconn_->Execute(pBikeInfoTable))
		{
			LOG_ERROR("create table bikeinfo table failed. error msg:%s" , sqlconn_->GetErrInfo());
			return false;
		}
		return true;
	}

	bool CreateAccountRecordsTable()
	{
		const char* pAccountRecordsTable = " \
									   CREATE TABLE IF NOT EXISTS accountrecords( \
									   id		 int(16)		  NOT NULL primary key auto_increment, \
									   mobile	 int(16)		  NOT NULL default 0, \
									   stmp		 timestamp        NOT NULL default CURRENT_TIMESTAMP, \
									   type      tinyint(1)       NOT NULL default 0, \
                                       money     int              NOT NULL default 0 \
									   )";

		if (!sqlconn_->Execute(pAccountRecordsTable))
		{
			LOG_ERROR("create table account records table failed. error msg:%s", sqlconn_->GetErrInfo());
			return false;
		}
		return true;
	}

	bool CreateTravelRecordsInfo()
	{
		const char* pTravelRecordsInfoTable = "CREATE TABLE IF NOT EXISTS travelinfo( \
				id			 int(16)	  NOT NULL primary key auto_increment, \
				userid		 int(16)	  NOT NULL default 0, \
				type         tinyint(1)   NOT NULL default 0, \
                mileage      double       ,\
                discharge    double       ,\
                calorie      double       ,\
                stmp         timestamp    NOT NULL default 19700101080001, \
                duration     int(4)       NOT NULL default 0, \
                amount       int(1)       NOT NULL default 0 \
				)";

		if (!sqlconn_->Execute(pTravelRecordsInfoTable))
		{
			LOG_ERROR("create table arenainfo failed. error msg:%s" , sqlconn_->GetErrInfo());
			return false;
		}
		return true;
	}

private:
	std::shared_ptr<MysqlConnection> sqlconn_;
};

#endif

