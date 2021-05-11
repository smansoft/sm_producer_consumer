/*
 *  Copyright (c) 2021 SManSoft <http://www.smansoft.com/>
 *  Sergey Manoylo <info@smansoft.com>
 *
 *  sm_tools.h
 *
 *  Created on: Apr 26, 2021
 *      Author: sman
 */

#pragma once

#include "pch.h"

#include "sm_main.h"

#if defined __cplusplus
extern "C" {
#endif

/**
 * print current date time in the follow format:
 * %02d-%02d-%02d(%d) %02d:%02d:%02d.%03d
 * year-month-day(week day) hour:minutes:seconds.mseconds
 */
void sm_print_curr_dtime();

errno_t sm_init_dpath(char * const res_dpath, const size_t res_dpath_len, const char * const dpath);

errno_t sm_init_fpath(char * const res_fpath, const size_t res_fpath_len, const char * const dpath, const char * const fname);

#if defined __cplusplus
}
#endif
