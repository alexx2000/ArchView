/////////////////////////////////////////////////////////////////////////////
//TTar.h
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TArchive.h"

#ifndef _TTAR_CLASS_H
#define _TTAR_CLASS_H

#define RECORDSIZE  512
#define NAMSIZE     100
#define TUNMLEN     32
#define TGNMLEN     32

class TTar: public TArchive
{
private:
	//Zagolovok fayla v arhive
	typedef union _TAR_FILE_HEAD
	{
		char charptr[RECORDSIZE];
		struct
		{
			char name[NAMSIZE];
			char mode[8];
			char uid[8];
			char gid[8];
			char size[12];
			char mtime[12];
			char chksum[8];
			char linkflag;
			char linkname[NAMSIZE];
			char magic[8];
			char uname[TUNMLEN];
			char gname[TGNMLEN];
			char devmajor[8];
			char devminor[8];
		};
	} TAR_FILE_HEAD;

	//strukturi dlya chteniya arhiva
	TAR_FILE_HEAD m_FILE_HEAD;	//zagolovok fayla

	//vnutrennie funkcii
	int AnalyzeFileHead(TAR_FILE_HEAD&);	//obrabotka zagolovka fayla/papki

public:
	TTar(DataForArchive& dfa, LanguageMessages& langmsg, LanguageResults& langres, char* namearch = "TAR"): TArchive(dfa, langmsg, langres, namearch) {;}
	~TTar() {;}

	int TestFile        (char*);	//fayl yavlyaetsya arhivom TAR
	int AnalyzeInfoOfArc(char*);	//opredelit' parametri (info) arhiva

	ULONGLONG GetUnpackSizeFiles  () {return m_UnpackSizeFiles;}	//vozvrat real'nogo razmera faylov v BYTEs
	ULONGLONG GetPackSizeFiles    () {return m_PackSizeFiles;}		//vozvrat sjatogo razmera faylov v BYTEs
	ULONGLONG GetArchiveSize      () {return m_ArchiveSize;}		//vozvrat razmera arhiva v BYTEs
	unsigned  GetNumberFiles      () {return m_NumberFiles;}		//vozvrat kolichestvo faylov v arhive
	unsigned  GetNumberFolders    () {return m_NumberFolders;}		//vozvrat kolichestvo papok v arhive
	char*     GetMagic            () {return m_pOriginalName;}		//vozvrat pole magic
	double    GetRatioArchiveSize () {return m_RatioArchiveSize;}	//sjatie ot razmera arhiva (%)
	double    GetRatioPackFileSize() {return m_RatioPackFileSize;}	//sjatie ot upak razmera faylov (%)
};

#endif	//_TTAR_CLASS_H