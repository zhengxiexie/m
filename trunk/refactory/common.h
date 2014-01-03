/*
 * common.h
 *
 *  Created on: 2013-4-27
 *      Author: guoweijiang
 */

#ifndef COMMON_H_
#define COMMON_H_

#include "../content_list.h"
#include <stdlib.h>
#include <sys/syscall.h>

#include <iterator>
#include <list>
#include <vector>
#include <string>
#include <map>

using namespace std;

#define MYSQL_INT_VALUE_LEN 16
#define OBJECT_POOL_SIZE 40

// ����������ִ���������
#define KEY_NAME_MAX_LEN 64
#define VALUE_MAX_LEN 1024

// �� char* �� �ǿ��ַ����ж�
#define IS_VALUED_STRING(str_instence)\
        (str_instence != NULL && strcmp(str_instence, "") != 0)

// �ж����� char* �ַ����Ƿ���ͬ
#define IS_SAME_VALUED_STRING(str1, str2)\
        (str1 == str2 || strcmp(str1, str2) == 0)

// ר�Ŵ���MYSQL�Ĳ�������
#define SET_MYSQL_INSERT_CONDITION(value, column_name, table)\
        if(IS_VALUED_STRING(value))\
        {\
            table[column_name]->set_value(value);\
        }

// ר�Ŵ���MYSQL��ɸѡ��������
#define SET_MYSQL_SELECT_CONDITION(value, column_name, table, oper)\
        if(IS_VALUED_STRING(value))\
        {\
            table[column_name]->add_and_condition(oper, value);\
        }

