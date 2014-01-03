/*
 * abstract_mcp_content.h
 *
 *  ������ܵĳ��� module
 *  ����ִ�д� mongoDB �л�ȡ��ѯ���ݵ�ģ��
 *  ��ȡ�õ����ݱ������ڲ��Ľ������
 *
 *  Created on: 2013-4-25
 *      Author: guoweijiang
 */

#ifndef WEIGHT_CONTENT_LIST_H_
#define WEIGHT_CONTENT_LIST_H_

#include "../common.h"
#include "../decorator/abstract_decorator.h"
#include "../factory/object_pool_factory.h"

using namespace std;
using namespace content;

#define MONGO_DB_NAME "mcp"

/*
 *  ר�����ڰѴ����ݿ��еõ���һ����¼���뵽  result_set ��
 *
 *  1. ����� result_set.key ��Ӧ��� Record
 *     ��ֻ�õ�һ�� Record �� key_name.c_str() ָ����Ϊ����
 *
 *  2. ������ָ������  id_s ��
 */
#define FILL_RESULT_BUCKET(detail, key_name)\
{\
    int index = CommonInterface::find_key_index(detail->key_name.c_str(), this->id_s);\
    LOGD("[GWJ] %s: current key[%s], index[%d]", __FUNCTION__, detail->key_name.c_str(), index);\
\
    if(index < 0)\
    {\
        LOGD("[GWJ] %s: 1st Record of key[%s]", __FUNCTION__, detail->key_name.c_str());\
        this->id_s.push_back(detail->key_name.c_str());\
\
        KeyBucket* vec_records = ObjectPoolFactory<KeyBucket>::instance().fetch_object();\
\
        vec_records->push_back(detail);\
\
        int size = this->id_s.size();\
        this->result_set[this->id_s[size-1]] = vec_records;\
    }\
    else\
    {\
        LOGD("[GWJ] %s: Exist Record of key[%d:%s]. list_id[size:%hu]",\
             __FUNCTION__, index, this->id_s[index], this->id_s.size());\
\
        this->result_set[this->id_s[index]]->push_back(detail);\
    }\
}

/*
 *  ר�Ÿ�����ս�����дӶ���ػ�ȡ�Ķ���
 */
#define RECYCLE_RESULT_SET(type_name)\
{\
    int size = this->id_s.size();\
\
    for(int i = 0; i < size; i++)\
    {\
        KeyBucket* vec_records = this->result_set[this->id_s[i]];\
\
        if(vec_records != NULL)\
        {\
            int vec_size = vec_records->size();\
\
            for(int vec_index = 0; vec_index < vec_size; vec_index++)\
            {\
                LOGFC("[GWJ] %s: Recycle Object", __FUNCTION__);\
                ObjectPoolFactory<type_name>::instance().recycle_object((type_name*)(*vec_records)[vec_index]);\
            }\
        }\
\
        LOGFC("[GWJ] %s: Recycle Vec Records", __FUNCTION__);\
        ObjectPoolFactory<KeyBucket>::instance().recycle_object(vec_records);\
\
    }\
\
    this->result_set.clear();\
}

/*
 *  һ�� mongoDB ������ṹ�ĳ���
 *  ����ṹ���ݱ�ṹ�Ĳ�ͬ������������
 */
class AbstractMCPRecord
{
public:

    AbstractMCPRecord()
    {
    }

    virtual void clear()
    {
    }

    virtual ~AbstractMCPRecord()
    {
    }
};

/*
 *  ר��ʢ��ӵ����ͬ KEY ֵ�ļ�¼
 *
 *  Ҫͨ������ع���
 *  ����Ҫʵ�� ClassName �� clear ����
 */
class KeyBucket
{
public:

    static const char* ClassName;
    vector<AbstractMCPRecord*> records;

    virtual ~KeyBucket()
    {
        this->records.clear();
    }

    void clear()
    {
        this->records.clear();
    }

    inline int size()
    {
        return this->records.size();
    }

    inline AbstractMCPRecord* operator[] (int index)
    {
        return this->records[index];
    }

    inline void push_back(AbstractMCPRecord* record)
    {
        this->records.push_back(record);
    }
};

class AbstractMCPContent
{
protected:

    mongo *_conn;

    uint32_t _unit_times;
    uint32_t _unit_num;
    uint32_t _start_pos;

    string _basic_coll;
    string _basic_ns;

