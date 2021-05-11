/*
 *  Copyright (c) 2021 SManSoft <http://www.smansoft.com/>
 *  Sergey Manoylo <info@smansoft.com>
 *
 *  sm_log_tool.h
 *
 *  Created on: Apr 26, 2021
 *      Author: sman
 */

#pragma once

#include "pch.h"

#include "sm_files_tools.h"
#include "sm_log.h"

#if defined __cplusplus
extern "C" {
#endif

/**
 *  initializing and starting the log output
 */
errno_t sm_init_log();

/**
 *  stopping and closing the log output
 */
errno_t sm_close_log();

#if defined __cplusplus
}
#endif
