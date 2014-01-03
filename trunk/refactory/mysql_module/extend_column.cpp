/*
 * extend_column.cpp
 *
 *  Created on: 2013-5-6
 *      Author: guoweijiang
 */

#include "extend_column.h"

ExMysqlColumn::ExMysqlColumn(MysqlTable* owner, const char* column_name, bool is_string_type)
{
    LOGD("[GWJ] %s:  start", __FUNCTION__);

    this->owner_table = owner;

    if(this->owner_table != NULL)
    {
        if(IS_VALUED_STRING(this->owner_table->nick_name))
        {
            this->column_name = string(this->owner_table->nick_name) + ".`"
                                + column_name + "`";
        }
        else
        {
            this->column_name = "`" + string(column_name) + "`";
        }

        LOGD("[GWJ] %s: Ex-column[%s]", __FUNCTION__, this->column_name.c_str());
    }
    else
    {
        LOGD("[GWJ] %s: NULL owner table ERROR!", __FUNCTION__);
    }

    this->order_type = NULL;
    this->is_group_by = false;
    this->is_string_value = is_string_type;
}

ExMysqlColumn::~ExMysqlColumn()
{
    this->clear();
}

void ExMysqlColumn::clear()
{
    this->column_name.clear();
    this->column_value.clear();

    int size = this->where_condition.size();

    for(int i = 0; i < size; i++)
    {
        this->where_condition[i].clear();
    }

    this->where_condition.clear();

    size = this->join_conditon.size();

    for(int i = 0; i < size; i++)
    {
        this->join_conditon[i].clear();
    }

    this->join_conditon.clear();
}

/*
 *  �жϴ���� col_name �Ƿ�Ϊ�� column_name
 */
bool ExMysqlColumn::is_column_name(const char* col_name)
{
    const char* pure_this_col_name;
    const char* pure_col_name;
    string pure_column;

    if(IS_VALUED_STRING(this->owner_table->nick_name))
    {
        pure_this_col_name = CommonInterface::find_pure_column_name(this->column_name.c_str());
    }
    else
    {
        pure_this_col_name = this->column_name.c_str();
    }

    if(IS_VALUED_STRING(col_name))
    {
        pure_col_name = CommonInterface::find_pure_column_name(col_name);
    }
    else
    {
        LOGD("[GWJ] %s: Error input [col_name]", __FUNCTION__);
        return false;
    }

    if(!IS_VALUED_STRING(pure_col_name))
    {
        LOGD("[GWJ] %s: [col_name:%s] is pure itself", __FUNCTION__, col_name);
        pure_column = string("`") + col_name + "`";
        pure_col_name = pure_column.c_str();
    }

    if(pure_this_col_name == NULL ||
            pure_col_name == NULL)
    {
        LOGD("[GWJ] %s: Error input [col_name:%s] or [Column_name:%s]",
             __FUNCTION__, pure_col_name, pure_this_col_name);

        return false;
    }

    if(strcmp(pure_this_col_name, pure_col_name) == 0)
    {
        LOGD("[GWJ] %s: Same name[%s][%s]",
             __FUNCTION__, pure_col_name, pure_this_col_name);

        return true;
    }
    else
    {
        LOGD("[GWJ] %s: Different name[%s][%s]",
             __FUNCTION__, pure_col_name, pure_this_col_name);

        return false;
    }
}

const char* ExMysqlColumn::get_entire_column_name()
{
    return this->column_name.c_str();
}

/*
 *  ������� Column ���ϵ� JOIN ��
 *  һ����˵ֻ����  ON column �������ӱ���
 *  ���������Ҫ�����������չ����  ext_condition
 *
 *  ʵ����ʾ��һ�������   ext_condition �������ŵ������������
 */
void ExMysqlColumn::add_join(const char* type,
                             MysqlTable* join_table,
                             const char* on_column,
                             const char* ext_condition)
{
    if(type == NULL || join_table == NULL || on_column == NULL)
    {
        LOGD("[GWJ] %s: [join type] or [join table] or [ON column] NULL ERROR!! ", __FUNCTION__);
        return;
    }

    JoinCondition join_condition(type, join_table, on_column);

    if(ext_condition != NULL)
    {
        join_condition.set_extra(ext_condition);
    }

    /*
     *  ��ִ�� join ����ʱ��Ҫ�Ѳ��� joint �ı���Ҫ select ���м��뵽 owner ����
     *  ��֤��ִ�� SQL ���ʱ�ܹ�ѡ�� joint ������Ҫ����
     */
    MysqlTable* this_table = this->owner_table;
    int joint_col_size = join_table->get_select_column_size();

    for(int i = 0; i < joint_col_size; i++)
    {
        const char* joint_column = (*join_table)[i];

        if(joint_column != NULL)
        {
            this_table->add_select_column(joint_column, join_table->nick_name);
        }
        else
        {
            LOGD("[GWJ] %s: NULL joint column[%d]", __FUNCTION__, i);
        }
    }

    this_table->add_join_table(join_table);
    this->join_conditon.push_back(join_condition);

    LOGD("[GWJ] %s:add join[%s %s]", __FUNCTION__, type, join_table->table_name);
}

