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
#include <iostream>
#include <algorithm>
#include <string>
#include <vector>
#include "mcp_session.h"
#include "mcp.idl.mcp.h"
#include "mcpsvr.h"
#include "tools.h"

#define PHONE_HEAD_WORD_ST_TIMEOUT 100
#define PHONE_HEAD_WORD_ST_EXPIRED 60*60*24

typedef struct {
    char head_word[256];
    int total;
}phone_head_word_st;

//����phone_head_word_st�е�total�ֶ�����
int compar(const void *arg1, const void *arg2){
	phone_head_word_st *a = (phone_head_word_st *)arg1;
	phone_head_word_st *b = (phone_head_word_st *)arg2;
	
	return a->total - b->total;
}

/**
  *@brief ���ݳ���ID��ø��ֻ��������в�ͬ��ͷ��ĸ����(CACHE)
  *
  *@param [in]  brand_id          ָ������ID
  *
  * @return    int                ���ֻ��������в�ͬ��ͷ��ĸ����,С��0ʧ��
  */
int get_phone_headword_cnt_by_brand_id_fromCache(string brand_id)
{
    int ret = 0;
    string  valueout("0");
    //��KEY
    string mainkey = "brand_head_word_info";
    string subkey = brand_id+"_count";
    
    mcp_session conn;
    ret = conn.session_search( mainkey, subkey, valueout );
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
        UB_LOG_NOTICE("[ERROR IN REDIS] %s MAINKEY %s SUBKEY %s ",__func__,mainkey.c_str(), subkey.c_str() );
        return -1;
    }
}
/**
  *@brief ���볧��ID�������в�ͬ��ͷ��ĸ����(CACHE)
  *
  *@param [in]  brand_id          ָ������ID
  *@param [in]  cnt               ���ֻ��������в�ͬ��ͷ��ĸ����
  *
  *@return int  0 �ɹ� -1 ʧ��
  */
int insert_phone_headword_cnt_by_brand_id_toCache(string brand_id,int cnt)
{
    int ret = 0;
    if( cnt < 0 ){
        return -1;
    }
    //��KEY
    string mainkey = "brand_head_word_info";
    //��KEY
    string subkey = brand_id+"_count";
    //����Ҫ�����ֵ
    char num[16];
    sprintf(num, "%d", cnt);
    string value = num;

    mcp_session conn;
    //��ɾ��
    ret = conn.session_delete( mainkey, subkey);
    if (0 != ret)
    {
        UB_LOG_NOTICE("[DELETE SESSION ERROR] %s MAINKEY %s SUBKEY %s",__func__,mainkey.c_str(), subkey.c_str() );
        return -1;
    }
    //����
    ret = conn.session_insert(mainkey, subkey, value, PHONE_HEAD_WORD_ST_EXPIRED );
    if (0 != ret)
    {
        UB_LOG_NOTICE("[INSERT REDIS ERROR] %s MAINKEY %s SUBKEY %s",__func__, mainkey.c_str(), subkey.c_str() );
        return -1;
    }
    UB_LOG_NOTICE("[INSERT REDIS SUCCESS] %s MAINKEY %s SUBKEY %s",__func__, mainkey.c_str(), subkey.c_str() );
    return 0;
}
/**
  *@brief  ���ݳ���ID��ø��ֻ��������в�ͬ��ͷ��ĸ����(DB)
  *
  *@param [in]  brand_id          ָ������ID
  *
  * @return    int                ���ֻ��������в�ͬ��ͷ��ĸ����,С��0ʧ��
  */
