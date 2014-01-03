/*
 * mysql_general_module.h
 *
 *  Created on: 2013-5-3
 *      Author: guoweijiang
 */

#ifndef MYSQL_GENERAL_MODULE_H_
#define MYSQL_GENERAL_MODULE_H_

#include "../common.h"
#include "extend_column.h"
#include "../../duokoo_mysql.h"

class MysqlTable;

/*
 *  �� MySQL ���ص����ݶ���һά���ݣ�������Ƕ�׹�ϵ
 *  �����ýӿ�  DuokooMysql::getResult ֻ���� string ���Ͷ���Ƶ�
 */
class MySQLRecord
{
friend class MysqlTable;

    public:/* TODO Only for TEST */
    /*
     *  KEY ָ�� select_fields �е���
     */
    map<const char*, string> record;
};

class MysqlTable
{

// ���� ExMysqlColumn Ϊ MysqlTable ����Ԫ��
friend class ExMysqlColumn;

protected:

    /*
     *  ��ע�⡿
     *
     *  key_name ����ָ��  select_fields �е�ĳ����Ա����
     *  �����������ĵط�����������ͬ���ַ���
     */
    const char* key_name;

    /*
     *  ר������ SQL ����в��� where / orderby / groupby ���жϺ�  join ��"��"��������
     */
    vector<ExMysqlColumn> extend_columns;

    /*
     *  ��� Table �� JOIN ��һ��ı����
     */
    vector<MysqlTable*> joint_tables;

    /*
     *  ִ�� INSERT / UPDATE ����ʱ���к�ֵ
     */
    vector<MCPTupleItem> insert_update_conditions;
    vector<string> insert_transaction_sqls;
    vector<string> update_transaction_sqls;

    // ��ʵ����
    const char* table_name;

    // �������������Ϊ��
    const char* nick_name;

    /*
     *  ר������ SQL ����� select ����
     *  �� extent_columns ���ò�ͬ
     *
     *  ���� select column �п�����ƴ�Ӳ���
     *  ���Դ˴����� string ʵ�壬������ const char*
     */
    vector<string> select_fields;

    /*
     *  ������� result_set �������  MySQLRecord ʵ���ָ��
     *  �������������  KEY
     */
    vector<const char*> id_s;

    /*
     *  ������� [KEY] �� id_s �е�ָ�붼ָ��ͬһ��λ�ã���  MySQLRecord �� key_name.c_str()
     */
    map<const char*, vector<MySQLRecord> > result_set;

    /*
     *  ֧�ַ�ҳ��ʾ
     */
    uint32_t _unit_times;
    uint32_t _unit_num;
    uint32_t _start_pos;

    /*
     *  Ԫ���� SQL ���
     */
    string select_query;
    bool has_select_column;

    string from_query;

    string join_query;
    bool has_join;

    string groupby_query;
    bool has_groupby;

    string orderby_query;
    bool has_orderby;

    string where_query;
    bool has_where;

    string limited_query;
    bool has_limited;

    /*
     *  ���������Ľ����������ҳǰ����
     */
    int total_results;

    /*
     *  ���һ�������Ƿ��Ѿ���  nick.column ����ʽ
     */
    bool is_had_nick_name(const char* column);

    /*
     *  ֻ�ǻ�ȡ SQL ����е�  SELECT ��
     *  ��Ҫѡ������������
     */
    bool get_select_query(string& query);

    /*
     *  ��ȡ���е� WHERE ���
     *  ����� JOIN ���ϱ���� JOIN ��� WHERE ���Ҳ�����
     *  ���� JOIN ����У����Ƚ��� JOIN Ȼ��� JOIN ��Ľ������ WHERE
     */
    bool get_where_query(string& query);

    /*
     *  ��ȡ��ǰ����� WHERE ���
     *
     *  ��ע�⡿
     *  �ڴ���  AND / OR �߼�ʱ������� OR �߼��Ļ������˵��ж���䶼������һ�� ( )
     *  ���� (xxx) OR (xxx) ���� xxx �ľ���������ʲô
     */
    bool get_self_where_query(string& where_query);

    bool get_join_query(string& join_query);
    bool get_self_join_query(string& join_query);

    /*
     *  ��ȡ���е� ORDER BY ���
     *  ����� JOIN ���ϱ���� JOIN ��� ORDER BY ��������
     *  ���� JOIN ����У����Ƚ��� JOIN Ȼ��� JOIN ��Ľ������ ORDER BY
     */
    bool get_orderby_query(string& query);

    /*
     *  ��ȡ���е� GROUP BY ���
     *  ����� JOIN ���ϱ���� JOIN ��� GROUP BY ��������
     *  ���� JOIN ����У����Ƚ��� JOIN Ȼ��� JOIN ��Ľ������ GROUP BY
     */
    bool get_groupby_query(string& query);

    /*
     *  ��ȡ���е� LIMITED ���
     *  ���÷�ҳ����
     */
    bool get_limited_query(string& query);

