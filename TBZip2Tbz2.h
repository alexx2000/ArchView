/////////////////////////////////////////////////////////////////////////////
//TBZip2Tbz2.h
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TArchive.h"

#ifndef _TBZIP2TBZ2_CLASS_H
#define _TBZIP2TBZ2_CLASS_H

class TBZip2Tbz2: public TArchive
{
private:
	//zagolovok arhiva
	typedef struct __BZIP2TBZ2_HEAD
	{	
		BYTE SIGNATURE1;
		BYTE SIGNATURE2;
		BYTE SIGNATURE3;
		BYTE METHOD;
		BYTE RESERVED1;
		BYTE RESERVED2;
		BYTE RESERVED3;
		BYTE RESERVED4;
		BYTE RESERVED5;
		BYTE RESERVED6;
		BYTE CRC1;
		BYTE CRC2;
		BYTE CRC3;
		BYTE CRC4;
	} BZIP2TBZ2_HEAD;

	//strukturi dlya chteniya arhiva
	BZIP2TBZ2_HEAD m_ARC_HEAD;		//zagolovok arhiva

public:
	TBZip2Tbz2(DataForArchive& dfa, LanguageMessages& langmsg, LanguageResults& langres, char* namearch = "BZIP2/TBZ2"): TArchive(dfa, langmsg, langres, namearch) {;}
	~TBZip2Tbz2() {;}

	int TestFile        (char*);	//fayl yavlyaetsya arhivom BZIP2
	int AnalyzeInfoOfArc(char*);	//opredelit' parametri (info) arhiva

	ULONGLONG GetPackSizeFiles() {return m_PackSizeFiles;}	//vozvrat sjatogo razmera faylov v BYTEs
	ULONGLONG GetArchiveSize  () {return m_ArchiveSize;}	//vozvrat razmera arhiva v BYTEs
	unsigned  GetNumberFiles  () {return m_NumberFiles;}	//vozvrat kolichestvo faylov v arhive
	short     GetMethodPack   () {return m_MethodPack;}		//vozvrat metod sjatiya | 0x31 - Rate1 (Fastest) | 0x32 - Rate2 (Fast) | 0x33 - Rate3 (Fast) | 0x34 - Rate4 (Normal) | 0x35 - Rate5 (Normal) | 0x36 - Rate6 (Normal) | 0x37 - Rate7 (Good) | 0x38 - Rate8 (Good) | 0x39 - Rate9 (Best)
};

#endif	//_TBZIP2TBZ2_CLASS_H
