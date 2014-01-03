/*
 * weight_content_list.cpp
 *
 *  Created on: 2013-4-25
 *      Author: guoweijiang
 */

#include "abstract_mcp_content.h"

using namespace std;
using namespace content;

const char* KeyBucket::ClassName = "KeyBucket";

AbstractMCPContent::AbstractMCPContent(const char* key_name)
{
    LOGD("[GWJ] %s: start", __FUNCTION__);

    this->_conn = mongo_pool->getConnection();

    this->decorator = NULL;
    this->total_counts = 0;
    this->key_name = key_name;
    this->id_s.clear();
    this->result_set.clear();
    this->select_fields.clear();

    this->_unit_num = 10;
    this->_unit_times = 1;
    this->_start_pos = this->_unit_num * (this->_unit_times - 1);

    LOGD("[GWJ] %s: end. unit_num[%d], unit_times[%d], start_pos[%d]",
         __FUNCTION__, this->_unit_num, this->_unit_times, this->_start_pos);
}

AbstractMCPContent::~AbstractMCPContent()
{
    LOGD("[GWJ] %s: start", __FUNCTION__);

    if( _conn != NULL )
    {
        mongo_pool->releaseConnection(_conn);
    }

    this->id_s.clear();
    this->select_fields.clear();

    this->_basic_coll.clear();
    this->_basic_ns.clear();

    LOGD("[GWJ] %s: end", __FUNCTION__);
}

// װ��װ����
void AbstractMCPContent::load_decorator(AbstractDecorator* dec)
{
    this->decorator = dec;
}

void AbstractMCPContent::free_data(mongo_cursor* cursor,
                                   bson* query,
                                   bson* fields,
                                   bson* bson_result)
{
    if(cursor != NULL)
    {
        mongo_cursor_destroy(cursor);
    }
    if(query != NULL && query->data != NULL)
    {
        bson_destroy(query);
    }
    if(fields != NULL && fields->data != NULL)
    {
        bson_destroy(fields);
    }
    if(bson_result != NULL && bson_result->data != NULL)
    {
        bson_destroy(bson_result);
    }
}

void AbstractMCPContent::set_unit_info(const uint32_t unit_num,
                                       const uint32_t unit_times)
{
    if(unit_num == 0)
    {
        _unit_num = 10;
    }
    else
    {
        _unit_num = unit_num;
    }
    if(unit_times == 0)
    {
        _unit_times = 1;
    }
    else
    {
        _unit_times = unit_times;
    }
    _start_pos = _unit_num * (_unit_times - 1);

    if(this->decorator != NULL)
    {
        // ����� Aggregate װ��������Ҫ����ҳ��Ϣ��װ����
        this->decorator->set_page_info(_start_pos, _unit_num);
    }

    LOGD("[GWJ] %s: unit_num[%d], unit_times[%d], start_pos[%d]]",
         __FUNCTION__, _unit_num, _unit_times, _start_pos);
}

// ר�Ŵ���� mongoDB ��ȡ�����������������ݣ����������
void AbstractMCPContent::set_vector_data_from_bson(bson_iterator* bson_it, vector<string>& list)
{
    bson_iterator sub[1];
    bson_iterator_subiterator(bson_it, sub);

    while(bson_iterator_next(sub))
    {
        list.push_back(bson_iterator_string(sub));
    }

    return;
}

// ר�Ŵ���� mongoDB ��ȡ�����������������ݣ����������
void AbstractMCPContent::set_vector_data_from_bson(bson_iterator* bson_it, vector<int>& list)
{
    bson_iterator sub[1];
    bson_iterator_subiterator(bson_it, sub);

    while(bson_iterator_next(sub))
    {
        list.push_back(bson_iterator_int(sub));
    }

    return;
}

// ר�Ŵ���� mongoDB ��ȡ�����������������ݣ����������
void AbstractMCPContent::set_vector_data_from_bson(bson_iterator* bson_it, vector<double>& list)
{
    bson_iterator sub[1];
    bson_iterator_subiterator(bson_it, sub);

    while(bson_iterator_next(sub))
    {
        list.push_back(bson_iterator_double(sub));
    }

    return;
}

