/*
 * game_brief_info_t.h
 *
 *  Created on: 2013-5-10
 *      Author: guoweijiang
 */

#ifndef GAME_BRIEF_INFO_T_H_
#define GAME_BRIEF_INFO_T_H_

#include "common_header.h"
#include "../module/mcp_content_news.h"

/*
struct game_info_t
{
    string info_type;           //��Ѷ���� 1-���� 2-���� 3-� 4-����
    string info_id;
    string c_id;
    string info_title;
    string info_content;
    string info_date;
    string type_name;
    string game_type;           //��Ϸ���� 0-���� 1-���� 2-�޶�Ӧ��Ϸ
};
*/
class game_info_t
{
public:
    static void set_outparameter(idl::game_info_t* out,
                                 MCPNewsInfo* binded_record);
    static void set_outparameter_hall(idl::game_info_t_hall* out,
                                 MCPNewsInfo* binded_record);
};

#endif /* GAME_BRIEF_INFO_T_H_ */
