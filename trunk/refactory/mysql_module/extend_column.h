/*
 * extend_column.h
 *
 *  Created on: 2013-5-6
 *      Author: guoweijiang
 */

#ifndef EXTEND_COLUMN_H_
#define EXTEND_COLUMN_H_

#include "../common.h"
#include "mysql_general_module.h"

class MysqlTable;

class WhereCondition
{

// ���� ExMysqlColumn Ϊ  WhereCondition ����Ԫ��
friend class ExMysqlColumn;

protected:

    // ���� and / or
    const char* where_type;

    // > < =
    const char* operation;

    string value;

public:
    WhereCondition(const char* type,
                   const char* operation)
    {
        LOGD("[GWJ] %s: new where type[%s], oper[%s]",
             __FUNCTION__, type, operation);

        this->where_type = type;
        this->operation = operation;
    }

    ~WhereCondition()
    {
        this->clear();
    }

    void clear()
    {
        this->value.clear();
    }

    void set_value(const char* value)
    {
        this->value = string(value);
    }

    void set_value(const int value)
    {
        char value_str[MYSQL_INT_VALUE_LEN];
        memset(value_str, 0x00, sizeof(value_str));
        sprintf(value_str, "%d", value);
        this->value = string(value_str);
    }
};


class JoinCondition
{

// ���� ExMysqlColumn Ϊ  JoinCondition ����Ԫ��
friend class ExMysqlColumn;

protected:

    // ���� left join / right join / inner join
    const char* join_type;

    /*
     *  ���� join ʱ�� ON ������
     */
    const char* on_column;

    /*
     *  ���� join ����ʱ ON ������Ҳ��һ������߼�
     *  ����������ڱ�����Щ���������߼�
     */
    string extra_condition;

    // ���� join �ı�
    MysqlTable* join_table;

public:

    JoinCondition(const char* type,
                  MysqlTable* join_table,
                  const char* on_column)
    {
        /*
         *  Join ����ʱ
         *  join_type , join_table ��  ON column �Ǳ����
         */
        this->join_type = type;
        this->join_table = join_table;
        this->on_column = on_column;
        this->extra_condition = "";
    }

    ~JoinCondition()
    {
        this->clear();
    }

    void clear()
    {
        this->extra_condition.clear();
    }

    void set_extra(const char* ex_cond)
    {
        this->extra_condition = ex_cond;
    }
};

class ExMysqlColumn
{

// ���� MysqlTable Ϊ ExMysqlColumn ����Ԫ��
friend class MysqlTable;

protected:

    // ���������ı�
    MysqlTable* owner_table;

    /*
     *  ����� nick_name �˴�����  nick_name.column_name ����ʽ
     */
    string column_name;

    /*
     *  column_value �������� INSERT ���
     */
    string column_value;

    bool is_string_value;
    bool is_group_by;

    vector<WhereCondition> where_condition;
    vector<JoinCondition> join_conditon;

    const char* order_type;

    void add_condition(const char* type,
                       const char* operation,
                       const char* value);

    void add_condition(const char* type,
                       const char* operation,
                       const int value);

    /*
     *  ������� Column ���ϵ� JOIN ��
     *  һ����˵ֻ����  ON column �������ӱ���
     *  ���������Ҫ�����������չ����  ext_condition
     *
     *  ʵ����ʾ��һ�������   ext_condition �������ŵ������������
     */
    void add_join(const char* type,
                  MysqlTable* join_table,
                  const char* on_column,
                  const char* ext_condition);

    /*
     *  �жϴ���� col_name �Ƿ�Ϊ�� column_name
     */
    bool is_column_name(const char* col_name);

public:
    ExMysqlColumn(MysqlTable* owner,
                  const char* column_name,
                  bool is_string_type = true);

    ~ExMysqlColumn();

    void clear();

    /*
     *  ��ȡ������������������������������
     */
    const char* get_entire_column_name();

    /*
     *  ���ж�� Ex-column ���� where �������ʱ
     *  this->where_condition[0].where_type ����������  Ex-column ��� and / or �߼���
     *  ����ӿ������Ƿ������� Ex-column �� and / or �߼�
     *
     *  this->where_condition[i].where_type(i>0)
     *  �������Щ where_type ����������� Ex-column �ڲ� and / or �߼���
     */
    const char* get_header_where_type();

    /*
     *  ��ȡ���ڴ� Ex-column �� WHERE ���
     *  û�� WHERE ���ʱ����  false
     */
    bool get_where_condition_query(string& query);

    /*
     *  ��ȡ���ڴ� Ex-column �� JOIN ���
     *  ���û�� JOIN ����򷵻� false
     */
    bool get_join_query(string& query);

    bool get_order_by_query(string& order_query);
    bool get_group_by_query(string& query);

    void add_and_after_now();
    void add_and_before_now();

    void add_or_after_now();
    void add_or_before_now();


    void add_or_condition(const char* operation,
                          const char* value);

    void add_or_condition(const char* operation,
                          const int value);

    void add_or_condition(const char* operation,
                          ExMysqlColumn& column);

    void add_or_conditions(const char* operation,
                           vector<const char*>& values);

    void add_or_conditions(const char* operation,
                           vector<int>& values);

    void add_and_condition(const char* operation,
                           const char* value);

    void add_and_condition(const char* operation,
                           const int value);

    void add_and_condition(const char* operation,
                           ExMysqlColumn& column);

    void add_and_conditions(const char* operation,
                            vector<const char*>& values);

    void add_and_conditions(const char* operation,
                            vector<int>& values);

    void add_left_join(MysqlTable* join_table,
                       const char* on_column,
                       const char* extra_condition);

    void add_right_join(MysqlTable* join_table,
                        const char* on_column,
                        const char* extra_condition);

    void add_inner_join(MysqlTable* join_table,
                        const char* on_column,
                        const char* extra_condition);

    void add_orderby_decrease();
    void add_orderby_acrease();

    /*
     *  �Ⱥ����������ֻӦ����������ֵ
     *  �� INSERT / UPDATE ʱʹ��
     */
    void set_value(const char* value);
    void set_value(int value);
};


#endif /* EXTEND_COLUMN_H_ */