void ExMysqlColumn::add_left_join(MysqlTable* join_table,
                                  const char* on_column,
                                  const char* extra_condition)
{
    if(join_table == NULL || on_column == NULL)
    {
        LOGD("[GWJ] %s: LEFT JOIN [join table] or [ON column] NULL ERROR!! ", __FUNCTION__);
        return;
    }

    this->add_join("LEFT JOIN", join_table, on_column, extra_condition);
}

void ExMysqlColumn::add_right_join(MysqlTable* join_table,
                                   const char* on_column,
                                   const char* extra_condition)
{
    if(join_table == NULL || on_column == NULL)
    {
        LOGD("[GWJ] %s: RIGHT JOIN [join table] or [ON column] NULL ERROR!! ", __FUNCTION__);
        return;
    }

    this->add_join("RIGHT JOIN", join_table, on_column, extra_condition);
}

void ExMysqlColumn::add_inner_join(MysqlTable* join_table,
                                   const char* on_column,
                                   const char* extra_condition)
{
    if(join_table == NULL || on_column == NULL)
    {
        LOGD("[GWJ] %s: INNER JOIN [join table] or [ON column] NULL ERROR!! ", __FUNCTION__);
        return;;
    }

    this->add_join("INNER JOIN", join_table, on_column, extra_condition);
}

void ExMysqlColumn::add_condition(const char* type,
                                  const char* operation,
                                  const char* value)
{
    if(type == NULL || value == NULL)
    {
        LOGD("[GWJ] %s: [condition type] or [value] NULL ERROR!! ", __FUNCTION__);
        return;
    }

    WhereCondition condition(type, operation);
    condition.set_value(value);
    this->where_condition.push_back(condition);
}

void ExMysqlColumn::add_condition(const char* type,
                                  const char* operation,
                                  const int value)
{
    if(type == NULL)
    {
        LOGD("[GWJ] %s: [condition type] NULL ERROR!! ", __FUNCTION__);
        return;
    }

    WhereCondition condition(type, operation);
    condition.set_value(value);
    this->where_condition.push_back(condition);
}

void ExMysqlColumn::add_and_after_now()
{
    this->is_string_value = false;
    this->add_condition("AND", " > ", " now() ");
}

void ExMysqlColumn::add_and_before_now()
{
    this->is_string_value = false;
    this->add_condition("AND", " < ", " now() ");
}

void ExMysqlColumn::add_or_after_now()
{
    this->is_string_value = false;
    this->add_condition("OR", " > ", " now() ");
}

void ExMysqlColumn::add_or_before_now()
{
    this->is_string_value = false;
    this->add_condition("OR", " < ", " now() ");
}

void ExMysqlColumn::add_or_condition(const char* operation,
                                     const char* value)
{
    this->is_string_value = true;
    this->add_condition("OR", operation, value);
}

void ExMysqlColumn::add_or_condition(const char* operation,
                                     const int value)
{
    this->is_string_value = false;
    this->add_condition("OR", operation, value);
}

void ExMysqlColumn::add_or_condition(const char* operation,
                                     ExMysqlColumn& column)
{
    this->is_string_value = false;

    this->add_condition("OR", operation,
                        column.get_entire_column_name());
}

void ExMysqlColumn::add_or_conditions(const char* operation,
                                      vector<const char*>& values)
{
    this->is_string_value = true;

    int size = values.size();

    for(int i = 0 ; i < size; i++)
    {
        LOGD("[GWJ] %s: or %dth [%s]", __FUNCTION__, i, values[i]);
        this->add_condition("OR", operation, values[i]);
    }
}

void ExMysqlColumn::add_or_conditions(const char* operation,
                                      vector<int>& values)
{
    this->is_string_value = false;

    int size = values.size();

    for(int i = 0 ; i < size; i++)
    {
        LOGD("[GWJ] %s: or %dth [%d]", __FUNCTION__, i, values[i]);
        this->add_condition("OR", operation, values[i]);
    }
}

void ExMysqlColumn::add_and_condition(const char* operation,
                                      const char* value)
{
    this->is_string_value = true;
    this->add_condition("AND", operation, value);
}

