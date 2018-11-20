
#include "protocol_head.h"
#include "protocol_codec.h"


#include <assert.h>
#include <string.h>
#include <arpa/inet.h>


bool protocol_head_codec_t::decode(u8* buffer, u32 size, protocol_head_t* head)
{
    assert(buffer != NULL);
    assert(sizeof(head) <= size);

    head->tag_     = get_u16(buffer, size, HEAD_TAG_POS);
    head->version_ = get_byte(buffer, size, HEAD_VERSION_POS);
    head->type_    = get_byte(buffer, size, HEAD_TYPE_POS);
    head->len_     = get_u16(buffer, size, HEAD_LEN_POS);
    head->msg_id_  = get_u16(buffer, size, HEAD_MID_POS);
    head->msg_sn   = get_u64(buffer, size, HEAD_MSN_POS);
    head->reserve_ = get_u32(buffer, size, HEAD_RESERVE_POS);

    return true;
}

bool protocol_head_codec_t::encode(protocol_head_t* head, u8* buffer, u32 size)
{
    assert(buffer != NULL);
    assert(sizeof(head) <= size);

    set_u16(buffer, size, HEAD_TAG_POS, head->tag_);
    set_byte(buffer, size, HEAD_VERSION_POS, head->version_);
    set_byte(buffer, size, HEAD_TYPE_POS, head->type_);
    set_u16(buffer, size, HEAD_LEN_POS, head->len_);
    set_u16(buffer, size, HEAD_MID_POS, head->msg_id_);
    set_u32(buffer, size, HEAD_RESERVE_POS, head->reserve_);
}

u8 protocol_head_codec_t::get_byte(u8* buffer, u32 size, u32 offset)
{
    assert(buffer != NULL);
    assert(offset <= size);
    return buffer + offset;
}

void protocol_head_codec_t::set_byte(u8* buffer, u32 size, u32 offset, u8 val)
{
    assert(buffer != NULL );
    assert(offset + sizeof(u8) <= size);
    buffer[offset] = val;
}

u16 protocol_head_codec_t::get_u16(u8* buffer, u32 size, u32 offset)
{
    assert(buffer != NULL);
    assert(offset + sizeof(u16) <= size);
    u16 val = 0;
    memcpy(&val, buffer + offset, 2);
    return ntohs(val);
}

void protocol_head_codec_t::set_u16(u8* buffer, u32 size, u32 offset, u16 val)
{
    assert(buffer != NULL);
    assert(offset + sizeof(u16) <= size);
    u16 temp = htons(val);
    memcpy(buffer + offset, &temp, 2);
}

u32 protocol_head_codec_t::get_u32(u8* buffer, u32 size, u32 offset)
{
    assert(buffer != NULL);
    assert(offset + sizeof(u32) <= size);
    u32 val = 0;
    memcpy(&val, buffer + offset, 4);
    return ntohl(val);
}

void protocol_head_codec_t::set_u32(u8* buffer, u32 size, u32 offset, u32 val)
{
    assert(buffer != NULL);
    assert(offset + sizeof(u32) <= size);
    u32 temp = htonl(val);
    memcpy(buffer + offset, &temp, 4);
}

u64 protocol_head_codec_t::get_u64(u8* buffer, u32 size, u32 offset)
{
    assert(buffer != NULL);
    assert(offset + sizeof(u64) <= size);
    u64 val = this->get_u32(buffer, size, offset);
    val = val << 32;
    val |= this->get_u32(buffer, size, offset + 4);
    return val;
}

void protocol_head_codec_t::set_u64(u8* buffer, u32 size, u32 offset, u64 val)
{
    assert(buffer != NULL);
    assert(offset + sizeof(u64) <= size);
    u32 temp = htonl(static_cast<u32>(val));
    memcpy(buffer + offset + 4, &temp, 4);

    val = val >> 32;
    temp = htonl(static_cast<u32>(val));
    memcpy(buffer + offset, &temp, 4);
}