/*
 *  ����� mongoDB ȡ�õ���� struct �ṹ�����飬�����κα��
 */
void AbstractMCPContent::set_struct_data_from_bson(bson_iterator* it,
                                                  vector<MCPStructItem>* target,
                                                  vector<const char*> key_vector)
{
        //LOGG("[GWJ] %s:  start. Tuple list[key:%s, value:%s]",
             //__FUNCTION__, bson_key, bson_value);

		int key_size = key_vector.size();
        //vector<MCPTupleItem>* detail_list = (vector<MCPTupleItem>*)tuple;

        bson_iterator sub[1];
        bson_iterator_subiterator(it, sub);
        bson_iterator subsub[1];
        //MCPTupleItem tuple_info;

        while(bson_iterator_next(sub))
        {
            bson_iterator_subiterator(sub, subsub);

			MCPStructItem struct_item;
            while(bson_iterator_next(subsub))
            {
				for(int i=0; i<key_size; i++)
				{
					string bson_key = bson_iterator_key(subsub);
					string need_key = key_vector[i];
					if(strcmp(bson_key.c_str(), need_key.c_str()) == 0)
					{
						string need_value = bson_iterator_string(subsub);
						struct_item.record.insert(pair<string, string>(need_key, need_value));

						LOGD("[zx] %s: push map [%s %s]",
									 __FUNCTION__, need_key.c_str(), need_value.c_str());
						break;
					}
					else 
					{
						continue;	
					}
				}
			}
			target->push_back( struct_item );
        }
}
/*
 *  ����� mongoDB ȡ�õ���� [key : value] �ṹ�����飬�����κα��
 *  KEY / VALUE ���� string ����
 */
void AbstractMCPContent::set_tuple_data_from_bson(bson_iterator* it,
                                                  void* tuple,
                                                  bool  is_vector,
                                                  char* bson_key,
                                                  char* bson_value)
{
    if(is_vector == true)
    {
        LOGG("[GWJ] %s:  start. Tuple list[key:%s, value:%s]",
             __FUNCTION__, bson_key, bson_value);

        vector<MCPTupleItem>* detail_list = (vector<MCPTupleItem>*)tuple;

        bson_iterator sub[1];
        bson_iterator_subiterator(it, sub);
        bson_iterator subsub[1];
        MCPTupleItem tuple_info;

        while(bson_iterator_next(sub))
        {
            bson_iterator_subiterator(sub, subsub);
            tuple_info.clear();

            while(bson_iterator_next(subsub))
            {
                if(strcmp(bson_iterator_key(subsub), bson_key) == 0)
                {
                    tuple_info.key = bson_iterator_string(subsub);

                    LOGG("[GWJ] %s: tuple_info[key:%s]",
                                 __FUNCTION__, tuple_info.key.c_str());
                }
                else if(strcmp(bson_iterator_key(subsub), bson_value) == 0)
                {
                    tuple_info.value = bson_iterator_string(subsub);

                    LOGG("[GWJ] %s: tuple_info[value:%s]",
                         __FUNCTION__, tuple_info.value.c_str());
                }
            }

            detail_list->push_back(tuple_info);
        }
    }
    else
    {
        LOGG("[GWJ] %s:  start. Tuple single[key:%s, value:%s]",
             __FUNCTION__, bson_key, bson_value);

        // TODO MCPTupleItem* detail_tuple = (MCPTupleItem*)tuple;
    }
}

/*
 *  ����� mongoDB ȡ�õ���� [key : value] �ṹ�����飬�����κα��
 *  VALUE �� int ����
 */
