/*
 * =====================================================================================
 *
 *       Filename:  tools.cpp
 *
 *    Description:  ���÷���ʵ���ļ�
 *
 *        Version:  1.0
 *        Created:  08/12/2011 04:01:19 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  RD. Hongming li (lhm), lihongming@baidu.com
 *        Company:  baidu, Beijing, China
 *
 * =====================================================================================
 */

#include <string.h>
#include <stdlib.h>

void copy_result(char* dest,char* src){
        if(NULL == dest){
            return;
        }
        if(NULL == src){
            strcpy(dest,"");
        }else{
            strcpy(dest,src);
        }
}

int get_cnt_this_page(int total_cnt,int page,int cnt_per_page){
    if( total_cnt <= 0 ){
        return 0;
    }
    if(page <= 0 || cnt_per_page <= 0 ){
        return 0;
    }

    int start_index = (page-1) * cnt_per_page;
    int end_index = page * cnt_per_page - 1;

    //������Χ
    if( start_index >= total_cnt ){
        return 0;
    }
    //��Ч�����ҳ 
    if(end_index >= total_cnt){
        return total_cnt % cnt_per_page;
    }
    //�м�ҳ
    return cnt_per_page;

}
