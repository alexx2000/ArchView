/////////////////////////////////////////////////////////////////////////////
//TGZipTgz.h
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TArchive.h"

#ifndef _TGZIPTGZ_CLASS_H
#define _TGZIPTGZ_CLASS_H

class TGZipTgz: public TArchive
{
private:
	//Zagolovok fayla v arhive
	typedef struct __GZIPTGZ_FILE_HEAD
	{
		BYTE ID1;
		BYTE ID2;
		BYTE METHOD;
		BYTE FLAGS;
		BYTE FTIME1;
		BYTE FTIME2;
		BYTE FTIME3;
		BYTE FTIME4;
		BYTE XFLAGS;
		BYTE HOST_OS;
	} GZIPTGZ_FILE_HEAD;

	//konec arhiva
	typedef struct __GZIPTGZ_ARC_END
	{	
		DWORD CRC;
		DWORD ORIG_SIZE;
	} GZIPTGZ_ARC_END;

	//strukturi dlya chteniya arhiva
	GZIPTGZ_FILE_HEAD m_FILE_HEAD;	//zagolovok fayla
	GZIPTGZ_ARC_END   m_ARC_END;	//konec arhiva

	char m_buffer[MAX_LEN_STR];

	//vnutrennie funkcii
	int AnalyzeFileHead(GZIPTGZ_FILE_HEAD&);	//obrabotka zagolovka fayla

public:
	TGZipTgz(DataForArchive& dfa, LanguageMessages& langmsg, LanguageResults& langres, char* namearch = "GZIP/TGZ"): TArchive(dfa, langmsg, langres, namearch) {;}
	~TGZipTgz() {;}

	int TestFile        (char*);	//fayl yavlyaetsya arhivom GZIP
	int AnalyzeInfoOfArc(char*);	//opredelit' parametri (info) arhiva

	ULONGLONG GetUnpackSizeFiles  () {return m_UnpackSizeFiles;}	//vozvrat real'nogo razmera faylov v BYTEs
	ULONGLONG GetPackSizeFiles    () {return m_PackSizeFiles;}		//vozvrat sjatogo razmera faylov v BYTEs
	ULONGLONG GetArchiveSize      () {return m_ArchiveSize;}		//vozvrat razmera arhiva v BYTEs
	unsigned  GetNumberFiles      () {return m_NumberFiles;}		//vozvrat kolichestvo faylov v arhive
	char*     GetOriginalName     () {return m_pOriginalName;}		//vozvrat imya zapakovannogo fayla
	short     GetHostOS           () {return m_HostOS;}				//vozvrat ispolzuemaya pri arhivirovanii OS | 0x00 - MS-DOS | 0x01 - Amiga | 0x02 - Vax VMS | 0x03 - Unix | 0x04 - VM/CMS | 0x05 - Atari ST | 0x06 - OS/2 | 0x07 - Mac OS | 0x08 - Z-System | 0x09 - CP/M | 0x0A - TOPS-20 | 0x0B - WinNT | 0x0C - SMS/QDOS | 0x0D - Acorn RISC
	short     GetMethodPack       () {return m_MethodPack;}			//vozvrat metod sjatiya | 0x08 - (Deflated)
	short     GetMethodPackAdd    () {return m_MethodPackAdd;}		//vozvrat metod sjatiya | 0x00 - normal'noe (Normal) | 0x02 - maksimal'noe (Best) | 0x04 - bistroe (Fast)
	double    GetRatioArchiveSize () {return m_RatioArchiveSize;}	//sjatie ot razmera arhiva (%)
	double    GetRatioPackFileSize() {return m_RatioPackFileSize;}	//sjatie ot upak razmera faylov (%)
	BOOL      GetMainComment      () {return m_MainComment;}		//TRUE - arhiv imeet kommentariy FALSE - net
};

#endif	//_TGZIPTGZ_CLASS_H