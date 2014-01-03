/*
 * orderby_decorator.h
 *
 *  ר�Ŵ��� mongoDB �ж�ĳ�����ֶ�ִ�� orderby �߼���װ����
 *
 *  Created on: 2013-4-27
 *      Author: guoweijiang
 */

#ifndef ORDERBY_DECORATOR_H_
#define ORDERBY_DECORATOR_H_

#include "abstract_single_decorator.h"

class OrderbyDecorator : public AbstractSingleDecorator
{
public:

    // �������ͣ� 1 ���� ; -1 ����
    int order_type;

    OrderbyDecorator();
    virtual ~OrderbyDecorator();

    /*
     *  ʵ�ֳ���ӿڣ�
     */
    int custom_result_set_query(bson* query);
};


#endif /* ORDERBY_DECORATOR_H_ */
