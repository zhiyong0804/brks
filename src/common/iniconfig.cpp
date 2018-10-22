
#include "iniconfig.h"
#include <iniparser/iniparser.h>
#include "Logger.h"

Iniconfig::Iniconfig(): _isloaded(false)
{

}

Iniconfig::~Iniconfig()
{

}

bool Iniconfig::loadfile(const std::string& path)
{
    dictionary*   ini = NULL;

    ini = iniparser_load(path.c_str());
    if (ini==NULL)
    {
        LOG_ERROR("cannot parse file: %s\n", path.c_str());
        return false;
    }

    char* ip    = iniparser_getstring(ini, "database:ip", "127.0.0.1");
    int   port  = iniparser_getint(ini, "database:port", 3306);
    char* user  = iniparser_getstring(ini, "database:user", "root");
    char* pwd   = iniparser_getstring(ini, "database:pwd", "123456");
    char* db    = iniparser_getstring(ini, "database:db", "dongnaobike");
    int   sport = iniparser_getint(ini, "server:port", 9090);

    _config = st_env_config(std::string(ip), port, std::string(user), \
        std::string(pwd), std::string(db), sport);

    iniparser_freedict(ini);

    _isloaded = true;

    return true;
}

const st_env_config& Iniconfig::getconfig()
{
    return _config;
}