void AbstractMCPContent::set_tuple_data_from_bson_integer_value(bson_iterator* it,
                                                                void* tuple,
                                                                bool  is_vector,
                                                                char* bson_key,
                                                                char* bson_value)
{
    if(is_vector == true)
    {
        LOGG("[GWJ] %s:  start. Tuple list[key:%s, value:%s]",
             __FUNCTION__, bson_key, bson_value);

        vector<MCPTupleItem>* detail_list = (vector<MCPTupleItem>*)tuple;

        bson_iterator sub[1];
        bson_iterator_subiterator(it, sub);
        bson_iterator subsub[1];
        MCPTupleItem tuple_info;
        stringstream ss;

        while(bson_iterator_next(sub))
        {
            bson_iterator_subiterator(sub, subsub);
            tuple_info.clear();
            ss.clear();

            while(bson_iterator_next(subsub))
            {
                if(strcmp(bson_iterator_key(subsub), bson_key) == 0)
                {
                    tuple_info.key = bson_iterator_string(subsub);
                }
                else if(strcmp(bson_iterator_key(subsub), bson_value) == 0)
                {
                    ss.clear();
                    ss << bson_iterator_int(subsub);
                    ss >> tuple_info.value;
                }
            }

            detail_list->push_back(tuple_info);
        }
    }
    else
    {
        LOGG("[GWJ] %s:  start. Tuple single[key:%s, value:%s]",
             __FUNCTION__, bson_key, bson_value);

        // TODO MCPTupleItem* detail_tuple = (MCPTupleItem*)tuple;
    }
}

/*
 *  ����� mongoDB ȡ�õ���� [key : value] �ṹ�����飬�����κα��
 *  VALUE �� double ����
 */
void AbstractMCPContent::set_tuple_data_from_bson_double_value(bson_iterator* it,
                                                               void* tuple,
                                                               bool  is_vector,
                                                               char* bson_key,
                                                               char* bson_value)
{
    if(is_vector == true)
    {
        LOGG("[GWJ] %s:  start. Tuple list[key:%s, value:%s]",
             __FUNCTION__, bson_key, bson_value);

        vector<MCPTupleItem>* detail_list = (vector<MCPTupleItem>*)tuple;

        bson_iterator sub[1];
        bson_iterator_subiterator(it, sub);
        bson_iterator subsub[1];
        MCPTupleItem tuple_info;
        stringstream ss;

        while(bson_iterator_next(sub))
        {
            bson_iterator_subiterator(sub, subsub);
            tuple_info.clear();
            ss.clear();

            while(bson_iterator_next(subsub))
            {
                if(strcmp(bson_iterator_key(subsub), bson_key) == 0)
                {
                    tuple_info.key = bson_iterator_string(subsub);
                }
                else if(strcmp(bson_iterator_key(subsub), bson_value) == 0)
                {
                    ss.clear();
                    ss << bson_iterator_double(subsub);
                    ss >> tuple_info.value;
                }
            }

            detail_list->push_back(tuple_info);
        }
    }
    else
    {
        LOGG("[GWJ] %s:  start. Tuple single[key:%s, value:%s]",
             __FUNCTION__, bson_key, bson_value);

        // TODO MCPTupleItem* detail_tuple = (MCPTupleItem*)tuple;
    }
}

// ��������������  [_start_pos, _unit_num]
int AbstractMCPContent::modify_query_boundary(int res)
{
    LOGD("[GWJ] %s:  start. start[%d], len[%d], total[%d]",
         __FUNCTION__, _start_pos, _unit_num, res);

    if(res > 0 && (uint32_t)res < _start_pos)
    {
        _start_pos = ((res - 1) / _unit_num) * _unit_num;
    }

    LOGD("[GWJ] %s:  end. start[%d], len[%d], total[%d]",
         __FUNCTION__, _start_pos, _unit_num, res);

    return 0;
}

/*
 * ���������
 * ����װ�����趨�Ĳ�ѯ�������õ���ѯ�б��������
 * ���������������� [_start_pos, _unit_num]
 */
int AbstractMCPContent::get_list_count()
{
    LOGD("[GWJ] %s:  start", __FUNCTION__);

    if( _conn == NULL )
    {
        LOGD("[GWJ] %s:  mongo connection is NULL", __FUNCTION__);
        return -5;
    }

    int res = 0;
    bson query[1];

    // ��װ������װ�ξ���Ĳ�ѯ����
    bson_init(query);
    int dec_ret = this->decorator->get_count_query(query);
    if(dec_ret < 0)
    {
        bson_destroy(query);

        LOGD("[GWJ] %s: No filter Return!", __FUNCTION__);
        return -1;
    }
    bson_finish(query);

    res = mongo_count(_conn, MCP_MONGODB_NAME.c_str(), _basic_coll.c_str(), query);

    LOGD("[GWJ] %s: content list count is [%d], basic coll[%s]",
           __FUNCTION__, res, _basic_coll.c_str());

    if( res < 0 )
    {
        bson_destroy(query);

        LOGD("[GWJ] %s:  mongo_count failed. res[%d]", __FUNCTION__, res);
        return -1;
    }

    // ��������������  [_start_pos, _unit_num]
    //this->modify_query_boundary(res);
    bson_destroy(query);

    LOGD("[GWJ] %s:  end", __FUNCTION__);
    return res;
}

