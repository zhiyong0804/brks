
#include "json_protocol_codec.h"
#include "Logger.h"

#include <json/json.h>


bool json_protocol_codec_t::encode(iEvent* ev, u8* buffer, u32 size)
{

}

iEvent* json_protocol_codec_t::decode(u16 mid, u8* buffer, u32 size)
{
    Json::Reader reader;
    Json::Value attributes;
    if (!reader.parse((const char*)buffer, attributes))
        return -1;

    iEvent* ev= NULL;

    switch (mid )
    {
    case EEVENTID_GET_MOBILE_CODE_REQ:
        ev = decode_2_mobile_code_req_ev(attributes);
        break;
    case EEVENTID_LOGIN_REQ:
        ev = decode_2_login_req_ev(attributes);
        break;
    case EEVENTID_RECHARGE_REQ:

        break;
    default:
        LOG_WARN("mid %d is invalid.", mid);
        break;
    }

    return ev;
}

MobileCodeReqEv* json_protocol_codec_t::decode_2_mobile_code_req_ev(const Json::Value& attributes)
{
    if (attributes["mobile"] == Json::nullValue)
    {
        return NULL;
    }

    std::string mobile = attributes["mobile"].asString();

    MobileCodeReqEv* ev = new MobileCodeReqEv(mobile);

    return ev;
}

LoginReqEv* json_protocol_codec_t::decode_2_login_req_ev(const Json::Value& attributes)
{
    if ((attributes["mobile"] == Json::nullValue) || (attributes["code"] == Json::nullValue))
    {
        return NULL;
    }

    std::string mobile = attributes["mobile"].asString();
    std::string code   = attributes["code"].asString();
    LoginReqEv* ev = new LoginReqEv(mobile, code);

    return ev;
}

RechargeReqEv* json_protocol_codec_t::decode_2_recharge_req_ev(const Json::Value& attributes)
{
    if ((attributes["mobile"] == Json::nullValue) || (attributes["amount"] == Json::nullValue))
    {
        return NULL;
    }

    std::string mobile = attributes["mobile"].asString();
    int amount = attributes["amount"].asInt();
    RechargeReqEv* ev = new RechargeReqEv(mobile, amount);

    return ev;
}

GetAccountBalanceReqEv* json_protocol_codec_t::decode_2_get_account_balance_req_ev(const Json::Value& attributes)
{
    if (attributes["mobile"] == Json::nullValue)
    {
        return NULL;
    }

    std::string mobile = attributes["mobile"].asString();
    GetAccountBalanceReqEv* ev = new GetAccountBalanceReqEv(mobile);

    return ev;
}

UnlockReqEv* json_protocol_codec_t::decode_2_unlock_req_ev(const Json::Value& attributes)
{
    if ((attributes["mobile"] == Json::nullValue) || (attributes["bikecode"] == Json::nullValue))
    {
        return NULL;
    }

    std::string mobile   = attributes["mobile"].asString();
    std::string bikecode = attributes["bikecode"].asString();

    return new UnlockReqEv(mobile, bikecode);
}

LockReqEv* json_protocol_codec_t::decode_2_lock_req_ev(const Json::Value& attributes)
{
    if ((attributes["mobile"] == Json::nullValue) || (attributes["bikecode"] == Json::nullValue))
    {
        return NULL;
    }

    std::string mobile   = attributes["mobile"].asString();
    std::string bikecode = attributes["bikecode"].asString();

    return new LockReqEv(mobile, bikecode);
}

ListAccountRecordsReqEv* json_protocol_codec_t::decode_2_list_account_record_req_ev(const Json::Value& attributes)
{
    if (attributes["mobile"] == Json::nullValue)
    {
        return NULL;
    }

    std::string mobile = attributes["mobile"].asString();
    return new ListAccountRecordsReqEv(mobile);
}

ListTravelRecordsReqEv* json_protocol_codec_t::decode_2_list_travel_records_req_ev(const Json::Value& attributes)
{
    if (attributes["mobile"] == Json::nullValue)
    {
        return NULL;
    }

    std::string mobile = attributes["mobile"].asString();
    return new ListTravelRecordsReqEv(mobile);
}