int get_phone_headword_cnt_by_brand_id_fromDB(string brand_id){

    MysqlConnection *database_connection;
    string sql_count = "SELECT count(*) FROM `mcp_phone_brand_sw` WHERE `mcp_phone_brand_sw`.`brand_id`  =  " + brand_id + " ";

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
  *@brief  ���ݳ���ID��ø��ֻ��������в�ͬ��ͷ��ĸ����
  *
  *@param [in]  brand_id          ָ������ID
  *
  * @return    int                ���ֻ��������в�ͬ��ͷ��ĸ����,С��0ʧ��
  */
int get_phone_headword_cnt_by_brand_id(string brand_id){
    int cache_flag = 0;
    int cnt = 0;
#ifndef REAL_TIME_DATA
    //Search from cache
    cnt = get_phone_headword_cnt_by_brand_id_fromCache(brand_id);
    if(cnt < 0){
        //not in cache
        cache_flag = 0;
    }else{
       //in cache
       cache_flag = 1;
       UB_LOG_NOTICE("[GET PHONE_HEADWORD_CNT  OK] %s brand_id = %s CNT = %d", __func__,brand_id.c_str(),cnt);
    }
#endif
    if( 0 == cache_flag ){
        //not in cache or doese use cache
        cnt = get_phone_headword_cnt_by_brand_id_fromDB(brand_id);
        if( cnt < 0 ){
            //DB query error
            UB_LOG_NOTICE("[GET PHONE_HEADWORD_CNT  ERROR] %s brand_id = %s ", __func__,brand_id.c_str());
        }else{
            //DB query OK
            UB_LOG_NOTICE("[GET PHONE_HEADWORD_CNT  OK] %s brand_id = %s CNT = %d", __func__,brand_id.c_str(),cnt);
#ifndef REAL_TIME_DATA
            //insert into cache
            insert_phone_headword_cnt_by_brand_id_toCache(brand_id,cnt);
#endif
        }
    }
    return cnt;
}





/**
  *@brief  ���ݳ���ID��ø��ֻ��������в�ͬ��ͷ��ĸ�б�(CAHCE)
  *
  *@param [in]  brand_id          ָ������ID
  *@param [in]  cnt               ���ֻ��������в�ͬ��ͷ��ĸ����
  *@param [out] result            ��������ĸ��Ϣ�ṹ��
  *
  * @return    int                0 �ɹ�, С�� 0 ʧ��
  */
int get_phone_headword_list_by_brand_id_fromCache(string brand_id,int cnt,phone_head_word_st *result)
{
    int ret = 0;
    //cache��Ϣ
    if( NULL == result || cnt < 0 ){
        return -1;
    }
    //��KEY
    string mainkey = "brand_head_word_info";
    //��KEY
    string subkey = brand_id;
    //���÷���ֵ��С
    string valueout;

    mcp_session conn;
    ret = conn.session_search( mainkey, subkey, valueout );
    if ( valueout.length() <= 0 )
    {
        UB_LOG_DEBUG("[NOT IN REDIS] %s MAINKEY %s SUBKEY %s",__func__, mainkey.c_str(), subkey.c_str() );
        return -1;
    }
    else if ( 0 == ret )
    {
        UB_LOG_DEBUG("[FOUND IN REDIS] %s MAINKEY %s SUBKEY %s",__func__, mainkey.c_str(), subkey.c_str() );
        UB_LOG_DEBUG("valueout[%s]", valueout.c_str());
    }
    else
    {
        UB_LOG_NOTICE("[ERROR IN REDIS] %s MAINKEY %s SUBKEY %s",__func__, mainkey.c_str(), subkey.c_str() );
        return -1;
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
	  strlcpy( result[k].head_word, json_value["head_word"].asString().c_str(), sizeof(result[k].head_word) );
	  result[k].total = atoi( json_value["total"].asString().c_str() );
       }
       k++;
    }
    return cnt;
}

/**
  *@brief ���볧�����в�ͬ��ͷ��ĸ�б�
  *
  *@param [in]  brand_id          ָ������ID
  *@param [in]  cnt               ���ֻ��������в�ͬ��ͷ��ĸ����
  *@param [in] result             ��������ĸ��Ϣ�ṹ��
  *
  *@return int  									0 �ɹ� -1 ʧ��
  */
