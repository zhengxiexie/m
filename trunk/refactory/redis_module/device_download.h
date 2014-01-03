/*
 * device_download.h
 *
 *  Created on: 2013-6-18
 *      Author: guoweijiang
 */

#ifndef DEVICE_DOWNLOAD_H_
#define DEVICE_DOWNLOAD_H_

#include "../../duokoo_mysql.h"
#include "redis_module.h"

class DeviceDownloadInfo
{
    static const char* KEY_SUFFIX;

public:

    // �����豸�����ص���Ϸ
    static int set_new_download(const char* u_id, const char* c_id);

    // �豸ɾ�����ع�����Ϸ
    static int remove_download(const char* u_id, const char* c_id);

    // ��ȡ�豸���ص���Ϸ����
    static int get_device_download_games_count(const char* u_id);

    // ��ȡ�豸���ص���ϷID�б�
    static int get_device_download_games(const char* u_id,
                                         list<string> &vs);

    static int remove_key_value_list(const char* key, list<string>& values);
};


#endif /* DEVICE_DOWNLOAD_H_ */
