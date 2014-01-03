/*
 * mcp_content_data.h
 *
 *  ר�Ÿ������  MongoDB ��  mcp.mcp_content_data �����
 *
 *  Created on: 2013-4-27
 *      Author: guoweijiang
 */

#ifndef MCP_CONTENT_DATA_H_
#define MCP_CONTENT_DATA_H_

#include "abstract_mcp_content.h"

/*
 *  Ҫͨ������ع���
 *  ����Ҫʵ�� ClassName �� clear ����
 */
class MCPContentDataInfo : public AbstractMCPRecord
{
public:

    static const char* ClassName;

    string id;
    string baidu_path_url;
    string path_url;
    string path_type;
    string media_format_id;
    string size;
    string mtype;
    string charge_typeid;
    string c_id;
    string value;
    string modify_date;
    vector<MCPTupleItem> adapt_info;
    string channel_info;
    string channel_id;
    string package_name;
    string int_version;
    string string_version;
    string sdk_version;
    string apk_md5;
    string file_md5;

    void clear()
    {
        id.clear();
        path_url.clear();
        baidu_path_url.clear();
        path_type.clear();
        media_format_id.clear();
        mtype.clear();
        charge_typeid.clear();
        c_id.clear();
        modify_date.clear();
        adapt_info.clear();
        channel_info.clear();
        channel_id.clear();
        package_name.clear();
        int_version.clear();
        string_version.clear();
        sdk_version.clear();
        apk_md5.clear();
        file_md5.clear();
    }

    MCPContentDataInfo():AbstractMCPRecord()
    {
    }

    virtual ~MCPContentDataInfo()
    {
        this->clear();
    }
};

class MCPContentData : public AbstractMCPContent
{
protected:

    /*
     *  ���� mcp.mcp_content ���в�ѯ����һ�� Record ���浽  id_s �� result_set ��
     */
    int set_basic_info_from_bson_result(bson* bson_out,
                                        const char* aggregate_name);

public:
    MCPContentData(const char* key_name);
    virtual ~MCPContentData();
};

#endif /* MCP_CONTENT_DATA_H_ */