int insert_phone_headword_list_by_brand_id_toCache(string brand_id,int cnt,phone_head_word_st *result)
{
    int ret = 0;
    //cache��Ϣ
    if( NULL == result || cnt < 0 )
    {
        return -1;
    }
    //��KEY
    string mainkey = "brand_head_word_info";
    //��KEY
    string subkey = brand_id;
    //����Ҫ�����ֵ
    string value;
    string head_word;
    string total;
    char num[16];
    for(int i=0; i<cnt; i++)
    {
       memset(num, 0x00, sizeof(num));
       head_word.clear(); head_word = result[i].head_word;
       total.clear();
       sprintf(num, "%d", result[i].total);
       total = num;
       value += "{\"head_word\":\""+ head_word + "\",\"total\":\"" + total +"\"}";
    }
    mcp_session conn;
    //��ɾ��
    ret = conn.session_delete( mainkey, subkey);
    if (0 != ret)
    {
        UB_LOG_NOTICE("[DELETE REDIS ERROR] %s MAINKEY %s SUBKEY %s",__func__,mainkey.c_str(), subkey.c_str() );
        return -1;
    }
    //����
    ret = conn.session_insert( mainkey, subkey, value, PHONE_HEAD_WORD_ST_EXPIRED );
    if (0 != ret)
    {
        UB_LOG_NOTICE("[INSERT REDIS ERROR] %s MAINKEY %s SUBKEY %s ",__func__,mainkey.c_str(), subkey.c_str() );
        return -1;
    }
    UB_LOG_NOTICE("[INSERT REDIS SUCCESS] %s MAINKEY %s SUBKEY %s",__func__,mainkey.c_str(), subkey.c_str() );
    return 0;
}

/**
  *����brand_id����ֻ�ϵ����Ϣ(DB)
  *
  *@param [in]  brand_id               ָ���ֻ���brand_id
  *@param [in]  cnt               ���ֻ���Ӧ��ϵ������
  *@param [out] result            ָ���ֻ���ϵ����Ϣ�ṹ��
  *
  * @return    int         0 �ɹ�, С�� 0 ʧ��
  *
  *@return int                  ���͸�����С��0��ʾ��ѯ����
  */
int get_phone_headword_list_by_brand_id_fromDB(string brand_id,int cnt, phone_head_word_st* result){

    UB_LOG_NOTICE("[Start Search Series From DB] %s : brand_id = %s ", __func__,brand_id.c_str());
    int err = 0;
    int ret = 0;
    MysqlConnection *database_connection;
    MyclientRes data_res;
    string sql =  "SELECT  `mcp_phone_brand_sw`.`startwith`,`mcp_phone_brand_sw`.`total`  FROM  `mcp_phone_brand_sw` WHERE  `mcp_phone_brand_sw`.`brand_id` = " + brand_id;

    //�õ�connection����
    database_connection = database_pool->fetchConnection("default_mysql", &err, 0);
    if ( NULL == database_connection )
    {
        UB_LOG_NOTICE("[Search Series From DB Connect ERROR] %s : brand_id = %s ", __func__,brand_id.c_str());
        data_res.free();
        return -1;
    }

    UB_LOG_DEBUG("%s { %s }",__func__,sql.c_str());
    ret = database_connection->query(sql.c_str(), &data_res, true);
    
    if ( ret < 0 ){
        UB_LOG_NOTICE("[Search Series From DB ERROR] %s : brand_id = %s ", __func__,brand_id.c_str());
        ret = database_pool->putBackConnection(database_connection, false);
        if (ret != 0){
            UB_LOG_NOTICE("[Search Series PutBack Connection ERROR] %s : brand_id = %s ", __func__,brand_id.c_str());
        }
        data_res.free();
        return -1;
    }
    
    //������ɢ����ĸ
    phone_head_word_st *tmp = (phone_head_word_st *)malloc(sizeof(phone_head_word_st) * ret);
    phone_head_word_st *tmp_result = (phone_head_word_st *)malloc(sizeof(phone_head_word_st) * ret);
    
    for ( int i = 0; i < ret ; i++){    
//      UB_LOG_NOTICE("[Saving Reuslt From DB INTO result] index = %d function %s id = %s name = %s brand_name = %s  ",i,__func__,data_res[i][0],data_res[i][1],data_res[i][2]);
        copy_result(tmp[i].head_word,data_res[i][0]);
        tmp[i].total = atoi(data_res[i][1]);
    }
    //����total������С��������
    qsort(tmp,ret,sizeof(phone_head_word_st),compar);
    //���ݹ���ϲ�����
    int group_cnt = 0;
    int combine_total = 5;
    int combine_group_cnt = 0;
    int max_group_items_cnt = 4;
    //�µ�ÿ����Ϣ
    //������
    string group_name("");
   	//���������
    int group_total = 0;
    
    int i = 0;
    //ѭ��ÿ��Ԫ��
    for( ; i < ret; i++){
    	//����С�ںϲ�Ҫ��
    	if(tmp[i].total <= combine_total){
    		//����ϲ���С�����Ҫ��
    		if( combine_group_cnt < max_group_items_cnt){
    			//�ϲ�
    			group_name = group_name + tmp[i].head_word + "/";
    			group_total = group_total + tmp[i].total;
    			combine_group_cnt++;
    		}else{
    			strcpy(tmp_result[group_cnt].head_word,group_name.c_str());
    			tmp_result[group_cnt].total = group_total;
    			group_name.clear();
    			group_name = group_name + tmp[i].head_word + "/";
    			group_total = tmp[i].total;
    			combine_group_cnt = 1;
    			group_cnt++;
    		}
    	}else{
    		break;
    	}
    }
    //���ڲ���max_group_items_cnt����û����
    if(combine_group_cnt > 0){
    	strcpy(tmp_result[group_cnt].head_word,group_name.c_str());
    	tmp_result[group_cnt].total = group_total;
    	group_cnt++;
    }
    //�����д���combine_total����׷�ӵ�ĩβ
    for( ;i < ret; i++){
    	strcpy(tmp_result[group_cnt].head_word,tmp[i].head_word);
    	tmp_result[group_cnt].total = tmp[i].total;
    	group_cnt++;
    }
    
    //�鿴Ҫ���صĽ���ռ��Ƿ��㹻��
    if( group_cnt > cnt ){
    	group_cnt = cnt;
    }
    //��������result
    memcpy(result,tmp_result,sizeof(phone_head_word_st)* cnt);
    free(tmp);
    free(tmp_result);
    UB_LOG_DEBUG("[Save Reuslt From DB INTO result (OK)] %s : brand_id = %s ", __func__,brand_id.c_str());
    //�ͷ���Դ
    database_pool->putBackConnection(database_connection, false);
    data_res.free();
    return group_cnt;
}

