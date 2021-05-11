/*
 *  Copyright (c) 2021 SManSoft <http://www.smansoft.com/>
 *  Sergey Manoylo <info@smansoft.com>
 *
 * 	sm_producer.h
 *
 *  Created on: Apr 26, 2021
 *      Author: sman
 */

#pragma once

#include "pch.h"

#include "sm_main.h"
#include "sm_producer_item.h"

namespace sm_producer_consumer {

/**
 *	CSMProducerBase
 *
 *	base class of suite of Producers, which provide generation
 *	of Item data instances of the CSMProducerItem class,
 *	using separate thread;
 *
 *	generated items are inserted into queue:
 *	std::queue<CSMProducerItem> m_items_queue;
 *
 *	m_items_queue has limited size (defined by m_items_queue_max);
 *
 *	extracts generated items from the queue:
 *	std::queue<CSMProducerItem> m_items_queue;
 *
 *	uses synchronizing primitives for access to the queue (m_items_queue):
 * 	std::mutex m_mtx_queue;
 *	std::condition_variable m_cv_queue;
 *
 *	periodicity of generation of items are defined by m_delay (sleep value)
 *
 *	producer(some ProducerType type) --> m_items_queue
 */
class CSMProducerBase {

public:
	virtual ~CSMProducerBase();

	size_t getQueueSize();

	bool getProducerItem(CSMProducerItem& producer_item);

	ProducerType getProducerType() {	return m_type;	};

	void startAddData();
	void stopAddData();

	void clearData();

	bool isStarted() const {	return m_run_thread;	}

protected:
	CSMProducerBase(const ProducerType type);
	CSMProducerBase(CSMProducerBase&& producer) = delete;

	unsigned int m_max_num;	// max number of integers in random line,
							// generated during one step
	unsigned int m_delay;	// delay in msecs (sleep time in the thread)

 	unsigned int m_items_queue_max;	// max size of m_items_queue (queue of CSMProducerItem elements)

private:
	void genProducerItem();		// generation the CSMProducerItem and inserting it into
								// std::queue<CSMProducerItem> m_items_queue
								// is called by threadFunc (by the thread function)
	void threadFunc();			// thread function (is used by std::unique_ptr<std::thread>m_thread)

	const ProducerType m_type; 	// type of producer
								// same as in CSMProducerType1, CSMProducerType2, ... CSMProducerType5
								// Producers and consumers, which have same type are joined (are used same
								// std::queue<CSMProducerItem> m_items_queue)

	std::queue<CSMProducerItem> m_items_queue;	// queue, that is generated in the thread, created by the producer
	std::mutex m_mtx_queue;						// synchronization primitive (mutex) for access to m_items_queue
	std::condition_variable m_cv_queue;			// synchronization primitive (mutex) for access to m_items_queue
												// condition is used, when m_mtx_queue is locked, but
												// m_items_queue is empty or its size equals m_items_queue_max
												// i.e. maximum value and new items can't be added
												// and producer should wait till CSMConsumerFin extracts
												// item from m_items_queue

	std::unique_ptr<std::thread>m_thread;		// thread, that is used for generation of CSMProducerItem in m_items_queue
	bool m_run_thread;							// if m_run_thread == false, thread (m_thread) will be finished

	std::mt19937 m_rnd_eng; // default constructed, seeded with fixed seed

};

/**
 *	CSMProducerType1
 *
 *	type of Producer 1
 *
 *  uses:
 *
 *	m_max_num = DEF_MAX_SIZE_RND_TYPE1;
 *	m_delay = DEF_PRODUCER_DELAY_TYPE1;
 *	m_items_queue_max = DEF_PRODUCER_QUEUE_TYPE1_MAX;
 *
 *	DEF_MAX_SIZE_RND_TYPE1				8
 *	DEF_PRODUCER_DELAY_TYPE1			111
 *	DEF_PRODUCER_QUEUE_TYPE1_MAX		100
 */
class CSMProducerType1 : public CSMProducerBase {

public:
	CSMProducerType1();
	CSMProducerType1(CSMProducerType1&& producer) = delete;
	virtual ~CSMProducerType1();

};

/**
 *	CSMProducerType2
 *
 *	type of Producer 2
 *
 *  uses:
 *
 *	m_max_num = DEF_PRODUCER_DELAY_TYPE2;
 *	m_delay = DEF_PRODUCER_DELAY_TYPE2;
 *	m_items_queue_max = DEF_PRODUCER_QUEUE_TYPE2_MAX;
 *
 *	DEF_MAX_SIZE_RND_TYPE2				16
 *	DEF_PRODUCER_DELAY_TYPE2			222
 *	DEF_PRODUCER_QUEUE_TYPE2_MAX		100
 */
class CSMProducerType2 : public CSMProducerBase {

public:
	CSMProducerType2();
	CSMProducerType2(CSMProducerType2&& producer) = delete;
	virtual ~CSMProducerType2();

};

/**
 *	CSMProducerType3
 *
 *	type of Producer 3
 *
 *  uses:
 *
 *	m_max_num = DEF_PRODUCER_DELAY_TYPE3;
 *	m_delay = DEF_PRODUCER_DELAY_TYPE3;
 *	m_items_queue_max = DEF_PRODUCER_QUEUE_TYPE3_MAX;
 *
 *	DEF_MAX_SIZE_RND_TYPE3				32
 *	DEF_PRODUCER_DELAY_TYPE3			333
 *	DEF_PRODUCER_QUEUE_TYPE3_MAX		100
 */
class CSMProducerType3 : public CSMProducerBase {

public:
	CSMProducerType3();
	CSMProducerType3(CSMProducerType3&& producer) = delete;
	virtual ~CSMProducerType3();

};

/**
 *	CSMProducerType4
 *
 *	type of Producer 4
 *
 *  uses:
 *
 *	m_max_num = DEF_PRODUCER_DELAY_TYPE4;
 *	m_delay = DEF_PRODUCER_DELAY_TYPE4;
 *	m_items_queue_max = DEF_PRODUCER_QUEUE_TYPE4_MAX;
 *
 *	DEF_MAX_SIZE_RND_TYPE4				64
 *	DEF_PRODUCER_DELAY_TYPE4			555
 *	DEF_PRODUCER_QUEUE_TYPE4_MAX		100
 */
class CSMProducerType4 : public CSMProducerBase {

public:
	CSMProducerType4();
	CSMProducerType4(CSMProducerType4&& producer) = delete;
	virtual ~CSMProducerType4();

};

/**
 *	CSMProducerType5
 *
 *	type of Producer 5
 *
 *  uses:
 *
 *	m_max_num = DEF_PRODUCER_DELAY_TYPE5;
 *	m_delay = DEF_PRODUCER_DELAY_TYPE5;
 *	m_items_queue_max = DEF_PRODUCER_QUEUE_TYPE5_MAX;
 *
 *	DEF_MAX_SIZE_RND_TYPE5				128
 *	DEF_PRODUCER_DELAY_TYPE5			777
 *	DEF_PRODUCER_QUEUE_TYPE5_MAX		100
 */
class CSMProducerType5 : public CSMProducerBase {

public:
	CSMProducerType5();
	CSMProducerType5(CSMProducerType5&& producer) = delete;
	virtual ~CSMProducerType5();

};

}

