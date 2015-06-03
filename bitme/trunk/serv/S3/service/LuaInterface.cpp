
//-----------------------------------------------------------------------------
// 文件名 : LuaInterface.cpp
// 模块    :    Script
// 功能     :    脚本系统提供给外界使用的Lua的接口
// 修改历史:
//-----------------------------------------------------------------------------
#include "stdlib.h"
#include "string.h"
#include "LuaInterface.h"

extern int luaopen_bson(lua_State *L);
extern int luaopen_conf_core(lua_State *L);


//#define NOPREFIXCALL 


int total_alloc = 0;
int total_delloc = 0;
int max_req_size = 0;


static void *l_alloc (void *ud, void *ptr, size_t osize, size_t nsize) 
{
	if(nsize > max_req_size){
		max_req_size = nsize;
	}

	if (nsize == 0) {
		if(ptr) free(ptr);
		total_delloc += osize;
		return NULL;
	}
	else{
		total_alloc -= osize;
		total_alloc += nsize;
		return realloc(ptr, nsize);
	}
}


LuaInterface::LuaInterface(lua_State* luaVM):lua_state_(0)
{
__ENTER_FUNCTION
    if(luaVM)
    {
        lua_state_ = luaVM;
        lua_settop(lua_state_, 0);
        return;
    }

    lua_state_ = luaL_newstate();
    luaL_openlibs(lua_state_);
    ex_function(lua_state_);
    
    luaopen_lfs(lua_state_);
    luaopen_bson(lua_state_);
    luaopen_conf_core(lua_state_);
    
    lua_settop(lua_state_, 0);
__LEAVE_FUNCTION
}
 
template<>
void LuaInterface::_Call<void>(int param_num)
{
__ENTER_FUNCTION
    if(lua_pcall(lua_state_,param_num,0,0) != 0)
    {
        char* err = getValueNoPop<char*>();
        printf("Lua error: %s\n",err);
        Pop();
    }

    return;
__LEAVE_FUNCTION
}

template<>
void LuaInterface::callGlobalFunc<void>(const char* func)
{
    //LOKI_STATIC_CHECK(TypeTraits<R>::isReference==false,must_use_a_value)
	__ENTER_FUNCTION

	// to protect lua stack
    size_t before = lua_gettop(lua_state_);
    lua_getglobal(lua_state_, func);
    
    _Call<void>(0);
    lua_settop(lua_state_,before);
    
	__LEAVE_FUNCTION
}


#define C_ENV_SHARED_LIGHTUD_LEN 512

void** __make_sure_c_env_get_shared_lightud_exists()
{
	static void** aa = NULL;
	if(NULL == aa){
		int len = sizeof(void*) * C_ENV_SHARED_LIGHTUD_LEN;
		aa = (void**)malloc(len);
		memset(aa,0,len);
	}
	return aa;
}

int __c_env_get_shared_lightud(lua_State* L)
{
	void** p = __make_sure_c_env_get_shared_lightud_exists();
	if(NULL == p) return 0;
	
	lua_Integer index = lua_tointeger(L,1);
	
	if(index<0 || index>=C_ENV_SHARED_LIGHTUD_LEN) return 0;
	
	lua_pushlightuserdata(L,p[index]);
	return 1;
}

int __c_env_set_shared_lightud(lua_State* L)
{
	void** p = __make_sure_c_env_get_shared_lightud_exists();
	if(NULL == p) return 0;
	
	lua_Integer index = lua_tointeger(L,1);
	void* lightud = lua_touserdata(L,2);
	
	if(index<0 || index>=C_ENV_SHARED_LIGHTUD_LEN) return 0;
	
	p[index] = lightud;
	
	lua_pushboolean(L,1);
	return 1;
}


void LuaInterface::ex_function(lua_State* luaVM)
{
	lua_register(luaVM,"l_env_get_shared_lightud",__c_env_get_shared_lightud);
	lua_register(luaVM,"l_env_set_shared_lightud",__c_env_set_shared_lightud);
}


void LuaInterface::Init()
{    
__ENTER_FUNCTION
    
	int aa = doFile("./lua/init.lua");
   
__LEAVE_FUNCTION
}


