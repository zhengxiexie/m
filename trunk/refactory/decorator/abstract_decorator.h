/*
 * abstract_decorator.h
 *
 * һ��ר���������� bson_query �Ľӿ�
 * ���ݾ���ҵ������ʵ�����࣬���ò�ѯ�������޸� bson_query
 *
 *  Created on: 2013-4-26
 *      Author: guoweijiang
 */

#ifndef ABSTRACTDECORATOR_H_
#define ABSTRACTDECORATOR_H_

#include "../common.h"

using namespace std;

enum WeightType
{
    // H5����Ϸ����վ��ʹ�õ�һ��Ȩ��
    CLASS_WEIGHT_TYPE = 0,
    LATEST_WEIGHT_TYPE,
    DOWNLOAD_WEIGHT_TYPE,
    POP_WEIGHT_TYPE,
    NO_WEIGH_TYPE,

    // ר��Ϊ��Ϸ����Ԥ����Ȩ��
    CLIENT_CLASS_WEIGHT_TYPE,
    CLIENT_LATEST_WEIGHT_TYPE,
    CLIENT_DOWNLOAD_WEIGHT_TYPE,
    CLIENT_POP_WEIGHT_TYPE,
    CLIENT_WEEKLY_DOWNLOAD_TYPE,

    // ���� column.p ����
    COLUMN_PRIORIY,

    // ���� class.p ����
    CLASS_PRIORIY,
	// ��װ���������������
	DOWNLOAD_DAY_WEIGHT_TYPE
};

class AbstractDecorator
{
protected:

    /*
     *  �� MongoDB ��һЩ������ǰ���� $query
     *  ���� $orderby
     *  �μ���http://docs.mongodb.org/manual/reference/operator/orderby/#op._S_orderby
     */
    bool is_special;

    /*
     *  �ж��Ƿ���һ������װ����
     *  ����ǳ���װ�����������е�����Ҫ�ڳ���װ������ִ��
     *  ���ҳ���װ������ִ��ʱҪʹ�� mongo_run_command ������������ mongo_find ����
     */
    bool is_aggregate;


    /*
     *  �����һ������װ������Ҫ��ʾװ�ε��ֶ�����
     */
    const char* extra_name;

    /*
     *  Aggregate װ������Ҫ֪����ҳ��Ϣ
     */
    uint32_t start_pos;
    uint32_t page_size;

public:

    /*
     * ��  need_get_id_list �ӿ�һ����, �������ѯʱ�ĸ�����
     * ��ס����һ��Ҫ��  mcp. ǰ׺, ���� mcp.mcp_xxx_xxx ����ʽ
     */
    const char* secondary_table_name;

    /*
     *  ���漰��Ƕ�������ѯʱ���Ӹ����в�ѯ�� KEY ���п��ܲ��� c_id
     *  ������Ҫ�趨һ�£�Ĭ���� "c_id"
     */
    const char* secondary_query_key_name;

    /*
     *  ���漰��Ƕ�������ѯʱ���������в�ѯ�� KEY ���п��ܲ��� id
     *  ������Ҫ�趨һ�£�Ĭ���� "id"
     */
    const char* primary_query_key_name;

    AbstractDecorator();
    virtual ~AbstractDecorator();

    /*
     *  ������ mongoDB ��Ŀ�������û��Ҫ��ѯɸѡ�� xxx_id��
     *  ��ʱ����Ҫ��  secondary_table_name ����в�ѯ xxx_id ��Ӧ�� c_id
     *  Ȼ���ٸ���   c_id ��ѯĿ����
     *  ��ʱ����Ҫһ���жϽӿڣ��ж��Ƿ���Ҫ�Ȳ� id_list
     */
    bool need_get_id_list();

    /*
     * ��������ɸѡ�� xxx_id ���ò�ѯ���
     * ɸѡ�����Ͱ��� filter_classes �� filter_columns �� filter_labels ��
     */
    void set_filter_query(vector<const char*>& filter_list,
                          bson* query,
                          const char* filter_log_ptr,
                          const char* bson_filter);


    /*
     *  һ��ͨ�ýӿڣ�
     *  �������е� id_list ������ѯɸѡ id ������
     *  ����ӿ�ר������ͨ���Ӹ������ҵ��� id_list �����ձ�
     */
    void set_basic_info_query_by_id_list(vector<string>& id_list, bson* query);

    /*
     *  һ��ͨ�ýӿڣ�
     *  ���ݴ���� select_fields ����ɸѡ�������������
     */
    void set_fields_bson(bson* query, vector<const char*>& select_fields);

    bool is_special_query();

    bool is_aggregate_query();
    const char* get_aggregate_extra_name();

    void set_page_info(const uint32_t start_pos,
                       const uint32_t page_size);

    /*
     *  һ�����ӿڣ�
     *  Ĭ��ִ�������  custom_result_set_query ����
     *
     *  ��ʵ�ܶ������ custom_result_set_query ��ͬ
     *  set_id_list_fetching_query ���õ��Ǵ�  secondary_table_name �����в�ѯ������
     *  custom_result_set_query ���õľ��Ǵ�����Ŀ����в�ѯ������
     */
    virtual int set_id_list_fetching_query(bson* query);

    /*
     *  һ�����ӿڣ�
     *
     *  ���� single decorator ��˵ֱ�ӵ��ú��ķ�������
     *  �� compound decorator �л���д���麯��
     */
    virtual int get_count_query(bson* query);

    /*
     *  ����ӿڣ�
     *  ����װ��ָ��ҵ���ѯ����������ѯ����Ŀ�����װ�����ĺ��Ľӿ�
     */
    virtual int custom_result_set_query(bson* query) = 0;
};


#endif /* ABSTRACTDECORATOR_H_ */
