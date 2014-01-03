/*
 * abstract_compound_decorator.cpp
 *
 *  Created on: 2013-4-26
 *      Author: guoweijiang
 */

#include "abstract_compound_decorator.h"

using namespace std;

AbstractCompoundDecorator::AbstractCompoundDecorator()
{
    LOGD("[GWJ] %s:  start", __FUNCTION__);

    this->primary_table_query.clear();
    this->secondery_table_query.clear();

    // �����װ����ͳһΪ�������߼�
    this->is_special = false;

    LOGD("[GWJ] %s:  end", __FUNCTION__);
}

AbstractCompoundDecorator::~AbstractCompoundDecorator()
{
    LOGD("[GWJ] %s:  start", __FUNCTION__);

    this->primary_table_query.clear();
    this->secondery_table_query.clear();

    LOGD("[GWJ] %s:  end", __FUNCTION__);
}

void AbstractCompoundDecorator::add_primary_table_decorator(AbstractSingleDecorator* decorator)
{
    this->primary_table_query.push_back(decorator);
}

void AbstractCompoundDecorator::add_secondery_table_decorator(AbstractSingleDecorator* decorator)
{
    this->secondery_table_query.push_back(decorator);
}

/* add by zhengxie */
void AbstractCompoundDecorator::remove_primary_table_decorator(AbstractSingleDecorator* decorator)
{
	vector<AbstractDecorator*>::iterator it;
	vector<AbstractDecorator*>& vec = this->primary_table_query;
	for(it=vec.begin(); it!=vec.end(); it++)
	{
		if(*it == decorator) {
			vec.erase(it);
			break;
		}
	}
    LOGD("[GWJ] %s: after remove primary_table_query size[%d]", __FUNCTION__, (int)vec.size());
}

/* add by zhengxie */
void AbstractCompoundDecorator::remove_secondery_table_decorator(AbstractSingleDecorator* decorator)
{
	vector<AbstractDecorator*>::iterator it;
	vector<AbstractDecorator*>& vec = this->secondery_table_query;
	for(it=vec.begin(); it!=vec.end(); it++)
	{
		if(*it == decorator) {
			vec.erase(it);
			break;
		}
	}
    LOGD("[GWJ] %s: after remove secondery_table_query size[%d]", __FUNCTION__, (int)vec.size());
}

/* add by zhengxie */
void AbstractCompoundDecorator::remove_all_primary_table_decorator()
{
	this->primary_table_query.clear();
    LOGD("[GWJ] %s: after remove all primary_table_query size[%d]", __FUNCTION__, (int)(this->primary_table_query.size()));
}

/* add by zhengxie */
void AbstractCompoundDecorator::remove_all_secondery_table_decorator()
{
	this->secondery_table_query.clear();
    LOGD("[GWJ] %s: after remove all secondery_table_query size[%d]", __FUNCTION__, (int)(this->secondery_table_query.size()));
}
int AbstractCompoundDecorator::set_id_list_fetching_query(bson* query)
{
    int size = this->secondery_table_query.size();

    vector<int> basic_logic;

    // һ�β�ѯ��ֻ������һ�� special �߼�
    int special_logic = -1;

    LOGD("[GWJ] %s: secondery_table_query size[%d]", __FUNCTION__, size);

    if(size == 0)
    {
        LOGD("[GWJ] %s: secondery_table_query NULL error!!", __FUNCTION__);
        return -1;
    }

    // ���ֻ����߼��� special �߼�
    for(int i = 0; i < size; i++)
    {
        if(this->secondery_table_query[i]->is_special_query())
        {
            special_logic = i;
        }
        else
        {
            basic_logic.push_back(i);
        }
    }

    size = basic_logic.size();

    if(special_logic != -1 && size > 0)
    {
        LOGD("[GWJ] %s: Have special Logic[%d]. Begin $query", __FUNCTION__, special_logic);
        bson_append_start_object( query, "$query" );
    }

    for(int i = 0; i < size; i++)
    {
        int ret = this->secondery_table_query[i]->custom_result_set_query(query);

        if(ret < 0)
        {
            LOGD("[GWJ] %s: Logic[%d]. Empty", __FUNCTION__, i);
            return -1;
        }
    }

    if(special_logic != -1)
    {
        if(size > 0)
        {
            LOGD("[GWJ] %s: Have special Logic. End $query", __FUNCTION__);
            bson_append_finish_object(query);
        }

        this->secondery_table_query[special_logic]->custom_result_set_query(query);
    }

    return 1;
}