/*
 *  һ��ͨ�ýӿڣ�
 *  ���������κα��
 *  ����ָ����ɸѡ��������װ����ָ���ı��еõ���Ӧ�� id_list
 *  ��ʱĬ��������Ϣ�б���Ҫ�ҵĶ���  c_id
 */
int AbstractMCPContent::get_id_list(vector<string>& id_list)
{
    LOGD("[GWJ] %s:  start. [%d, %d]", __FUNCTION__, this->_start_pos, this->_unit_num);

    bson query, fields;
    vector<const char*> select_fields;
    mongo_cursor *cursor = NULL;
    const bson* result;

    if(this->decorator->secondary_table_name == NULL     ||
       this->decorator->secondary_query_key_name == NULL )
    {
        LOGD("[GWJ] %s: secondary collection or key_name is NULL! error", __FUNCTION__);
        return -5;
    }

    // ���ôӸ�����ɸѡ���ݵĲ�ѯ����
    bson_init(&query);
    int dec_ret = this->decorator->set_id_list_fetching_query(&query);
    if(dec_ret < 0)
    {
        free_data(cursor, &query, NULL, NULL);
        LOGD("[GWJ] %s: No filter Return", __FUNCTION__);
        return -1;
    }
    bson_finish(&query);

    // ���ò�ѯ����ʱҪɸѡ���� key ����һ��ֻҪ "c_id" ����
    select_fields.push_back(this->decorator->secondary_query_key_name);
    this->decorator->set_fields_bson(&fields, select_fields);
    select_fields.clear();

    cursor = mongo_find(_conn,
                        this->decorator->secondary_table_name,
                        &query,
                        &fields, 0, 0, 0);

    if( cursor == NULL )
    {
        LOGD("[GWJ] %s: mongo_find has no record", __FUNCTION__);
        free_data(cursor, &query, &fields, NULL);
        return -1;
    }

    LOGD("[GWJ] %s:  mongo_find success", __FUNCTION__);

    id_list.clear();
    int cnt =0;
    char ch[50];

    bson_iterator it[1];

    while(mongo_cursor_next(cursor)==MONGO_OK)
    {
        cnt++;
        result=mongo_cursor_bson(cursor);
        bson_find(it, result, this->decorator->secondary_query_key_name);
        memset(ch,0,sizeof(ch));
        strcpy(ch, bson_iterator_string(it));

        LOGD("[GWJ] %s: id[%s]", __FUNCTION__, ch);
        id_list.push_back(string(ch));
        LOGD("[GWJ] %s: push id_list[%d]:%s", __FUNCTION__, cnt-1, id_list[cnt-1].c_str());
    }

    free_data(cursor, &query, &fields, NULL);

    LOGD("[GWJ] %s: OK end, id_cnt[%d]", __FUNCTION__, cnt);
    return cnt;
}

/*
 *  һ��ͨ�ýӿڣ�
 *  ���������κα��
 *  ֱ�Ӹ����ƶ���װ�����е������� mongoDB �л�ȡ����
 *  �������ݱ��浽  result_set �� id_s ��
 *
 *  need_modify_page : �Ƿ���Ҫ����ָ��ҳ���Сȡ������
 *                     ��Ϊ false ʱ�򲻿���ҳ���С������ȡ��ȫ�����ݼ���
 */
