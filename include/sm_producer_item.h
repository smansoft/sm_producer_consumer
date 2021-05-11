/*
 *	Copyright (c) 2021 SManSoft <http://www.smansoft.com/>
 *  Sergey Manoylo <info@smansoft.com>
 *
 * 	sm_producer_item.h
 *
 *  Created on: Apr 26, 2021
 *      Author: sman
 */

#pragma once

#include "pch.h"

#include "sm_main.h"

namespace sm_producer_consumer {

/**
 *	CSMProducerItem
 *
 *	data class, that just contains array of unsigned chars
 *	(std::shared_ptr<unsigned char> m_data)
 *	and length of this array (m_size);
 *
 *	class provides interface (api), that allows to get and change these data
 *	(array of unsigned chars and length of this array);
 */
class CSMProducerItem {

public:
	CSMProducerItem();
	CSMProducerItem(const unsigned char * const data, const unsigned int size);
	CSMProducerItem(const CSMProducerItem& producer_item);
	CSMProducerItem(CSMProducerItem&& producer_item) = delete;
	virtual ~CSMProducerItem();

	CSMProducerItem& operator = (const CSMProducerItem &producer_item);

	void setDataAndSize(const unsigned char *const data, const unsigned int size);

	unsigned char* getData() const		{	return m_data.get(); };
	unsigned int getSize() const		{	return m_size;	};

private:
	std::shared_ptr<unsigned char> m_data;		// data (array of unsigned chars), created by one of Producer classes
												// CSMProducerType1, CSMProducerType2, ... CSMProducerType5
	unsigned int m_size;						// length of m_data array (including last zero symbol)
};

}
