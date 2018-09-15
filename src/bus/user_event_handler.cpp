
#include "user_event_handler.h"
#include "Logger.h"

#include <arpa/inet.h>
#include <assert.h>
#include <errno.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <netdb.h>
#include <unistd.h>
#include <json/json.h>


#include "user_service.h"


#define SA struct sockaddr
#define MAXLINE 4096
#define MAXSUB  2000
#define MAXPARAM 2048
#define LISTENQ 1024

extern int h_errno;

UserEventHandler::UserEventHandler(std::shared_ptr<DispatchMsgService> dms, std::shared_ptr<MysqlConnection> conn)
    : dms_(dms), sqlconn_(conn), basefd_(0)
{
    strcpy(hostname_, "sms.yunpian.com");
    strcpy(send_sms_uri_, "/v2/sms/single_send.json");

    dms_->subscribe(EEVENTID_GET_MOBILE_CODE_REQ, this);
    dms_->subscribe(EEVENTID_LOGIN_REQ, this);
    dms_->subscribe(EEVENTID_RECHARGE_REQ, this);
    dms_->subscribe(EEVENTID_GET_ACCOUNT_BALANCE_REQ, this);
    dms_->subscribe(EEVENTID_LIST_ACCOUNT_RECORDS_REQ, this);
}

UserEventHandler::~UserEventHandler()
{
    dms_->unsubscribe(EEVENTID_GET_MOBILE_CODE_REQ, this);
    dms_->unsubscribe(EEVENTID_LOGIN_REQ, this);
    dms_->unsubscribe(EEVENTID_RECHARGE_REQ, this);
    dms_->unsubscribe(EEVENTID_GET_ACCOUNT_BALANCE_REQ, this);
    dms_->unsubscribe(EEVENTID_LIST_ACCOUNT_RECORDS_REQ, this);
}

iEvent* UserEventHandler::handle(const iEvent* ev)
{
    if (ev == NULL)
    {
        LOG_ERROR("input ev is NULL");
    }

    u32 eid = ev->get_eid();

    if (eid == EEVENTID_GET_MOBILE_CODE_REQ)
    {
        return handle_mobile_code_req((MobileCodeReqEv*) ev);
    }
    else if (eid == EEVENTID_LOGIN_REQ)
    {
        return handle_login_req((LoginEv*) ev);
    }
    else if (eid == EEVENTID_RECHARGE_REQ)
    {
        return handle_recharge_req((RechargeEv*) ev);
    }
    else if (eid == EEVENTID_GET_ACCOUNT_BALANCE_REQ)
    {
        return handle_get_account_balance_req((GetAccountBalanceEv*) ev);
    }
    else if (eid == EEVENTID_LIST_ACCOUNT_RECORDS_REQ)
    {
        return handle_list_account_records_req((ListAccountRecordsReqEv*) ev);
    }

    return NULL;
}

ListAccountRecordsRspEv* UserEventHandler::handle_list_account_records_req(ListAccountRecordsReqEv* ev)
{
    std:string mobile = ev->get_mobile();
    UserService us(sqlconn_);
    std::vector<AccountRecord> records;

    if (!us.list_consume_records(mobile, records))
    {
        return new ListAccountRecordsRspEv(ERRO_PROCCESS_FAILED, "send sms failed", "", records);
    }

    return new ListAccountRecordsRspEv(ERRC_SUCCESS, "success", "", records);
}

CommonRspEv* UserEventHandler::handle_recharge_req(RechargeEv* ev)
{
    std::stringstream ss;
    ev->dump(ss);
    LOG_DEBUG("recved %s", ss.str().c_str());
    UserService us(sqlconn_);
    if ( us.update_user_money(ev->get_mobile(), ev->get_amount()) )
    {
        // �����ֵ��¼
        if ( us.insert_consume_record(ev->get_mobile(), 1, ev->get_amount()) )
        {
            return new CommonRspEv(200, "success", "");
        }
        else
        {
            // TODO : rollback

        }
    }
    else
    {
        // TODO : how to fix recharge failed problem, report alarm and servicer update it by manual.
    }

    return new CommonRspEv(ERRO_PROCCESS_FAILED, "send sms failed", "");
}

CommonRspEv* UserEventHandler::handle_mobile_code_req(MobileCodeReqEv* ev)
{
    mobile_ = ev->get_mobile();

    LOG_DEBUG("try to get moblie phone %s validate code .", mobile_.c_str());

    m2c_[mobile_] = code_gen();
    std::string result = get_sms_code();
    i32 code = parse_sms_response(result);
    if (0 == code)
    {
        LOG_DEBUG("send sms success");
        return new CommonRspEv(200, "success", "");
    }

    LOG_DEBUG("send sms failed with error code = %d", code);

    return new CommonRspEv(ERRO_PROCCESS_FAILED, "send sms failed", "");
}