int AbstractMCPContent::get_full_set_info(bool need_modify_page)
{
    LOGD("[GWJ] %s:  start", __FUNCTION__);

    if( _conn == NULL )
    {
        LOGD("[GWJ] %s: mongo connection is NULL", __FUNCTION__);
        return -5;
    }

    bson query[1], fields[1];
    mongo_cursor *cursor = NULL;

    // ���ݶ��Ƶ�װ���������ѯ����
    bson_init(query);
    int dec_ret = this->decorator->custom_result_set_query(query);
    if(dec_ret < 0)
    {
        LOGD("[GWJ] %s: No filter Return!", __FUNCTION__);
        free_data(cursor, query, NULL, NULL);
        return -1;
    }
    bson_finish(query);

    // ѡ��������
    this->decorator->set_fields_bson(fields, this->select_fields);

    // ����� Aggregate ���͵������Ҫһ������Ĺ���
    if(this->decorator->is_aggregate_query() == true)
    {
        LOGD("[GWJ] %s: Is Aggregate Query. Use [mongo_run_command]. table[%s]",
             __FUNCTION__, _basic_ns.c_str());

        bson out[1];
        int res = mongo_run_command(_conn, MONGO_DB_NAME, query, out);

        if(res!=0)
        {
            LOGD("[GWJ] %s: run mongo_command failed. Error!!", __FUNCTION__);
            free_data( NULL, query, fields, NULL );
            return -1;
        }

        bson_iterator it[1], sub[1];
        bson subsub[1];
        bson_find(it, out, "result");
        bson_iterator_subiterator( it, sub );

        while(bson_iterator_next(sub))
        {
            bson_iterator_subobject(sub, subsub);

            // ���ݲ�ͬ�ı�񣬸���������������α���õ������� Record ���ݣ��� aggregate
            const char* aggregate_name = this->decorator->get_aggregate_extra_name();
            int res = this->set_basic_info_from_bson_result(subsub, aggregate_name);

            if(res == 0)
            {
                LOGD("[GWJ] %s: push id to [MAP_Size:%hu]",
                      __FUNCTION__, this->result_set.size());
            }
        }

        free_data( NULL, query, fields, out );
        LOGD("[GWJ] %s: end", __FUNCTION__);
        return 0;
    }
    else
    {
        if(need_modify_page)
        {
            // ���չ涨ҳ���С��ȡ��¼
            LOGD("[GWJ] %s: mongo_find[%s]. _unit_num[%d], _start_pos[%d]",
                 __FUNCTION__, _basic_ns.c_str(), _unit_num, _start_pos);

            cursor = mongo_find( _conn, _basic_ns.c_str(), query, fields, _unit_num, _start_pos, 0 );
        }
        else
        {
            // ����Ҫ����ҳ���С������ȡ�����м�¼
            LOGD("[GWJ] %s: mongo_find[%s]. No page", __FUNCTION__, _basic_ns.c_str());
            cursor = mongo_find( _conn, _basic_ns.c_str(), query, fields, 0, 0, 0 );
        }

        if(cursor == NULL)
        {
            LOGD("[GWJ] %s: mongo_find has no record[%s]", __FUNCTION__, _basic_ns.c_str());
            free_data( NULL, query, fields, NULL );
            return -1;
        }

        while(mongo_cursor_next(cursor) == MONGO_OK)
        {
            bson* bson_out = (bson*)mongo_cursor_bson(cursor);

            // ���ݲ�ͬ�ı�񣬸���������������α���õ������� Record ���ݣ��� aggregate
            int res = this->set_basic_info_from_bson_result(bson_out, NULL);

            if(res == 0)
            {
                LOGD("[GWJ] %s: push id to [MAP_Size:%hu]",
                        __FUNCTION__, this->result_set.size());
            }
        }

        free_data( cursor, query, fields, NULL );
        LOGD("[GWJ] %s: end", __FUNCTION__);
        return 0;
    }
}

/*
 *  һ��ͨ�ýӿڣ�
 *  ���������κα��
 *  ���ݴ���� id_list ��ָ����Ŀ�������ҵ���Ӧ��ȫ����
 *  �������ݱ��浽  result_set �� id_s ��
 *  ����  id_s ����ָ��  result_set ʵ�����ݵ�ָ�룬��֤��Сռ���ڴ棬ֻ��һ�����ݿ���
 */
