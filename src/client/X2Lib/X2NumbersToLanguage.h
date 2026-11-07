#pragma once
/**	@file	: 2010/08/31
	@desc	: 숫자가 아닌 다른 형태로 숫자를 표현
	@since	: 31:8:2010   19:41
	@author	: blackhydra
*/

#include <sstream>

using std::string;
using std::stringstream;

/** @class	: CNumbersToLanguage
	@brief	:
	@desc	: 숫자를 입력하면 한글 또는 다른 언어의 형태로 출력하는 클래스이다.
	@date	: 2010/08/31
*/
class CX2NumbersToLanguage
{
public:
	/**
		@brief TranslateLanguage

		변환될 언어 정의
	*/
	enum TypeLanguage
	{
		TL_CHN,
		TL_ENG,
		TL_JPN,
		TL_KOR,
		//#ifdef _LANGUAGE_FIX_CHINESE_
		TL_CHN_SIMPLIFIED,
		//#endif _LANGUAGE_FIX_CHINESE_
	};

	/**
		@brief LanguageData

		변환 데이터
	*/
	struct LanguageData
	{
		string LD_KOR[3][10];
		string LD_CHN[3][10];

		LanguageData()
		{
			/// 한국
			string kor1[10] = { "",	"일", "이", "삼", "사", "오", "육", "칠", "팔", "구" };
			std::copy( std::begin( kor1 ), std::end( kor1 ), LD_KOR[0] );
			string kor2[10] = { "",	"십", "백", "천", "", "", "", "", "", "" };
			std::copy( std::begin( kor2 ), std::end( kor2 ), LD_KOR[1] );
			string kor3[10] = { "", "만","억","조", "경", "해", "자", "양", "구", "간" };
			std::copy( std::begin( kor3 ), std::end( kor3 ), LD_KOR[2] );

			/// 한자
			string chn1[10] = { "",	"一", "二", "三", "四", "五", "六", "七", "八", "九" };
			std::copy( std::begin( chn1 ), std::end( chn1 ), LD_CHN[0] );
			string chn2[10] = { "",	"十", "百", "千", "", "", "", "", "", "" };
			std::copy( std::begin( chn2 ), std::end( chn2 ), LD_CHN[1] );
			string chn3[10] = { "", "万","億","兆", "京", "", "", "", "", "" };
			std::copy( std::begin( chn3 ), std::end( chn3 ), LD_CHN[2] );


		}

	};

public:
	/// 생성자
	CX2NumbersToLanguage(void);
	/// 소멸자
	~CX2NumbersToLanguage(void);

public:
	/// 숫자를 문자열로
	template < class T >
	string ToString(const T& arg)
	{
		stringstream out;

		out.imbue(std::locale("C"));

		out << arg;

		return(out.str());
	};

	/// 문자를 숫자로
	int ToNumbers(const char v) {
		stringstream ss;
		ss << v;
		int ret;
		ss >> ret;
		return ret;
	}

	/// string형을 wstring형으로
	std::wstring
		mbs_to_wcs(std::string const& str, std::locale const& loc = std::locale())
	{
		typedef std::codecvt<wchar_t, char, std::mbstate_t> codecvt_t;
		codecvt_t const& codecvt = std::use_facet<codecvt_t>(loc);
		std::mbstate_t state = std::mbstate_t();
		std::vector<wchar_t> buf(str.size() + 1);
		char const* in_next = str.c_str();
		wchar_t* out_next = &buf[0];
		std::codecvt_base::result r = codecvt.in(state,
			str.c_str(), str.c_str() + str.size(), in_next,
			&buf[0], &buf[0] + buf.size(), out_next);
		if (r == std::codecvt_base::error)
			throw std::runtime_error("can't convert string to wstring");
		return std::wstring(&buf[0]);
	}

	/// wstring형을 string형으로
	std::string
		wcs_to_mbs(std::wstring const& str, std::locale const& loc = std::locale())
	{
		typedef std::codecvt<wchar_t, char, std::mbstate_t> codecvt_t;
		codecvt_t const& codecvt = std::use_facet<codecvt_t>(loc);
		std::mbstate_t state = std::mbstate_t();
		std::vector<char> buf((str.size() + 1) * codecvt.max_length());
		wchar_t const* in_next = str.c_str();
		char* out_next = &buf[0];
		std::codecvt_base::result r = codecvt.out(state,
			str.c_str(), str.c_str() + str.size(), in_next,
			&buf[0], &buf[0] + buf.size(), out_next);
		if (r == std::codecvt_base::error)
			throw std::runtime_error("can't convert wstring to string");
		return std::string(&buf[0]);
	}

	/// 변환 함수
	void TranslateDigits(TypeLanguage, __int64, string&);

	//void TranslateDigits(TypeLanguage, string, string&);

public:
	/// 언어 형태
	TypeLanguage getType(void);

private:
	TypeLanguage	m_eType;			///< 언어 형태
	LanguageData	m_LanguageData;		///< 언어 데이터
};
