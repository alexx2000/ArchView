/////////////////////////////////////////////////////////////////////////////
//TCpio.h
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TArchive.h"
#include <string.h>

#ifndef _TCPIO_CLASS_H
#define _TCPIO_CLASS_H

#define CPIO_SIZE_FILD_SIGNATURE 6
#define CPIO_SIZE_FILD_DIGI_SIZE 8

class TCpio: public TArchive
{
private:
	/*Zagolovok fayla v arhive
	bez FILE_NAME*/
	typedef struct __CPIO_FILE_HEAD
	{
		char FILD01[CPIO_SIZE_FILD_DIGI_SIZE];
		char ATTR[CPIO_SIZE_FILD_DIGI_SIZE];
		char FILD03[CPIO_SIZE_FILD_DIGI_SIZE];
		char FILD04[CPIO_SIZE_FILD_DIGI_SIZE];
		char TYPE[CPIO_SIZE_FILD_DIGI_SIZE];
		char FILD06[CPIO_SIZE_FILD_DIGI_SIZE];
		char FILE_SIZE[CPIO_SIZE_FILD_DIGI_SIZE];
		char FILD08[CPIO_SIZE_FILD_DIGI_SIZE];
		char FILD09[CPIO_SIZE_FILD_DIGI_SIZE];
		char FILD10[CPIO_SIZE_FILD_DIGI_SIZE];
		char FILD11[CPIO_SIZE_FILD_DIGI_SIZE];
		char NAME_SIZE[CPIO_SIZE_FILD_DIGI_SIZE];
		char FILD13[CPIO_SIZE_FILD_DIGI_SIZE];
	} CPIO_FILE_HEAD;

	//strukturi dlya chteniya arhiva
	CPIO_FILE_HEAD m_FILE_HEAD;	//zagolovok fayla

	char m_SIGNATURE[CPIO_SIZE_FILD_SIGNATURE];

	//vnutrennie funkcii
	int AnalyzeFileHead(CPIO_FILE_HEAD&, int&);	//obrabotka zagolovka fayla/papki

public:
	TCpio(DataForArchive& dfa, LanguageMessages& langmsg, LanguageResults& langres, char* namearch = "CPIO"): TArchive(dfa, langmsg, langres, namearch) {;}
	~TCpio() {;}

	int TestFile        (char*);	//fayl yavlyaetsya arhivom CPIO
	int AnalyzeInfoOfArc(char*);	//opredelit' parametri (info) arhiva

	ULONGLONG GetUnpackSizeFiles  () {return m_UnpackSizeFiles;}	//vozvrat real'nogo razmera faylov v BYTEs
	ULONGLONG GetPackSizeFiles    () {return m_PackSizeFiles;}		//vozvrat sjatogo razmera faylov v BYTEs
	ULONGLONG GetArchiveSize      () {return m_ArchiveSize;}		//vozvrat razmera arhiva v BYTEs
	unsigned  GetNumberFiles      () {return m_NumberFiles;}		//vozvrat kolichestvo faylov v arhive
	unsigned  GetNumberFolders    () {return m_NumberFolders;}		//vozvrat kolichestvo papok v arhive
	double    GetRatioArchiveSize () {return m_RatioArchiveSize;}	//sjatie ot razmera arhiva (%)
	double    GetRatioPackFileSize() {return m_RatioPackFileSize;}	//sjatie ot upak razmera faylov (%)
};

#endif	//_TCPIO_CLASS_H