int AbstractMCPContent::get_full_set_info_by_id_list(vector<string>& id_list)
{
    LOGD("[GWJ] %s:  start", __FUNCTION__);

    if( _conn == NULL )
    {
        LOGD("[GWJ] %s: mongo connection is NULL", __FUNCTION__);
        return -5;
    }

    bson query[1], fields[1];
    mongo_cursor *cursor = NULL;

    /*
     *  ����ָ���� id_list ���ò������
     *  �����վ���ҵ��涨�Ĳ�ѯ������������ query ���
     */
    bson_init(query);
    int dec_ret = this->decorator->custom_result_set_query(query);
    if(dec_ret < 0)
    {
        free_data( cursor, query, NULL, NULL );
        LOGD("[GWJ] %s: No Filter Logic Error!", __FUNCTION__);
        return -1;
    }

    // �ȸ��� id_list �ҵ����ϸ��������ļ�¼
    this->decorator->set_basic_info_query_by_id_list(id_list, query);
    bson_finish(query);

    CommonInterface::clear_vector_string(id_list);

    // ѡ��������
    this->decorator->set_fields_bson(fields, this->select_fields);

    cursor = mongo_find( _conn, _basic_ns.c_str(), query, fields, _unit_num, _start_pos, 0 );

    if(cursor == NULL)
    {

        LOGD("[GWJ] %s: mongo_find has no recordt", __FUNCTION__);
        free_data( NULL, query, fields, NULL );
        return -5;
    }

    while(mongo_cursor_next(cursor) == MONGO_OK)
    {
        bson* bson_out = (bson*)mongo_cursor_bson(cursor);

        // ���ݲ�ͬ�ı�񣬸���������������α���õ������� Record ����
        int res = this->set_basic_info_from_bson_result(bson_out, NULL);

        if(res == 0)
        {
            LOGD("[GWJ] %s: push id to [MAP_Size:%hu]",
                    __FUNCTION__, this->result_set.size());
        }
    }

    free_data( cursor, query, fields, NULL );

    LOGD("[GWJ] %s:  end. result_set size[%hu]", __FUNCTION__, this->result_set.size());
    return 0;
}

/*
 *  ���ؽ����������
 *  �����򲻵��ѣ�������Ҫʹ���������
 */
vector<const char*>* AbstractMCPContent::get_result_keys()
{
    return &this->id_s;
}

/*
 *  ���ؽ�����е���������
 */
int AbstractMCPContent::get_key_size()
{
    return this->id_s.size();
}

/*
 *  ��ȡ������е�����ֵ
 */
const char* AbstractMCPContent::get_key_value(int key_index)
{
    int size = this->id_s.size();
    LOGD("[GWJ] %s: start. key_size[%d]", __FUNCTION__, size);

    if(size == 0)
    {
        LOGD("[GWJ] %s: No Key Index[%d]", __FUNCTION__, size);
        return NULL;
    }

    if(key_index < 0 || size <= key_index)
    {
        LOGD("[GWJ] %s: key_index[%d] out of bound[%d]",
             __FUNCTION__, key_index, size);

        return NULL;
    }

    const char* key_value = this->id_s[key_index];
    LOGD("[GWJ] %s: key_value[%s]", __FUNCTION__, key_value);

    return key_value;
}
/*
 *  ��ȡ������е�����ֵ
 */
const char* AbstractMCPContent::get_key_value(const char* key)
{
    int key_index = CommonInterface::find_key_index(key, this->id_s);

    if(key_index < 0)
    {
        LOGD("[GWJ] %s: No Key_Index[%s]", __FUNCTION__, key);
        return NULL;
    }

    return this->get_key_value(key_index);
}

/*
 *  ��ȡͬһ�����µ����м�¼
 */
KeyBucket* AbstractMCPContent::get_key_bucket(int key_index)
{
    const char* key_value = this->get_key_value(key_index);

    if(key_value == NULL)
    {
        LOGD("[GWJ] %s: key_index[%d] Error!", __FUNCTION__, key_index);
        return NULL;
    }

    LOGD("[GWJ] %s: key_value[%s]", __FUNCTION__, key_value);
    return this->result_set[key_value];
}

/*
 *  ��ȡͬһ�����µ����м�¼
 */
KeyBucket* AbstractMCPContent::get_key_bucket(const char* key)
{
    const char* key_value = this->get_key_value(key);

    if(key_value == NULL)
    {
        LOGD("[GWJ] %s: key[%s] Error!", __FUNCTION__, key);
        return NULL;
    }

    LOGD("[GWJ] %s: key_value[%s]", __FUNCTION__, key_value);
    return this->result_set[key_value];
}