    /*
     *   ֻ���� id ��ָ�룬������ֵ�ڽ����  result_set �� AbstractMCPRecord ��
     *
     *   ����  id_s ��Ŀ�ģ�
     *
     *   1. ��֤������м�¼�����򲻱� map �ƻ�
     *      id_s �е��������������������
     *
     *   2. ���ҵ��Ƚϸ�����Ҫ���Ƕ�ײ�ѯʱ
     *      id_s ������Ϊ�ٲ�ѯ���������
     *      ����һ���ѯ��ֱ��ʹ�� or_decorator װ�������� id_s ����
     *
     *   id_s ����ȷ������Ӧ������������
     *   һ����˵�󲿷�ʱ�����ݶ���ͨ�� id ��������
     */
    vector<const char*> id_s;

    /*
     *   map �� KEY Ҳ�� id ��ָ�룬��һ�� id ���ܻ��ж�����¼�����
     *   ���� id ��ֵ�� AbstractMCPRecord �ṹ���һ����¼�� id.c_str() ָ��
     *
     *   ��ע�⡿:
     *   �� result_set[x] ����һ����¼ʱ
     *   ����� KEY ������  id_s ���ֵ
     *   �����Ǳ��ֵ�������Ҳ�����Ӧ�ļ�¼
     *   ��Ϊʹ�õ���ָ���ַ�������õĵ�ֵַ��Ϊ KEY
     */
    map<const char*, KeyBucket* > result_set;

    /*
     *  ��������������
     *  this->id_s �е�����ֵ�������ָ���� key_name Ϊ׼
     */
    const char* key_name;

    /*
     *  ����װ���������ݲ�ͬ��ҵ���ѯ����ʵ�ֲ�ͬ������
     *  ������  module �仯��˵�����
     */
    AbstractDecorator* decorator;

    /*
     *  �����ѯ�����ļ�¼���������Ƿ�ҳ��ĸ���
     */
    int total_counts;

    /*
     *  �����趨�����ձ���Ҫȡ�õ��ֶ���
     *  ������ʱĬ��ȡ�ֶ�ȫ��
     */
    vector<const char*> select_fields;

    // ת����������
    string convert_mongo_ts_to_string(bson_iterator* it);

    void free_data(mongo_cursor * cursor,
                   bson* query,
                   bson* fields, bson*
                   bson_result);

    // ר�Ŵ���� mongoDB ��ȡ�����������������ݣ����������
    void set_vector_data_from_bson(bson_iterator* bson_it, vector<string>& list);
    void set_vector_data_from_bson(bson_iterator* bson_it, vector<int>& list);
    void set_vector_data_from_bson(bson_iterator* bson_it, vector<double>& list);

    /*
     *  ����� mongoDB ȡ�õ���� [key : value] �ṹ�����飬�����κα��
     *  KEY / VALUE ���� string ����
     */
    void set_tuple_data_from_bson(bson_iterator* it,
                                  void* detail,
                                  bool  is_vector,
                                  char* bson_key,
                                  char* bson_value);

	void AbstractMCPContent::set_struct_data_from_bson(bson_iterator* it,
                                                  vector<MCPStructItem>* target,
                                                  vector<const char*> key_vector);
    /*
     *  ����� mongoDB ȡ�õ���� [key : value] �ṹ�����飬�����κα��
     *  VALUE �� int ����
     */
    void set_tuple_data_from_bson_integer_value(bson_iterator* it,
                                                void* tuple,
                                                bool  is_vector,
                                                char* bson_key,
                                                char* bson_value);

    /*
     *  ����� mongoDB ȡ�õ���� [key : value] �ṹ�����飬�����κα��
     *  VALUE �� double ����
     */
    void set_tuple_data_from_bson_double_value(bson_iterator* it,
                                               void* tuple,
                                               bool  is_vector,
                                               char* bson_key,
                                               char* bson_value);

    // ��������������  [_start_pos, _unit_num]
    int modify_query_boundary(int total_count);

    /*
     * ���������
     * ����װ�����趨�Ĳ�ѯ�������õ���ѯ�б��������
     * ���������������� [_start_pos, _unit_num]
     */
    int get_list_count();

    /*
     *  һ��ͨ�ýӿڣ�
     *  ���������κα��
     *  ����ָ����ɸѡ��������װ����ָ���ı��еõ���Ӧ�� id_list
     */
    int get_id_list(vector<string>& id_list);

