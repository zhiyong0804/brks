
#ifndef BRKS_COMMON_ININCONFIG_H_
#define BRKS_COMMON_ININCONFIG_H_

#include <string>

#include "configdef.h"

class Iniconfig
{
public :
    Iniconfig();
    virtual ~Iniconfig();
    bool loadfile(const std::string& path);
    const st_env_config& getconfig();

private:
    st_env_config _config;
    bool _isloaded;
};

#endif

