/*
 * abstract_decorator.cpp
 *
 *  Created on: 2013-4-26
 *      Author: guoweijiang
 */

#include "abstract_decorator.h"

using namespace std;
using namespace content;

AbstractDecorator::AbstractDecorator()
{
    this->secondary_table_name = NULL;

    // �Ӹ����в�ѯ���һ����Ҫ "c_id" ����
    this->secondary_query_key_name = "c_id";

    // �������в�ѯ���һ��ؼ������� "id"
    this->primary_query_key_name = "id";

    // Ĭ�ϲ��� aggregate ����װ����
    this->is_aggregate = false;

    // Ĭ��û�� aggregate ����������ֶ���
    this->extra_name = NULL;
}

AbstractDecorator::~AbstractDecorator()
{
}

/*
 *  ������ mongoDB ��Ŀ�������û��Ҫ��ѯɸѡ�� xxx_id��
 *  ��ʱ����Ҫ��  secondary_table_name ����в�ѯ xxx_id ��Ӧ�� c_id
 *  Ȼ���ٸ���   c_id ��ѯĿ����
 *  ��ʱ����Ҫһ���жϽӿڣ��ж��Ƿ���Ҫ�Ȳ� id_list
 */
bool AbstractDecorator::need_get_id_list()
{
    // �ж��Ƿ������˸����������ж��Ƿ���ҪǶ�ײ�ѯ
    if(this->secondary_table_name == NULL)
    {
        return false;
    }
    else
    {
        return true;
    }
}

/*
 * ��������ɸѡ�� xxx_id ���ò�ѯ���
 * ɸѡ�����Ͱ��� filter_classes �� filter_columns �� filter_labels ��
 */
void AbstractDecorator::set_filter_query(vector<const char*>& filter_list,
                                         bson* query,
                                         const char* filter_log_ptr,
                                         const char* bson_filter)
{
    int size = filter_list.size();
    LOGD("[GWJ] %s:  start. %s_size[%d]", __FUNCTION__, filter_log_ptr, size);

    if(size > 0)
    {
        char index[8];
        bson_append_start_object(query, bson_filter);
        bson_append_start_array(query, "$in");

        for(int i = 0; i < size; i++)
        {
            memset(index, 0x00, sizeof(index));
            sprintf(index, "%d", i);

            bson_append_string(query, index, filter_list[i]);
            LOGD("[GWJ] %s: set %s[%s]", __FUNCTION__, bson_filter, filter_list[i]);
        }

        bson_append_finish_array(query);
        bson_append_finish_object(query);
    }
    else if(size == 0)
    {
        LOGD("[GWJ] %s: %s no filter", __FUNCTION__, filter_log_ptr);
    }
}

/*
 *  һ��ͨ�ýӿڣ�
 *  �������е� id_list ������ѯɸѡ id ������
 *  ����ӿ�ר������ͨ���Ӹ������ҵ��� id_list �����ձ�
 */
void AbstractDecorator::set_basic_info_query_by_id_list(vector<string>& id_list,
                                                        bson* query)
{
    LOGD("[GWJ] %s:  start. id_list size[%d]",
         __FUNCTION__, id_list.size());

    char index[8];
    bson_append_start_object(query, this->primary_query_key_name);
    bson_append_start_array(query, "$in");

    vector<string>::const_iterator it = id_list.begin();

    for(int i = 0; it != id_list.end(); ++it, ++i)
    {
        memset(index, 0x00, sizeof(index));
        sprintf(index, "%d", i);
        bson_append_string(query, index, it->c_str());
        LOGD("[GWJ] %s:  push id[%s]", __FUNCTION__, it->c_str());
    }

    bson_append_finish_array(query);
    bson_append_finish_object(query);
    LOGD("[GWJ] %s:  end", __FUNCTION__);
    return;
}

/*
 *  һ��ͨ�ýӿڣ�
 *  ���ݴ���� select_fields ����ɸѡ�������������
 */
void AbstractDecorator::set_fields_bson(bson* query, vector<const char*>& select_fields)
{
    LOGD("[GWJ] %s:  start. select_fields size[%d]",
             __FUNCTION__, select_fields.size());

    bson_init(query);

    int size = select_fields.size();

    for(int i = 0; i < size; i++)
    {
        if(select_fields[i] != NULL)
        {
            bson_append_int(query, select_fields[i], 1);
            LOGD("[GWJ] %s:  select[%s]", __FUNCTION__, select_fields[i]);
        }
    }

    bson_finish(query);

    LOGD("[GWJ] %s:  end", __FUNCTION__);
    return;
}

/*
 *  ʵ��һ�����ӿڣ�
 *  Ĭ��ִ�������  custom_result_set_query ����
 *
 *  �ܶ������ custom_result_set_query ��ͬ
 *  set_id_list_fetching_query ���õ��Ǵ�  secondary_table_name �����в�ѯ������
 *  custom_result_set_query ���õľ��Ǵ�����Ŀ����в�ѯ������
 */
int AbstractDecorator::set_id_list_fetching_query(bson* query)
{
    // ��ʱĬ��Ϊ  custom_result_set_query , ���������������������
    return this->custom_result_set_query(query);
}

/*
 *  һ�����ӿڣ�
 *
 *  ���� single decorator ��˵ֱ�ӵ��ú��ķ�������
 *  �� compound decorator �л���д���麯��
 */
int AbstractDecorator::get_count_query(bson* query)
{
    return this->custom_result_set_query(query);
}

bool AbstractDecorator::is_special_query()
{
    return this->is_special;
}

bool AbstractDecorator::is_aggregate_query()
{
    return this->is_aggregate;
}

const char* AbstractDecorator::get_aggregate_extra_name()
{
    LOGD("[GWJ] %s: AggregateColumn[%s]",
         __FUNCTION__, this->extra_name);

    return this->extra_name;
}

void AbstractDecorator::set_page_info(const uint32_t start_pos,
                                      const uint32_t page_size)
{
    LOGD("[GWJ] %s: Aggregate Page[start_pos:%d, size:%d]",
         __FUNCTION__, start_pos, page_size);

    this->start_pos = start_pos;
    this->page_size = page_size;
}
