
#include "events_def.h"
#include <iostream>
#include <sstream>

std::ostream& MobileCodeReqEv::dump(std::ostream& out) const
{
	out << "MobileCodeReqEv sn =" << get_sn() << ",";
    out << "mobile=" << mobile_ << std::endl;

	return out;
}

std::ostream& LoginEv::dump(std::ostream& out) const
{
    out << "LoginEv sn =" << get_sn() << ",";
    out << "mobile=" << mobile_ << ",";
    out << "code=" << code_ << std::endl;

    return out;
}

std::ostream& RechargeEv::dump(std::ostream& out) const
{
	out << "RechargeEv sn =" << get_sn() << ",";
    out << "mobile=" << mobile_ << ",";
    out << "amount=" << amount_ << std::endl;

	return out;
}

std::ostream& GetAccountBalanceEv::dump(std::ostream& out) const
{
	out << "GetAccountBalance sn =" << get_sn() << ",";
    out << "mobile=" << mobile_ << std::endl;

	return out;
}

std::ostream& GetAccountBalanceRspEv::dump(std::ostream& out) const
{
	out << "GetAccountBalance sn =" << get_sn() << ",";
    out << "balance=" << balance_ << std::endl;

	return out;
}

std::ostream& CommonRspEv::dump(std::ostream& out) const
{
	out << "CommonRspEv sn =" << get_sn() << ",";
    out << "code=" << code_ << ",";
    out << "msg=" << msg_ << ",";
    out << "data=" << data_ << std::endl;

	return out;
}

std::ostream& UnlockEv::dump(std::ostream& out) const
{
	out << "UnlockEv sn =" << get_sn() << ",";
    out << "mobile=" << mobile_ << ",";
    out << "bike_code" << bike_code_ << std::endl;

	return out;
}

std::ostream& LockEv::dump(std::ostream& out) const
{
	out << "LockEv sn =" << get_sn() << ",";
    out << "mobile=" << mobile_ << ",";
    out << "bike_code" << bike_code_ << std::endl;

	return out;
}

std::ostream& LockResultRspEv::dump(std::ostream& out) const
{
	out << "LockResultRspEv sn =" << get_sn() << ",";
    out << "mileage="  << ti_.mileage << ",";
    out << "discharge" << ti_.discharge << ",";
    out << "calorie"   << ti_.calorie << ",";

    //TODO
    //travel parameters

	return out;
}

std::ostream& ListAccountRecordsReqEv::dump(std::ostream& out) const
{
	out << "ListAccountRecordsReqEv sn =" << get_sn() << ",";
    out << "mobile="  << mobile_ << std::endl;

	return out;
}

std::ostream& ListTravelRecordsReqEv::dump(std::ostream& out) const
{
	out << "ListTravelRecordsReqEv sn =" << get_sn() << ",";
    out << "mobile="  << mobile_ << std::endl;

	return out;
}

std::ostream& ListAccountRecordsRspEv::dump(std::ostream& out) const
{
	out << "ListAccountRecordsRspEv sn =" << get_sn() << ",";
    i32 i = 0;
    for(auto iter = records_.begin(); iter != records_.end(); iter ++, i++)
    {
        out << "record["<< i <<"]: type=" << iter->type << "limit=" << iter->limit <<"timestamp="<<iter->timestamp;
    }
    out << std::endl;

	return out;
}

std::ostream& ListTravelRecordsRspEv::dump(std::ostream& out) const
{
	out << "ListTravelRecordsRspEv sn =" << get_sn() << ",";
    i32 i = 0;
    out << "mileage="<<ti_.mileage<<",discharge="<<ti_.discharge<<",calorie"<<ti_.calorie;
    for(auto iter = ti_.records.begin(); iter != ti_.records.end(); iter ++, i++)
    {
        out << "record["<< i <<"]: startTimeStamp=" << iter->startTimeStamp << ",duration=" << iter->duration <<",amount="<<iter->amount;
    }
    out << std::endl;

	return out;
}