/*
 *  ��ȡͬһ�����µ����м�¼����
 */
int AbstractMCPContent::get_key_bucket_size(int key_index)
{
    KeyBucket* key_bucket = this->get_key_bucket(key_index);

    if(key_bucket == NULL)
    {
        return 0;
    }
    else
    {
        return key_bucket->size();
    }
}

/*
 *  ��ȡͬһ�����µ����м�¼����
 */
int AbstractMCPContent::get_key_bucket_size(const char* key)
{
    KeyBucket* key_bucket = this->get_key_bucket(key);

    if(key_bucket == NULL)
    {
        return 0;
    }
    else
    {
        return key_bucket->size();
    }
}

/*
 *  һ��ͨ�ýӿڣ�
 *  ���ؽ��������ͬ id �����м�¼
 *  key_index : ����  key_index �ҵ��������Դ� key_index �� records
 *  vec_index : �ҵ����ڴ� KEY �����ĵڼ�����¼
 */
AbstractMCPRecord* AbstractMCPContent::get_record_by_key_vec(int key_index,
                                                             int vec_index)
{
    KeyBucket* key_vec_records = this->get_key_bucket(key_index);

    if(key_vec_records == NULL)
    {
        LOGD("[GWJ] %s: No Key_Index[%d]", __FUNCTION__, key_index);
        return NULL;
    }

    int vec_size = key_vec_records->size();

    if(vec_size == 0)
    {
        LOGD("[GWJ] %s: No Vector of key_index[%d]",
             __FUNCTION__, key_index);

        return NULL;
    }

    if(vec_index < 0 || vec_size <= vec_index)
    {
        LOGD("[GWJ] %s: vec_index[%d] out of bound[%d]",
             __FUNCTION__, vec_index, vec_size);

        return NULL;
    }

    LOGD("[GWJ] %s: Found Record[key:%d, vec:%d]",
         __FUNCTION__, key_index, vec_index);

    return (*key_vec_records)[vec_index];
}

/*
 *  һ��ͨ�ýӿڣ�
 *  ���ؽ��������ͬ id �����м�¼
 *  key       : ���� key �ҵ��������Դ� key �� records
 *  vec_index : �ҵ����ڴ� KEY �����ĵڼ�����¼
 */
AbstractMCPRecord* AbstractMCPContent::get_record_by_key_vec(const char* key,
                                                             int vec_index)
{
    int key_index = CommonInterface::find_key_index(key, this->id_s);

    if(key_index < 0)
    {
        LOGD("[GWJ] %s: No key[%s]", __FUNCTION__, key);
        return NULL;
    }

    return this->get_record_by_key_vec(key_index, vec_index);
}

int AbstractMCPContent::get_total_result_count()
{
    return this->total_counts;
}

/*
 *  һ��ͨ�ýӿڣ�
 *  ��������������ѯ����
 *  �������õ�װ����(decorator Ŀ¼)�õ���ѯ���
 *  ���ѽ�����浽 id_s �� result_set ��
 *
 *  need_modify_page : �Ƿ���Ҫ����ָ��ҳ���Сȡ������
 *                     ��Ϊ false ʱ�򲻿���ҳ���С������ȡ��ȫ�����ݼ���
 */