void ExMysqlColumn::add_and_condition(const char* operation,
                                      const int value)
{
    this->is_string_value = false;
    this->add_condition("AND", operation, value);
}

void ExMysqlColumn::add_and_condition(const char* operation,
                                      ExMysqlColumn& column)
{
    this->is_string_value = false;
    this->add_condition("AND", operation,
                        column.get_entire_column_name());
}

void ExMysqlColumn::add_and_conditions(const char* operation,
                                       vector<const char*>& values)
{
    this->is_string_value = true;

    int size = values.size();

    for(int i = 0 ; i < size; i++)
    {
        LOGD("[GWJ] %s: and %dth [%s]", __FUNCTION__, i, values[i]);
        this->add_condition("AND", operation, values[i]);
    }
}

void ExMysqlColumn::add_and_conditions(const char* operation,
                                       vector<int>& values)
{
    this->is_string_value = false;

    int size = values.size();

    for(int i = 0 ; i < size; i++)
    {
        LOGD("[GWJ] %s: and %dth [%d]", __FUNCTION__, i, values[i]);
        this->add_condition("AND", operation, values[i]);
    }
}

void ExMysqlColumn::add_orderby_decrease()
{
    this->order_type = "DESC";
}


void ExMysqlColumn::add_orderby_acrease()
{
    this->order_type = "ASC";
}

/*
 *  �Ⱥ����������ֻӦ����������ֵ
 *  �� INSERT / UPDATE ʱʹ��
 */
void ExMysqlColumn::set_value(const char* value)
{
    MCPTupleItem tuple_condition;
    tuple_condition.key = this->column_name;
    tuple_condition.value = value;
    tuple_condition.is_string_value = true;

    this->owner_table->insert_update_conditions.push_back(tuple_condition);
}

/*
 *  �Ⱥ����������ֻӦ����������ֵ
 *  �� INSERT / UPDATE ʱʹ��
 */
void ExMysqlColumn::set_value(int value)
{
    char value_str[10];
    memset(value_str, 0x00, sizeof(value_str));
    sprintf(value_str, "%d", value);

    MCPTupleItem tuple_condition;
    tuple_condition.key = this->column_name;
    tuple_condition.value = value_str;
    tuple_condition.is_string_value = false;

    this->owner_table->insert_update_conditions.push_back(tuple_condition);
}

/*
 *  ���ж�� Ex-column ���� where �������ʱ
 *  this->where_condition[0].where_type ����������  Ex-column ��� and / or �߼���
 *  ����ӿ������Ƿ������� Ex-column �� and / or �߼�
 *
 *  this->where_condition[i].where_type(i>0)
 *  �������Щ where_type ����������� Ex-column �ڲ� and / or �߼���
 */
const char* ExMysqlColumn::get_header_where_type()
{
    if(this->where_condition.size() > 0 &&
       IS_VALUED_STRING(this->where_condition[0].where_type))
    {
        LOGD("[GWJ] %s: Header where condition[%s]",
                 __FUNCTION__, this->where_condition[0].where_type);

        return this->where_condition[0].where_type;
    }
    else
    {
        LOGD("[GWJ] %s: Error header where condition. NULL or Empty", __FUNCTION__);
        return NULL;
    }
}

/*
 *  ��ȡ���ڴ� Ex-column �� WHERE ���
 *  û�� WHERE ���ʱ����  false
 */
bool ExMysqlColumn::get_where_condition_query(string& query)
{
    if(!IS_VALUED_STRING(this->column_name.c_str()))
    {
        LOGD("[GWJ] %s: Ex-Column Name Empty Error!", __FUNCTION__);
        return false;
    }

    int size = this->where_condition.size();

    if(size == 0)
    {
        LOGD("[GWJ] %s: No WHERE SQL", __FUNCTION__);
        return false;
    }

    LOGD("[GWJ] %s:  start. where size[%d]", __FUNCTION__, size);

    for(int i = 0; i < size; i++)
    {
        string cur_condition;

        /*
         * ����� string ���ͣ�value Ҫ�� "value" ����
         * ������� string ���ͣ���ֱ������ value ����
         */
        if(this->is_string_value)
        {
            cur_condition = this->column_name + this->where_condition[i].operation + " \"" + this->where_condition[i].value + "\" ";
        }
        else
        {
            cur_condition = this->column_name + this->where_condition[i].operation + this->where_condition[i].value;
        }

        if(i > 0)
        {
            /*
             *  i = 0 ���Ǹ� where_condition[0].where_type ������������� this->where_condition �õ�
             *  �� this->where_condition �ڲ��ò���  where_condition[0].where_type
             */
            query = query + string(this->where_condition[i].where_type) + " " + cur_condition + " ";
        }
        else
        {
            query = string(" ") + cur_condition + " ";
        }

        LOGD("[GWJ] %s: Current where[%s]",
             __FUNCTION__, cur_condition.c_str());
    }

    query = string("(") + query + ")";

    LOGD("[GWJ] %s: end.where[%s]", __FUNCTION__, query.c_str());
    return true;
}

