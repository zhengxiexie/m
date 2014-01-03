/*
 * common_simple_buf.h
 *
 *  Created on: 2013-5-15
 *      Author: guoweijiang
 */

#ifndef COMMON_SIMPLE_BUF_H_
#define COMMON_SIMPLE_BUF_H_

#include <iostream>
#include <sstream>
#include <string>
#include <map>
#include <vector>

#include "common.h"
#include "mysql_module/mysql_general_module.h"

class CommonSimpleBuffer
{
    // ����ģʽ
    NSP_SINGLETONIZE(CommonSimpleBuffer)

    // ��Ϸ��������
    MysqlTable* m_public_propertys;

    // ��Ϸ��Ѷ����
    MysqlTable* m_game_info_type;

    // ��ȡ��������
    int fill_public_property();

    // ��ȡ��Ϸ��Ѷ��������
    int fill_game_info_type_name();

    int get_public_property_url(const char* id, const char* prop_url);

public:
    ~CommonSimpleBuffer();

    int init_buffer();
    int get_game_info_type_name(const char* id, string& type_name);
};

#endif /* COMMON_SIMPLE_BUF_H_ */
