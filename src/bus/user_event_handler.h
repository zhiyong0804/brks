
#ifndef BRKS_BUS_USERM_HANDLER_H_
#define BRKS_BUS_USERM_HANDLER_H_

#include "glo_def.h"
#include "iEventHandler.h"
#include "sqlconnection.h"
#include "events_def.h"
#include "DispatchMsgService.h"

#include <curl/curl.h>
#include <string>
#include <map>
#include <memory>

class UserEventHandler : public iEventHandler
{
public:
    UserEventHandler(std::shared_ptr<DispatchMsgService> dms, std::shared_ptr<MysqlConnection> conn);
    virtual ~UserEventHandler();
    virtual iEvent* handle(const iEvent* ev);

    static size_t write_data(void *ptr, size_t size, size_t nmemb, void *stream);
private:
    CommonRspEv* handle_mobile_code_req(MobileCodeReqEv* ev);
    CommonRspEv* handle_login_req(LoginEv* ev);
    CommonRspEv* handle_recharge_req(RechargeEv* ev);
    GetAccountBalanceRspEv* handle_get_account_balance_req(GetAccountBalanceEv* ev);
    ListAccountRecordsRspEv* handle_list_account_records_req(ListAccountRecordsReqEv* ev);

    /* 短信相关 */
    std::string send_sms(const char *apikey, const char *mobile, const char *text);
    std::string get_sms_code();

    std::string code_gen();
    i32 parse_sms_response(const std::string& response);

private:
    std::shared_ptr<MysqlConnection> sqlconn_;
    std::shared_ptr<DispatchMsgService> dms_;
    std::string mobile_;

    std::map<std::string, std::string> m2c_; //first is mobile, second is code
    CURL *curl_;
    int basefd_;
    char hostname_[256];
    char send_sms_uri_[256];
};

#endif
