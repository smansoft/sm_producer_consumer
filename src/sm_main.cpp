/*
 *	Copyright (c) 2021 SManSoft <http://www.smansoft.com/>
 *	Sergey Manoylo <info@smansoft.com>
 *
 * 	sm_main.cpp
 *
 *  Created on: Apr 26, 2021
 *      Author: sman
 */

#include "pch.h"

#include "safe_str_lib.h"
#include "safe_mem_lib.h"
#include "safe_fopen.h"

#include "sm_files_tools.h"

#include "sm_main.h"
#include "sm_consumer_factory.h"
#include "sm_consumer_fin.h"
#include "sm_consumer_mid.h"
#include "sm_exceptions.h"
#include "sm_log_tool.h"
#include "sm_producer.h"
#include "sm_producer_factory.h"
#include "sm_tools.h"

extern sm_log_config gsm_log_config;	//	global instance of main log support structure
#define SM_LOG_CONFIG &gsm_log_config	//	just synonym: SM_LOG_CONFIG == &gsm_log_config - for usage in log api calls

/**
 *	main entry point
 */
int main(int argc, char* argv[])
{
    errno_t err = SM_RES_OK;

    err = sm_init_log();	// initializing the log output (file defined by DEF_LOG_FNAME; console)

    if(err != SM_RES_OK)
    	printf("sm_producer_consumer: error initializing of the log output\n");

    try {
	    sm_log_printf(SM_LOG_CONFIG, SM_LOG_LEVEL_INFO, __PRETTY_FUNCTION__, "%s %s", __PRETTY_FUNCTION__, "---------------------------------------------------------------------- >>");
	    sm_log_print(SM_LOG_CONFIG, SM_LOG_LEVEL_INFO, __PRETTY_FUNCTION__, "producer_consumer is started");
	    sm_log_print(SM_LOG_CONFIG, SM_LOG_LEVEL_INFO, __PRETTY_FUNCTION__, "------------------");

		sm_producer_consumer::CSMProducerFactory* producer_factory = sm_producer_consumer::CSMProducerFactory::getInstance();
		sm_producer_consumer::CSMProducerBase* producer;

		// creating producers (types: ProducerType_1, ProducerType_2, ... ProducerType_5)

		// creating the producer (ProducerType_1)
		producer_factory->createProducer(sm_producer_consumer::ProducerType_1, &producer);
		producer_factory->addProducer(producer);

		// creating the producer (ProducerType_2)
		producer_factory->createProducer(sm_producer_consumer::ProducerType_2, &producer);
		producer_factory->addProducer(producer);

		// creating the producer (ProducerType_3)
		producer_factory->createProducer(sm_producer_consumer::ProducerType_3, &producer);
		producer_factory->addProducer(producer);

		// creating the producer (ProducerType_4)
		producer_factory->createProducer(sm_producer_consumer::ProducerType_4, &producer);
		producer_factory->addProducer(producer);

		// creating the producer (ProducerType_5)
		producer_factory->createProducer(sm_producer_consumer::ProducerType_5, &producer);
		producer_factory->addProducer(producer);

		sm_producer_consumer::CSMConsumerFactory* consumer_factory = sm_producer_consumer::CSMConsumerFactory::getInstance();

		// creating ConsumerMids (intermediate consumers) (types: ProducerType_1, ProducerType_2, ... ProducerType_5 )
		// all ConsumerMids will be created suspended and will be started later (together)

		// another way to create ConsumerMids
		consumer_factory->createAddConsumerMid(sm_producer_consumer::ProducerType_1, false);
		consumer_factory->createAddConsumerMid(sm_producer_consumer::ProducerType_2, false);
		consumer_factory->createAddConsumerMid(sm_producer_consumer::ProducerType_3, false);
		consumer_factory->createAddConsumerMid(sm_producer_consumer::ProducerType_4, false);
		consumer_factory->createAddConsumerMid(sm_producer_consumer::ProducerType_5, false);

#if 0
		// another way to create ConsumerMids
		consumer_factory->addConsumerMid(new sm_producer_consumer::CSMConsumerMid(sm_producer_consumer::ProducerType_1));
		consumer_factory->addConsumerMid(new sm_producer_consumer::CSMConsumerMid(sm_producer_consumer::ProducerType_2));
		consumer_factory->addConsumerMid(new sm_producer_consumer::CSMConsumerMid(sm_producer_consumer::ProducerType_3));
		consumer_factory->addConsumerMid(new sm_producer_consumer::CSMConsumerMid(sm_producer_consumer::ProducerType_4));
		consumer_factory->addConsumerMid(new sm_producer_consumer::CSMConsumerMid(sm_producer_consumer::ProducerType_5));
#endif

		// creating ConsumerFin (final consumer)
		// output file dir: DEF_RES_DPATH
		// output file name: DEF_RES_FNAME
		consumer_factory->createSetConsumerFin(DEF_DATA_DPATH, DEF_OUT_FNAME, false);

		producer_factory->startAllProducers();		// starting all producers (every thread of every producer)
		consumer_factory->startAllConsumerMids();	// starting all ConsumerMids (intermediate consumers) (every thread of every consumer)
		consumer_factory->startConsumerFin();		// starting ConsumerFin (final consumer) (thread, that processes his queue and generates file output)

	    sm_log_printf(SM_LOG_CONFIG, SM_LOG_LEVEL_INFO, __PRETTY_FUNCTION__, "Common number of Producers 	= %d", producer_factory->getNumProducers());
	    sm_log_printf(SM_LOG_CONFIG, SM_LOG_LEVEL_INFO, __PRETTY_FUNCTION__, "Common number of ConsumerMids	= %d", consumer_factory->getConsumerMids());
	    sm_log_printf(SM_LOG_CONFIG, SM_LOG_LEVEL_INFO, __PRETTY_FUNCTION__, "Common number of ConsumerFin	= %d", (consumer_factory->getConsumerFin() ? 1 : 0));

	    // cycle, that checks file (defined by: DEF_STOP_FPATH)
	    // every DEF_CHECK_STOP_FILE_DELAY msecs;
	    // if file DEF_STOP_FPATH found, cycle checking will be finished
	    // all threads will be finished
	    // and application will be closed
		std::ifstream check_file;

		errno_t err = SM_RES_ERROR;
		char fpath[MAX_PATH];
		err = sm_init_fpath(fpath, SM_ARRAY_SIZE(fpath), DEF_DATA_DPATH, DEF_STOP_FNAME);
		if(err == SM_RES_OK) {
			std::string fpath_str(fpath);
			while(true) { // cycle, check if file fpath_str(fpath) exists
			    sm_log_printf(SM_LOG_CONFIG, SM_LOG_LEVEL_DEBUG, __PRETTY_FUNCTION__, "reader_factory->getConsumerFin()->getQueueSize() = %d", consumer_factory->getConsumerFin()->getQueueSize());
				check_file.open(fpath_str);
				if(check_file.is_open()) {
				    sm_log_printf(SM_LOG_CONFIG, SM_LOG_LEVEL_INFO, __PRETTY_FUNCTION__, "stop file: %s has been found. finishing...", fpath);
					check_file.close();
					break;
				}
				else
				    sm_log_printf(SM_LOG_CONFIG, SM_LOG_LEVEL_DEBUG, __PRETTY_FUNCTION__, "stop file: %s hasn't been found.", fpath);
				usleep(DEF_CHECK_STOP_FILE_DELAY*1000);
			}
		}
		else {	// just cycle without checking file
			while(true)
				usleep(DEF_CHECK_STOP_FILE_DELAY*1000);
		}

		producer_factory->stopAllProducers();			// stopping all producers (every thread of every producer)
		consumer_factory->stopAllConsumerMids();		// stopping all ConsumerMids (intermediate consumers) (every thread of every consumer)
		consumer_factory->stopConsumerFin();			// stopping ConsumerFin (final consumer) (thread, that processes his queue and generates file output)

		consumer_factory->getConsumerFin()->printQueue();	// print (after all stopping) final state of the queue of ConsumerFin

		sm_producer_consumer::CSMConsumerFactory::freeInstance(); 	//	frees memory, allocated by the consumer factory singleton (CSMConsumerFactory)
		sm_producer_consumer::CSMProducerFactory::freeInstance();	//	frees memory, allocated by the producer factory singleton (CSMProducerFactory)

	    sm_log_print(SM_LOG_CONFIG, SM_LOG_LEVEL_DEBUG, __PRETTY_FUNCTION__, "-------------------------------------------------------------------- <<");
    }
	catch (sm_producer_consumer::CSMProducerException &ex) {
	    sm_log_printf(SM_LOG_CONFIG, SM_LOG_LEVEL_FATAL, __PRETTY_FUNCTION__, "Error: %s", ex.message().c_str());
	    err = SM_RES_ERROR;
	}
	catch (sm_producer_consumer::CSMProducerFactoryException &ex) {
	    sm_log_printf(SM_LOG_CONFIG, SM_LOG_LEVEL_FATAL, __PRETTY_FUNCTION__, "Error: %s", ex.message().c_str());
	    err = SM_RES_ERROR;
	}
	catch (sm_producer_consumer::CSMConsumerFactoryException &ex) {
	    sm_log_printf(SM_LOG_CONFIG, SM_LOG_LEVEL_FATAL, __PRETTY_FUNCTION__, "Error: %s", ex.message().c_str());
	    err = SM_RES_ERROR;
	}
	catch (sm_producer_consumer::CSMConsumerMidException &ex) {
	    sm_log_printf(SM_LOG_CONFIG, SM_LOG_LEVEL_FATAL, __PRETTY_FUNCTION__, "Error: %s", ex.message().c_str());
	    err = SM_RES_ERROR;
	}
	catch (sm_producer_consumer::CSMConsumerFinException &ex) {
	    sm_log_printf(SM_LOG_CONFIG, SM_LOG_LEVEL_FATAL, __PRETTY_FUNCTION__, "Error: %s", ex.message().c_str());
	    err = SM_RES_ERROR;
	}
	catch (sm_producer_consumer::CSMBaseException &ex) {
	    sm_log_printf(SM_LOG_CONFIG, SM_LOG_LEVEL_FATAL, __PRETTY_FUNCTION__, "Error: %s", ex.message().c_str());
	    err = SM_RES_ERROR;
	}
	catch (std::exception *ex) {
	    sm_log_printf(SM_LOG_CONFIG, SM_LOG_LEVEL_FATAL, __PRETTY_FUNCTION__, "Error: %s", ex->what());
	    err = SM_RES_ERROR;
	}
	catch (std::exception &ex) {
	    sm_log_printf(SM_LOG_CONFIG, SM_LOG_LEVEL_FATAL, __PRETTY_FUNCTION__, "Error: %s", ex.what());
	    err = SM_RES_ERROR;
	}
	catch (...) {
	    sm_log_printf(SM_LOG_CONFIG, SM_LOG_LEVEL_FATAL, __PRETTY_FUNCTION__, "Error: Some exception happened...");
	    err = SM_RES_ERROR;
	}

    sm_log_print(SM_LOG_CONFIG, SM_LOG_LEVEL_INFO, __PRETTY_FUNCTION__, "------------------");
    sm_log_printf(SM_LOG_CONFIG, SM_LOG_LEVEL_INFO, __PRETTY_FUNCTION__, "result = %d", err);
    sm_log_print(SM_LOG_CONFIG, SM_LOG_LEVEL_INFO, __PRETTY_FUNCTION__, "producer_consumer is finished");
    sm_log_print(SM_LOG_CONFIG, SM_LOG_LEVEL_INFO, __PRETTY_FUNCTION__, "main ---------------------------------------------------------------------- <<");

    sm_close_log();	// closing the log output (file defined by DEF_LOG_FNAME; console)

	return err;
}

