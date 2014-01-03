/*
 * abstract_single_decorator.h
 *
 *  Created on: 2013-4-26
 *      Author: guoweijiang
 */

#ifndef ABSTRACTSINGLEDECORATOR_H_
#define ABSTRACTSINGLEDECORATOR_H_

#include "../abstract_decorator.h"

class AbstractSingleDecorator : public AbstractDecorator
{
protected:

    /*
     *  �������еĹ�������ֻ֧�� string ����
     *  ����Ժ���Ҫ���������͵�������չ����
     */
    vector<MCPTupleDecoratorItem> fitler_conditions;

public:

    // Ȩ�����������
    WeightType weight_type;

    AbstractSingleDecorator();
    virtual ~AbstractSingleDecorator();

    void clear()
    {
        this->fitler_conditions.clear();
    }

    /*
     *  ��һ����֪��vector�б��л�ȡ��ѯ���������� fitler_conditions
     *  ����������������ͬһ�������Ķ������
     */
    int set_same_field_filter_conditions(vector<const char*>* condition_list, // ��֪������ֵ
                                         const char* table_field_name,        // ���������жϵ�����
                                         bool is_string_value);               // �����б��ֵ�Ƿ�Ϊ string ����

    /*
     *  ����  KEY:VALUE �����Զ�
     */
    void add_field_filter_condition(const char* key_name, const char* key_value);

    /*
     *  ����   this->fitler_conditions ��ֻ����ָ��
     *  ���Դ����  value_entity ����  key_value �� string ����ʵ��
     *  this->fitler_conditions ָ�����ʵ��
     */
    void add_field_filter_condition(const char* key_name,
                                    const int key_value,   // �����жϵ�ֵ
                                    string& value_entity); // �ж�ֵת���� string ���͵�ʵ��

    void push_back(MCPTupleDecoratorItem& decorator);

    /*
     *  ����ӿڣ�
     *  ����װ��ָ��ҵ���ѯ����������װ�����ĺ��Ľӿ�
     */
    virtual int custom_result_set_query(bson* query) = 0;
};

#endif /* ABSTRACTSINGLEDECORATOR_H_ */
