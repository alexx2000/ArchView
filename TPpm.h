/////////////////////////////////////////////////////////////////////////////
//TPpm.h
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TArchive.h"

#ifndef _TPPM_CLASS_H
#define _TPPM_CLASS_H

class TPpm: public TArchive
{
private:
	//zagolovok arhiva
	typedef struct __PPM_HEAD
	{	
		BYTE SIGNATURE1;
		BYTE SIGNATURE2;
		BYTE SIGNATURE3;
		BYTE SIGNATURE4;
		BYTE SIGNATURE5;
		BYTE SIGNATURE6;
		BYTE METHOD;
		BYTE MEMORY_SIZE;
		BYTE ORIG_SIZE1;
		BYTE ORIG_SIZE2;
		BYTE ORIG_SIZE3;
		BYTE ORIG_SIZE4;
		BYTE FTIME1;
		BYTE FTIME2;
		BYTE FTIME3;
		BYTE FTIME4;
		BYTE NAME_SIZE1;
		BYTE NAME_SIZE2;
		BYTE NAME_SIZE3;
		BYTE NAME_SIZE4;
	} PPM_HEAD;

	//strukturi dlya chteniya arhiva
	PPM_HEAD m_ARC_HEAD;	//zagolovok arhiva

public:
	TPpm(DataForArchive& dfa, LanguageMessages& langmsg, LanguageResults& langres, char* namearch = "PPM"): TArchive(dfa, langmsg, langres, namearch) {;}
	~TPpm() {;}

	int TestFile        (char*);	//fayl yavlyaetsya arhivom PPM
	int AnalyzeInfoOfArc(char*);	//opredelit' parametri (info) arhiva

	ULONGLONG GetUnpackSizeFiles  () {return m_UnpackSizeFiles;}	//vozvrat real'nogo razmera faylov v BYTEs
	ULONGLONG GetPackSizeFiles    () {return m_PackSizeFiles;}		//vozvrat sjatogo razmera faylov v BYTEs
	ULONGLONG GetArchiveSize      () {return m_ArchiveSize;}		//vozvrat razmera arhiva v BYTEs
	unsigned  GetNumberFiles      () {return m_NumberFiles;}		//vozvrat kolichestvo faylov v arhive
	short     GetMethodPack       () {return m_MethodPack;}			//vozvrat metod sjatiya | 0x02 - Rate1 (Fastest) | 0x03 - Rate2 (Fast) | 0x04 - Rate3 (Fast) | 0x05 - Rate4 (Fast) | 0x06 - Rate5 (Fast) | 0x07 - Rate6 (Normal) | 	0x08 - Rate7 (Normal) | 0x09 - Rate8 (Normal)  | 0x0A - Rate9 (Normal) | 0x0B - Rate10 (Normal) | 0x0C - Rate11 (Good) | 0x0D - Rate12 (Good) | 0x0E - Rate13 (Good) | 0x0F - Rate14 (Good)| 0x10 - Rate15 (Best)
	BYTE      GetMemorySize       () {return m_MemorySize;}			//vozvrat ob'em ispol'zuemoy operativnoy pamyati v Mb(dlya PPM)
	double    GetRatioArchiveSize () {return m_RatioArchiveSize;}	//sjatie ot razmera arhiva (%)
	double    GetRatioPackFileSize() {return m_RatioPackFileSize;}	//sjatie ot upak razmera faylov (%)
};

#endif	//_TPPM_CLASS_H
