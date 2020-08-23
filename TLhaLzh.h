/////////////////////////////////////////////////////////////////////////////
//TLhaLzh.h
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TArchive.h"

#ifndef _TLHALZH_CLASS_H
#define _TLHALZH_CLASS_H

class TLhaLzh: public TArchive
{
private:
	//Zagolovok fayla v arhive
	typedef struct __LHALZH_FILE_HEAD
	{
		BYTE HEAD_SIZE;
		BYTE HEAD_CHECKSUM;
		BYTE METHOD1;
		BYTE METHOD2;
		BYTE METHOD3;
		BYTE METHOD4;
		BYTE METHOD5;
		BYTE PACK_SIZE1;
		BYTE PACK_SIZE2;
		BYTE PACK_SIZE3;
		BYTE PACK_SIZE4;
		BYTE UNP_SIZE1;
		BYTE UNP_SIZE2;
		BYTE UNP_SIZE3;
		BYTE UNP_SIZE4;
		BYTE FTIME1;
		BYTE FTIME2;
		BYTE FTIME3;
		BYTE FTIME4;
		BYTE ATTR;
		BYTE HEAD_LEVEL;
		BYTE NAME_SIZE;
	} LHALZH_FILE_HEAD;

	//strukturi dlya chteniya arhiva
	LHALZH_FILE_HEAD m_FILE_HEAD;	//zagolovok fayla

	//vnutrennie funkcii
	int AnalyzeFileHead(LHALZH_FILE_HEAD&, int&);	//obrabotka zagolovka fayla/papki

public:
	TLhaLzh(DataForArchive& dfa, LanguageMessages& langmsg, LanguageResults& langres, char* namearch = "LHA/LZH"): TArchive(dfa, langmsg, langres, namearch) {;}
	~TLhaLzh() {;}

	int TestFile        (char*);	//fayl yavlyaetsya arhivom LHA/LZH
	int AnalyzeInfoOfArc(char*);	//opredelit' parametri (info) arhiva

	ULONGLONG GetUnpackSizeFiles  () {return m_UnpackSizeFiles;}	//vozvrat real'nogo razmera faylov v BYTEs
	ULONGLONG GetPackSizeFiles    () {return m_PackSizeFiles;}		//vozvrat sjatogo razmera faylov v BYTEs
	ULONGLONG GetArchiveSize      () {return m_ArchiveSize;}		//vozvrat razmera arhiva v BYTEs
	unsigned  GetNumberFiles      () {return m_NumberFiles;}		//vozvrat kolichestvo faylov v arhive
	unsigned  GetNumberFolders    () {return m_NumberFolders;}		//vozvrat kolichestvo papok v arhive
	unsigned  GetDictionarySize   () {return m_DictionarySize;}		//vozvrat razmer slovarya | 0x00 - 2 Kb | 0x01 - 4 Kb | 0x02 - 8 Kb
	short     GetMethodPack       () {return m_MethodPack;}			//vozvrat metod sjatiya | 0x00 - -lh0- (Stored) | 0x01 - -lh1- (Superfast) | 0x02 - -lh2- (Fastest) | 0x03 - -lh3- (Fast) | 0x04 - -lh4- (Normal) | 0x05 - -lh5- (Good) | 0x06 - -lh6- (Verygood) | 0x07 - -lh7- (Best) | 0x08 - -lzs- (Normal) | 0x09 - -lz4- (Stored) | 0x0A - -lz5- (Good)
	double    GetRatioArchiveSize () {return m_RatioArchiveSize;}	//sjatie ot razmera arhiva (%)
	double    GetRatioPackFileSize() {return m_RatioPackFileSize;}	//sjatie ot upak razmera faylov (%)
	unsigned  GetSfxModule        () {return m_SfxModule;}			//razmer modulya SFX (*.exe) esli 0 to net
	BOOL      GetArchiveType      () {return m_ArchiveType;}		//TRUE - LZH arhiv FALSE - LHA arhiv
};

#endif	//_TLHALZH_CLASS_H