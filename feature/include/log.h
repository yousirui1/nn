#ifndef __LOG_H__
#define __LOG_H__

#ifdef _WIN32
    #define LOG_DIR  "c://Users//Public//Documents//Log"
    #define LOG_ERR_FILE "c://Users//Public//Documents//Log//err.log"
    #define FD_SETSIZE 1024
#else
    #define LOG_DIR  "./log"
    #define LOG_ERR_FILE "./log/remote_monitor_err.log"
#endif  //WIN32


#define LOG_NONE_LEVEL     0
#define LOG_ERROR_LEVEL    1
#define LOG_INFO_LEVEL     2
#define LOG_DEBUG_LEVEL    3
#define LOG_DUMP_LEVEL     4

#ifndef LOG_LEVEL
#define LOG_LEVEL    LOG_DUMP_LEVEL
#endif

#if LOG_LEVEL >= LOG_ERROR_LEVEL
#define LOG_ERROR(fmt,...) \
        do { printf("[ERROR] (%s:%d fun:%s) " fmt "\r\n", __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__); \
            exit(0); \
        }while(0)
#else
#define FAIL(fmt, ...)
#endif

#if LOG_LEVEL >= LOG_INFO_LEVEL
#define LOG_INFO(fmt,...) \
        do { printf("[INFO] (%s:%d fun:%s) " fmt "\r\n", __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__); \
        }while(0)
#else
#define INFO(fmt, ...)
#endif

#if LOG_LEVEL >= LOG_DEBUG_LEVEL
#define LOG_DEBUG(fmt,...) \
        do { printf("[DEBUG] (%s:%d fun:%s) " fmt "\r\n", __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__); \
        }while(0)
#else
#define LOG_ERROR(fmt,...) 
#define LOG_DEBUG(fmt,...)
#define LOG_INFO(fmt,...) 
#endif


#endif //  __LOG_H__
