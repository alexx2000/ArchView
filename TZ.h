/////////////////////////////////////////////////////////////////////////////
//TZ.h
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TArchive.h"

#ifndef _TZ_CLASS_H
#define _TZ_CLASS_H

class TZ: public TArchive
{
private:
	//zagolovok arhiva
	typedef struct __Z_HEAD
	{	
		BYTE ID1;
		BYTE ID2;
		BYTE ID3;
	} Z_HEAD;

	//strukturi dlya chteniya arhiva
	Z_HEAD m_ARC_HEAD;	//zagolovok arhiva

public:
	TZ(DataForArchive& dfa, LanguageMessages& langmsg, LanguageResults& langres, char* namearch = "Z"): TArchive(dfa, langmsg, langres, namearch) {;}
	~TZ() {;}

	int TestFile        (char*);	//fayl yavlyaetsya arhivom Z
	int AnalyzeInfoOfArc(char*);	//opredelit' parametri (info) arhiva

	ULONGLONG GetPackSizeFiles() {return m_PackSizeFiles;}	//vozvrat sjatogo razmera faylov v BYTEs
	ULONGLONG GetArchiveSize  () {return m_ArchiveSize;}	//vozvrat razmera arhiva v BYTEs
	unsigned  GetNumberFiles  () {return m_NumberFiles;}	//vozvrat kolichestvo faylov v arhive
};

#endif	//_TZ_CLASS_H