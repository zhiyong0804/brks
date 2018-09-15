
#ifndef BRKS_SVR_BIKE_SERVICE_H_
#define BRKS_SVR_BIKE_SERVICE_H_

#include "glo_def.h"
#include "sqlconnection.h"
#include "events_def.h"

#include <memory>

/* 单车的devno是纯数字编码  */


enum bike_status {
    BIKE_ST_LOCK   = 0,
    BIKE_ST_UNLOCK = 1,
    BIKE_ST_DAMANGE = 2,

    BIKE_ST_UNKNOW = 0xFF
};

enum bike_type {
    BIKE_TYPE_MANUAL     = 0,
    BIKE_TYPE_POWER_AUTO = 1,

    BIKE_TYPE_UNKNOW     = 0xFF
};

class Bike
{
public:
    Bike(int devno, const std::string& mobile);
    Bike(int devno, const std::string& mobile, bike_type type);
    Bike(int devno, const std::string& mobile, bike_type type, bike_status st, int trouble, const std::string& tmsg);
    Bike(int devno, const std::string& mobile, bike_type type, bike_status st, int trouble, const std::string& tmsg,
        double latitude, double longitude, u64 unlock_tm);

    int            id_;
    bike_status    st_;        // 单车状态
    int            devno_;     // 设备编码
    int            trouble_;   // 故障种类
    std::string    tmsg_;      // 故障信息
    double         latitude_;  // 纬度
    double         longitude_; // 经度
    bike_type      type_;      // 单车类型
    std::string    mobile_;    // 当前骑行的用户
    u64            unlock_tm_; // 解锁时间
};

class BikeService
{
public:
    BikeService(std::shared_ptr<MysqlConnection> sql_conn);
    bool insert(const Bike& bk);

    /* 报销车辆 */
    bool remove(int devno);

    /* 省去了上传图片的需求 */
    bool report_damage(int devno, int trouble, const std::string& tmsg);

    bool get_bike(int devno, Bike& bk);
    bool lock(Bike& bk);
    bool unlock(Bike& bk);

    bool insert_travel_record(const std::string& mobile, int type, double mileage,
                              double discharge, double calorie, u64 startTimeStamp, i32 duration, i32 amount);

    bool list_travel_records(const std::string& mobile, TravelInfo& travel);

    bool get_current_stmp(u64& stmp);
private:
    std::shared_ptr<MysqlConnection> sql_conn_;
};

#endif

