/*
 * ex_compare.cpp
 *
 *  Created on: 2013-7-17
 *      Author: guoweijiang
 */

#include "ex_compare.h"

/*
 *  return:
 *      1  : front_value > rear_value
 *      0  : front_value = rear_value
 *      -1 : front_value < rear_value
 */
int StringValuesCompare::compare(void* front_value,
                                 void* rear_value)
{
    const char* f_val = (const char*)front_value;
    const char* r_val = (const char*)rear_value;

    if(!IS_VALUED_STRING(f_val))
    {
        LOGD("[GWJ] %s: f_val[%s] Empty!!", __FUNCTION__, f_val);
        return -1;
    }

    if(!IS_VALUED_STRING(r_val))
    {
        LOGD("[GWJ] %s: r_val[%s] Empty!!", __FUNCTION__, r_val);
        return 1;
    }

    int f_len = strlen(f_val);
    int r_len = strlen(r_val);

    if(f_len > r_len)
    {
        LOGD("[GWJ] %s: f_val[%s : %d] > r_val[%s : %d]",
             __FUNCTION__, f_val, f_len, r_val, f_len);

        return 1;
    }
    else if(f_len < r_len)
    {
        LOGD("[GWJ] %s: f_val[%s : %d] < r_val[%s : %d]",
             __FUNCTION__, f_val, f_len, r_val, f_len);

        return -1;
    }
    else
    {
        for(int i = 0; i < f_len; i++)
        {
            if(f_val[i] > r_val[i])
            {
                LOGD("[GWJ] %s: f_val[%s : %d] > r_val[%s : %d]",
                     __FUNCTION__, f_val, f_len, r_val, f_len);

                return 1;
            }
            else if(f_val[i] < r_val[i])
            {
                LOGD("[GWJ] %s: f_val[%s : %d] < r_val[%s : %d]",
                     __FUNCTION__, f_val, f_len, r_val, f_len);

                return -1;
            }
        }

        LOGD("[GWJ] %s: f_val[%s : %d] = r_val[%s : %d]",
              __FUNCTION__, f_val, f_len, r_val, f_len);

        return 0;
    }
}

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
int SequenceProcessor::process_sequence(map<string, string>& recource_data,
                                        list<const char*>& result_seq_key_list,
                                        const char* seq_field_name,
                                        const char* key_field_name,
                                        CompareTwoValues* compare,
                                        bool is_acs)
{
    int res_size = recource_data.size();
    LOGD("[GWJ] %s: resource_size[%d], seq_name[%s], key_name[%s], is_acs[%d]",
         __FUNCTION__, res_size, seq_field_name, key_field_name, is_acs);


}
