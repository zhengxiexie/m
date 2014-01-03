/*
 * input_extractor.h
 *
 *  Created on: 2013-6-18
 *      Author: guoweijiang
 */

#ifndef INPUT_EXTRACTOR_H_
#define INPUT_EXTRACTOR_H_

#include "../common.h"
using namespace std;

/*
 *  �����������д���
 *      KEY_1[XXX,XXX,XXX];KEY_2[XXX,XXX,XXX];
 *
 *  ��飺
 *      ���� KEY_X Ϊɸѡ����������
 *      XXX Ϊɸѡ��ֵ��ͬһ�� KEY_X �ڶ��ֵ֮���� OR �߼�
 *      ��ͬ KEY_X ֮���� AND �߼�
 */
class InputExtractor
{
private:

    /*
     *  ����������������:
     *
     *  KEY   : ������������е� KEY_X
     *  VALUE : ��[]�е��ַ�������  XXX,XXX,XXX
     */
    map<string, string> extracted_params;

    void extract_parameters(string& key_name,
                            string& value_str,
                            string input_param);
public:
    ~InputExtractor();

    /*
     *  ��������� ��KEY_1[XXX,XXX,XXX];KEY_2[XXX,XXX,XXX];
     *  ��񻯱��浽  extracted_params ��
     */
    int parse_input_params(const char* input_param);

    /*
     *  ��ȡָ�� key_name �� XXX,XXX,XXX ֵ
     */
    const char* get_filter_condition(const char* key_name);
};

#endif /* INPUT_EXTRACTOR_H_ */
