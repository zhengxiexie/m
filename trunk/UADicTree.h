/*
 * =====================================================================================
 *
 *       Filename:  UADicTree.h
 *
 *    Description:  ���ŸߵĻ����Ͻ���UA�ֵ�����װ�ɵ�������
 *
 *        Version:  1.0
 *        Created:  08/11/2011 03:09:13 PM
 *       Revision:  none
 *       Compiler:  g++
 *
 *         Author:  RD. Hongming li (lhm), lihongming@baidu.com
 *        Company:  baidu, Beijing, China
 *
 * =====================================================================================
 */

#ifndef UADICTREE_H
#define UADICTREE_H

#include <stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include <time.h>
#include <cctype>
#include <map> 
#include <algorithm>
using namespace std;

typedef struct TrieNode{
    char c; 
    vector<string> id_list;
    map<char ,struct TrieNode*> Next;
}TrieNode;

typedef struct UaJsonNode{ //added by laixiaokang 2013.09.03
    string fixedSeg; //����ֶβ�û����;
    string uaJson;
    map<string ,struct UaJsonNode*> Next; //�ǵ�ua��Ϣ�ܳ������̶���С���ַ���ƥ��(����ʵ�����ݣ�4λ�Ǻ��ʵĳ��ȣ��������ظ���Ƶ���½�)
}UaJsonNode;


class UADicTree{

public:
    UADicTree(){ 
        this->createTime = 0;
        //��ʼ���̻߳�����
        pthread_mutex_init(&locker, NULL);
    };
    ~UADicTree(){
        pthread_mutex_destroy(&locker);
    };
    int isTimeout(long expired);
    int Search(string key,vector<string> *id);
    int create_trie_list();  
    void delete_trie();
    void updateCreateTime();
private:
    void delete_trie_list(TrieNode* trie);
    // ת��ΪСд��ĸ
    void string_ToLower(string &SrcString);

    void AddAKey(string key, string id);

    time_t createTime; 
    TrieNode trie;
    //add pthread locker by li-yong
    pthread_mutex_t locker;
};

//added by laixiaokang on 2013.09.03
class UAJsonDicTree{

public:
    UAJsonDicTree(){ 
        //��ʼ���̻߳�����
        pthread_mutex_init(&locker, NULL);
        nFixedLen = 4;
    };
    ~UAJsonDicTree(){
        pthread_mutex_destroy(&locker);
    };
    int Search(string ua,string& uaJson);
    int create_uajson_list();  
    // ת��ΪСд��ĸ
    void string_ToLower(string &SrcString);

    void destroyTree(struct UaJsonNode* pNode);

private:
    void AddAKey(string ua, string uaJson);

    UaJsonNode uaJsonNode;
    int nFixedLen;
    //add pthread locker by li-yong
    pthread_mutex_t locker;
};

#endif
