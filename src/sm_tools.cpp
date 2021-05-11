/*
 *  Copyright (c) 2021 SManSoft <http://www.smansoft.com/>
 *  Sergey Manoylo <info@smansoft.com>
 *
 *  sm_tools.cpp
 *
 *  Created on: Apr 26, 2021
 *      Author: sman
 */

#include "pch.h"

#include "safe_str_lib.h"
#include "sm_log_tool.h"

extern sm_log_config gsm_log_config;    //  global instance of main log support structure
#define SM_LOG_CONFIG &gsm_log_config   //  just synonym: SM_LOG_CONFIG == &gsm_log_config - for usage in log api calls

#if defined __cplusplus
extern "C" {
#endif

/**
 * print current date time in the follow format:
 * %02d-%02d-%02d(%d) %02d:%02d:%02d.%03d
 * year-month-day(week day) hour:minutes:seconds.mseconds
 */
void sm_print_curr_dtime()
{
    sm_log_print(SM_LOG_CONFIG, SM_LOG_LEVEL_TRACE, __PRETTY_FUNCTION__, "----------------------------------------------------- >>");

    struct timeval tv_o;
    struct tm tm_o;

    gettimeofday(&tv_o, NULL);
    localtime_r(&tv_o.tv_sec, &tm_o);

    sm_log_printf(SM_LOG_CONFIG, SM_LOG_LEVEL_DEBUG, __PRETTY_FUNCTION__,
            "current dtime: %02d-%02d-%02d(%d) %02d:%02d:%02d.%03d",
            (tm_o.tm_year + 1900),
            tm_o.tm_mon,
            tm_o.tm_mday,
            tm_o.tm_wday,
            tm_o.tm_hour,
            tm_o.tm_min,
            tm_o.tm_sec,
            (int)(tv_o.tv_usec / 1000)
    );

    sm_log_print(SM_LOG_CONFIG, SM_LOG_LEVEL_TRACE, __FUNCTION__, "----------------------------------------------------- <<");
}

errno_t sm_init_dpath(char * const res_dpath, const size_t res_dpath_len, const char * const dpath)
{
    errno_t err = SM_RES_OK;
    while(true) {
        BOOL res;
        err = sm_make_path_abs(res_dpath, res_dpath_len, dpath, SM_BIN_PATH_MARKER);
        if (err != SM_RES_OK)
            break;
        err = sm_is_dpath_found(res_dpath, &res);
        if(err != SM_RES_OK || !res )
            err = sm_create_dpath(res_dpath);
        break;
    }
    return err;
}

errno_t sm_init_fpath(char * const res_fpath, const size_t res_fpath_len, const char * const dpath, const char * const fname)
{
    errno_t err = SM_RES_OK;
    while(true) {
        char res_dpath[MAX_PATH] = { 0 };
        size_t len = safe_strnlen(dpath, MAX_PATH);
        if (len <= 0) {
            err = SM_RES_ERROR;
            break;
        }
        len = safe_strnlen(fname, MAX_PATH);
        if (len <= 0) {
            err = SM_RES_ERROR;
            break;
        }
        err = sm_init_dpath(res_dpath, SM_ARRAY_SIZE(res_dpath), dpath);
        if (err != SM_RES_OK)
            break;
        size_t res_dpath_len = safe_strnlen(res_dpath, MAX_PATH);
        if (res_dpath_len <= 0) {
            err = SM_RES_ERROR;
            break;
        }
        if (res_dpath[res_dpath_len - 1] == '/' || res_dpath[res_dpath_len - 1] == '\\')
            snprintf(res_fpath, res_fpath_len, "%s%s", res_dpath, fname);
        else
            snprintf(res_fpath, res_fpath_len, "%s" SM_FILES_DIR_SLASH "%s", res_dpath, fname);
        size_t res_fpath_len_f = safe_strnlen(res_fpath, MAX_PATH);
        if (res_fpath_len_f <= 0) {
            err = SM_RES_ERROR;
            break;
        }
        break;
    }
    return err;
}

#if defined __cplusplus
}
#endif