int AbstractMCPContent::general_fetch_list_records(bool need_modify_page)
{
    LOGD("[GWJ] %s:  start", __FUNCTION__);

    int res = 0;

    // ���ж��Ƿ���ҪǶ�ײ�ѯ
    if(this->decorator->need_get_id_list())
    {
        /*
         *  ��ѯ�ı��û��ָ�������� id ������Ҫ����Ӧ�����ѯ c_id
         *  Ȼ���ٸ��ݲ�ѯ����  c_id ��Ϊ��ѯ�������������������ѯ�ı��
         */
        vector<string> id_list;
        res = this->get_id_list(id_list);

        if(res < 0)
        {
            LOGD("[GWJ] %s: get_id_list failed", __FUNCTION__);
            return res;
        }

        this->total_counts = id_list.size();

        /*
         *  ���ݴӸ����в�ѯ���� c_id ������ѯ����
         */
        res = this->get_full_set_info_by_id_list(id_list);

        if(res < 0)
        {
            LOGD("[GWJ] %s:  get_content_basic_info_by_id_list failed!", __FUNCTION__);
            return res;
        }
    }
    else
    {
        // ��ѯ�ı����ָ�������� id ��ֱ�Ӳ����

        // ��ò�ѯ��¼���ܸ���
        if(need_modify_page)
        {
            this->total_counts = this->get_list_count();
        }
        else
        {
            this->total_counts = 1;
        }

        LOGD("[GWJ] %s:  count is %d", __FUNCTION__, this->total_counts);

        if(this->total_counts == 0)
        {
            return 0;
        }
        else if(this->total_counts < 0)
        {
            LOGD("[GWJ] %s:  count[%d] error", __FUNCTION__, this->total_counts);
            return this->total_counts;
        }

        // ����װ����ָ���Ĳ�ѯ������ѯ mongoDB
        res = this->get_full_set_info(need_modify_page);

        if(res < 0)
        {
            LOGD("[GWJ] %s: get_full_set_info failed!", __FUNCTION__);
            this->total_counts = 0;
            return res;
        }

        if(!need_modify_page)
        {
            this->total_counts = this->get_key_size();
        }
    }

    LOGD("[GWJ] %s: end OK", __FUNCTION__);
    return this->total_counts;
}

/*
 *  �� mongoDB �е�һ�� collection ɾ����
 */
int AbstractMCPContent::general_drop_collection()
{
    if(this->_conn == NULL)
    {
        LOGD("[GWJ] %s: mongo connection is NULL", __FUNCTION__);
        return -5;
    }

    mongo_cmd_drop_collection(this->_conn, MCP_MONGODB_NAME.c_str(), this->_basic_coll.c_str(), NULL);
    return 0;
}

/*
 *  �� mongoDB �в�������
 */
int AbstractMCPContent::general_insert_records()
{
    LOGD("[GWJ] %s: start", __FUNCTION__);

    if(this->_conn == NULL)
    {
        LOGD("[GWJ] %s: mongo connection is NULL", __FUNCTION__);
        return -5;
    }

    if(this->decorator != NULL)
    {
        bson query[1];
        bson_init(query);
        int dec_ret = this->decorator->custom_result_set_query(query);
        if(dec_ret < 0)
        {
            bson_finish(query);
            LOGD("[GWJ] %s: No Filter Logic Error!", __FUNCTION__);
            return -1;
        }

        mongo_insert(this->_conn, this->_basic_ns.c_str(), query);
        bson_finish(query);

        LOGD("[GWJ] %s: end", __FUNCTION__);
        return 0;
    }
    else
    {
        LOGD("[GWJ] %s: No Decorator Loaded Error!", __FUNCTION__);
        return -1;
    }
}

void AbstractMCPContent::add_select_column(const char* column_name)
{
    if(IS_VALUED_STRING(column_name))
    {
        LOGD("[GWJ] %s: add column[%s]", __FUNCTION__, column_name);
        this->select_fields.push_back(column_name);
    }
    else
    {
        LOGD("[GWJ] %s: column_name[%s] Empty!!", __FUNCTION__, column_name);
    }
}

int AbstractMCPContent::general_get_list_records_count()
{
    LOGD("[GWJ] %s:  start", __FUNCTION__);

    this->total_counts = this->get_list_count();

    LOGD("[GWJ] %s:  end. count[%d]",
         __FUNCTION__, this->total_counts);

    return this->total_counts;
}

string AbstractMCPContent::convert_mongo_ts_to_string(bson_iterator* it)
{
    time_t ts= bson_iterator_time_t(it);
    struct tm *t;
    t = gmtime(&ts);
    string time_string;
    char c_time[32];
    memset(c_time, 0x00, sizeof(c_time));
    sprintf( c_time, "%04d-%02d-%02d %02d:%02d:%02d", t->tm_year+1900,t->tm_mon+1,t->tm_mday,t->tm_hour,t->tm_min,t->tm_sec );
    time_string = c_time;
    return time_string;
}