/*
 *  һ�����ӿڣ�
 *
 *  �����ڼ��� count ʱ��֧�� is_special_query(���磺orderby) �߼�
 *  ������ count ʱҪ�ٳ���Щ�߼������������߼�����
 *  ��������Ҫ���ݾ���ҵ����Ҫ�����ղ�ͬ��Ҫ�������Ӧ������Ƕ��
 */
int AbstractCompoundDecorator::get_count_query(bson* query)
{
    int size = this->primary_table_query.size();

    vector<int> basic_logic;

    // һ�β�ѯ��ֻ������һ�� special �߼�
    int special_logic = -1;

    LOGD("[GWJ] %s: primary_table_query size[%d]", __FUNCTION__, size);

    if(size == 0)
    {
        LOGD("[GWJ] %s: primary_table_query NULL error!!", __FUNCTION__);
        return -1;
    }

    // ���ֻ����߼��� special �߼�
    for(int i = 0; i < size; i++)
    {
        if(this->primary_table_query[i]->is_special_query())
        {
            LOGD("[GWJ] %s: %d is special logic", __FUNCTION__, i);
            special_logic = i;
        }
        else
        {
            LOGD("[GWJ] %s: %d is basic logic", __FUNCTION__, i);
            basic_logic.push_back(i);
        }
    }

    size = basic_logic.size();
    LOGD("[GWJ] %s: basic logic size[%d]", __FUNCTION__, size);

    // ���� count ʱִֻ�л����߼�
    for(int i = 0; i < size; i++)
    {
        int b_logic = basic_logic[i];
        LOGD("[GWJ] %s: basic logic index[%d]", __FUNCTION__, b_logic);

        int res = this->primary_table_query[b_logic]->custom_result_set_query(query);

        if(res < 0)
        {
            LOGD("[GWJ] %s: decorator [%d] error!!", __FUNCTION__, i);
            return -1;
        }
    }

    return 1;
}

/*
 *  һ�����ӿڣ�
 *
 *  һ��򵥵�ҵ���У��򵥵�Ƕ�����в�ѯ��������
 *  һ�����ӿڵ�ʵ���У��Ὣ���в�ѯ�����򵥵�Ƕ����һ��
 *
 *  ��ע�⡿��
 *  ����� is_special_query �߼�ʱ����Ҫ�������߼��� $query Ƕ������
 *
 *  ��������Ҫ���ݾ���ҵ����Ҫ�����ղ�ͬ��Ҫ�������Ӧ������Ƕ��
 */
int AbstractCompoundDecorator::custom_result_set_query(bson* query)
{
    int size = this->primary_table_query.size();

    vector<int> basic_logic;

    // һ�β�ѯ��ֻ������һ�� special �߼�
    int special_logic = -1;

    LOGD("[GWJ] %s: primary_table_query size[%d]", __FUNCTION__, size);

    if(size == 0)
    {
        LOGD("[GWJ] %s: primary_table_query NULL error!!", __FUNCTION__);
        return -1;
    }

    // ���ֻ����߼��� special �߼�
    for(int i = 0; i < size; i++)
    {
        if(this->primary_table_query[i]->is_special_query())
        {
            special_logic = i;
        }
        else
        {
            basic_logic.push_back(i);
        }
    }

    size = basic_logic.size();

    if(special_logic != -1 && size > 0)
    {
        LOGD("[GWJ] %s: Have special Logic[%d]. Begin $query", __FUNCTION__, special_logic);
        bson_append_start_object( query, "$query" );
    }

    for(int i = 0; i < size; i++)
    {
        int b_logic = basic_logic[i];
        LOGD("[GWJ] %s: basic logic index[%d]", __FUNCTION__, b_logic);

        int ret = this->primary_table_query[b_logic]->custom_result_set_query(query);

        if(ret < 0)
        {
            LOGD("[GWJ] %s: Logic[%d]. Empty", __FUNCTION__, i);
            return -1;
        }
    }

    if(special_logic != -1)
    {
        if(size > 0)
        {
            LOGD("[GWJ] %s: Have special Logic. End $query", __FUNCTION__);
            bson_append_finish_object(query);
        }

        this->primary_table_query[special_logic]->custom_result_set_query(query);
    }

    return 1;
}
