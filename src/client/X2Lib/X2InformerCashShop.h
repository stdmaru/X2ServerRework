#pragma once

class CX2InformerCashShop : public CX2Informer
{
	public:
		CX2InformerCashShop(void);
		virtual ~CX2InformerCashShop(void);

	HRESULT OnFrameMove( double fTime, float fElapsedTime );
	void Reset();

	protected:

		void SetNowInfo();
		bool IsNowInfoChange();
		void SetShow( bool bShow );

		bool	m_bCheckFirstConnect;
		float	m_fTimeCheckIsNowInfoChange;
		int		m_NowArrivedPresentNum;

		bool	m_bCanClassChange;
		int		m_LastCheckLevel;
};
