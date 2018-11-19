#ifndef BRKS_COMMON_EVENT_TYPE_H_
#define BRKS_COMMON_EVENT_TYPE_H_

enum EErrorCode
{
      ERRC_SUCCESS              = 200,
	  ERRC_INVALID_MSG          = 400,
	  ERRC_INVALID_DATA         = 404,
	  ERRC_METHOD_NOT_ALLOWED   = 405,
	  ERRO_PROCCESS_FAILED      = 406,
	  ERRO_BIKE_IS_TOOK         = 407,
	  ERRO_BIKE_IS_RUNNING      = 408,
	  ERRO_BIKE_IS_DAMAGED      = 409
};

/* 事件ID */
enum EventID
{
    EEVENTID_COMMON_RSP                    = 0x00,

    EEVENTID_GET_MOBILE_CODE_REQ           = 0x01,
    EEVENTID_GET_MOBILE_CODE_RSP           = 0x02,

    EEVENTID_LOGIN_REQ                     = 0x03,
    EEVENTID_LOGIN_RSP                     = 0x04,

    EEVENTID_RECHARGE_REQ                  = 0x05,
    EEVENTID_RECHARGE_RSP                  = 0x06,

    EEVENTID_GET_ACCOUNT_BALANCE_REQ       = 0x07,
    EEVENTID_GET_ACCOUNT_BALANCE_RSP       = 0x08,

    EEVENTID_LIST_ACCOUNT_RECORDS_REQ      = 0x09,
    EEVENTID_LIST_ACCOUNT_RECORDS_RSP      = 0x10,

    EEVENTID_UNLOCK_REQ                    = 0x11,
    EEVENTID_UNLOCK_RSP                    = 0x12,

    EEVENTID_LIST_TRAVELS_REQ              = 0x13,
    EEVENTID_LIST_TRAVELS_RSP              = 0x14,

	EEVENTID_LOCK_REQ                      = 0x15,
	EEVENTID_LOCK_RSP                      = 0x16,

	EEVENTID_UNKOWN                        = 0xFFFF
};


#endif