std::string UserEventHandler::code_gen()
{
    unsigned int code = 0;
    srand((unsigned int)time(NULL));

    code = (unsigned int)(rand() % (999999 - 100000) + 1000000);

    char buffer[20] = {0};
    sprintf(buffer, "%d", code);
    strcpy(buffer, buffer + 1);

    return std::string(buffer);
}

CommonRspEv* UserEventHandler::handle_login_req(LoginEv* ev)
{
    std::string mobile = ev->get_mobile();
    std::string code = ev->get_code();
    LOG_DEBUG("try to handle login ev, mobile = %s, code = %s,", mobile.c_str(), code.c_str());
    auto iter = m2c_.find(mobile);
    if ( ((iter != m2c_.end()) && (0 != code.compare(iter->second)))
         || (iter == m2c_.end()))
    {
        return new CommonRspEv(404, "varify sms code failed, please check your input", "");
    }

    UserService us(sqlconn_);
    bool result = false;
    if (!us.exist(mobile))
    {
        result = us.insert(mobile);
        if (!result)
        {
            LOG_ERROR("insert user(%s) to db failed.", mobile_.c_str());
            return new CommonRspEv(ERRO_PROCCESS_FAILED, "insert db failed", "");
        }
    }

    return new CommonRspEv(200, "success", "");
}

GetAccountBalanceRspEv* UserEventHandler::handle_get_account_balance_req(GetAccountBalanceEv* ev)
{
    int balance = 0;
    UserService us(sqlconn_);
    if (!us.get_user_balance(ev->get_mobile(), balance))
    {
        LOG_ERROR("get user = %s balance failed from db", ev->get_mobile().c_str());
        return new GetAccountBalanceRspEv(ERRO_PROCCESS_FAILED, "query db failed.", "", balance);
    }

    LOG_DEBUG("get user = %s balance = %d success from db", ev->get_mobile().c_str(), balance);

    return new GetAccountBalanceRspEv(200, "success", "", balance);
}

std::string UserEventHandler::send_sms(const char *apikey, const char *mobile, const char *text)
{
    char params[MAXPARAM + 1];
    char *cp = params;
    sprintf(cp, "apikey=%s&mobile=%s&text=%s", apikey, mobile, text);

    char *url = "https://sms.yunpian.com/v2/sms/single_send.json";

    curl_easy_setopt(curl_, CURLOPT_WRITEFUNCTION, write_data);
    std::stringstream out;
    curl_easy_setopt(curl_, CURLOPT_WRITEDATA, &out);
    // specify the url
    curl_easy_setopt(curl_, CURLOPT_URL, url);
    // specify the POST data
    curl_easy_setopt(curl_, CURLOPT_POSTFIELDS, cp);

    LOG_DEBUG("send data is : %s", cp);

    // get response data
    CURLcode res = curl_easy_perform(curl_);
    std::string str_json = out.str();

    LOG_DEBUG("get response data is : %s.", str_json.c_str());

    return str_json;
}

size_t UserEventHandler::write_data(void *ptr, size_t size, size_t nmemb, void *stream)
{
    std::string data((const char*) ptr, (size_t) size * nmemb);
    *((std::stringstream*) stream) << data << endl;

    return size * nmemb;
}

std::string UserEventHandler::get_sms_code(void)
{
    curl_ = curl_easy_init();
    if(NULL == curl_)
    {
        LOG_ERROR("curl_ open failed.\n");
        return "";
    }

    char* mobile_urlencode = curl_easy_escape(curl_, mobile_.c_str(), strlen(mobile_.c_str()));

    char* apikey = "9ac068166011ccef83d9882c112810ac";
    char text[1024] = {0};
    sprintf(text,
        "【动脑单车】欢迎使用动脑单车,您的验证码是%s,请不要把验证码透露给别人.",
        m2c_[mobile_].c_str());
    std::string result = send_sms(apikey, mobile_urlencode, text);

    curl_free(mobile_urlencode);
    curl_easy_cleanup(curl_);

    return result;
}

i32 UserEventHandler::parse_sms_response(const std::string& response)
{
    Json::Reader reader;
    Json::Value attributes;
    i32 code = -1;

    if (!reader.parse(response, attributes))
        return -1;

    if ((attributes["code"] != Json::nullValue) && (attributes["code"].isInt()))
    {
        code = attributes["code"].asInt();
    }

    return code;
}

