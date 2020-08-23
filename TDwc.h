/////////////////////////////////////////////////////////////////////////////
//TDwc.h
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TArchive.h"

#ifndef _TDWC_CLASS_H
#define _TDWC_CLASS_H

class TDwc: public TArchive
{
private:
	//Konec arhiva
	typedef struct __DWC_END
	{
		BYTE HEAD_SIZE1;
		BYTE HEAD_SIZE2;
		BYTE ENTRIES_SIZE1;
		BYTE ENTRIES_SIZE2;
		BYTE RESERVED01;
		BYTE RESERVED02;
		BYTE RESERVED03;
		BYTE RESERVED04;
		BYTE RESERVED05;
		BYTE RESERVED06;
		BYTE RESERVED07;
		BYTE RESERVED08;
		BYTE RESERVED09;
		BYTE RESERVED10;
		BYTE RESERVED11;
		BYTE RESERVED12;
		BYTE RESERVED13;
		BYTE RESERVED14;
		BYTE RESERVED15;
		BYTE RESERVED16;
		BYTE ENTRIES_COUNT1;
		BYTE ENTRIES_COUNT2;
		BYTE ENTRIES_COUNT3;
		BYTE ENTRIES_COUNT4;
		BYTE ID1;
		BYTE ID2;
		BYTE ID3;
	} DWC_END;

	//Zagolovok fayla v arhive
	typedef struct __DWC_FILE_HEAD
	{
		BYTE NAME01;
		BYTE NAME02;
		BYTE NAME03;
		BYTE NAME04;
		BYTE NAME05;
		BYTE NAME06;
		BYTE NAME07;
		BYTE NAME08;
		BYTE NAME09;
		BYTE NAME10;
		BYTE NAME11;
		BYTE NAME12;
		BYTE NAME13;
		BYTE ORIG_SIZE1;
		BYTE ORIG_SIZE2;
		BYTE ORIG_SIZE3;
		BYTE ORIG_SIZE4;
		BYTE FTIME1;
		BYTE FTIME2;
		BYTE FTIME3;
		BYTE FTIME4;
		BYTE PACK_SIZE1;
		BYTE PACK_SIZE2;
		BYTE PACK_SIZE3;
		BYTE PACK_SIZE4;
		BYTE OFFSET1;
		BYTE OFFSET2;
		BYTE OFFSET3;
		BYTE OFFSET4;
		BYTE METHOD;
		BYTE COMMENT_SIZE;
		BYTE RESERVED1;
		BYTE RESERVED2;
		BYTE RESERVED3;
	} DWC_FILE_HEAD;

	//strukturi dlya chteniya arhiva
	DWC_END       m_ARC_END;	//konec arhiva
	DWC_FILE_HEAD m_FILE_HEAD;	//zagolovok fayla

	//vnutrennie funkcii
	int AnalyzeArcEnd  (DWC_END&);		//obrabotka konca arhiva
	int AnalyzeFileHead(DWC_FILE_HEAD&);//obrabotka zagolovka fayla

	BOOL m_FirstFile;	//dlya pervogo fayla proveit' est' li SFX

public:
	TDwc(DataForArchive& dfa, LanguageMessages& langmsg, LanguageResults& langres, char* namearch = "DWC"): TArchive(dfa, langmsg, langres, namearch) {;}
	~TDwc() {;}

	int TestFile        (char*);	//fayl yavlyaetsya arhivom DWC
	int AnalyzeInfoOfArc(char*);	//opredelit' parametri (info) arhiva

	ULONGLONG GetUnpackSizeFiles  () {return m_UnpackSizeFiles;}	//vozvrat real'nogo razmera faylov v BYTEs
	ULONGLONG GetPackSizeFiles    () {return m_PackSizeFiles;}		//vozvrat sjatogo razmera faylov v BYTEs
	ULONGLONG GetArchiveSize      () {return m_ArchiveSize;}		//vozvrat razmera arhiva v BYTEs
	unsigned  GetNumberFiles      () {return m_NumberFiles;}		//vozvrat kolichestvo faylov v arhive
	short     GetMethodPack       () {return m_MethodPack;}			//vozvrat metod sjatiya | 0x01 - Crunched (Normal) | 0x02 - Stored
	double    GetRatioArchiveSize () {return m_RatioArchiveSize;}	//sjatie ot razmera arhiva (%)
	double    GetRatioPackFileSize() {return m_RatioPackFileSize;}	//sjatie ot upak razmera faylov (%)
	unsigned  GetSfxModule        () {return m_SfxModule;}			//razmer modulya SFX (*.exe) esli 0 to net
	BOOL      GetFileComment      () {return m_FileComment;}		//TRUE - fayli imeyut kommentarii FALSE - net
	BOOL      GetPassword         () {return m_Password;}			//TRUE - arhiv zashishen parolem FALSE - net
};

#endif	//_TDWC_CLASS_H