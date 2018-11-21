
#ifndef BRKS_INTF_PROTOBUF_PROTOCOL_CODEC_H_
#define BRKS_INTF_PROTOBUF_PROTOCOL_CODEC_H_

#include "glo_def.h"
#include "protocol_head.h"
#include "protocol_codec.h"

class protobuf_protocol_codec_t : public protocol_codec_t
{
public:
	virtual bool encode(iEvent* ev, u8* buffer, u32 size);
	virtual iEvent* decode(u16 mid, u8* buffer, u32 size);
private:
   MobileCodeReqEv* decode_2_mobile_code_req_ev(u8* buffer, u32 size);
   bool encode_common_rsp_ev(CommonRspEv* rsp, u8* buffer, u32 size);
};

#endif

