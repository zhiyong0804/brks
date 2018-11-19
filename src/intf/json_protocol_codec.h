
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
};

#endif