    /*
     *  һ��ͨ�ýӿڣ�
     *  ���������κα��
     *  ���ݴ���� id_list ��ָ����Ŀ�������ҵ���Ӧ��ȫ����
     *  �������ݱ��浽  result_set �� id_s ��
     *  ����  id_s ����ָ��  result_set ʵ�����ݵ�ָ�룬��֤��Сռ���ڴ棬ֻ��һ�����ݿ���
     */
    int get_full_set_info_by_id_list(vector<string>& id_list);

    /*
     *  һ��ͨ�ýӿڣ�
     *  ���������κα��
     *  ֱ�Ӹ����ƶ���װ�����е������� mongoDB �л�ȡ����
     *  �������ݱ��浽  result_set �� id_s ��
     *
     *  need_modify_page : �Ƿ���Ҫ����ָ��ҳ���Сȡ������
     *                     ��Ϊ false ʱ�򲻿���ҳ���С������ȡ��ȫ�����ݼ���
     */
    int get_full_set_info(bool need_modify_page);

    /*
     *  ����ӿڣ�
     *  ������ʵ�֣�����  mongoDB ���صı������ȫ�������ö�Ӧ�� id_s �� result_set
     *  ����ӿ����ڱ����ѯ����һ�� Record
     */
    virtual int set_basic_info_from_bson_result(bson* bson_out,
                                                const char* aggregate_name) = 0;

public:

    AbstractMCPContent(const char* key_name);
    virtual ~AbstractMCPContent();

    // װ��װ����
    void load_decorator(AbstractDecorator* dec);

    /*
     *  ��ҳ����
     */
    void set_unit_info(const uint32_t unit_num,
                       const uint32_t unit_times);

    /*
     *  ���ز�ѯ���������������Ƿ�ҳ��ĸ���
     *  �Ƿ�ҳǰ������
     */
    int get_total_result_count();

    /*
     *  ���ؽ����������
     *  �����򲻵��ѣ�������Ҫʹ���������
     */
    vector<const char*>* get_result_keys();

    /*
     *  ���ؽ�����е���������
     */
    int get_key_size();

    /*
     *  ��ȡ������е�����ֵ
     */
    const char* get_key_value(int key_index);

    /*
     *  ��ȡ������е�����ֵ
     */
    const char* get_key_value(const char* key);

    /*
     *  ��ȡͬһ�����µ����м�¼
     */
    KeyBucket* get_key_bucket(int key_index);

    /*
     *  ��ȡͬһ�����µ����м�¼
     */
    KeyBucket* get_key_bucket(const char* key);

    /*
     *  ��ȡͬһ�����µ����м�¼����
     */
    int get_key_bucket_size(int key_index);

    /*
     *  ��ȡͬһ�����µ����м�¼����
     */
    int get_key_bucket_size(const char* key);

    /*
     *  һ��ͨ�ýӿڣ�
     *  ���ؽ��������ͬ id �����м�¼
     *  key       : ���� key �ҵ��������Դ� key �� records
     *  vec_index : �ҵ����ڴ� KEY �����ĵڼ�����¼
     */
    AbstractMCPRecord* get_record_by_key_vec(const char* key,
                                             int vec_index);

    /*
     *  һ��ͨ�ýӿڣ�
     *  ���ؽ��������ͬ id �����м�¼
     *  key_index : ����  key_index �ҵ��������Դ� key_index �� records
     *  vec_index : �ҵ����ڴ� KEY �����ĵڼ�����¼
     */
    AbstractMCPRecord* get_record_by_key_vec(int key_index,
                                             int vec_index);

    /*
     *  һ��ͨ�ýӿڣ�
     *  ��������������ѯ����
     *  �������õ�װ����(decorator Ŀ¼)�õ���ѯ���
     *  ���ѽ�����浽 id_s �� result_set ��
     *
     *  need_modify_page : �Ƿ���Ҫ����ָ��ҳ���Сȡ������
     *                     ��Ϊ false ʱ�򲻿���ҳ���С������ȡ��ȫ�����ݼ���
     */
    int general_fetch_list_records(bool need_modify_page);

    /*
     *  ��ȡװ�������������ļ�¼����
     */
    int general_get_list_records_count();

    /*
     *  �� mongoDB �в�������
     */
    int general_insert_records();

    /*
     *  �� mongoDB �е�һ�� collection ɾ����
     */
    int general_drop_collection();

    /*
     *  �趨Ҫɸѡ�����ֶ�
     */
    void add_select_column(const char* column_name);
};

#endif /* WEIGHT_CONTENT_LIST_H_ */
