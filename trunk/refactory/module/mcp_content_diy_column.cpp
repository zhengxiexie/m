#include "mcp_content_diy_column.h"

const char* MCPDIYColumnRecord::ClassName = "MCPDIYColumnRecord";

MCPDIYColumnInfo::MCPDIYColumnInfo(const char* key_name):AbstractMCPContent(key_name)
{
    LOGD("[GWJ] %s: start", __FUNCTION__);
    this->_basic_coll = "mcp_content_diy_column";
    this->_basic_ns = string("mcp") + "." + this->_basic_coll;
    LOGD("[GWJ] %s: end", __FUNCTION__);
}

MCPDIYColumnInfo::~MCPDIYColumnInfo()
{
    LOGD("[GWJ] %s: start. recycle result_set", __FUNCTION__);
    RECYCLE_RESULT_SET(MCPDIYColumnRecord)
    LOGD("[GWJ] %s: end", __FUNCTION__);
}

int MCPDIYColumnInfo::set_basic_info_from_bson_result(bson* bson_out,
                                                      const char* aggregate_name)
{

    LOGD("[GWJ] %s:  start", __FUNCTION__);

    const char* key;
    bson_iterator it[1];
    stringstream ss;

    MCPDIYColumnRecord* detail = ObjectPoolFactory<MCPDIYColumnRecord>::instance().fetch_object();
    bson_iterator_init(it, bson_out);

    while(bson_iterator_next(it))
    {
        ss.clear();
        key = bson_iterator_key(it);

        if(0 == strcmp(key, "type_id"))
        {
            detail->type_id = bson_iterator_string(it);


            LOGG("[GWJ] %s: set MCPDIYColumnRecord.type_id[%s]",
                     __FUNCTION__, detail->type_id.c_str());
        }
        else if(0 == strcmp(key, "name"))
        {
            detail->name = bson_iterator_string(it);


            LOGG("[GWJ] %s: set MCPDIYColumnRecord.name[%s]",
                     __FUNCTION__, detail->name.c_str());
        }
        else if(0 == strcmp(key, "id"))
        {
            detail->id = bson_iterator_string(it);


            LOGG("[GWJ] %s: set MCPDIYColumnRecord.id[%s]",
                     __FUNCTION__, detail->id.c_str());
        }
    }// while

    if(IS_SAME_VALUED_STRING(this->key_name, "id"))
    {
        FILL_RESULT_BUCKET(detail, id)
    }
    else if(IS_SAME_VALUED_STRING(this->key_name, "type_id"))
    {
        FILL_RESULT_BUCKET(detail, type_id)
    }
    else if(IS_SAME_VALUED_STRING(this->key_name, "name"))
    {
        FILL_RESULT_BUCKET(detail, name)
    }

    LOGD("[GWJ] %s: end", __FUNCTION__);
    return 0;
}
