/*
 * =====================================================================================
 *
 *       Filename:  mcpsvr_phone_headword.cpp
 *
 *    Description:  ʵ�ֽӿ�get_phone_headword_from_maker
 *
 *        Version:  1.0
 *        Created:  08/11/2011 06:38:42 PM
 *       Revision:  none
 *       Compiler:  g++
 *
 *         Author:  RD. Hongming li (lhm), lihongming@baidu.com
 *        Company:  baidu, Beijing, China
 *
 * =====================================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include "mcp_session.h"
#include "mcp.idl.mcp.h"
#include "mcpsvr.h"
#include "tools.h"

#define PHONE_SEARCH_ST_TIMEOUT 100
#define PHONE_SEARCH_ST_EXPIRED 60*24*60

typedef struct {
    char phone_id[12];
    char phone_name[256];
    char brand_id[12];
}search_phone_st;

/**
  *@brief ���ݹؼ��ֻ��ƥ���������(REDIS)
  *
  *@param [in]  key       �ؼ���
  *
  * @return    int        ����Ҫ��Ļ�������,С��0ʧ��
  */
int get_phone_search_cnt_by_key_fromCache(string key)
{
    int ret = 0;
    //��KEY
    string mainkey = "search_phone_info";
    //��KEY
    string subkey = key+"_count";
    //���÷���ֵ��С
    string valueout;
    mcp_session conn;
    ret = conn.session_search( mainkey, subkey, valueout);
    if ( valueout.length() <= 0 )
    {
        UB_LOG_NOTICE("[NOT IN REDIS] %s MAINKEY %s SUBKEY %s",__func__,mainkey.c_str(), subkey.c_str() );
        return -1;
    }
    else if ( 0 == ret )
    {
        UB_LOG_NOTICE("[FOUND IN REDIS] %s MAINKEY %s SUBKEY %s ",__func__,mainkey.c_str(), subkey.c_str() );
        return atoi( valueout.c_str() );
    }
    else
    {
        UB_LOG_NOTICE("[ERROR IN REDIS] %s MAINKEY %s SUBKEY %s",__func__,mainkey.c_str(), subkey.c_str() );
        return -1;
    }
}
/**
  *@brief ����ؼ��ֻ��ƥ���������(REDIS)
  *
  *@param [in]  key          �ؼ���
  *@param [in]  cnt          ����Ҫ��Ļ�������
  *
  *@return int  0 �ɹ� -1 ʧ��
  */
int insert_phone_search_cnt_by_key_toCache(string key,int cnt)
{
    int ret = 0;
    //cache��Ϣ
    if( cnt < 0 ){
        return -1;
    }
    //��KEY
    string mainkey = "search_phone_info";
    //��KEY
    string subkey = key+"_count";
    //����Ҫ�����ֵ
    char num[16];
    sprintf(num , "%d", cnt);
    string value = num;
    mcp_session conn;
    //��ɾ��
    ret = conn.session_delete( mainkey, subkey );
    if (0 != ret)
    {
        UB_LOG_NOTICE("[DELETE REDIS ERROR] %s MAINKEY %s SUBKEY %s ",__func__,mainkey.c_str(), subkey.c_str() );
        return -1;
    }
    //����
    ret = conn.session_insert(mainkey, subkey, value, PHONE_SEARCH_ST_EXPIRED );
    if (0 != ret)
    {
        UB_LOG_NOTICE("[INSERT REDIS ERROR] %s MAINKEY %s SUBKEY %s",__func__,mainkey.c_str(), subkey.c_str() );
        return -1;
    }
    UB_LOG_NOTICE("[INSERT REDIS SUCCESS] %s MAINKEY %s SUBKEY %s",__func__,mainkey.c_str(), subkey.c_str() );
    return 0;
}
/**
  *@brief  ���ݹؼ��ֻ��ƥ���������(DB)
  *
  *@param [in]  key          �ؼ���
  *
  * @return    int           ����Ҫ��Ļ�������,С��0ʧ��
  */
