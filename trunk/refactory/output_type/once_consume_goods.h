/*
 * once_consume_goods.h
 *
 *  Created on: 2013-5-9
 *      Author: guoweijiang
 */

#ifndef ONCE_CONSUME_GOODS_H_
#define ONCE_CONSUME_GOODS_H_

#include "common_header.h"

/*
 *  һ���Թ�����Ʒ
orderid     string  ������
itemid      string  ��ƷID
ordertime   string  ��������ʱ��
udid        string  �û��豸��ʶ
 */
class once_consume_goods_t
{
public:
    static void set_once_goods_out(idl::once_consume_goods_t* out_parameter,
                                   MysqlTable& result_set,
                                   int row_index);

    static unsigned int set_once_goods_out(idl::vector<idl::once_consume_goods_t>* out_parameter,
                                           MysqlTable& result_set);
};

#endif /* ONCE_CONSUME_GOODS_H_ */
