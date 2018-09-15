
#ifndef BRKS_COMMON_EVENTS_DEF_H_
#define BRKS_COMMON_EVENTS_DEF_H_

#include "event.h"
#include "glo_def.h"
#include "eventtype.h"

#include <string>
#include <vector>

class MobileCodeReqEv : public iEvent
{
public:
    MobileCodeReqEv(const std::string& mobile) :
        iEvent(EEVENTID_GET_MOBILE_CODE_REQ, iEvent::generateSeqNo()), mobile_(mobile)
    {};

    const std::string& get_mobile(){return mobile_;};
    virtual std::ostream& dump(std::ostream& out) const;
private:
    std::string mobile_;
};

class LoginEv : public iEvent
{
public :
	LoginEv(const std::string& mobile, const std::string& code) :
        iEvent(EEVENTID_LOGIN_REQ, iEvent::generateSeqNo()), mobile_(mobile), code_(code)
	{};

    const std::string& get_mobile(){return mobile_;};
    const std::string& get_code(){return code_;};
    virtual std::ostream& dump(std::ostream& out) const;
private :
    std::string mobile_;
    std::string code_;
};

class CommonRspEv : public iEvent
{
public:
    CommonRspEv(i32 code, const std::string& msg, const std::string& data) :
        iEvent(EEVENTID_COMMON_RSP, generateSeqNo()), code_(code), msg_(msg), data_(data)
	{};

    i32 get_code(){return code_;};
    const std::string& get_msg(){return msg_;};
    const std::string& get_data(){ return data_;};
    virtual std::ostream& dump(std::ostream& out) const;

private:
    i32           code_;  //杩斿洖浠ｇ爜
    std::string   msg_;   //杩斿洖娑堟伅锛屽浠ｇ爜鍦ㄥ叿浣撲笟鍔″満鏅笅鐨勮В閲?
    std::string   data_;  //杈呭姪鏁版嵁
};

class RechargeEv : public iEvent
{
public :
	RechargeEv(const std::string& mobile, i32 amount) :
        iEvent(EEVENTID_RECHARGE_REQ, generateSeqNo()), mobile_(mobile), amount_(amount)
	{};

    std::string& get_mobile(){return mobile_;};
    i32 get_amount(){return amount_;};
    virtual std::ostream& dump(std::ostream& out) const;
private :
    std::string mobile_;
    i32 amount_;
};

class GetAccountBalanceEv : public iEvent
{
public :
	GetAccountBalanceEv(const std::string& mobile) :
        iEvent(EEVENTID_GET_ACCOUNT_BALANCE_REQ, generateSeqNo()), mobile_(mobile)
	{};

    std::string& get_mobile(){return mobile_;};
    virtual std::ostream& dump(std::ostream& out) const;
private :
    std::string mobile_;
};

class GetAccountBalanceRspEv : public CommonRspEv
{
public :
    GetAccountBalanceRspEv(i32 code, const std::string& msg, const std::string& data, i32 balance) :
        CommonRspEv(code, msg, data), balance_(balance)
    {
        set_eid(EEVENTID_BALANCE_RSP);
    };

    i32& get_balance(){return balance_;};
    virtual std::ostream& dump(std::ostream& out) const;
private :
    i32 balance_;
};

class UnlockEv : public iEvent
{
public :
	UnlockEv(const std::string& mobile, const std::string& bike_code) :
        iEvent(EEVENTID_UNLOCK_REQ, generateSeqNo()), mobile_(mobile), bike_code_(bike_code)
	{};

    std::string& get_mobile(){return mobile_;};
    std::string& get_bike_code(){ return bike_code_;};
    virtual std::ostream& dump(std::ostream& out) const;
private :
    std::string mobile_;
    std::string bike_code_;
};

class LockEv : public iEvent
{
public :
	LockEv(const std::string& mobile, const std::string& bike_code) :
        iEvent(EEVENTID_LOCK_REQ, generateSeqNo()), mobile_(mobile), bike_code_(bike_code)
	{};

    std::string& get_mobile(){return mobile_;};
    std::string& get_bike_code(){ return bike_code_;};
    virtual std::ostream& dump(std::ostream& out) const;
private :
    std::string mobile_;
    std::string bike_code_;
};

