
#ifndef BRKS_INTF_PROTOCOL_HEAD_H_
#define BRKS_INTF_PROTOCOL_HEAD_H_

#include "glo_def.h"

enum protocol_type_t
{
    JSON_PROTOCOL_TYPE   = 0,  // jason
    PB_PROTOCOL_TYPE     = 1,  // Google Protocol Buffer
    FB_PROTOCOL_TYPE     = 2,  // FlatBuffers
    BINARY_PROTOCOL_TYPE = 3,  // binary format

    UNKNOW_PROTOCOL_TYPE = 0xFF
};

enum message_arrow_t
{
    APP_2_CLOND_ARROW   = 0,
    CLOND_2_APP_ARROW   = 1,

    BIKE_2_CLOND_ARROW  = 2,
    CLOND_2_BIKE_ARROW  = 3,

    UNKONW_ARROW        = 63  // message arrow of 6 bit is 111111.
};

#pragma pack(4)
struct protocol_head_t
{
    u16        tag_;        // 0xFBFC
    u8         version_;    // protocol version, high 4 bit is master version, low 4 bit is sub version
    u8         type_;       // content type
    u16        len_;        // content length
    u16        msg_id_;     // message id
    u64        msg_sn;     // message serise number
    u32        reserve_;    // reserve bytes
};
#pragma pack()

#define HEAD_TAG_POS        0
#define HEAD_VERSION_POS    2
#define HEAD_TYPE_POS       3
#define HEAD_LEN_POS        4
#define HEAD_MID_POS        6
#define HEAD_MSN_POS        8
#define HEAD_RESERVE_POS    16


class protocol_head_codec_t
{
public:
    virtual bool decode(u8* buffer, u32 size, protocol_head_t* head);
    virtual bool encode(protocol_head_t* head, u8* buffer, u32 size);

private:
    u8 get_byte(u8* buffer, u32 size, u32 offset);
    void set_byte(u8* buffer, u32 size, u32 offset, u8 val);
    u16 get_u16(u8* buffer, u32 size, u32 offset);
    void set_u16(u8* buffer, u32 size, u32 offset, u16 val);
    u32 get_u32(u8* buffer, u32 size, u32 offset);
    void set_u32(u8* buffer, u32 size, u32 offset, u32 val);
    u64 get_u64(u8* buffer, u32 size, u32 offset);
    void set_u64(u8* buffer, u32 size, u32 offset, u64 val);
};

#endif

