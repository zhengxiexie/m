/*
 * or_decorator.h
 *
 *  ר�Ŵ��� mongoDB �ж�ĳһ���ֶ�ִ�� or �߼���װ����
 *
 *  Created on: 2013-4-27
 *      Author: guoweijiang
 */

#ifndef OR_DECORATOR_H_
#define OR_DECORATOR_H_

#include "abstract_single_decorator.h"

class OrDecorator : public AbstractSingleDecorator
{
public:

    OrDecorator();
    virtual ~OrDecorator();

    /*
     *  ʵ�ֳ���ӿڣ�
     */
    int custom_result_set_query(bson* query);
};

#endif /* IN_DECORATOR_H_ */
