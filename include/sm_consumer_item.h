/*
 *  Copyright (c) 2021 SManSoft <http://www.smansoft.com/>
 *  Sergey Manoylo <info@smansoft.com>
 *
 *  sm_consumer_item.h
 *
 *  Created on: Apr 26, 2021
 *      Author: sman
 */

#pragma once

#include "pch.h"

#include "sm_main.h"

namespace sm_producer_consumer {

/**
 *  CSMConsumerItem
 *
 *  data class, that just contains array of unsigned chars
 *  (std::shared_ptr<unsigned char> m_data)
 *  and length of this array (m_size);
 *
 *  class provides interface (api), that allows to get and change these data
 *  (array of unsigned chars and length of this array);
 */
class CSMConsumerItem {

public:
    CSMConsumerItem();
    CSMConsumerItem(const unsigned char* const data, const unsigned int size);
    CSMConsumerItem(const CSMConsumerItem& consumer_item);
    CSMConsumerItem(CSMConsumerItem&& consumer_item) = delete;
    virtual ~CSMConsumerItem();

    CSMConsumerItem& operator = (const CSMConsumerItem &consumer_item);

    void setDataAndSize(const unsigned char* const data, const unsigned int size);

    unsigned char* getData() const  {   return m_data.get(); };
    unsigned int getSize()  const   {   return m_size;  };

private:
    std::shared_ptr<unsigned char> m_data;      // data (array of unsigned chars), created by Consumer class
                                                // during converting (copying) from CSMProducerItem
    unsigned int m_size;                        // length of m_data array (including last zero symbol)
};

}