int get_phone_search_cnt_by_key_fromDB(string key){

    MysqlConnection *database_connection;
    string sql_count = "SELECT count(DISTINCT  `mcp_phone_info`.`name` , `mcp_phone_info`.`brand_id`) FROM  `mcp_phone_info` WHERE `mcp_phone_info`.`name` like '%"+key+"%' ";

    int err = 0;
    int ret = 0;
    
    //�õ�connection����
    database_connection = database_pool->fetchConnection("default_mysql", &err, 0);
    if ( NULL == database_connection )
    {
        UB_LOG_NOTICE("[GET PHONE_HEADWORD_CNT BY BRAND_ID CONNECT DB ERROR] %s",__func__);
        return -1;
    }
    UB_LOG_NOTICE("%s { %s }",__func__,sql_count.c_str());
    ret = database_connection->selectCount(sql_count.c_str());
    database_pool->putBackConnection(database_connection, false);
    return ret;
}

/**
  *@brief  ���ݹؼ��ֻ��ƥ���������
  *
  *@param [in]  key          �ؼ���
  *
  * @return    int           ����Ҫ��Ļ�������,С��0ʧ��
  */
int get_phone_search_cnt_by_key(string key){
    int cache_flag = 0;
    int cnt = 0;
#ifndef REAL_TIME_DATA
    //Search from cache
    cnt = get_phone_search_cnt_by_key_fromCache(key);
    if(cnt <= 0){
        //not in cache
        cache_flag = 0;
    }else{
       //in cache
       cache_flag = 1;
       UB_LOG_NOTICE("[GET PHONE_SEARCH_CNT  OK] %s key = %s CNT = %d", __func__,key.c_str(),cnt);
    }
#endif
/*
    if( 0 == cache_flag ){
        //not in cache or doese use cache
        cnt = get_phone_search_cnt_by_key_fromDB(key);
        if( cnt < 0 ){
            //DB query error
            UB_LOG_NOTICE("[GET PHONE_SEARCH_CNT  ERROR] %s key = %s ", __func__,key.c_str());
        }else{
            //DB query OK
            UB_LOG_NOTICE("[GET PHONE_SEARCH_CNT  OK] %s key = %s CNT = %d", __func__,key.c_str(),cnt);
#ifndef REAL_TIME_DATA
            //insert into cache
            insert_phone_search_cnt_by_key_toCache(key,cnt,logid);
#endif
        }
    }
*/
    return cnt;
}

/**
  *@brief  ���ݹؼ��ֻ��ƥ������б�(REDIS)
  *
  *@param [in]  key          �ؼ���
  *@param [in]  cnt          ����Ҫ��Ļ�������
  *@param [out] result       ��������ṹ��
  *
  * @return    int           0 �ɹ�, С�� 0 ʧ��
  */
