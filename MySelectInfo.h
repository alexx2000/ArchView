/////////////////////////////////////////////////////////////////////////////
//MySelectInfo.h
//////////////////////////////////////////////////////////////////////

#ifndef T_MY_SELECT_INFO_H
#define T_MY_SELECT_INFO_H

#include "MListParametrs.h"
#include "MySettings.h"
#include "MyLanguage.h"

#include "TArchive.h"
#include "TAce.h"
#include "TArcPak.h"
#include "TArj.h"
#include "TBh.h"
#include "TBZip2Tbz2.h"
#include "TCab.h"
#include "TCpio.h"
#include "TDwc.h"
#include "TGZipTgz.h"
#include "THa.h"
#include "TLhaLzh.h"
#include "TPpm.h"
#include "TRar.h"
#include "TSqz.h"
#include "TTar.h"
#include "TZ.h"
#include "TZipJar.h"
#include "TZoo.h"
#include "TWcxPluginTC.h"

//klass dlya testirivaniya faylov
//prosto sobrat' eti funkcii vmeste
class MySelectInfo
{
private:
	//edinici izmereniya informacii
	enum UnitSize
	{
		T_BYTE,		//bayti
		T_KBYTE,	//kilobayti
		T_MBYTE,	//megabayti
		T_GBYTE,	//gigabayti
		T_TBYTE		//terrabayti
	};

	MySettings*     m_pSettings;
	MyLanguage*     m_pLanguage;
	TArchive*       m_pArchData;
	TypeOfArchive*  m_pArchiveType;
	MListParametrs* m_pListParam;
	char**          m_ppTextComment;
	double          m_TestTime;
	int*            m_pRatio1;
	int*            m_pRatio2;

	//vspomogatel'nie funkcii
	char* GetUnitSizeStr               (UnitSize, LanguageOther&);
	int   NumberSeparatorString        (char*, ULONGLONG);
	int   NumberSeparatorStringUnitSize(char*, ULONGLONG, UnitSize, LanguageOther&);
	int   NumberFloatStringUnitSize    (char*, ULONGLONG, UnitSize, LanguageOther&);

	int SelectInfoAce        ();
	int SelectInfoArcPak     ();
	int SelectInfoArj        ();
	int SelectInfoBh         ();
	int SelectInfoBZip2Tbz2  ();
	int SelectInfoCab        ();
	int SelectInfoCpio       ();
	int SelectInfoDwc        ();
	int SelectInfoGZipTgz    ();
	int SelectInfoHa         ();
	int SelectInfoLhaLzh     ();
	int SelectInfoPpm        ();
	int SelectInfoRar        ();
	int SelectInfoSqz        ();
	int SelectInfoTar        ();
	int SelectInfoZ          ();
	int SelectInfoZipJar     ();
	int SelectInfoZoo        ();
	int SelectInfoWcxPluginTC();

public:
	MySelectInfo()  {;}
	~MySelectInfo() {;}

	int SelectInfoArchive(MySettings*, MyLanguage*, TArchive*, TypeOfArchive*, MListParametrs*, char**, double, int*, int*);
};

#endif //T_MY_SELECT_INFO_H
