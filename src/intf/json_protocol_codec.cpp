
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
    default:
        LOG_WARN("mid %d is invalid.", mid);
        break;
    }

    return ev;
}

MobileCodeReqEv* json_protocol_codec_t::decode_2_mobile_code_req_ev(const Json::Value& attributes)
{

    return NULL;
}