int get_phone_search_list_by_key_fromCache(string key,int cnt,search_phone_st* result)
{
    int ret = 0;
    //cache��Ϣ
    if( NULL == result || cnt < 0 ){
        return -1;
    }
    if( 0 == cnt )
    {
        UB_LOG_NOTICE("cnt is %d,not have record",cnt);
        return MCP_OK;
    }
    //��KEY
    string mainkey = "search_phone_info";
    //��KEY
    string subkey = key + "list";
    //���÷���ֵ��С
    string valueout;
    mcp_session conn;
    ret = conn.session_search( mainkey, subkey, valueout );
    if ( valueout.length() <= 0 )
    {
        UB_LOG_NOTICE("[NOT IN REDIS] %s MAINKEY %s SUBKEY %s",__func__,mainkey.c_str(), subkey.c_str() );
        return MCP_NG;
    }
    else if ( 0 == ret )
    {
        UB_LOG_NOTICE("[FOUND IN REDIS] %s MAINKEY %s SUBKEY %s",__func__,mainkey.c_str(), subkey.c_str() );
    }
    else
    {
        UB_LOG_NOTICE("[ERROR IN REDIS] %s MAINKEY %s SUBKEY %s",__func__,mainkey.c_str(), subkey.c_str() );
        return MCP_NG;
    }
    
    Json::Reader json_reader;
    Json::Value json_value;
    string tmpmsg;
    size_t pos= 0; 
    int k = 0;
    while( (pos = valueout.find_first_of( '{', pos )) < valueout.size() && k < cnt )
    {   
       tmpmsg.assign( valueout, pos, valueout.find_first_of( '}', pos ) - pos +1 );
       ++pos;
       if(json_reader.parse(tmpmsg,json_value))
       {
	  strlcpy( result[k].phone_id, json_value["phone_id"].asString().c_str(), sizeof(result[k].phone_id) );
	  strlcpy( result[k].phone_name, json_value["phone_name"].asString().c_str(), sizeof(result[k].phone_name) );
	  strlcpy( result[k].brand_id, json_value["brand_id"].asString().c_str(), sizeof(result[k].brand_id) );
       }
       k++;
    }
    return MCP_OK;
}
/**
  *@brief  ����ؼ��ֻ��ƥ������б�(REDIS)
  *
  *@param [in]  key          �ؼ���
  *@param [in]  cnt          ����Ҫ��Ļ�������
  *@param [out] result       ��������ṹ��
  *
  * @return    int           0 �ɹ�, С�� 0 ʧ��
  */
int insert_phone_search_list_by_key_toCache(string key,int cnt,search_phone_st *result)
{
    int ret = 0;
    if( NULL == result || cnt < 0 ){
        return -1;
    }
    //��KEY
    string mainkey = "search_phone_info";
    //��KEY
    string subkey = key + "list";
    
    //����Ҫ�����ֵ
    string value;
    string phone_id;
    string phone_name;
    string brand_id;
    for( int i=0; i< cnt; i++ )
    {
       phone_id.clear(); phone_id = result[i].phone_id;
       phone_name.clear(); phone_name = result[i].phone_name;
       brand_id.clear(); brand_id = result[i].brand_id;
       value += "{\"phone_id\":\"" + phone_id +"\",\"phone_name\":\"" + phone_name + "\",\"brand_id\":\"" + brand_id + "\"}";
    }
    mcp_session conn;
    //��ɾ��
    ret = conn.session_delete( mainkey, subkey );
    if (0 != ret)
    {
        UB_LOG_FATAL("[DELETE REDIS ERROR] %s MAINKEY %s SUBKEY %s",__func__,mainkey.c_str(), subkey.c_str() );
        return MCP_NG;
    }
    //����
    ret = insert_phone_search_cnt_by_key_toCache(key,cnt);
    if (0 != ret){
        UB_LOG_FATAL("insert_phone_search_cnt_by_key_toCache is failed, cnt is %d",cnt);
        return MCP_NG;
    }
    ret = conn.session_insert(mainkey, subkey, value, PHONE_SEARCH_ST_EXPIRED );
    if (0 != ret)
    {
        UB_LOG_FATAL("[INSERT REDIS ERROR] %s MAINKEY %s SUBKEY %s",__func__,mainkey.c_str(), subkey.c_str() );
        return MCP_NG;
    }
    UB_LOG_NOTICE("[INSERT REDIS SUCCESS] %s MAINKEY %s SUBKEY %s",__func__,mainkey.c_str(), subkey.c_str() );
    return MCP_OK;
}

/**
  *@brief  ���ݹؼ��ֻ��ƥ������б�(DB)
  *
  *@param [in]  key          �ؼ���
  *@param [in]  cnt          ����Ҫ��Ļ�������
  *@param [out] result       ��������ṹ��
  *
  * @return    int           �ɹ� ���͸���, С�� 0 ʧ��
  */
