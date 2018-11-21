
#ifndef BRKS_INTF_JSON_PROTOCOL_CODEC_H_
#define BRKS_INTF_JSON_PROTOCOL_CODEC_H_

#include "glo_def.h"
#include "protocol_head.h"
#include "protocol_codec.h"

#include <json/json.h>


class json_protocol_codec_t : public protocol_codec_t
{
public:
	virtual bool encode(iEvent* ev, u8* buffer, u32 size);
	virtual iEvent* decode(u16 mid, u8* buffer, u32 size);

private:
    MobileCodeReqEv* decode_2_mobile_code_req_ev(const Json::Value& attributes);
    LoginReqEv* decode_2_login_req_ev(const Json::Value& attributes);
    RechargeReqEv* decode_2_recharge_req_ev(const Json::Value& attributes);
    GetAccountBalanceReqEv* decode_2_get_account_balance_req_ev(const Json::Value& attributes);
    UnlockReqEv* decode_2_unlock_req_ev(const Json::Value& attributes);
    LockReqEv* decode_2_lock_req_ev(const Json::Value& attributes);
    ListAccountRecordsReqEv* decode_2_list_account_record_req_ev(const Json::Value& attributes);
    ListTravelRecordsReqEv* decode_2_list_travel_records_req_ev(const Json::Value& attributes);

private:
    virtual bool encode_common_rsp_ev(CommonRspEv* rsp, u8* buffer, u32 size);
    virtual bool encode_get_account_balance_rsp_ev(GetAccountBalanceRspEv* rsp, u8* buffer, u32 size);
};

#endif

