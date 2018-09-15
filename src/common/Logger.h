#ifndef DISTRIBUTED_LOGGER_H_
#define DISTRIBUTED_LOGGER_H_

#include <string>
#include <log4cpp/Category.hh>

class Logger
{
public:
    bool init(const std::string& log_conf_file);
    static Logger* instance(){ return  &instance_; };

    log4cpp::Category* GetHandle()
    {
        return category_;
    }

protected:
    static Logger instance_;
    log4cpp::Category * category_;
	
};

#define LOG_INFO  Logger::instance()->GetHandle()->info
#define LOG_DEBUG Logger::instance()->GetHandle()->debug
#define LOG_ERROR Logger::instance()->GetHandle()->error
#define LOG_WARN  Logger::instance()->GetHandle()->warn


#endif