int get_phone_search_list_by_key_fromDB(string key, vector<search_phone_st>& result){

    UB_LOG_NOTICE("[Start Search Series From DB] %s : key = %s ", __func__,key.c_str());
    int err = 0;
    int ret = 0;
    int j = 0;
    search_phone_st temp_result;
    MysqlConnection *database_connection;
    MyclientRes data_res;
    string sql =  "SELECT `mcp_phone_info`.`id`,`mcp_phone_info`.`name`,`mcp_phone_info`.`brand_id`,`mcp_phone_brand`.`name`  FROM  `mcp_phone_info` LEFT JOIN `mcp_phone_brand` ON `mcp_phone_info`.`brand_id` = `mcp_phone_brand`.`id` WHERE `mcp_phone_info`.`name` like '%"+key+"%'" + " ORDER BY  `mcp_phone_info`.`name` ASC ";

    //�õ�connection����
    database_connection = database_pool->fetchConnection("default_mysql", &err, 0);
    if ( NULL == database_connection )
    {
        UB_LOG_NOTICE("[Search Series From DB Connect ERROR] %s : key = %s ", __func__,key.c_str());
        data_res.free();
        return -1;
    }

    UB_LOG_NOTICE("%s { %s }",__func__,sql.c_str());
    ret = database_connection->query(sql.c_str(), &data_res, true);
    
    if ( ret < 0 ){
        UB_LOG_NOTICE("[Search Series From DB ERROR] %s : key = %s ", __func__,key.c_str());
        ret = database_pool->putBackConnection(database_connection, false);
        if (ret != 0){
            UB_LOG_NOTICE("[Search Series PutBack Connection ERROR] %s : key = %s ", __func__,key.c_str());
        }
        data_res.free();
        return -1;
    }
    
//    if( ret > cnt ){
//    	ret = cnt;
//    }
    UB_LOG_NOTICE("from db ret is %d",ret); 
    for(int i = 0; i < ret; i++){
    	char tmp_name[256];
    	snprintf(tmp_name,sizeof(tmp_name),"%s-%s",data_res[i][3],data_res[i][1]);
        if ( j > 0 )
        {
            if ( 0 == strcmp( result[j-1].phone_name,tmp_name) )
            {
                UB_LOG_NOTICE("phone name %s is same", result[j-1].phone_name);
                continue;
            }
        }
        result.push_back(temp_result);
    	copy_result(result[j].phone_id,data_res[i][0]);
    	copy_result(result[j].phone_name,tmp_name);
    	copy_result(result[j].brand_id,data_res[i][2]);
        j++;
    }
    ret = j;
    UB_LOG_NOTICE("last ret is %d",ret);
    UB_LOG_NOTICE("[Save Reuslt From DB INTO result (OK)] %s : key = %s ", __func__,key.c_str());
    //�ͷ���Դ
    database_pool->putBackConnection(database_connection, false);
    data_res.free();
    return ret;
}

/**
  *@brief  ���ݹؼ��ֻ��ƥ������б�
  *
  *@param [in]  key          �ؼ���
  *@param [in]  cnt          ����Ҫ��Ļ�������
  *@param [out] result       ��������ṹ��
  *
  * @return    int           �ɹ� ���͸���, С�� 0 ʧ��
  */