/**
  *����brand_id����ֻ�ϵ����Ϣ(DB)
  *
  *@param [in]  brand_id               ָ���ֻ���brand_id
  *@param [in]  cnt               ���ֻ���Ӧ��ϵ������
  *@param [out] result            ָ���ֻ���ϵ����Ϣ�ṹ��
  *
  * @return    int              0 �ɹ�, С�� 0 ʧ��
  *
  *@return int                  ���͸�����С��0��ʾ��ѯ����
  */
int get_phone_headword_list_by_brand_id(string brand_id,int cnt,phone_head_word_st* result){
    int cache_flag = 0;

    int ret = 0;
    if( cnt <= 0 ){
        return -1;
    }
    cache_flag = 0;
#ifndef REAL_TIME_DATA
    ret = get_phone_headword_list_by_brand_id_fromCache(brand_id,cnt,result);
    if(ret < 0){
        //not in cache
        cache_flag = 0;
    }else{
       //in cache
       cache_flag = 1;  
    }
#endif
    if( 0 == cache_flag ){
        ret = get_phone_headword_list_by_brand_id_fromDB(brand_id,cnt,result);
        if( ret < 0 ){
            //DB query error
            UB_LOG_NOTICE("[GET PHONE SERIES LIST ERROR] %s brand_id = %s ", __func__,brand_id.c_str());
        }else{
            //DB query OK
            UB_LOG_NOTICE("[GET PHONE SERIES LIST OK] %s brand_id = %s ", __func__,brand_id.c_str());
#ifndef REAL_TIME_DATA
            //insert into cache
            insert_phone_headword_list_by_brand_id_toCache(brand_id,ret,result);
#endif
        }
    }
    return ret;
}


/*
 *@brief    ����ƽ̨ID��ÿҳ������ҳ�������ֻ�
 *
 *@param    [in]     brand_id            ƽ̨ID
 *@param    [in]     unit_num            ÿҳ����
 *@param    [in]     unit_times          ҳ��
 *@param    [out]    phones_cnt          �ֻ��ܸ���
 *@param    [out]    phones_id           ��ҳ�������ֻ�ID
 *@param    [out]    phones_name         ��ҳ�������ֻ�����
 */