// ר���������� OR ����ʱʹ�õ�  vector<string> ����
#define CLEAR_VECTOR_STRING_CONDITIONS(column_name)\
        CommonInterface::clear_vector_string(column_name##_id_vals);

// ר�Ŵ���MYSQL��ɸѡ OR ����
#define SET_MYSQL_OR_CONDITION(value, column_name, table, oper)\
        vector<string> value##_id_vals;\
        if(IS_VALUED_STRING(value))\
        {\
            string value##_id_str = value;\
            int size = CommonInterface::splic_string(value##_id_str, value##_id_vals, ",");\
\
            if(size > 0)\
            {\
                table[column_name]->add_and_condition(oper, value##_id_vals[0].c_str());\
\
                for(int i = 1; i < size; i++)\
                {\
                    table[column_name]->add_or_condition(oper, value##_id_vals[i].c_str());\
                }\
            }\
        }

#define WEIGHT_ORDER_CONDITION(column_id, class_id, weight_order, compound_decor, ultimate)\
        WeightDecorator weight_decor;\
        OrderTypeStruct order_struct;\
        order_struct.order_type = weight_order;\
        order_struct.extra_tuple["column"] = column_id;\
        order_struct.extra_tuple["class"] = class_id;\
        order_struct.outer_decorator = &compound_decor;\
        weight_decor.weight_type = weight_decor.get_weight_type(order_struct);\
\
        if(!weight_decor.is_aggregate_query())\
        {\
            compound_decor.add_primary_table_decorator(&weight_decor);\
            ultimate = &compound_decor;\
        }\
        else\
        {\
            ultimate = &weight_decor;\
        }

// ר�����ڴ���������� NOT-EQUAL �߼��ĺ�ģ�壬�� JSON ��ʽ����
#define NOT_EQUAL_CONDITION_JSON(field_name, mongodb_fname, compound_decor)\
    NotEqualDecorator field_name##_not_equal_dec;\
    vector<char*> field_name##_id_vals;\
\
    if(IS_VALUED_STRING(field_name))\
    {\
        int size = CommonInterface::splic_string((char*)field_name, field_name##_id_vals, ";");\
\
        if(size > 0)\
        {\
            for(int i = 0; i < size; i++)\
            {\
                field_name##_not_equal_dec.add_field_filter_condition(mongodb_fname,\
                                                                      field_name##_id_vals[i]);\
            }\
\
            compound_decor.add_primary_table_decorator(&field_name##_not_equal_dec);\
        }\
    }

// ר�����ڴ���������� OR �߼��ĺ�ģ�壬�� JSON ��ʽ����
#define OR_CONDITION_JSON(field_name, mongodb_fname, compound_decor)\
    OrDecorator field_name##_or_dec;\
    vector<char*> field_name##_id_vals;\
\
    if(IS_VALUED_STRING(field_name))\
    {\
        int size = CommonInterface::splic_string((char*)field_name, field_name##_id_vals, ";");\
\
        if(size > 0)\
        {\
            for(int i = 0; i < size; i++)\
            {\
                field_name##_or_dec.add_field_filter_condition(mongodb_fname,\
                                                               field_name##_id_vals[i]);\
            }\
\
            compound_decor.add_primary_table_decorator(&field_name##_or_dec);\
        }\
    }

// ר�����ڴ���������� IS �߼��ĺ�ģ�壬��򵥵� IS �߼���û��������ʽ
#define IS_CONDITION_TEMPLATE(field_value, mongodb_fname, compound_decor)\
    IsDecorator field_value##_is_dec;\
\
    if(IS_VALUED_STRING(field_value))\
    {\
        field_value##_is_dec.add_field_filter_condition(mongodb_fname, field_value);\
        compound_decor.add_primary_table_decorator(&field_value##_is_dec);\
    }\

// add by zhengxie ר������ɾ��������� IS �߼��ĺ�ģ�壬��򵥵� IS �߼���û��������ʽ
#define REMOVE_IS_CONDITION_TEMPLATE(field_value, compound_decor)\
        compound_decor.remove_primary_table_decorator(&field_value##_is_dec);\

// ר�����ڴ���������� OR �߼��ĺ�ģ�壬���Զ����ʽ����
#define OR_CONDITION_TEMPLATE(field_name, mongodb_fname, compound_decor)\
    OrDecorator field_name##_or_dec;\
    vector<char*> field_name##_id_vals;\
\
    if(IS_VALUED_STRING(field_name))\
    {\
        int size = CommonInterface::splic_string((char*)field_name, field_name##_id_vals, ",");\
\
        if(size > 0)\
        {\
            for(int i = 0; i < size; i++)\
            {\
                field_name##_or_dec.add_field_filter_condition(mongodb_fname,\
                                                               field_name##_id_vals[i]);\
            }\
\
            compound_decor.add_primary_table_decorator(&field_name##_or_dec);\
        }\
    }

// add by zhengxie ר������ɾ��������� OR �߼��ĺ�ģ�壬��򵥵� OR �߼���û��������ʽ
#define REMOVE_OR_CONDITION_TEMPLATE(field_value, compound_decor)\
        compound_decor.remove_primary_table_decorator(&field_value##_or_dec);\


// ר��Ϊ���� class ��Ϸ����д�ĺ�ģ�壬����������ȫ�����࣬ȫ���������࣬���η����Լ�����ֻ��ĳһ����
#define CLASS_ID_OR_CONDITION(field_name, mongodb_fname, compound_decor)\
        OrDecorator field_name##_or_dec;\
\
        if(IS_VALUED_STRING(field_name))\
        {\
            if(strcmp(field_name, "-1") == 0)\
            {\
                field_name##_or_dec.add_field_filter_condition(mongodb_fname, "97");\
                field_name##_or_dec.add_field_filter_condition(mongodb_fname, "98");\
                field_name##_or_dec.add_field_filter_condition(mongodb_fname, "99");\
                field_name##_or_dec.add_field_filter_condition(mongodb_fname, "100");\
                field_name##_or_dec.add_field_filter_condition(mongodb_fname, "101");\
                field_name##_or_dec.add_field_filter_condition(mongodb_fname, "102");\
                field_name##_or_dec.add_field_filter_condition(mongodb_fname, "104");\
                field_name##_or_dec.add_field_filter_condition(mongodb_fname, "108");\
                field_name##_or_dec.add_field_filter_condition(mongodb_fname, "337");\
                field_name##_or_dec.add_field_filter_condition(mongodb_fname, "341");\
            }\
            else if(strcmp(field_name, "-2") == 0)\
            {\
                field_name##_or_dec.add_field_filter_condition(mongodb_fname, "97");\
                field_name##_or_dec.add_field_filter_condition(mongodb_fname, "98");\
                field_name##_or_dec.add_field_filter_condition(mongodb_fname, "99");\
                field_name##_or_dec.add_field_filter_condition(mongodb_fname, "100");\
                field_name##_or_dec.add_field_filter_condition(mongodb_fname, "101");\
                field_name##_or_dec.add_field_filter_condition(mongodb_fname, "102");\
                field_name##_or_dec.add_field_filter_condition(mongodb_fname, "104");\
            }\
            else if(strcmp(field_name, "-3") == 0)\
            {\
                field_name##_or_dec.add_field_filter_condition(mongodb_fname, "108");\
            }\
            else\
            {\
                field_name##_or_dec.add_field_filter_condition(mongodb_fname, field_name);\
            }\
\
            compound_decor.add_primary_table_decorator(&field_name##_or_dec);\
        }

// ר�����ڴ���������� "���ڵ���" �߼��ĺ�ģ��
#define BIGGER_EQUAL_CONDITION_TEMPLATE(field_name, mongodb_fname, compound_decor)\
        BiggerThanDecorator bigger_##field_name;\
        bigger_##field_name.is_bigger_and_equal = true;\
        if(IS_VALUED_STRING(field_name))\
        {\
            smaller_##field_name.add_field_filter_condition(mongodb_fname, field_name);\
            compound_decor.add_primary_table_decorator(&bigger_##field_name);\
        }

// ר�����ڴ���������� "����" �߼��ĺ�ģ��
#define BIGGER_CONDITION_TEMPLATE(field_name, mongodb_fname, compound_decor)\
        BiggerThanDecorator bigger_##field_name;\
        if(IS_VALUED_STRING(field_name))\
        {\
            bigger_##field_name.add_field_filter_condition(mongodb_fname, field_name);\
            compound_decor.add_primary_table_decorator(&bigger_##field_name);\
        }

// ר�����ڴ���������� "С�ڵ���" �߼��ĺ�ģ��
#define SMALLER_EQUAL_CONDITION_TEMPLATE(field_name, mongodb_fname, compound_decor)\
        SmallerThanDecorator smaller_##field_name;\
        smaller_##field_name.is_smaller_and_equal = true;\
        if(IS_VALUED_STRING(field_name))\
        {\
            smaller_##field_name.add_field_filter_condition(mongodb_fname, field_name);\
            compound_decor.add_primary_table_decorator(&smaller_##field_name);\
        }

// ר�����ڴ���������� "С��" �߼��ĺ�ģ��
#define SMALLER_CONDITION_TEMPLATE(field_name, mongodb_fname, compound_decor)\
        SmallerThanDecorator smaller_##field_name;\
        if(IS_VALUED_STRING(field_name))\
        {\
            smaller_##field_name.add_field_filter_condition(mongodb_fname, field_name);\
            compound_decor.add_primary_table_decorator(&smaller_##field_name);\
        }

// ר�������������еĺ�ģ��
#define ACREASE_ORDER_CONDITION_TEMPLATE(field_name, mongodb_fname, compound_decor)\
        OrderbyDecorator field_name##_order;\
        field_name##_order.order_type = 1;\
        field_name##_order.add_field_filter_condition(mongodb_fname, "");\
        compound_decor.add_primary_table_decorator(&field_name##_order);

// ר�����ڽ������еĺ�ģ��
#define DECREASE_ORDER_CONDITION_TEMPLATE(field_name, mongodb_fname, compound_decor)\
        OrderbyDecorator field_name##_order;\
        field_name##_order.order_type = -1;\
        field_name##_order.add_field_filter_condition(mongodb_fname, "");\
        compound_decor.add_primary_table_decorator(&field_name##_order);

// ר�����ö���������ĺ�ģ�壬�Զ��������ʽ
#define SET_INPUT_PARAMETERS_TEMPLATE(string_value, input_type_mem)\
        if(IS_VALUED_STRING(string_value))\
        {\
            string contents_str = string_value;\
            CommonInterface::splic_string(contents_str,\
                                          input_type_mem, ",");\
        }

// ר�����ö���������ĺ�ģ�壬JSON��ʽ
#define SET_INPUT_PARAMETERS_JSON(string_value, input_type_mem)\
        if(IS_VALUED_STRING(string_value))\
        {\
            string contents_str = string_value;\
            CommonInterface::splic_string(contents_str,\
                                          input_type_mem, ";");\
        }

// ר�Ŵ��� upmongoDB �У��� mysql ֵ���õ� BSON �е�ģ��
#define SET_BSON_VALUE(mysql_row, mongodb_fname, value_type)\
        if(mysql_row == NULL)\
        {\
            bson_append_##value_type(b, mongodb_fname, "");\
        }\
        else\
        {\
            bson_append_##value_type(b, mongodb_fname, mysql_row);\
        }

// ר��Ϊ�������������Ѷ�������Ƶĺ�ģ��
#define SET_NEWS_TYPE_NAME(news_record, out, out_name)\
        LOGD("[GWJ] %s: type_id[%s]", __FUNCTION__, news_record->type_id.c_str());\
\
        string type_name_value;\
        if(strcmp(news_record->type_id.c_str(), "1") == 0)\
        {\
            CommonSimpleBuffer::instance().get_game_info_type_name("1", type_name_value);\
            out->set_##out_name(type_name_value.c_str());\
        }\
        else if(strcmp(news_record->type_id.c_str(), "2") == 0)\
        {\
            CommonSimpleBuffer::instance().get_game_info_type_name("2", type_name_value);\
            out->set_##out_name(type_name_value.c_str());\
        }\
        else if(strcmp(news_record->type_id.c_str(), "3") == 0)\
        {\
            CommonSimpleBuffer::instance().get_game_info_type_name("3", type_name_value);\
            out->set_##out_name(type_name_value.c_str());\
        }\
        else if(strcmp(news_record->type_id.c_str(), "4") == 0)\
        {\
            CommonSimpleBuffer::instance().get_game_info_type_name("4", type_name_value);\
            out->set_##out_name(type_name_value.c_str());\
        }\
        else if(strcmp(news_record->type_id.c_str(), "5") == 0)\
        {\
            CommonSimpleBuffer::instance().get_game_info_type_name("5", type_name_value);\
            out->set_##out_name(type_name_value.c_str());\
        }\
        else if(strcmp(news_record->type_id.c_str(), "6") == 0)\
        {\
            CommonSimpleBuffer::instance().get_game_info_type_name("6", type_name_value);\
            out->set_##out_name(type_name_value.c_str());\
        }\
        else if(strcmp(news_record->type_id.c_str(), "7") == 0)\
        {\
            CommonSimpleBuffer::instance().get_game_info_type_name("7", type_name_value);\
            out->set_##out_name(type_name_value.c_str());\
        }\
        else if(strcmp(news_record->type_id.c_str(), "8") == 0)\
        {\
            CommonSimpleBuffer::instance().get_game_info_type_name("8", type_name_value);\
            out->set_##out_name(type_name_value.c_str());\
        }\
        else if(strcmp(news_record->type_id.c_str(), "9") == 0)\
        {\
            CommonSimpleBuffer::instance().get_game_info_type_name("9", type_name_value);\
            out->set_##out_name(type_name_value.c_str());\
        }\
        else if(strcmp(news_record->type_id.c_str(), "10") == 0)\
        {\
            CommonSimpleBuffer::instance().get_game_info_type_name("10", type_name_value);\
            out->set_##out_name(type_name_value.c_str());\
        }

// ר��Ϊ�����������ͼƬ�����͵ĺ�ģ��
#define SET_OUTPUT_IMG_INFO(img_info, select_imgs, output_img)\
        vector<MCPTupleItem>::const_iterator itr = img_info.begin();\
        vector<MCPTupleItem>::const_iterator end_itr = img_info.end();\
        for(int i = 0; itr != end_itr; itr++)\
        {\
            const char* img_type = (*itr).key.c_str();\
\
            int index = CommonInterface::find_key_index(img_type, select_imgs);\
\
            if(index < 0)\
            {\
                LOGD("[GWJ] %s: No need img_type[%s]", __FUNCTION__, img_type);\
            }\
            else\
            {\
                LOGD("[GWJ] %s: need img_type[%s]", __FUNCTION__, img_type);\
                output_img(i)->set_type(img_type);\
                output_img(i)->set_url((*itr).value.c_str());\
                i++;\
            }\
        }

#define SET_OUTPUT_PUB_PROP_INFO_NEW(i, key, value)\
	if( !strcmp(key, "name") )\
	{\
			out->mutable_pub_prop_info(i)->set_name(value);\
	}\
	else if( !strcmp(key, "color") )\
	{\
			out->mutable_pub_prop_info(i)->set_color(value);\
	}\
	else if( !strcmp(key, "prop_url") )\
	{\
			out->mutable_pub_prop_info(i)->set_prop_url(value);\
	}\
	LOGA("[zx] %s: set struct [%s %s]",__FUNCTION__, key, value);


//add by zhengxie 
#define SET_OUTPUT_PUB_PROP_INFO(pub_prop_info,  output_pub_prop_info)\
		LOGD("[ZX] pub_prop_info size:%d ", pub_prop_info.size());\
        vector<MCPTupleItem>::const_iterator itr2 = pub_prop_info.begin();\
        vector<MCPTupleItem>::const_iterator end_itr2 = pub_prop_info.end();\
        for(int i=0; itr2 != end_itr2; itr2++)\
        {\
            const char* name = (*itr2).key.c_str();\
            const char* color = (*itr2).value.c_str();\
			output_pub_prop_info(i)->set_name(name);\
			output_pub_prop_info(i)->set_color(color);\
			i++;\
			LOGD("[ZX] set name:%s: name[%s]", __FUNCTION__, name);\
			LOGD("[ZX] set color:%s: color[%s]", __FUNCTION__, color);\
        }

// ����һ����Ϊ����ģʽ�ĺ�
#define NSP_SINGLETONIZE(class_name__)                      \
    protected:                                              \
        class_name__();                                     \
        class_name__(const class_name__&);                  \
        class_name__& operator = (const class_name__&);     \
    public:                                                 \
        static class_name__& instance()                     \
        {                                                   \
            static class_name__ singletonInstance;          \
            return  singletonInstance;                      \
        }                                                   \
    protected:


/*
 *  ���� Refactory �ع������ȫ�� string ����
 *  �����ݿ��в�ѯ�����ݲ�����Ϊ��������
 */
struct MCPTupleItem
{
    string key;
    string value;
    bool is_string_value;

    void clear()
    {
        key.clear();
        value.clear();
    }
};

struct MCPStructItem
{
	map<string, string> record;
};
/*
 *  ר���������ò�ѯ������С����
 */
struct MCPTupleDecoratorItem
{
    /*
     *  ���õ� value �Ƿ�Ϊ string ����
     *  ������� string ������?integer ����������
     */
    bool is_string_value;

    // Ҫ�����ı����������  key
    const char* table_column;

    /*
     *  �������������
     *  ���Ȱ��� char* ����������
     *  ���崦��ʱ�ж��� string ���ͻ��� integer ����
     *  ��ʱ��������Ӧ��ת��
     */
    const char* value;

    MCPTupleDecoratorItem()
    {
        // Ĭ��Ϊ string ����
        is_string_value = true;
    }
};

/*
 *  û�г�Ա������ͨ�ýӿ���
 *  �������Ϊ����ģʽ
 */
class CommonInterface
{
public:

    CommonInterface();
    ~CommonInterface();

    // �ҵ� value �� list �е�������������ʱ���� -1
    static int find_key_index(const char* value, vector<const char*>& list);
    static int find_key_index(const char* value, vector<string>& list);
    static int find_key_index(const char* value, list<const char*>& list);
    static int find_key_index(const char* value, list<string>& list);

    // ���鸳ֵ����
    static void copy_vector_values(vector<string>& src, vector<string>& des);

    // ɾ���ַ�����������
    static void clear_vector_string(vector<string>& list);

    // �ҵ� MYSQL ���в�������������
    static const char* find_pure_column_name(const char* column_name);

    /*
     * �ж�������ַ����Ƿ��Ǳ�׼��������ʽ���� `column_name`
     *
     * ���ս������ standard_column ��֤��ʵһ����׼��ʽ
     *
     * return : 0 : ���Ǳ�׼��ʽ����Ҫת��
     *          1 : �Ѿ��Ǳ�׼��ʽ������Ҫת��
     *          -1 : �����������ʽ
     */
    static int is_standard_column_format(const char* column_name, string& standard_column);

    /*
     * ��ȡ���� column �е� nick name, ���浽���������
     *
     * return : 0 : column �в��� nick_name
     *          1 : ���� nick_name ������
     *          -1 : ���������
     */
    static int get_nick_name(const char* column, string& nick_name);

    // ��ȡϵͳ����ʱ���
    static void get_time_now(string& tm_now);

    /*
     *  ����ָ�����ŷָ��ַ���
     *  return : ���طָ����ַ�������
     */
    static int splic_string(string& orig_string, vector<string>& split_result, const char* seperator);
    static int splic_string(char* orig_string, vector<char*>& split_result, const char* seperator);

    /*
     *  ȥ����β�߽��ַ�����ȡ�м��ַ���
     */
    static char* elimite_shell_flag(char* orig_string,
                                    const char start_flag,
                                    const char end_flag);

    static void set_struct_mongodb_value(bson* b,
                                         const char* field_name,
                                         vector<const char*>& struct_cols,
                                         string& mysql_value);

    static void set_array_mongodb_value(bson* b,
                                        const char* field_name,
                                        string& mysql_value);

	static double get_interface_process_time( struct timeval &s, struct timeval &e);
	static pid_t get_thread_id();
};

/*
 *  ģ�黯 LOG ��־
 *
 *  LOGA �ļ�����ߣ������������߰� LOG
 *  ����һ������²�Ҫ�� LOGA
 *  LOGA ֻ��ӡ�����ӿڴ�����Ϣ����
 */
#define LOGA(...) UB_LOG_WARNING(__VA_ARGS__)

/* add by zhengxie ��ȷ�����뼶���ʱ����� */
#define LOGT(START, END) UB_LOG_WARNING("Interface[%s] process time [%lf ms]\n", __FUNCTION__, CommonInterface::get_interface_process_time(START, END))

/*
 *  һ��Ϊ������ LOG
 *  ������ҪҲ������������
 *  ����ס�������߰�ʱ�ص���Щ���� LOG
 */
#define MEM_HEAP_TEST 0
#if MEM_HEAP_TEST
#define LOGFC(...) //UB_LOG_NOTICE(__VA_ARGS__)
#else
#define LOGFC(...) //UB_LOG_NOTICE(__VA_ARGS__)
#endif

// ȫ�ֵ��� log
#define LOGD(...)  UB_LOG_NOTICE(__VA_ARGS__)

// �����ݿ��л�ȡ����ʱ�� log
#define LOGG(...)  //UB_LOG_NOTICE(__VA_ARGS__)

// test Ŀ¼�в����õ� log
#define LOGXX(...) //UB_LOG_NOTICE(__VA_ARGS__)
#define LOGX(...)  //UB_LOG_NOTICE(__VA_ARGS__)

// ���� RPC ͨ�ŵ� log
#define LOGN(...)  //UB_LOG_NOTICE(__VA_ARGS__)


#endif /* COMMON_H_ */
