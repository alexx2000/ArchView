/////////////////////////////////////////////////////////////////////////////
//TZoo.h
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TArchive.h"

#ifndef _TZOO_CLASS_H
#define _TZOO_CLASS_H

class TZoo: public TArchive
{
private:
	//Zagolovka arhiva
	typedef struct __ZOO_HEAD
	{
		BYTE TEXT01;
		BYTE TEXT02;
		BYTE TEXT03;
		BYTE TEXT04;
		BYTE TEXT05;
		BYTE TEXT06;
		BYTE TEXT07;
		BYTE TEXT08;
		BYTE TEXT09;
		BYTE TEXT10;
		BYTE TEXT11;
		BYTE TEXT12;
		BYTE TEXT13;
		BYTE TEXT14;
		BYTE TEXT15;
		BYTE TEXT16;
		BYTE TEXT17;
		BYTE TEXT18;
		BYTE TEXT19;
		BYTE TEXT20;
		BYTE ID1;
		BYTE ID2;
		BYTE ID3;
		BYTE ID4;
		BYTE OFFSET1;
		BYTE OFFSET2;
		BYTE OFFSET3;
		BYTE OFFSET4;
		BYTE OFFSET_OF1;
		BYTE OFFSET_OF2;
		BYTE OFFSET_OF3;
		BYTE OFFSET_OF4;
		BYTE VER_CREATED1;
		BYTE VER_CREATED2;
		BYTE VER_EXTRACT;
		BYTE COMMENT_OFFSET1;
		BYTE COMMENT_OFFSET2;
		BYTE COMMENT_OFFSET3;
		BYTE COMMENT_OFFSET4;
		BYTE COMMENT_SIZE1;
		BYTE COMMENT_SIZE2;
	} ZOO_HEAD;

	//Zagolovok fayla v arhive
	typedef struct __ZOO_FILE_HEAD
	{
		BYTE ID1;
		BYTE ID2;
		BYTE ID3;
		BYTE ID4;
		BYTE TYPE;
		BYTE METHOD;
		BYTE OFFSET1;
		BYTE OFFSET2;
		BYTE OFFSET3;
		BYTE OFFSET4;
		BYTE OFFSET_OF1;
		BYTE OFFSET_OF2;
		BYTE OFFSET_OF3;
		BYTE OFFSET_OF4;
		BYTE FTIME1;
		BYTE FTIME2;
		BYTE FTIME3;
		BYTE FTIME4;
		BYTE CRC1;
		BYTE CRC2;
		BYTE UNP_SIZE1;
		BYTE UNP_SIZE2;
		BYTE UNP_SIZE3;
		BYTE UNP_SIZE4;
		BYTE PACK_SIZE1;
		BYTE PACK_SIZE2;
		BYTE PACK_SIZE3;
		BYTE PACK_SIZE4;
		BYTE VER_COMPRESS;
		BYTE VER_EXTRACT;
		BYTE FLAG1;
		BYTE FLAG2;
		BYTE COMMENT_OFFSET1;
		BYTE COMMENT_OFFSET2;
		BYTE COMMENT_OFFSET3;
		BYTE COMMENT_OFFSET4;
		BYTE COMMENT_SIZE1;
		BYTE COMMENT_SIZE2;
	} ZOO_FILE_HEAD;

	//strukturi dlya chteniya arhiva
	ZOO_HEAD      m_ARC_HEAD;	//zagolovok arhiva
	ZOO_FILE_HEAD m_FILE_HEAD;	//zagolovok fayla

	//vnutrennie funkcii
	int AnalyzeArcHead (ZOO_HEAD&);		//obrabotka zagolovka arhiva
	int AnalyzeFileHead(ZOO_FILE_HEAD&);//obrabotka zagolovka fayla

	//prochitat' kommentariy arhiva
	int ReadComment();

public:
	TZoo(DataForArchive& dfa, LanguageMessages& langmsg, LanguageResults& langres, char* namearch = "ZOO"): TArchive(dfa, langmsg, langres, namearch) {;}
	~TZoo() {;}

	int TestFile        (char*);	//fayl yavlyaetsya arhivom ZOO
	int AnalyzeInfoOfArc(char*);	//opredelit' parametri (info) arhiva

	ULONGLONG GetUnpackSizeFiles  () {return m_UnpackSizeFiles;}	//vozvrat real'nogo razmera faylov v BYTEs
	ULONGLONG GetPackSizeFiles    () {return m_PackSizeFiles;}		//vozvrat sjatogo razmera faylov v BYTEs
	ULONGLONG GetArchiveSize      () {return m_ArchiveSize;}		//vozvrat razmera arhiva v BYTEs
	unsigned  GetNumberFiles      () {return m_NumberFiles;}		//vozvrat kolichestvo faylov v arhive
	short     GetMethodPack       () {return m_MethodPack;}			//vozvrat metod sjatiya | 0x00 - Stored | 0x01 - Crunched (Normal) | 0x02 - Good
	double    GetRatioArchiveSize () {return m_RatioArchiveSize;}	//sjatie ot razmera arhiva (%)
	double    GetRatioPackFileSize() {return m_RatioPackFileSize;}	//sjatie ot upak razmera faylov (%)
	double    GetPackVersion      () {return m_PackVersion;}		//vozvarat versiay zapakovki (X.X)
	double    GetUnpackVersion    () {return m_UnpackVersion;}		//vozvarat versiay dlya raspakovki (X.X)
	BOOL      GetMainComment      () {return m_MainComment;}		//TRUE - arhiv imeet kommentariy FALSE - net
	BOOL      GetFileComment      () {return m_FileComment;}		//TRUE - fayli imeyut kommentarii FALSE - net
	char*     GetTextComment      () {return m_pTextComment;}		//vernut' ukazatel' na kommentariy arhiva
};

#endif	//_TZOO_CLASS_H