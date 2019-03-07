/*
 * Debug library for IVS SDK
 */
#ifndef _SL_DEBUG_H
#define _SL_DEBUG_H

/*
 * Header files
 */
#include <sl_config.h>
#ifdef __KERNEL__
#include <linux/types.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

void sl_log(char *format, ...);
//#define DUMP_ERR_2_FILE
/*
 * Logging level definition
 */
#define LOG_LEVEL_ERROR     0x01
#define LOG_LEVEL_WARN      0x02
#define LOG_LEVEL_NOTE      0x04
#define LOG_LEVEL_DEBUG     0x08
/* Logging levels for release */
#define LOG_LEVELS_RELEASE  (LOG_LEVEL_NOTE|LOG_LEVEL_WARN| \
                             LOG_LEVEL_ERROR)
/* Logging levels for developing */
#define LOG_LEVELS_DEVELOP  (LOG_LEVEL_DEBUG|LOG_LEVEL_NOTE| \
                             LOG_LEVEL_WARN|LOG_LEVEL_ERROR)
/* Logging levels for all */
#define LOG_LEVELS_ALL      (LOG_LEVEL_DEBUG|LOG_LEVEL_NOTE| \
                             LOG_LEVEL_WARN|LOG_LEVEL_ERROR)

/*
 * Logging level setting
 */
#ifdef __KERNEL__
extern u32 log_levels;
#else
#define log_levels LOG_LEVELS_RELEASE
#endif

/*
 * The print handler
 */
#ifdef __KERNEL__
#define PRINT	printk
#else
#define PRINT	printf
#endif /* __KERNEL__ */

/*
 * Logging error
 */

#ifdef __KERNEL__
#define log_err(fmt, ...) do {    \
    if (log_levels & LOG_LEVEL_ERROR) { \
		printk("%s:%d:%s(): ERROR - " fmt "!\n",   \
			   __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__);  \
    }   \
} while (0)
#else
#ifdef DUMP_ERR_2_FILE
#define log_err(fmt, ...) do {    \
    if (log_levels & LOG_LEVEL_ERROR) { \
		sl_log("[ERR] %s %d: "fmt"\n", __func__, __LINE__, ##__VA_ARGS__); \
    }   \
} while (0)
#else
#define log_err(fmt, ...) do {    \
    if (log_levels & LOG_LEVEL_ERROR) { \
		printf("%s:%d:%s(): ERROR - " fmt "!\n",   \
			   __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__);  \
    }   \
} while (0)
#endif
#endif

/*
 * Logging warn
 */
#define log_warn(fmt, ...) do {    \
    if (log_levels & LOG_LEVEL_WARN) { \
		PRINT("%s:%d:%s(): WARN - " fmt "\n",   \
			   __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__);  \
    }   \
} while (0)

/*
 * Logging key note info
 */
#ifdef SYS_DEBUG
#define log_note(fmt, ...) do {    \
    if (log_levels & LOG_LEVEL_NOTE) { \
        PRINT("%s:%d:%s(): NOTE - " fmt "\n",   \
			   __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__);  \
    }   \
} while (0)
#else
#define log_note(fmt, ...) do {    \
    if (log_levels & LOG_LEVEL_NOTE) { \
        PRINT(fmt "\n", ##__VA_ARGS__); \
    }   \
} while (0)
#endif /* SYS_DEBUG */

/*
 * Logging key info
 */
#ifdef SYS_DEBUG
#define log_info(fmt, ...) do {    \
    if (log_levels & LOG_LEVEL_NOTE) { \
        PRINT("%s:%d:%s(): INFO - " fmt "\n",   \
			   __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__);  \
    }   \
} while (0)
#else
#define log_info(fmt, ...) do {    \
    if (log_levels & LOG_LEVEL_NOTE) { \
        PRINT(fmt "\n", ##__VA_ARGS__); \
    }   \
} while (0)
#endif /* SYS_DEBUG */


/*
 * Logging debug info
 */
#ifdef SYS_DEBUG
#define log_debug(fmt, ...) do {    \
    if (unlikely((log_levels & LOG_LEVEL_DEBUG))) { \
		PRINT("%s:%d:%s(): DEBUG - " fmt "\n",   \
			   __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__);  \
    }   \
} while (0)
#else
#define log_debug(fmt, ...) do {    \
    if (log_levels & LOG_LEVEL_DEBUG) { \
        PRINT(fmt "\n", ##__VA_ARGS__); \
    }   \
} while (0)
#endif

/*
 * Logging register value
 */
#ifdef SYS_DEBUG
#define log_regval(fmt, ...) do {   \
    PRINT("Reg value:" fmt "\n", ##__VA_ARGS__); \
} while (0)
#else
#define log_regval(fmt, ...)
#endif

/*
 * Assert
 */
#define SYS_ASSERT(condition) do {    \
    if (!(condition)) { \
		PRINT("%s:%d:%s(): ASSERT failed!\n",   \
			   __FILE__, __LINE__, __FUNCTION__);  \
    }   \
} while (0)

#define SYS_BUG_ON(condition) do {    \
    if (unlikely(condition)) { \
		PRINT("%s:%d:%s(): BUG ON !\n",   \
			   __FILE__, __LINE__, __FUNCTION__);  \
    }   \
} while (0)

#define SL_VERIFY_ARGUMENT SYS_ASSERT

#ifdef __cplusplus
}
#endif

#endif /* _SL_DEBUG_H */
