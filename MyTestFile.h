/////////////////////////////////////////////////////////////////////////////
//MyTestFile.h
//////////////////////////////////////////////////////////////////////

#ifndef T_MY_TEST_FILE_H
#define T_MY_TEST_FILE_H

#include "MListStrStr.h"
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
class MyTestFile
{
private:
	MySettings*    m_pSettings;
	MyLanguage*    m_pLanguage;
	TArchive*      m_pArchData;
	TypeOfArchive* m_pArchiveType;

	DataForArchive m_dfa;
	DataForThread  m_dft;

	int TestAce        ();
	int TestArcPak     ();
	int TestArj        ();
	int TestBh         ();
	int TestBZip2Tbz2  ();
	int TestCab        ();
	int TestCpio       ();
	int TestDwc        ();
	int TestGZipTgz    ();
	int TestHa         ();
	int TestLhaLzh     ();
	int TestPpm        ();
	int TestRar        ();
	int TestSqz        ();
	int TestTar        ();
	int TestZ          ();
	int TestZipJar     ();
	int TestZoo        ();
	int TestWcxPluginTC(char*);

public:
	MyTestFile(): m_pArchData(0) {;}
	~MyTestFile() {;}

	TArchive* TestArchive  (MySettings*, MyLanguage*, TypeOfArchive*, char*, char*);
};

#endif //T_MY_TEST_FILE_H
