/*
 * ex_compare.h
 *
 *  Created on: 2013-7-17
 *      Author: guoweijiang
 */

#ifndef EX_COMPARE_H_
#define EX_COMPARE_H_

#include "./common.h"

// ������ӿ���
class CompareTwoValues
{
    /*
     *  return:
     *      1  : front_value > rear_value
     *      0  : front_value = rear_value
     *      -1 : front_value < rear_value
     */
    virtual int compare(void* front_value, void* rear_value) = 0;
};

// �Ƚ��ַ�����ʵ����(�������������ַ����ıȽ�)
class StringValuesCompare : public CompareTwoValues
{
public:

    /*
     *   �ڲ��Ὣ  void* ת���� const char* ���ͱȽ�
     */
    int compare(void* front_value, void* rear_value);
};

// ����ӿ�
class SequenceProcessor
{
public:

    /*
     *   ��  MAP ����������ָ�����ֶΰ�ָ��˳������
     *   result_seq_key_list ������õ�ָ������
     *
     *   TODO : Ŀǰ�Ȳ��ü򵥲��������㷨���������ö��ֲ��������㷨��������õ��㷨
     *
     *   return:
     *     0  : recource_data Ϊ��
     *     -1 : ָ���� seq_field_name ������
     *     -2 : ָ���� key_field_name ������
     *     -3 : ��������г��ִ���
     *     1  : ����
     */
    int process_sequence(map<string, string>& recource_data,
                         list<const char*>& result_seq_key_list,
                         const char* seq_field_name,
                         const char* key_field_name,
                         CompareTwoValues* compare,
                         bool is_acs);
};
#endif /* EX_COMPARE_H_ */
