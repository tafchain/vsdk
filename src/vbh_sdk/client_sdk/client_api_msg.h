#ifndef CLIENT_API_MSG_H_221F987CAF6911E9A08860F18A3A20D1
#define CLIENT_API_MSG_H_221F987CAF6911E9A08860F18A3A20D1

#include <stdint.h>

namespace VBH_SDK
{
	//�����û�����Կ��key-gen���������û��Ĺ�Կ��˽Կ��ע��ɹ��󣬷���һ������˹�Կ��Ϣ���û���ʹ��key-merge���ߣ�������˹�Կ+�ͻ���˽Կmerge�ɿͻ���Կ�ļ�
	// ע���û��������Ӧ
	class CRegistUserReq
	{
	public:
		uint32_t m_nChannelID;
		uint32_t m_nSessionID; //���������ID���������ϲ�ҵ���sessionID

		char* m_pUserInfo; //�ύ��CC���û��ο���Ϣ����CC���û������ر���
		uint32_t m_nUserInfoLen;

		char* m_pCltPubKey;//�ͻ��˹�Կ //Hex�����ʽ
		uint32_t m_nCltPubKeyLen;
	};

	class CRegistUserRsp
	{
	public:
		int32_t m_nReturnCode;
		uint32_t m_nSessionID; //���������ID���������ϲ�ҵ���sessionID

		char* m_pVbhCryptKey;
		uint32_t m_nVbhCryptKeyLen; //hex����

		char* m_pVbhUserID;
		uint32_t m_nVbhUserIDLen; //hex����
	};

	//�����᰸�������Ӧ //�ݲ������û����᰸�漰������ͬһchannel�ĳ���
	class CProposeReq
	{
	public:
		uint32_t m_nActionID; 
		uint32_t m_nSessionID; //���������ID���������ϲ�ҵ���sessionID

		char* m_pProposal;
		uint32_t m_nProposalLen;//�᰸

		char* m_pUserCryptKey;
		uint32_t m_nUserCryptKeyLen; //base64����
	};

	class CProposeRsp
	{
	public:
		uint32_t m_nActionID;
		int32_t m_nReturnCode;
		uint32_t m_nSrcSessionID; //���������ID���������ϲ�ҵ���sessionID

		//CC��д�˲�����
		char* m_pReceipt;
		uint32_t m_nReceiptLen; //ccΪ�᰸���ɵĻ�ִ���᰸�ɹ�ִ�к󣬷��ص��ͻ��ˣ�����Ϣ������

		char* m_pTransKey;		
		uint32_t m_nTransKeyLen; //�ɹ�ʱ��������transaction-uuid
		char *m_pInfoID;
		uint32_t m_nInfoIdLen;
	};

	using CCreateInformationReq = CProposeReq;

	class CCreateInformationRsp
	{
	public:
		int32_t m_nReturnCode;
		uint32_t m_nSrcSessionID; //���������ID���������ϲ�ҵ���sessionID

		//������information-id�б�
	};

	//ϵͳ�в�����ֱ�Ӳ�ѯ�����еĲ�ѯ����CC
	class CQueryReq
	{
	public:
		uint16_t m_nDstPeerID; //Ҫ��ѯ��peer-id
		uint32_t m_nActionID;
		uint32_t m_nSrcSessionID; //���������ID���������ϲ�ҵ���sessionID

		char* m_pParam;
		uint32_t m_nParamLen; //��ѯ����������CC������action-id����

		char* m_pUserCryptKey;
		uint32_t m_nUserCryptKeyLen; // base64����
	};

	class CQueryRsp
	{
	public:
		int32_t m_nReturnCode;
		uint32_t m_nActionID; //��ѯʱ��action-id
		uint32_t m_nSrcSessionID; //���������ID���������ϲ�ҵ���sessionID

		char* m_pInfo;
		uint32_t m_nInfoLen; //��ѯ���ý��
	};

	//��ѯ�����������Ӧ
	class CGetTransactionInfoReq
	{
	public:
		uint32_t m_nSrcSessionID; //���������ID���������ϲ�ҵ���sessionID

		char* m_pUserCryptKey;
		uint32_t m_nUserCryptKeyLen;

		char* m_pTransKey;
		uint32_t m_nTransKeyLen; //Ҫ��ѯ��transaction-uuid
	};

	class CGetTransactionInfoRsp
	{
	public:
		int32_t m_nReturnCode;
		uint32_t m_nSrcSessionID; //���������ID���������ϲ�ҵ���sessionID

		char* m_pUserKey;
		uint32_t m_nUserKeyLen; //��ѯ��������

		char* m_pSignature;
		uint32_t m_nSignatureLen; //��ѯ��������

		char* m_pProposal;
		uint32_t m_nProposalLen; //��ѯ��������
	};

	//�������ѯӦ��-��ѯ������ϢӦ��
	class CExplorerQueryBlockInfoRsp
	{
	public:
		int32_t m_nReturnCode;
		uint32_t m_nSrcSessionID;

		char* m_pJsonBlockInfo;
		uint32_t m_nJsonBlockInfo;
	};

	//�������ѯӦ��-��ѯ��������
	class CExplorerQueryBlockCountRsp
	{
	public:
		int32_t m_nReturnCode;
		uint32_t m_nSrcSessionID;

		uint64_t m_nBlockCount;
	};

	//�������ѯӦ��-��ѯ����hash
	class CExplorerQueryBlockHashRsp
	{
	public:
		int32_t m_nReturnCode;
		uint32_t m_nSrcSessionID;

		char* m_pBlockHash;
		uint32_t m_nBlockHashLen;
	};

	//�������ѯӦ��-��ѯ�û���Ϣ
	class CExplorerQueryUserRsp
	{
	public:
		int32_t m_nReturnCode;
		uint32_t m_nSrcSessionID;

		char* m_pUserInfo;
		uint32_t m_nUserInfoLen;
	};

	class CExplorerQueryTransRsp
	{
	public:
		int32_t m_nReturnCode;
		uint32_t m_nSrcSessionID;

		char* m_pTransInfo;
		uint32_t m_nTransInfoLen;
	};

	//�������ѯӦ��-��ѯ������ϢӦ��
	class CExplorerQueryBlockTransListRsp
	{
	public:
		int32_t m_nReturnCode;
		uint32_t m_nSrcSessionID;

		char* m_pJsonBlockTransList;
		uint32_t m_nJsonBlockTransListLen;
	};

}

#endif
