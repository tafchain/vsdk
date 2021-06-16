#ifndef VBH_PERF_TEST_COMM_MSG_DEF_H_4328792347624323468842382346
#define VBH_PERF_TEST_COMM_MSG_DEF_H_4328792347624323468842382346

#include "dsc/codec/dsc_codec/dsc_codec.h"

#include "vbh_perf_test_comm/vbh_perf_test_comm_id_def.h"

namespace VBH_TEST
{
	class CTradeSumMsg
	{
	public:
		enum
		{
			EN_MSG_ID = VBH_TEST::EN_TRADE_SUM_MSG
		};

	public:
		DSC_BIND_ATTR(m_nTradeSum);

	public:
		ACE_UINT32 m_nTradeSum;
	};

	class CQuerySumMsg
	{
	public:
		enum
		{
			EN_MSG_ID = VBH_TEST::EN_QUERY_SUM_MSG
		};

	public:
		DSC_BIND_ATTR(m_nQuerySum);

	public:
		ACE_UINT32 m_nQuerySum;
	};
}

#endif