#ifndef VBH_PERF_TEST_CLIENT_APPMANAGER_H_879756465435167494562
#define VBH_PERF_TEST_CLIENT_APPMANAGER_H_879756465435167494562

#include "dsc/dsc_app_mng.h"

class CPerfTestAppManager : public CDscAppManager
{
public:
	enum
	{
		EN_TRADE_TYPE_TRADE = 1,
		EN_QUERY_TYPE_QUERY_USER ,
		EN_QUERY_TYPE_QUERY_TRADE,
		EN_QUERY_TYPE_QUERY_BLOCK
	};

public:
	ACE_UINT32 GetTestUserNum(void) const;

protected:
	virtual ACE_INT32 OnInit(void);
	virtual void OnParam(const int argc, ACE_TCHAR* argv);

public:
	ACE_UINT32 m_nGroupCount = 0; 
	ACE_UINT32 m_nChannelID = 1;
	ACE_UINT32 m_nTotalTradeCount; 

	bool m_bCryptPath = true; //Ĭ���߼���ͨ��
	//bool m_bTradeTestType = true; // Ĭ�� true ������
	ACE_UINT32 m_nClientType = EN_TRADE_TYPE_TRADE; // �ͻ��˷������ͣ� Ĭ��Ϊ��ͨ����
	ACE_UINT32 m_nLimitTimes = 0; // ÿ��Session�Ĳ�ѯ������Ĭ��Ϊ0������
	ACE_UINT16 m_nPeerID = 1;
	ACE_UINT64 m_nBlockNum = 0;
};

#endif