int get_phone_search_list_by_key(string key,int cnt,vector<search_phone_st>& result){
    int cache_flag = 0;

    int ret = 0;
    if( cnt >0  ){
//    cache_flag = 0;
#ifndef REAL_TIME_DATA
    //����ռ�
        search_phone_st *result_array = (search_phone_st *) malloc(sizeof(search_phone_st) * cnt);
        if( NULL == result_array ){
            UB_LOG_FATAL("create result_array is failed, cnt is %d",cnt);
            return -1;
        }
    //��ʼ���ڴ�
        memset(result_array,0,sizeof(search_phone_st) * cnt);
        ret = get_phone_search_list_by_key_fromCache(key,cnt,result_array);
        if(ret != 0){
            //not in cache
            cache_flag = 0;
        }else{
            //in cache
            UB_LOG_NOTICE(" cache is not exits");
            cache_flag = 1;  
            for( int i =0; i < cnt; i++)
            {
                result.push_back(result_array[i]);
            } 
            free(result_array);
            return cnt;
        }
        free(result_array);
#endif
    }
    cnt = 0;
    if( 0 == cache_flag ){
        ret = get_phone_search_list_by_key_fromDB(key,result);
        if( ret < 0 ){
            //DB query error
            UB_LOG_FATAL("[GET PHONE SEARCH LIST ERROR] %s key = %s ", __func__,key.c_str());
        }else if ( 0 == ret )
        {
            UB_LOG_NOTICE("key %s ,no search phone info",key.c_str());
            cnt = 0;
        }else
        {
            //DB query OK
            UB_LOG_NOTICE("[GET PHONE SEARCH LIST OK] %s key = %s ", __func__,key.c_str());
#ifndef REAL_TIME_DATA
            //insert into cache
            cnt = ret;
            search_phone_st *result_array = (search_phone_st *) malloc(sizeof(search_phone_st) * cnt);
            if( NULL == result_array ){
                UB_LOG_FATAL("create result_array is failed, cnt is %d",cnt);
                return MCP_NG;
            }
    //��ʼ���ڴ�
            memset(result_array,0,sizeof(search_phone_st) * cnt);
            for(int i =0; i< cnt; i++)
            {
                result_array[i] = result.at(i);
            } 
            ret = insert_phone_search_list_by_key_toCache(key,cnt,result_array);
            if ( ret != MCP_OK)
            {
                UB_LOG_FATAL("%s is failed, cnt is %d", __func__, cnt);
                return MCP_NG;
            }
            free(result_array);
#endif
        }
    }
    return cnt;
}


int McpServlet::search_phone_info(const idl::mcp_search_phone_info_params& in, idl::mcp_search_phone_info_response& out){
    
    //�������
    const string key = in.key();
    const uint32_t unit_num  = in.unit_num();
    const uint32_t unit_times = in.unit_times();
    vector<search_phone_st> result;
     
    int ret = 0;
    int total_cnt = 0;
    int show_cnt = 0;
    //int logid = rand();
    //���������Լ���
    if( unit_times < 1 ){
        return 0;
    }
    if( unit_num < 1 ){
        return 0;
    }
    //����������������л�������֮��
    total_cnt = get_phone_search_cnt_by_key(key);
    UB_LOG_NOTICE("total_cnt is %d",total_cnt);

//    if( 0 >= total_cnt ){
//        return 0;
//    }
    //�������ҳ��ʾ���ٸ�
//    show_cnt = get_cnt_this_page(total_cnt,unit_times,unit_num);
    //�������������ʼindex ��0��ʼ
//    int start_index = (unit_times - 1) * unit_num;
    
    //����ռ�
//    search_phone_st *result = (search_phone_st *) malloc(sizeof(search_phone_st) * total_cnt);
//    if( NULL == result ){
//        return -1;
//    }
    //��ʼ���ڴ�
//    memset(result,0,sizeof(search_phone_st) * total_cnt);
    ret = get_phone_search_list_by_key(key,total_cnt,result);
    //���÷���ֵ

    int start_index = (unit_times - 1) * unit_num;
    show_cnt = get_cnt_this_page(ret,unit_times,unit_num);
    //����
    out.m_result_params()->set_phone_count(ret);
    //����ÿһҳ
    UB_LOG_NOTICE("show_cnt is %d, start_index is %d, ret is %d", show_cnt, start_index, ret);
    UB_LOG_NOTICE("result size is %d",(int)result.size());
     for(int i = 0 ; i < show_cnt && start_index < ret; i++,start_index++){
        UB_LOG_NOTICE("start_index is %d",start_index);
    	out.m_result_params()->set_phone_name(i,result.at(start_index).phone_name);
    	out.m_result_params()->set_phone_id(i,result.at(start_index).phone_id);
    	out.m_result_params()->set_brand_id(i,result.at(start_index).brand_id);
    }
    //�ͷſռ�
//    free(result);
    
    return MCP_OK;
}

