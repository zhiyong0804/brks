
#include "bike_service.h"
#include <time.h>


Bike::Bike(int devno, const std::string& mobile) :
    devno_(devno), mobile_(mobile), type_(BIKE_TYPE_UNKNOW), st_(BIKE_ST_UNKNOW), trouble_(0xFFFFFFFF), tmsg_(""),
    latitude_(0), longitude_(0), unlock_tm_(0)
{
}


Bike::Bike(int devno, const std::string& mobile, bike_type type) :
    devno_(devno), mobile_(mobile), type_(type), st_(BIKE_ST_UNKNOW), trouble_(0xFFFFFFFF), tmsg_(""),
    latitude_(0), longitude_(0), unlock_tm_(0)
{
}

Bike::Bike(int devno, const std::string& mobile, bike_type type, bike_status st, int trouble, const std::string& tmsg) :
    devno_(devno), mobile_(mobile), type_(type), st_(st), trouble_(trouble), tmsg_(tmsg),
    latitude_(0), longitude_(0), unlock_tm_(0)

{

}

Bike::Bike(int devno, const std::string& mobile,bike_type type, bike_status st, int trouble, const std::string& tmsg,
    double latitude, double longitude, u64 unlock_tm) :
    devno_(devno), mobile_(mobile), type_(type), st_(st), trouble_(trouble), tmsg_(tmsg),
    latitude_(latitude), longitude_(longitude), unlock_tm_(unlock_tm)
{

}

BikeService::BikeService(std::shared_ptr<MysqlConnection> sql_conn) : sql_conn_(sql_conn)
{

}

bool BikeService::insert(const Bike& bk)//int devno, bike_type type, bike_status st)
{
    char sql_content[1024] = {0};
    sprintf(sql_content, \
        "insert into bikeinfo (devno type status) values (%d %d %d)", \
        bk.devno_, bk.type_, bk.st_);

    return sql_conn_->Execute(sql_content);
}

bool BikeService::remove(int devno)
{
    char sql_content[1024] = {0};
    sprintf(sql_content, \
        "delete from bikeinfo where devno = %d ", \
        devno);

    return sql_conn_->Execute(sql_content);
}

bool BikeService::get_bike(int devno, Bike& bk) //, const std::string& mobile, bike_status& st)
{
    char sql_content[1024] = {0};
    sprintf(sql_content, \
        "select id, status, trouble, tmsg, latitude, longitude, devno, type, mobile, unix_timestamp(sstmp) from bikeinfo where devno = %d ", \
        devno);

    SqlRecordSet record_set;
    if (!sql_conn_->Execute(sql_content, record_set))
    {
        LOG_ERROR("excute '%s' failed.", sql_content);
        return false;
    }

    if (record_set.GetRowCount() == 0)
    {
        LOG_ERROR("excute '%s' but no any result.", sql_content);
        return false;
    }

    MYSQL_ROW row;
    record_set.FetchRow(row);

    bk.st_        = atoi(row[1]);  // 获取状态码
    bk.trouble_   = atoi(row[2]);
    bk.tmsg_      = std::string(row[3]);
    bk.latitude_  = atof(row[4]);
    bk.longitude_ = atof(row[5]);
    bk.devno_     = atoi(row[6]);
    bk.type_      = atoi(row[7]);
    bk.mobile_    = std::string(row[8]); // 获取mobile字符串
    bk.unlock_tm_ = atoll(row[9]);

    LOG_DEBUG("bike value is : st=%d,trouble=%d,tmsg=%s,latitude=%.6f,longitude=%.6f,devno=%d,type=%d,mobile=%s, unlock_tm = %d",
        bk.st_, bk.trouble_, bk.tmsg_.c_str(), bk.latitude_, bk.longitude_, bk.devno_, bk.type_, bk.mobile_.c_str(), bk.unlock_tm_);

    return true;
}

