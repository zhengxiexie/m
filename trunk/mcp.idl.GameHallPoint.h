//Generated by ubrpcgen
//This file is generated automatically from idl file
//Do not edit this file, since any modification will be lost when this
//file is regenerated
#ifndef __MCP_IDL_GAMEHALLPOINT_H_
#define __MCP_IDL_GAMEHALLPOINT_H_
#include "mcp.idl.h"
#include "ubrpc/servlet/servlet.h"
namespace idl
{
	class GameHallPoint_servlet_trunk : public ubrpc::Servlet
	{
	protected:
		static const char * const ERROR_MSG [];
		/**
		 * @brief
		 * ���ó�����Ϣ�������ΪERROR_������dataΪ�û��Զ����ַ���
		**/
		int setError(unsigned int err_no, const char * data);
		/**
		 * @brief
		 * rpc���õĲο�ǩ�����û�ʵ�������б��������д�������
		**/
		virtual int addGameHallScore(const GameHallPoint_addGameHallScore_params &, GameHallPoint_addGameHallScore_response &) = 0;
		/**
		 * @brief
		 * rpc��ת��
		**/
		virtual int dispatch(int64_t id, const char * function, const mc_pack_t * param, mc_pack_t * result);
	};
}
#endif //__MCP_IDL_GAMEHALLPOINT_H_