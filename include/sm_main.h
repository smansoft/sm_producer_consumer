
/*
 *  Copyright (c) 2021 SManSoft <http://www.smansoft.com/>
 *  Sergey Manoylo <info@smansoft.com>
 *
 *  sm_main.h
 *
 *  Created on: Apr 26, 2021
 *      Author: sman
 */

#pragma once

#include "pch.h"

#define DEF_PRODUCER_CONSUMER_VERSION    "0.0.1.1"

// defines, which are used during generation of unsigned char arrays
// (content of unsigned char m_data[] of CSMConsumerItem and CSMProducerItem)
// max number of generated random integers
// used by CSMProducerType1, CSMProducerType2, ... CSMProducerType5
#define DEF_MAX_SIZE_RND_DEFAULT            8

#define DEF_MAX_SIZE_RND_TYPE1              8
#define DEF_MAX_SIZE_RND_TYPE2              16
#define DEF_MAX_SIZE_RND_TYPE3              32
#define DEF_MAX_SIZE_RND_TYPE4              64
#define DEF_MAX_SIZE_RND_TYPE5              128

// periodicity of generation of items
// delays in msecs in threads
// used by CSMProducerType1, CSMProducerType2, ... CSMProducerType5
#define DEF_PRODUCER_DELAY_DEFAULT          100

#define DEF_PRODUCER_DELAY_TYPE1            111
#define DEF_PRODUCER_DELAY_TYPE2            222
#define DEF_PRODUCER_DELAY_TYPE3            333
#define DEF_PRODUCER_DELAY_TYPE4            555
#define DEF_PRODUCER_DELAY_TYPE5            777

// limited size values of queues, where generated items are saved
// used by CSMProducerType1, CSMProducerType2, ... CSMProducerType5
#define DEF_PRODUCER_QUEUE_TYPE1_MAX        100
#define DEF_PRODUCER_QUEUE_TYPE2_MAX        100
#define DEF_PRODUCER_QUEUE_TYPE3_MAX        100
#define DEF_PRODUCER_QUEUE_TYPE4_MAX        100
#define DEF_PRODUCER_QUEUE_TYPE5_MAX        100

// periodicity of reading and processing of items
// delays in msecs in threads
// used by CSMConsumerMid (ProducerType_1, ProducerType_2, ... ProducerType_5)
//#define DEF_CONSUMER_MID_DELAY            100
#define DEF_CONSUMER_MID_DELAY              500

// periodicity of reading items and saving in the output file
// delays in msecs in the thread
// used by the CSMConsumerFin class
//#define DEF_CONSUMER_FIN_DELAY            100
#define DEF_CONSUMER_FIN_DELAY              1000

// limited size values of the queue, where processed items are saved
// used by the CSMConsumerFin class
#define DEF_CONSUMER_FIN_QUEUE_MAX          100

// periodicity of opening file stop file (file name defined by DEF_STOP_FNAME)
#define DEF_CHECK_STOP_FILE_DELAY   3000

// default directory, where log file will be saved
#define DEF_LOG_DPATH               SM_BIN_PATH_MARKER SM_FILES_DIR_SLASH ".." SM_FILES_DIR_SLASH "log"     //  default directory path, where log file will be created (current directory)

// file name of log file
#define DEF_LOG_FNAME               "sm_producer_consumer.log"                                              //  log file name

// default directory, where data file be saved (file name defined by DEF_OUT_FNAME)
// also this directory will be scanned during searching stop file (file name defined by DEF_STOP_FNAME)
#define DEF_DATA_DPATH              SM_BIN_PATH_MARKER SM_FILES_DIR_SLASH ".." SM_FILES_DIR_SLASH "share" SM_FILES_DIR_SLASH "smansoft" SM_FILES_DIR_SLASH "sm_producer_consumer"

// result file, that is created by CSMConsumerFin
// contains lines:
// len: 30  line:   165419817718862070872477220052
// len: 20  line:   39200062562017294509
#define DEF_OUT_FNAME               "sm_producer_consumer.out.txt"

// main thread of the application has cycle, that checks this file
// in the directory (directory path defined by DEF_DATA_DPATH);
// if this file is found, cycle will be interrupted and application
// will be closed
#define DEF_STOP_FNAME              ".stop"

namespace sm_producer_consumer {

// types of Producers
// these types are used by
// CSMProducerType1, CSMProducerType2, ... CSMProducerType5
// and CSMConsumerMid
// Producers and consumers, which have same type are joined (are used same
// std::queue<CSMProducerItem> m_items_queue)
typedef enum _ProducerType {
    ProducerType_None=0,
    ProducerType_1,
    ProducerType_2,
    ProducerType_3,
    ProducerType_4,
    ProducerType_5
} ProducerType;

}
