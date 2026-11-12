#include "stdafx.h"
#include "./X2NumbersToLanguage.h"

/** @function	: CNumbersToLanguage
	@brief		: 생성자
    @author		: JHKang
	@date		: 2010/08/31
*/
CX2NumbersToLanguage::CX2NumbersToLanguage(void):
m_eType(TL_KOR),
m_LanguageData()
{
}

/** @function	: ~CNumbersToLanguage
	@brief		: 소멸자
    @author		: JHKang
	@date		: 2010/08/31
*/
CX2NumbersToLanguage::~CX2NumbersToLanguage(void)
{
}

/** @function	: TranslateDigits
	@brief		: 숫자를 언어로(IN unsigned long)
    @author		: JHKang
	@date		: 2010/08/31
*/
void CX2NumbersToLanguage::TranslateDigits(TypeLanguage eType, __int64 i64lNumbers, string& strMsg)
{
	string InData = ToString(i64lNumbers);
	int length = static_cast<int>(InData.length());
	int iCount = 0;

	string (*Lang)[10] = NULL;

	switch (eType)
	{
	case TL_KOR:
		Lang = m_LanguageData.LD_KOR;
		break;
	case TL_CHN:
		Lang = m_LanguageData.LD_CHN;
		break;
	default:
		return;
	}

	while (length != 0)
	{
		/// 4자릿수로 끊기
		int num1 = length / 4;
		int modNum = length % 4;

		if (num1 == 0 && modNum == 0)
		{
			strMsg += "영";
			break;
		}

		if (modNum < 1)
			num1--;

		/// 4자릿수로 끊은 루프
		for (int i = num1; i >= 0; i--)
		{
			int num2 = length % 4;
			int temp = num2;
			int digitTotal = 0;

			if (num2 == 0)
				num2 = temp = 4;

			for (int j = 0; j < num2; j++)
			{
				int digit = ToNumbers(InData[iCount]);
				digitTotal += digit;

				temp--;

				if (digit != 0)
					strMsg += Lang[0][digit] + Lang[1][temp];
				else
					strMsg += "";

				length--;
				iCount++;
			}
			if (digitTotal > 0)
				strMsg += Lang[2][i];
		}
	}
}
