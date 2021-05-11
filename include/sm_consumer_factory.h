/*
 *  Copyright (c) 2021 SManSoft <http://www.smansoft.com/>
 *  Sergey Manoylo <info@smansoft.com>
 *
 * 	sm_consumer_factory.h
 *
 *  Created on: Apr 26, 2021
 *      Author: sman
 */

#pragma once

#include "pch.h"

#include "sm_producer.h"
#include "sm_consumer_item.h"
#include "sm_consumer_mid.h"
#include "sm_consumer_fin.h"
#include "sm_main.h"

namespace sm_producer_consumer {

/**
 *	CSMConsumerFactory
 *
 *	multi-thread synchronized singleton;
 *
 *	storage and control CSMConsumerMid objects (several, created for every Producer)
 *	and CSMConsumerFin object (single object);
 */
class CSMConsumerFactory {

public:
	static CSMConsumerFactory* getInstance(); 	// provides access to the singleton, creates instance
	static void freeInstance(); 				// deletes (frees) instance

	static void createConsumerMid(const ProducerType producer_type, CSMConsumerMid** const consumer_mid);
	void addConsumerMid(CSMConsumerMid* const consumer_mid);
	void createAddConsumerMid(const ProducerType producer_type, const bool auto_start = true);

	static void createConsumerFin(const std::string& out_dpath, const std::string& out_fname, CSMConsumerFin** const consumer_fin);
	void setConsumerFin(CSMConsumerFin* const consumer_fin);
	void createSetConsumerFin(const std::string& out_dpath, const std::string& out_fname, const bool auto_start = true);
	CSMConsumerFin* getConsumerFin();

	void startAllConsumerMids();
	void stopAllConsumerMids();

	void startConsumerFin();
	void stopConsumerFin();

	size_t getConsumerMids();
	size_t getNumStartedConsumerMids();

private:
	CSMConsumerFactory();	// private constructor, instance of the class will be created by
							// getInstance() function
	~CSMConsumerFactory();	// private destructor,  instance of the class will be deleted (free) by
							// freeInstance function

	std::vector<std::shared_ptr<CSMConsumerMid>> m_consumer_mids;	// vector of pointers to CSMConsumerMid objects
	std::mutex	m_mtx_consumer_mids;								// synchronization primitive, that makes
																	// m_consumer_mids an thread-safe queue

	std::shared_ptr<CSMConsumerFin> m_consumer_fin;	// pointer to the instance of CSMConsumerFin

	static CSMConsumerFactory* m_instance;	// pointer to instance of the singleton
	static std::mutex m_mtx_instance;		// thread-safe pointer
};

}