/*
 *  ��ȡ���ڴ� Ex-column �� JOIN ���
 *  ���û�� JOIN ����򷵻� false
 */
bool ExMysqlColumn::get_join_query(string& query)
{
    if(!IS_VALUED_STRING(this->column_name.c_str()))
    {
        LOGD("[GWJ] %s: Ex-Column Name Error!", __FUNCTION__);
        return false;
    }

    int size = this->join_conditon.size();

    if(size == 0)
    {
        LOGD("[GWJ] %s: No Join SQL", __FUNCTION__);
        return false;
    }

    LOGD("[GWJ] %s:  start. join size[%d]", __FUNCTION__, size);

    for(int i = 0; i < size; i++)
    {
        string cur_join;
        string cur_join_table;
        string cur_join_condition;

        if(this->join_conditon[i].join_table == NULL ||
           !IS_VALUED_STRING(this->join_conditon[i].join_type)  )
        {
            LOGD("[GWJ] %s:  start. join [%d] [table] or [type] NULL or Empty ERROR!!", __FUNCTION__, i);
            return false;
        }

        const char* joint_nick_name  = this->join_conditon[i].join_table->nick_name;
        const char* joint_table_name = this->join_conditon[i].join_table->table_name;
        const char* joint_on_column  = this->join_conditon[i].on_column;
        const char* joint_ext_cond   = this->join_conditon[i].extra_condition.c_str();

        if(!IS_VALUED_STRING(joint_table_name))
        {
            LOGD("[GWJ] %s: join [%d] [joint_table_name] NULL or Empty ERROR!!", __FUNCTION__, i);
            return false;
        }

        if(IS_VALUED_STRING(joint_nick_name))
        {
            cur_join_table = string(this->join_conditon[i].join_type) + " " +
                             joint_table_name + " AS " + joint_nick_name + " ON ";

            cur_join_condition = string(joint_nick_name) + "." +  joint_on_column  + " = " +
                                 this->column_name;
        }
        else
        {
            cur_join_table = string(this->join_conditon[i].join_type) +
                             " " + joint_table_name + " ON ";

            cur_join_condition = string(joint_on_column)  + " = " + this->column_name;
        }

        if(IS_VALUED_STRING(joint_ext_cond))
        {
            cur_join_condition = cur_join_condition + " AND " + joint_ext_cond;
        }

        cur_join = cur_join_table + "(" + cur_join_condition + ")";

        LOGD("[GWJ] %s: Cur join [%d]:%s", __FUNCTION__, i, cur_join.c_str());
        query = query + cur_join;
    }

    LOGD("[GWJ] %s: Join SQL %s", __FUNCTION__, query.c_str());
    return true;
}

/*
 *  ���� Order By ��䣬���û���򷵻� false
 */
bool ExMysqlColumn::get_order_by_query(string& order_query)
{
    if(IS_VALUED_STRING(this->order_type))
    {
        if(IS_VALUED_STRING(this->column_name.c_str()))
        {
            order_query = " " + this->column_name + " " + this->order_type;
        }
        else
        {
            LOGD("[GWJ] %s:Owner Ex-Column Name NULL Error!", __FUNCTION__);
            return false;
        }
    }
    else
    {
        LOGD("[GWJ] %s:Ex-Column Name has no OrderBy!", __FUNCTION__);
        return false;
    }


    LOGD("[GWJ] %s: ret[%s]", __FUNCTION__, order_query.c_str());
    return true;
}

/*
 *  ���� Group By ��䣬���û���򷵻� false
 */
bool ExMysqlColumn::get_group_by_query(string& query)
{
    if(this->is_group_by)
    {
        if(IS_VALUED_STRING(this->column_name.c_str()))
        {
            query = " " + this->column_name;
        }
        else
        {
            LOGD("[GWJ] %s:Owner Ex-Column Name NULL Error!", __FUNCTION__);
            return false;
        }
    }
    else
    {
        LOGD("[GWJ] %s:Ex-Column Name has no OrderBy!", __FUNCTION__);
        return false;
    }


    LOGD("[GWJ] %s: ret[%s]", __FUNCTION__, query.c_str());
    return true;
}