bool BikeService::report_damage(int devno, int trouble, const std::string& tmsg)
{
    char sql_content[1024] = {0};
    sprintf(sql_content, \
        "update bikeinfo set trouble = %d, tmsg=%s where devno=%d", \
        trouble, tmsg.c_str(), devno);

    if (!sql_conn_->Execute(sql_content))
    {
        return false;
    }

    return true;
}

bool BikeService::lock(Bike& bk)//int devno, const std::string& mobile)
{
    char sql_content[1024] = {0};
    sprintf(sql_content, \
        "update bikeinfo set status = %d, mobile = \"%s\" where devno=%d and status = %d", \
        BIKE_ST_LOCK, bk.mobile_.c_str(), bk.devno_, BIKE_ST_UNLOCK);

    if (!sql_conn_->Execute(sql_content))
    {
        LOG_ERROR("excute sql '%s' failed.", sql_content);
        return false;
    }

    return true;
}

bool BikeService::unlock(Bike& bk)//int devno, const std::string& mobile)
{
    char sql_content[1024] = {0};
    sprintf(sql_content, \
        "update bikeinfo set status = %d, mobile = \"%s\",sstmp = CURRENT_TIMESTAMP where devno=%d and status = %d", \
        BIKE_ST_UNLOCK, bk.mobile_.c_str(), bk.devno_, BIKE_ST_LOCK);

    if (!sql_conn_->Execute(sql_content))
    {
        LOG_ERROR("excute sql '%s' failed.", sql_content);
        return false;
    }

    return true;
}

bool BikeService::BikeService::insert_travel_record(const std::string& mobile,
    int type,
    double mileage,
    double discharge,
    double calorie,
    u64 startTimeStamp,
    i32 duration,
    i32 amount)
{
    char sql_content[1024] = {0};
    sprintf(sql_content, \
        "insert into travelinfo (mobile, type, mileage, discharge, calorie, stmp, duration, amount) " \
        "values (\"%s\", %d, %.2f, %.2f, %.2f, %lld, %d, %d)", \
        mobile.c_str(), type, mileage, discharge, calorie, startTimeStamp, duration, amount);

    if (!sql_conn_->Execute(sql_content))
    {
        LOG_ERROR("excute sql '%s' failed.", sql_content);
        return false;
    }

    LOG_INFO("excute sql '%s' success.", sql_content);

    return true;
}

bool BikeService::list_travel_records(const std::string& mobile, TravelInfo& travel)
{
    char sql_content[1024] = {0};
    sprintf(sql_content, \
            "select * from travelinfo where mobile=%s", \
            mobile.c_str());
    SqlRecordSet record_set;

    if(!sql_conn_->Execute(sql_content, record_set))
    {
        LOG_ERROR("excute sql '%s' failed.", sql_content);
        return false;
    }

    double mileage = 0;   // 里程
    double discharge = 0; // 排放
    double calorie = 0;   // 卡洛里
    std::vector<TravelRecord> records;

    i32 row_count = record_set.GetRowCount();
    for (int i = 0; i < row_count; i++)
    {
        MYSQL_ROW row;
        record_set.FetchRow(row);

        mileage   += atof(row[3]);
        discharge += atof(row[4]);
        calorie   += atof(row[5]);
        u64 startTimeStamp = atoll(row[6]);
        i32 duration = atoi(row[7]);
        i32 amount = atoi(row[8]);
        TravelRecord record(startTimeStamp, duration, amount);
        records.push_back(record);
    }

    travel.mileage   = mileage;
    travel.discharge = discharge;
    travel.calorie   = calorie;
    travel.records   = records;

    return true;
}

bool BikeService::get_current_stmp(u64& stmp)
{
    char sql_content[1024] = {0};
    sprintf(sql_content, \
        "select unix_timestamp(now())");

    SqlRecordSet record_set;
    if(!sql_conn_->Execute(sql_content, record_set))
    {
        LOG_ERROR("excute sql '%s' failed.", sql_content);
        return false;
    }

    MYSQL_ROW row;
    record_set.FetchRow(row);
    stmp = atoll(row[0]);

    return true;
}


