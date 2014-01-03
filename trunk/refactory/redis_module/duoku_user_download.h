/*
 * duoku_user_download.h
 *
 *  Created on: 2013-5-29
 *      Author: guoweijiang
 */

#ifndef DUOKU_USER_DOWNLOAD_H_
#define DUOKU_USER_DOWNLOAD_H_

#include "../../duokoo_mysql.h"
#include "redis_module.h"

class DuokuUserDownloadInfo
{
    static const char* KEY_SUFFIX;

public:

    // �����û������ص���Ϸ
    static int set_new_download(const char* u_id, const char* c_id);

    // �û�ɾ�����ع�����Ϸ
    static int remove_download(const char* u_id, const char* c_id);

    // ��ȡ�û����ص���Ϸ����
    static int get_user_download_games_count(const char* u_id);

    // ��ȡ�û����ص���ϷID�б�
    static int get_user_download_games(const char* u_id,
                                       list<string> &vs);

    static int remove_key_value_list(const char* key, list<string>& values);
};

#endif /* DUOKU_USER_DOWNLOAD_H_ */