    /*
     *  ƴ��  SQL ��ѯ��䣺
     *
     *  �˴�ֻ������ SQL ���Ԫ����Ϻü���
     */
    void get_every_sql_elements(bool need_seperate_page);

    /*
     *  ƴ��  SQL ��ȡ��ѯ������䣺
     */
    bool get_whole_count_query(string& query);

    /*
     *  ƴ��  SQL ��ѯ��䣺
     */
    bool get_whole_query_string(string& query);

    /*
     *  ƴ��  SQL ������䣺
     */
    bool get_whole_insert_string(string& query);

    /*
     *  ƴ��  SQL �޸���䣺
     */
    bool get_whole_update_string(string& query);

    /*
     *  �� this->insert_update_conditions ��ȡ������
     *  ��ƴ����Щ�����γ� SQL �������
     */
    bool append_column_values_list(string& columns, string& values);

    /*
     *  �� this->insert_update_conditions ��ȡ������
     *  ��ƴ����Щ�����γ� SQL �޸����
     */
    bool append_column_set_list(string& set_query);

    void add_join_table(MysqlTable* join_table);

    void fill_mysql_result_bucket(MySQLRecord& record);
    void fill_result_set(DuokooMysql& mysql, int record_count);

    /*
     *  �ж��Ƿ���Ҫ�����������ת��Ϊ  nick_name.column_name ����ʽ
     *  ���մ˶����ڲ�����ʽ��ͳһ��ʽ
     *
     *  return: -1 ���� ; 0 ����Ҫת�� ; 1 ��Ҫת��
     */
    int change_to_table_nick_column(const char* value, string& result_name);

    // ��ȡ����ҳʱ�ļ�¼����
    int get_result_total_count(DuokooMysql& mysql);

	// ��������������  [_start_pos, _unit_num]
    int modify_query_boundary(int total_count);
	

public:

    MysqlTable(const char* table_name);

    MysqlTable(const char* table_name,
               const char* nick_name);

    ~MysqlTable();

    int get_select_column_size();

    /*
     *  ���÷�ҳ��
     */
    void set_unit_info(const uint32_t unit_num,
                       const uint32_t unit_times);

    /*
     *  �������� SQL ����� select �ε�����
     *  �����ô���Ϊ KEY �����У���Ӧ this.id_s
     *  ֮��ͨ��  this.id_s ����ʱ��id ���Ǵ��е�����
     */
    void add_select_column(const char* col_name, const char* nick_name);
    void add_select_column(const char* col_name);

    /*
     *  [] ���������
     *  ���� ExMysqlColumn ���͵��У�Ӧ����Զ���ܷ���  NULL ָ��
     *  �������� column_name �����ڣ����½��У���������  extend_columns ��
     */
    ExMysqlColumn* operator[] (const char* column_name);

    /*
     *  [] ���������
     *  ���� string ���͵���, û��ʱ���� NULL
     *  �����õĲ��� SQL ����� select �ε�����
     */
    const char* operator[] (int i);

    /*
     *  ִ�в�ѯ��䣬�����ݿ��л�ȡ����
     */
    bool process_query_sql(const char* db_name, bool need_seperate_page);

    /*
     *  �������
     */
    bool add_new_insert_transaction();
    bool process_insert_transaction(const char* db_name);

    /*
     *  �޸Ĳ���
     */
    bool add_new_update_transaction();
    bool process_update_transaction(const char* db_name);

    unsigned int get_total_key_count();

    int get_total_columns_count();
	
	int get_record_count_by_key_index(int key_index);

    // ��ȡ����ҳʱ�ļ�¼����
	int get_total_count_without_page();

    /*
     *  ���ؽ���������ݵĽӿڣ�
     *  key_index    �ڼ�������ֵ
     *  vec_index    ������ֵ�µĵڼ�������
     *  column_index �������ݵĵڼ���
     */
    const char* get_result_set_value(int key_index,
                                     int vec_index,
                                     int column_index);

    /*
     *  ���ؽ���������ݵĽӿڣ�
     *  key       ����ֵ
     *  vec_index ������ֵ�µĵڼ�������
     *  column    �������ݵ�����
     *
     *  ��ע�⡿
     *  ����� JOIN ������������ڻ�ȡ����ʱ
     *  column ������ nick_name.column_name ����ʽ�������
     */
    const char* get_result_set_value(const char* key,
                                     int vec_index,
                                     const char* column);

    /*
     *  ���ؽ���������ݵĽӿڣ�
     *  key_index ����ֵ
     *  vec_index ������ֵ�µĵڼ�������
     *  column    �������ݵ�����
     *
     *  ��ע�⡿
     *  ����� JOIN ������������ڻ�ȡ����ʱ
     *  column ������ nick_name.column_name ����ʽ�������
     */
    const char* get_result_set_value(int key_index,
                                     int vec_index,
                                     const char* column);
};


#endif /* MYSQL_GENERAL_MODULE_H_ */
