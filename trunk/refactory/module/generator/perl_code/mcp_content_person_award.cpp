#include "mcp_content_person_award.h"

const char* MCPContentPersonAwardRecord::ClassName = "MCPContentPersonAwardRecord";

MCPContentPersonAward::MCPContentPersonAward(const char* key_name):AbstractMCPContent(key_name)
{
    LOGD("[GWJ] %s: start", __FUNCTION__);

    this->_basic_coll = "mcp_content_person_award";
    this->_basic_ns = string("mcp") + "." + this->_basic_coll;

    LOGD("[GWJ] %s: end", __FUNCTION__);

}

MCPContentPersonAward::~MCPContentPersonAward()
{

    LOGD("[GWJ] %s: start. recycle result_set", __FUNCTION__);
    RECYCLE_RESULT_SET(MCPContentPersonAwardRecord)
    LOGD("[GWJ] %s: end", __FUNCTION__);

}

int MCPContentPersonAward::set_basic_info_from_bson_result(bson* bson_out, const char* aggregate_name)
{

    LOGD("[GWJ] %s:  start", __FUNCTION__);

    const char* key;
    bson_iterator it[1];
    stringstream ss;

    MCPContentPersonAwardRecord* detail = ObjectPoolFactory<MCPContentPersonAwardRecord>::instance().fetch_object();
    bson_iterator_init(it, bson_out);

    while(bson_iterator_next(it))
    {
        ss.clear();
        key = bson_iterator_key(it);

        if(0 == strcmp(key, "priority"))
        {
            detail->priority = bson_iterator_string(it);


            LOGG("[GWJ] %s: set MCPContentPersonAwardRecord.priority[%s]",
                     __FUNCTION__, detail->priority.c_str());
        }
        else if(0 == strcmp(key, "kudou"))
        {
            detail->kudou = bson_iterator_int(it);


            LOGG("[GWJ] %s: set MCPContentPersonAwardRecord.kudou[%d]",
                     __FUNCTION__, detail->kudou;
        }
        else if(0 == strcmp(key, "commodity_id"))
        {
            detail->commodity_id = bson_iterator_string(it);


            LOGG("[GWJ] %s: set MCPContentPersonAwardRecord.commodity_id[%s]",
                     __FUNCTION__, detail->commodity_id.c_str());
        }
		else if(0 == strcmp(key, "type"))
        {
            detail->type = bson_iterator_string(it);


            LOGG("[GWJ] %s: set MCPContentPersonAwardRecord.type[%s]",
                     __FUNCTION__, detail->type.c_str());
        }
		else if(0 == strcmp(key, "date"))
        {
            detail->date = bson_iterator_string(it);


            LOGG("[GWJ] %s: set MCPContentPersonAwardRecord.date[%s]",
                     __FUNCTION__, detail->date.c_str());
        }
		else if(0 == strcmp(key, "icon"))
        {
            detail->icon = bson_iterator_string(it);


            LOGG("[GWJ] %s: set MCPContentPersonAwardRecord.icon[%s]",
                     __FUNCTION__, detail->icon.c_str());
        }
		else if(0 == strcmp(key, "name"))
        {
            detail->name = bson_iterator_string(it);


            LOGG("[GWJ] %s: set MCPContentPersonAwardRecord.name[%s]",
                     __FUNCTION__, detail->name.c_str());
        }
        else if(0 == strcmp(key, "user_id"))
        {
            detail->user_id = bson_iterator_string(it);


            LOGG("[GWJ] %s: set MCPContentPersonAwardRecord.user_id[%s]",
                     __FUNCTION__, detail->user_id.c_str());
        }
        else if(0 == strcmp(key, "id"))
        {
            detail->id = bson_iterator_string(it);


            LOGG("[GWJ] %s: set MCPContentPersonAwardRecord.id[%s]",
                     __FUNCTION__, detail->id.c_str());
        }
    }// while

    if(IS_SAME_VALUED_STRING(this->key_name, "priority"))
    {
        FILL_RESULT_BUCKET(detail, priority)
    }
    else if(IS_SAME_VALUED_STRING(this->key_name, "kudou"))
    {
        FILL_RESULT_BUCKET(detail, kudou)
    }
    else if(IS_SAME_VALUED_STRING(this->key_name, "commodity_id"))
    {
        FILL_RESULT_BUCKET(detail, commodity_id)
    }
    else if(IS_SAME_VALUED_STRING(this->key_name, "user_id"))
    {
        FILL_RESULT_BUCKET(detail, user_id)
    }
    else if(IS_SAME_VALUED_STRING(this->key_name, "id"))
    {
        FILL_RESULT_BUCKET(detail, id)
    }
    LOGD("[GWJ] %s: end", __FUNCTION__);
    return 0;
}
