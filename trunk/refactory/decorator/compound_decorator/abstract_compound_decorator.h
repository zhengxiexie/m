/*
 * abstract_compound_decorator.h
 *
 *  Created on: 2013-4-26
 *      Author: guoweijiang
 */

#ifndef ABSTRACT_COMPOUND_DECORATOR_H_
#define ABSTRACT_COMPOUND_DECORATOR_H_

#include "../single_decorator/abstract_single_decorator.h"

class AbstractCompoundDecorator : public AbstractDecorator
{
protected:

    // ��ѯ����ʱ������������Ƕ�ײ�ѯʱʹ��
    vector<AbstractDecorator*> secondery_table_query;

    // ��ѯ��������ʱ��ҵ������
    vector<AbstractDecorator*> primary_table_query;

public:

    AbstractCompoundDecorator();
    virtual ~AbstractCompoundDecorator();

    void add_primary_table_decorator(AbstractSingleDecorator* decorator);
    void add_secondery_table_decorator(AbstractSingleDecorator* decorator);

	/* add by zhengxie */
	void remove_primary_table_decorator(AbstractSingleDecorator* decorator);
    void remove_secondery_table_decorator(AbstractSingleDecorator* decorator);
	void remove_all_primary_table_decorator();
	void remove_all_secondery_table_decorator();
    /*
     *  һ�����ӿڣ�
     *
     *  һ��򵥵�ҵ���У��򵥵�Ƕ�����в�ѯ��������
     *  һ�����ӿڵ�ʵ���У��Ὣ���в�ѯ�����򵥵�Ƕ����һ��
     *
     *  ��ע�⡿��
     *  ����� is_special_query �߼�ʱ����Ҫ�������߼��� $query Ƕ������
     *
     *  ��������Ҫ���ݾ���ҵ����Ҫ�����ղ�ͬ��Ҫ�������Ӧ������Ƕ��
     */
    virtual int custom_result_set_query(bson* query);

    /*
     *  AbstractCompoundDecorator ���������п���Ҫ���վ����ҵ���߼��ٷֱ�ʵ����������һ�����ӿڣ�
     *  set_id_list_fetching_query ��  set_list_count_query
     *
     *  �ر���  set_id_list_fetching_query, ��Ϊ����漰����Ҫ��ѯ���ű��ʱ��
     *  set_id_list_fetching_query ��  custom_result_set_query ���п�������Ҫ��ͬ�Ĳ�ѯ����
     */
    virtual int set_id_list_fetching_query(bson* query);

    /*
     *  һ�����ӿڣ�
     *
     *  �����ڼ��� count ʱ��֧�� is_special_query(���磺orderby) �߼�
     *  ������ count ʱҪ�ٳ���Щ�߼������������߼�����
     *  ��������Ҫ���ݾ���ҵ����Ҫ�����ղ�ͬ��Ҫ�������Ӧ������Ƕ��
     */
    virtual int get_count_query(bson* query);
};

#endif /* ABSTRACT_COMPOUND_DECORATOR_H_ */
