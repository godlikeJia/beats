#ifndef _DECODE_PB_TO_JSON__
#define _DECODE_PB_TO_JSON__

#define safe_delete(ptr)        \
    do {                        \
        if (nullptr != ptr) {   \
            delete ptr;         \
            ptr = nullptr;      \
        }                       \
    } while(0)

#ifdef __cplusplus
extern "C" {
#endif
// init 
// @path proto file directory path
// @file prot file name
// @return 0 - success other - failed
int init(const char* path, const char* file);

// uninit
// ignore return
int uninit();

// dynamic_encode for testing
// @msg_name message type name, include package name
// @output output json string NOTE: delete it by caller
int dynamic_encode(const char* msg_name, char** output, int* len);

// dynamic_decode
// @msg_name message type name, include package name
// @data data to parse
// @len length of data
// @output output json string NOTE: delete it by caller
// @return 0 - success other - failed
int dynamic_decode(const char* msg_name, const char* data, int len, char** output, int* output_len);
#ifdef __cplusplus
}
#endif

#endif // _DECODE_PB_TO_JSON__