int get_phone_headword_by_brand_id_per_page(string brand_id,int unit_num,int unit_times,int* phones_cnt,char** head_word){
    int ret = 0;
    int total_cnt = 0;
    int index = 0;
    int i = 0;
    //���������Լ���
    if( unit_times < 1 ){
        unit_times = 1;
    }
    if( unit_num <= 0 ){
        unit_num = 10;
    }
    //����������������л�������֮��
    total_cnt = get_phone_headword_cnt_by_brand_id(brand_id);
    if( 0 >= total_cnt ){
        *phones_cnt = 0;
        return 0;
    }
    //���ҳ ҳ������ֵ
    int last_valide_page = 0;
    if( 0 == (total_cnt % unit_num)){
        last_valide_page = total_cnt / unit_num;
    }
    else{
        last_valide_page = total_cnt / unit_num + 1;
    }
    if(unit_times > last_valide_page){
        unit_times = last_valide_page;
    }
    //�������������ʼ �� ���� index ��0��ʼ
    int start_index = (unit_times - 1) * unit_num;
    //end_index = last_show_index + 1
    int end_index = unit_num * unit_times;
    if(end_index > total_cnt){
        end_index = total_cnt;
    }
    
    //����ռ�
    phone_head_word_st *result = (phone_head_word_st *) malloc(sizeof(phone_head_word_st) * total_cnt);
    if( NULL == result ){
        return -1;
    }
    //��ʼ���ڴ�
    memset(result,0,sizeof(phone_head_word_st) * total_cnt);
    
    ret = get_phone_headword_list_by_brand_id(brand_id,total_cnt,result);
    //�ϲ��������
    *phones_cnt = ret;
#ifndef REAL_TIME_DATA
    insert_phone_headword_cnt_by_brand_id_toCache(brand_id,ret);
#endif
    UB_LOG_NOTICE("[GET PHONES BY PLAT_ID OK %s]", __func__);
    UB_LOG_NOTICE("%s unit_num %d unit_times %d start_index %d end_index %d", __func__,unit_num,unit_times,start_index,end_index);
    //����ÿһҳ
    index = start_index;
    for( i = 0 ; index < end_index && i < unit_num ; index++,i++){
        //�ϳ��ֻ��� ����-�ֻ��ͺ�
        //copy_result(head_word[i],result[index].head_word);
        size_t len = strlen(result[index].head_word);
        strncpy(head_word[i],result[index].head_word,len);
        head_word[i][255] = '\0';
    }
    //�ͷſռ�
    free(result);
    return 0;
}

int McpServlet::get_phone_headword_from_maker(const idl::mcp_get_phone_headword_from_maker_params& in, idl::mcp_get_phone_headword_from_maker_response& out){

    //�������
    const string brand_id = in.maker_id();
    const uint32_t unit_num  = in.unit_num();
    const uint32_t unit_times = in.unit_times();
    //�������
    int total_cnt = 0;

    uint32_t show_cnt = 0;


    //��ʱ����
    uint32_t i = 0;
    int FULL_NAME_LEN = 256;

    if( unit_num <= 0 || unit_times <= 0){
        return 0;
    }

    //char **head_word = malloc2d(unit_num,FULL_NAME_LEN);
    char **head_word = new char *[unit_num];
    for( i = 0; i < unit_num; ++i )
    {
        head_word[i] = new char[FULL_NAME_LEN];
        memset(head_word[i],0x00,FULL_NAME_LEN);
    }
    if(NULL == head_word){
        return 0;
    }

    //info_id �� ���� ID
    get_phone_headword_by_brand_id_per_page(brand_id,unit_num,unit_times,&total_cnt,head_word);
    //���ؽ��
    out.m_result_params()->set_headword_count(total_cnt);

    show_cnt = get_cnt_this_page(total_cnt,unit_times,unit_num);

    for( i = 0; i < show_cnt; i++){
        out.m_result_params()->set_head_word(i,head_word[i]);
    }

    //�ͷ���Դ
    //free2d(head_word,unit_num); 
    for( i = 0; i < unit_num; ++i )
    {
        delete head_word[i];
    } 
    delete[] head_word;
    
    return MCP_OK;
}

