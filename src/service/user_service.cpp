
#include "user_service.h"
#include "mutex.h"

UserService::UserService(std::shared_ptr<MysqlConnection> sql_conn) : sql_conn_(sql_conn)
{

}

bool UserService::exist(const std::string& mobile)
{
    char sql_content[1024] = {0};//"select * from userinfo where mobile = ";
    sprintf(sql_content, \
            "select * from userinfo where mobile = %s", \
            mobile.c_str());
    SqlRecordSet record_set;

    if (!sql_conn_->Execute(sql_content, record_set))
    {
        return false;
    }

    return (record_set.GetRowCount() != 0);
}

bool UserService::insert(const std::string& mobile)
{
    char sql_content[1024] = {0};
    sprintf(sql_content, \
        "insert into userinfo (mobile) values (%s)", \
        mobile.c_str());
    return sql_conn_->Execute(sql_content);
}

bool UserService::update_user_name(const std::string& mobile, const std::string& name)
{
    if (exist(mobile))
    {
        char sql_content[1024] = {0};
        sprintf(sql_content, \
            "update userinfo set username = %s where mobile = %s", \
            name.c_str(), mobile.c_str());
        return sql_conn_->Execute(sql_content);
    }

    return false;
}

bool UserService::update_user_money(const std::string& mobile, int money)
{
    mutex_t mutex;
    bool result = false;

    mutex.lock();

    char sql_content[1024] = {0};
    sprintf(sql_content, \
        "select * from userinfo where mobile = %s", \
        mobile.c_str());
    SqlRecordSet record_set;
    if (!sql_conn_->Execute(sql_content, record_set))
    {
        return false;
    }
    MYSQL_ROW row;
    record_set.FetchRow(row);
    int money_db = atoi(row[5]);  //获取money列
    money_db += money;

    memset(sql_content, 0, 1024);
    sprintf(sql_content, \
        "update userinfo set money = %d where mobile = %s", \
        money_db, mobile.c_str());
    if (!sql_conn_->Execute(sql_content, record_set))
    {
        LOG_ERROR("update money from %d to %d failed where mobile = %s",
            money_db - money, money_db, mobile.c_str());
    }
    result = true;

    mutex.unlock();

    return result;
}

bool UserService::get_user_balance(const std::string& mobile, int& balance)
{
    char sql_content[1024] = {0};//"select * from userinfo where mobile = ";
    sprintf(sql_content, \
        "select * from userinfo where mobile = %s", \
        mobile.c_str());
    SqlRecordSet record_set;

    if (!sql_conn_->Execute(sql_content, record_set))
    {
        LOG_DEBUG("execute '%s' failed.", sql_content);
        return false;
    }

    if (0 == record_set.GetRowCount())
    {
        LOG_DEBUG("no user exist in db where mobile = %s", mobile.c_str());
        return false;
    }

    MYSQL_ROW row;
    record_set.FetchRow(row);

    balance = atoi(row[5]);  // 第五行是余额
    return true;
}

/**
 * 充值
 *
 * @param mobile
 * @param type , 0 : 骑行消费,  1 : 充值, 2 : 退款
 * @param money, 消费的金额
 */
bool UserService::insert_consume_record(const std::string& mobile, int type, int money)
{
    char sql_content[1024] = {0};//"select * from userinfo where mobile = ";
    sprintf(sql_content, \
        "insert into accountrecords (mobile, type, money) values (%s,%d,%d)", \
        mobile.c_str(), type, money);

    if (!sql_conn_->Execute(sql_content))
    {
        LOG_DEBUG("execute '%s' failed.", sql_content);
        return false;
    }

    return true;
}

bool UserService::list_consume_records(const std::string& mobile, std::vector<AccountRecord>& records)
{
    char sql_content[1024] = {0};//"select * from userinfo where mobile = ";
    sprintf(sql_content, \
        "select * from accountrecords where mobile = %s", \
        mobile.c_str());

    SqlRecordSet record_set;
    if (!sql_conn_->Execute(sql_content, record_set))
    {
        LOG_DEBUG("execute '%s' failed.", sql_content);
        return false;
    }

    i32 row_count = record_set.GetRowCount();
    for (int i = 0; i < row_count; i++)
    {
        MYSQL_ROW row;
        record_set.FetchRow(row);
        AccountRecord ar(atoi(row[3]), atoll(row[4]), row[2]);
        records.push_back(ar);
    }

    return true;
}


