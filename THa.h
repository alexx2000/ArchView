/////////////////////////////////////////////////////////////////////////////
//THa.h
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TArchive.h"

#ifndef _THA_CLASS_H
#define _THA_CLASS_H

class THa: public TArchive
{
	private:
	//zagolovok arhiva
	typedef struct __HA_HEAD
	{	
		WORD ID;
		WORD NUM_FILES;
	} HA_HEAD;

	/*Zagolovok fayla v arhive
	bez HIGH_PACK_SIZE[DWORD], HIGH_UNP_SIZE[DWORD],
	FILE_NAME[len(NAME_SIZE)], SALT[8*BYTE], EXT_TIME[?]
	v konce (oni ne vsegda prisutstvuyut)*/
	typedef struct __HA_FILE_HEAD
	{
		BYTE METHOD;
		BYTE PACK_SIZE1;	//PACK_SIZE[DWORD] razlojili na 4*[BYTE]
		BYTE PACK_SIZE2;	//ORIG_SIZE[DWORD] CRC[DWORD] FTIME[DWORD]
		BYTE PACK_SIZE3;
		BYTE PACK_SIZE4;
		BYTE ORIG_SIZE1;
		BYTE ORIG_SIZE2;
		BYTE ORIG_SIZE3;
		BYTE ORIG_SIZE4;
		BYTE CRC1;
		BYTE CRC2;
		BYTE CRC3;
		BYTE CRC4;
		BYTE FTIME1;
		BYTE FTIME2;
		BYTE FTIME3;
		BYTE FTIME4;
	} HA_FILE_HEAD;

	//dobavok k zagolovku fayla v arhive
	typedef struct __HA_FILE_ADD
	{	
		BYTE SIZE;
		BYTE HOST_OS;
		BYTE ATTR;
	} HA_FILE_ADD;

	//strukturi dlya chteniya arhiva
	HA_HEAD      m_ARC_HEAD;			//zagolovok arhiva
	HA_FILE_HEAD m_FILE_HEAD;	//zagolovok fayla
	HA_FILE_ADD  m_FILE_ADD;		//zagolovok fayla dobavka

	char m_buffer[MAX_LEN_STR];

	//vnutrennie funkcii
	int AnalyzeArcHead (HA_HEAD&);				//proverka zagolovka arhiva
	int AnalyzeFileHead(HA_FILE_HEAD&, int&);	//obrabotka zagolovka fayla/papki

public:
	THa(DataForArchive& dfa, LanguageMessages& langmsg, LanguageResults& langres, char* namearch = "HA"): TArchive(dfa, langmsg, langres, namearch) {;}
	~THa() {;}

	int TestFile        (char*);	//fayl yavlyaetsya arhivom HA
	int AnalyzeInfoOfArc(char*);	//opredelit' parametri (info) arhiva

	ULONGLONG GetUnpackSizeFiles  () {return m_UnpackSizeFiles;}	//vozvrat real'nogo razmera faylov v BYTEs
	ULONGLONG GetPackSizeFiles    () {return m_PackSizeFiles;}		//vozvrat sjatogo razmera faylov v BYTEs
	ULONGLONG GetArchiveSize      () {return m_ArchiveSize;}		//vozvrat razmera arhiva v BYTEs
	unsigned  GetNumberFiles      () {return m_NumberFiles;}		//vozvrat kolichestvo faylov v arhive
	unsigned  GetNumberFolders    () {return m_NumberFolders;}		//vozvrat kolichestvo papok v arhive
	short     GetHostOS           () {return m_HostOS;}				//vozvrat ispolzuemaya pri arhivirovanii OS | 0x01 - MS-DOS | 0x02 - Unix
	short     GetMethodPack       () {return m_MethodPack;}			//vozvrat metod sjatiya | 0x00 - bez szhatiya (CPY (Stored)) | 0x01 - default (ASC (Normal)) | 0x02 - finite context model and arithmetic coder (HSC (Good))
	double    GetRatioArchiveSize () {return m_RatioArchiveSize;}	//sjatie ot razmera arhiva (%)
	double    GetRatioPackFileSize() {return m_RatioPackFileSize;}	//sjatie ot upak razmera faylov (%)
};

#endif	//_THA_CLASS_H