///*
// * compound_binder.h
// *
// *  Created on: 2013-5-28
// *      Author: guoweijiang
// */

#ifndef COMPOUND_BINDER_H_
#define COMPOUND_BINDER_H_

#include "../common.h"
#include "../module/abstract_mcp_content.h"
#include "../factory/compound_binded_factory.h"

/*
 *  ���漰���������ʱ����Ҫ�ø��ϼ�������
 *  ÿ�ű��Ӧһ�� module����ͬ module ֮��ͨ�� key_value ����
 */
class CompoundBindedRecorder
{

friend class CompoundBindedRecordFactory;

public:

private:
    const char* key_value;

    /*
     *  ��� module �ϵ�һ�����ǵ� key_bucket ����ͬ�� key_value
     *  ����ͨ�������ͬ�� key_value ���жϲ�ͬ module �е��ĸ� key_bucket �ϵ�һ��
     *
     *  ��ע�⡿
     *   module_records ��ֻ�������ݵ� module��û��ռλ module
     */
    vector<KeyBucket*> module_records;

    /*
     *  ר�ż�¼ join ������ module ���ĸ���ռλ�����ݣ��ĸ���ʵ��������
     *  1: �������ݵ� module
     *  0: û�����ݵ�ռλ�� module
     *  �� CompoundBindedRecorder::add_key_bucket �е�ʹ��
     *
     *  ��ע�⡿
     *   modules_info �а����������� module ��ռλ�� module
     */
    vector<char> modules_info;

    /*
     *  �ж���������� module �Ƿ�Ϊ������ֻռλ�� module
     *
     *  out_mod_index ���������
     *  �� module_index ���ǿ�ռλ module ��modules_info �а����˿�ռλ module ʱ
     *  out_mod_index �������� module �� module_records ������������ֵ
     */
    bool is_right_module_index(int module_index, int* out_mod_index);
public:

    static const char* ClassName;

    CompoundBindedRecorder()
    {
        this->key_value = "";
    }

    CompoundBindedRecorder(const char* key)
    {
        this->key_value = key;
    }

    ~CompoundBindedRecorder()
    {
        this->clear();
    }

    void clear()
    {
        /*
         *   module_records �е�Ԫ��
         *   Ҳ�Ƕ�����з�������ģ����Դ˴����Ը���Ԫ�����κδ���
         *   �ȴ�������Ӧ������Լ�������
         *   �˴�ֻ���� vector ������
         */
        this->key_value = "";
        this->module_records.clear();
        this->modules_info.clear();
    }

    /*
     *  ��ȡ�����е����ݣ�
     *
     *  module_index : �ϼ��еڼ��� module
     *  vec_index    : ָ�� module �� key_bucket ������
     */
    AbstractMCPRecord* get_record(int module_index, int vec_index);

    /*
     *  ��ȡ����һ���ĳһ�� module �� KeyBucket �Ĵ�С
     */
    int get_key_bucket_size(int module_index);

    /*
     *  �Ӳ�ͬ module �����Ӿ�����ͬ key_value �� key_bucket
     */
    int add_key_bucket(KeyBucket* records);

    /*
     *  ��ȡ KEY ����ֵ
     */
    const char* get_key_value();
};

class CompoundBinder
{
protected:

    vector<CompoundBindedRecorder*> binded_result;

public:

    CompoundBinder();
    virtual ~CompoundBinder();

    /*
     *  ��ȡ�����еĽ������
     */
    int get_result_count();

    /*
     *  ��ȡ������ĳһ�� module �� KeyBucket �Ĵ�С
     */
    int get_bucket_size(int k_index,
                        int module_index);

    /*
     *  ��ȡ������ĳһ�� module �� KeyBucket ��ָ����һ����¼
     */
    AbstractMCPRecord* get_record_value(int k_index,
                                        int module_index,
                                        int vec_index);

    /*
     *  ���������е� module
     */
    void left_join(AbstractMCPContent* joint_module);
};

#endif /* COMPOUND_BINDER_H_ */
