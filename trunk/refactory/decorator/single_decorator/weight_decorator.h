/*
 * weight_decorator.h
 *
 *  ר�Ŵ�����ָ��Ȩ�������װ����
 *
 *  Created on: 2013-4-26
 *      Author: guoweijiang
 */

#ifndef WEIGHT_DECORATOR_H_
#define WEIGHT_DECORATOR_H_

#include "abstract_single_decorator.h"

class OrderTypeStruct
{
public:
    const char* order_type;
    map<string, const char*> extra_tuple;
    AbstractDecorator* outer_decorator;
};

class WeightDecorator : public AbstractSingleDecorator
{
private:
    void order_by_extra_id(bson* command);

public:

    char* defalt_orderby;

    /*
     *  ������  mcp_content ���  class.p, column.p ����ʱ
     *  ��Ҫһ�����⴦����Ҫ���� class.id ��  column.id
     */
    const char* extra_id;

    /*
     *  �����ִ�� aggregate �Ͳ��ܰ���һ��˳��ִ��
     */
    AbstractDecorator* outer_decorator;

    WeightDecorator();
    virtual ~WeightDecorator();

    /*
     *   һ�����������Ϊ NULL
     */
    WeightType get_weight_type(OrderTypeStruct& order_struct);

    /*
     *  ʵ�ֳ���ӿڣ�
     */
    int custom_result_set_query(bson* query);

    /*
     *  ʵ��һ�����ӿ�
     */
    int get_count_query(bson* query);
};

#endif /* WEIGHT_CLASS_DECORATOR_H_ */
