#ifndef __GX_C_FUNCTION_H
#define __GX_C_FUNCTION_H


// 这个文件的必要性在于，以纯C函数的形式导出一些功能，这样luaJIT可以使用 


#include "types.h"


#ifdef WIN32
#define CF_EXPORT  __declspec(dllexport)
#else
#define CF_EXPORT
#endif


// 为了让Lua能够使用而写的C函数
extern "C"{
	
struct Slice{
	uint16_t len_;
	const char *mem_;
};
	
CF_EXPORT void* gx_env_get_shared_ptr(int index);

CF_EXPORT bool gx_env_set_shared_ptr(int index,void *p);

CF_EXPORT bool gx_cur_stream_is_end();

CF_EXPORT s16 gx_cur_stream_get_int8();

CF_EXPORT s16 gx_cur_stream_get_int16();

CF_EXPORT s32 gx_cur_stream_get_int32();

CF_EXPORT s64 gx_cur_stream_get_int64();

CF_EXPORT f32 gx_cur_stream_get_float32();

CF_EXPORT f64 gx_cur_stream_get_float64();

CF_EXPORT struct Slice gx_cur_stream_get_slice();

CF_EXPORT s16 gx_cur_stream_peek_int16();

CF_EXPORT bool gx_cur_stream_push_int16(s16 v);

CF_EXPORT bool gx_cur_stream_push_int32(s32 v);

CF_EXPORT bool gx_cur_stream_push_int64(s64 v);

CF_EXPORT bool gx_cur_stream_push_float32(f32 v);

CF_EXPORT bool gx_cur_stream_push_slice(struct Slice s);


CF_EXPORT void gx_cur_write_stream_cleanup();

// 同步原路返回。messageid 是req的+1，内容是push到 stream里的内容。 
CF_EXPORT void cur_stream_write_back();

CF_EXPORT void cur_stream_write_back2(int message_id);

CF_EXPORT void cur_stream_broadcast(int message_id);

CF_EXPORT s32 cur_actor_id();

CF_EXPORT u64 cur_user_sn();

CF_EXPORT u32 cur_game_time();

CF_EXPORT u64 cur_game_usec();



// 因为传入了长度len，这个接口支持二进制数据 
CF_EXPORT bool log_write(int level,const char*,int len);

CF_EXPORT bool log_write2(int index,const char *ss,int len);

CF_EXPORT void log_force_flush();


CF_EXPORT int string_hash(const char *str);

CF_EXPORT int cur_message_loopback();


CF_EXPORT void MD5( const unsigned char *input, size_t ilen, unsigned char output[16] );

}


#endif

