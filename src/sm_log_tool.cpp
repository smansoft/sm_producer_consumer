/*
 *  Copyright (c) 2021 SManSoft <http://www.smansoft.com/>
 *  Sergey Manoylo <info@smansoft.com>
 *
 *  sm_log_tool.cpp
 *
 *  Created on: Apr 26, 2021
 *      Author: sman
 */

#include "pch.h"

#include "sm_main.h"
#include "sm_log_tool.h"

sm_log_config gsm_log_config;           //  global instance of main log support structure
#define SM_LOG_CONFIG &gsm_log_config   //  just synonym: SM_LOG_CONFIG == &gsm_log_config - for usage in log api calls

#if defined __cplusplus
extern "C" {
#endif

/**
 *  initializing and starting the log output
 */
errno_t sm_init_log()
{
    errno_t err = SM_RES_OK;
    err = sm_log_init_dpath_fname(SM_LOG_CONFIG,
            SM_LOG_OUT_FILE | SM_LOG_OUT_CONSOLE,
            SM_LOG_LEVEL_OFF,
            SM_LOG_LEVEL_ALL,
            DEF_LOG_DPATH,
            DEF_LOG_FNAME);
#if 0
    err = sm_log_init_dpath_fname(SM_LOG_CONFIG,
            SM_LOG_OUT_FILE | SM_LOG_OUT_CONSOLE,
            SM_LOG_LEVEL_OFF,
            SM_LOG_LEVEL_INFO,
            DEF_LOG_DPATH,
            DEF_LOG_FNAME);
#endif
    if (err == SM_RES_OK)
        err = sm_log_start(SM_LOG_CONFIG);  // starting created log
    return err;
}

/**
 *  stopping and closing the log output
 */
errno_t sm_close_log()
{
    sm_log_stop(SM_LOG_CONFIG);
    return sm_log_close(SM_LOG_CONFIG);
}

#if defined __cplusplus
}
#endif

