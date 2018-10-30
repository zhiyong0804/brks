#include <stdio.h>

//lua
extern "C"
{
    #include <lua.h>
    #include <lualib.h>
    #include <lauxlib.h>
    #include <luaconf.h>
}

//配置信息
typedef struct 
{
	//服务器端口
	unsigned short svr_port;
	
	//数据库配置信息
	struct DB_CONF_INFO
	{
		//数据库IP
		char db_ip[32],
		//数据库通讯端口
		unsigned short db_port,
		//数据库登录用户名
	 	char db_user[32], 
		//数据库登录密码
		char db_pwd[32],
		//数据库名
		char db_name[32],
	}dbConfInfo;
}CONF_INFO;

//配置信息
CONF_INFO confInfo;


static int dofile(lua_State *L, const char* path, const char *name)
{
    if (LUA_OK != luaL_dofile(L, "./conf.lua"))
    {
         return 0;
    }

    printf("sizeof: %d\n", lua_gettop(L));
    
    printf("typeof: %d\n", lua_type(L, -1));
    printf("typeof: %d\n", lua_type(L, -2));

    
    if ((lua_gettop(L) != 2)) 
    {
        return 0;
    }   
    
    return 1;
}


//初始化数据
static void InitConfInfo(void)
{
	memset((char*)&confInfo,0,sizeof(confInfo));
}

void ReadConfnfo(char** argv)
{
	InitConfInfo();
	
	//=========================================
	lua_State *L;       //创建虚拟机
    L = luaL_newstate();
    luaL_openlibs(L);
    if (0 == dofile(L, argv[1], "sport"))
    {
        printf("file check failed.");
        return -1;
    }
	
	/********************************************/
	//afxbase的源码
	//读单个数据
	lua_getglobal(L,"server_port");
	confInfo.svr_port = lua_tonumber(L,-1);//lua_tostring
	printf("sport : %d \n", confInfo.svr_port);
	lua_pop(L,1);
	
	//读表
	lua_getglobal(L,"db_conf");
	
	//读IP
	lua_getfield(L,-1,"db_ip");
	strcpy(confInfo.dbConfInfo.db_ip,lua_tostring(L,-1));
	printf("db_ip:%s\n", confInfo.dbConfInfo.db_ip);
	lua_pop(L,1);
	
	//读port
	lua_getfield(L,-1,"db_port");
	confInfo.dbConfInfo.db_port = lua_tonumber(L,-1);
	printf("db_port : %d \n", confInfo.dbConfInfo.db_port);
	lua_pop(L,1);
	
	//读user
	lua_getfield(L,-1,"db_user");
	confInfo.dbConfInfo.db_user = lua_tostring(L,-1);
	printf("db_user:%s\n", confInfo.dbConfInfo.db_user);
	lua_pop(L,1);
	
	//读password
	lua_getfield(L,-1,"db_pwd");
	confInfo.dbConfInfo.db_pwd = lua_tostring(L,-1);
	printf("db_pwd:%s\n", confInfo.dbConfInfo.db_pwd);
	lua_pop(L,1);
	
	//读name
	lua_getfield(L,-1,"db_name");
	confInfo.dbConfInfo.db_name = lua_tostring(L,-1);
	printf("db_name:%s\n", confInfo.dbConfInfo.db_name);
	//lua_pop(L,1);
	
	/********************************************/
	
    //销毁状态机
    lua_close(L);
	
	//=========================================
}
