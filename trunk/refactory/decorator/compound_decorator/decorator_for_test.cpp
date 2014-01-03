/*
 * weight_class_decorator.cpp
 *
 *  Created on: 2013-4-26
 *      Author: guoweijiang
 */

#include "decorator_for_test.h"

using namespace std;

TypedClassDecorator::TypedClassDecorator()
{

}

TypedClassDecorator::~TypedClassDecorator()
{

}

/*
 *  ʵ�ֳ���ӿڣ�
 *  ���� type.id �� class.id ��ѯʱ��Ҫ��ָ���� type.id �� class.id �ļ�¼ɸѡ����
 *  Ȼ���ٸ���ɸѡ�Ľ��ִ�������ҵ���߼�
 */
int TypedClassDecorator::custom_result_set_query(bson* query)
{
//    int size = this->primary_table_query.size();
//
//    LOGD("[GWJ] %s:  start. query_size[%d]", __FUNCTION__, size);
//
//    if(size == 0)
//    {
//        return 1;
//    }
//
//    bson_append_start_object(query, "$query");
//    this->primary_table_query[0]->custom_result_set_query(query);
//    bson_append_finish_object(query);
//
//    // ִ�������ҵ���߼�
//    for(int i = 1; i < size; i++)
//    {
//        this->primary_table_query[i]->custom_result_set_query(query);
//    }

    /*
     *
  1��       ֻ��
    bson_append_start_object( query, "$query" );
    bson_append_string(query, "content_type", "11");
    bson_append_finish_object(query);
          ʱ�������  $1 = 0

   2.    ֻ��
          bson_append_string(query, "content_type", "11");
          ʱ������� 899

   3��ֻ��
    bson_append_start_object(query, "class.id");
    bson_append_start_array(query, "$in");
    bson_append_string(query, "0", "97");
    bson_append_finish_array(query);
    bson_append_finish_object(query);
    ʱ������� $1 = 79

   4.   ֻ��
    bson_append_start_object(query, "class.id");
    bson_append_start_array(query, "$in");
    bson_append_string(query, "0", "97");
    bson_append_string(query, "1", "98");
    bson_append_finish_array(query);
    bson_append_finish_object(query);
    ʱ������� $1 = 120

   5. ���  content_type �� class.id ����  bson_append_start_object( query, "$query" );
    �����   $1 = 120

   6. ���  content_type �� class.id ����  bson_append_start_object( query, "$query" );
    �����  $1 = 0
     */
//    bson_append_start_object( query, "$query" );
//    bson_append_string(query, "content_type", "11");
//    bson_append_finish_object(query);

    bson_append_start_object(query, "$and");

    bson_append_start_object( query, "$query" );
    bson_append_string(query, "content_type", "11");
    bson_append_finish_object(query);

    bson_append_start_object(query, "class.id");
    bson_append_start_array(query, "$in");
    bson_append_string(query, "0", "97");
    bson_append_string(query, "1", "98");
    bson_append_finish_array(query);
    bson_append_finish_object(query);

    bson_append_finish_object(query);

//    bson_append_start_object(query, "$orderby");
//    bson_append_int(query, "download_weight", -1);
//    bson_append_int(query, "ptype_5", -1);
//    bson_append_finish_object(query);

    return 1;
}