typedef struct TravelRecord{
    u64 startTimeStamp;  //璧峰鏃堕棿
    i32 duration;        //楠戣鏃堕暱
    i32 amount;          //鎵�鑰楅噾棰?

    TravelRecord(u64 stmp, i32 duration, i32 amount)
    {
        this->startTimeStamp = stmp;
        this->duration       = duration;
        this->amount         = amount;
    };

    TravelRecord& operator =(const TravelRecord& tr)
    {
        if (this != &tr)
        {
            this->startTimeStamp = tr.startTimeStamp;
            this->duration       = tr.duration;
            this->amount         = tr.amount;
        }

        return *this;
    };
}TravelRecord_;

typedef struct TravelInfo
{
    double                 mileage;   // 里程
    double                 discharge; // 排放
    double                 calorie;   // 卡洛里
    std::vector<TravelRecord>  records;
    TravelInfo()
    {
    }

    TravelInfo(double mileage, double discharge, double calorie, std::vector<TravelRecord> records)
    {
        this->mileage   = mileage;
        this->discharge = discharge;
        this->calorie   = calorie;
        this->records   = records;
    };

    TravelInfo& operator =(const TravelInfo& ti)
    {
        if (this != &ti)
        {
            this->mileage   = ti.mileage;
            this->discharge = ti.discharge;
            this->calorie   = ti.calorie;
            this->records   = ti.records;
        }
        return *this;
    }
}TravelInfo_;

class LockResultRspEv : public CommonRspEv
{
public :
	LockResultRspEv(i32 code, const std::string& msg, const std::string& data, const TravelInfo& ti) :
        CommonRspEv(code, msg, data), ti_(ti)
	{
        set_eid(EEVENTID_LOCK_RSP);
    };
    const TravelInfo& get_travel_info(){ return ti_;};
    virtual std::ostream& dump(std::ostream& out) const;

private :
    TravelInfo ti_;
};

class ListAccountRecordsReqEv : public iEvent
{
public:
    ListAccountRecordsReqEv(const std::string& mobile) :
        iEvent(EEVENTID_LIST_ACCOUNT_RECORDS_REQ, generateSeqNo()), mobile_(mobile)
	{};

    const std::string& get_mobile(){ return mobile_;};
    virtual std::ostream& dump(std::ostream& out) const;

private:
    std::string mobile_;
};

class ListTravelRecordsReqEv : public iEvent
{
public:
    ListTravelRecordsReqEv(const std::string& mobile) :
        iEvent(EEVENTID_LIST_TRAVELS_REQ, generateSeqNo()), mobile_(mobile)
	{};

    const std::string& get_mobile(){ return mobile_;};
    virtual std::ostream& dump(std::ostream& out) const;

private:
    std::string mobile_;
};

typedef struct AccountRecord
{
    i16 type;   // 0 : 骑行消费,  1 : 充值, 2 : 退款
    i32 limit;  // 消费或者充值金额
    u64 timestamp;  //记录发生时的时间戳

    AccountRecord(i16 type, i32 limit, u64 timestamp)
    {
        this->type      = type;
        this->limit     = limit;
        this->timestamp = timestamp;
    };

    AccountRecord& operator =(const AccountRecord& ar)
    {
        if (this != &ar)
        {
            this->type      = ar.type;
            this->limit     = ar.limit;
            this->timestamp = ar.timestamp;
        }
        return *this;
    }
}AccountRecord_;

/*
 * 账户消费记录返回消息
 */
class ListAccountRecordsRspEv : public CommonRspEv
{
public:
    ListAccountRecordsRspEv(i32 code, const std::string& msg, const std::string& data, const std::vector<AccountRecord>& records) :
        CommonRspEv(code, msg, data), records_(records)
	{
        set_eid(EEVENTID_ACCOUNT_RECORDS_RSP);
    };

    const std::vector<AccountRecord>& get_records(){ return records_;};
    virtual std::ostream& dump(std::ostream& out) const;
private:
    std::vector<AccountRecord> records_;
};

/*
 * 骑行记录查询结果
 */
class ListTravelRecordsRspEv : public CommonRspEv
{
public:
    ListTravelRecordsRspEv(i32 code, const std::string& msg, const std::string& data, TravelInfo& ti) :
        CommonRspEv(code, msg, data), ti_(ti)
	{
        set_eid(EEVENTID_LIST_TRAVELS_RSP);
    };

    const TravelInfo& get_travel_info(){ return ti_;};
    virtual std::ostream& dump(std::ostream& out) const;
private:
    TravelInfo ti_;
};

#endif

