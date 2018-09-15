
#ifndef BRKS_SVR_USER_SERVICE_H_
#define BRKS_SVR_USER_SERVICE_H_

#include "glo_def.h"
#include "sqlconnection.h"
#include "events_def.h"

#include <memory>

class UserService
{
public:
    UserService(std::shared_ptr<MysqlConnection> sql_conn);
    bool exist(const std::string& mobile);
    bool insert(const std::string& mobile);
    bool update_user_name(const std::string& mobile, const std::string& name);
    bool update_user_money(const std::string& mobile, int money);
    bool get_user_balance(const std::string& mobile, int& balance);
    bool insert_consume_record(const std::string& mobile, int type, int money);
    bool list_consume_records(const std::string& mobile, std::vector<AccountRecord>& records);

private:
    std::shared_ptr<MysqlConnection> sql_conn_;
};

#endif

