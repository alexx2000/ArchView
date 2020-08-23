/////////////////////////////////////////////////////////////////////////////
//TArcPak.h
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TArchive.h"

#ifndef _TARCPAK_CLASS_H
#define _TARCPAK_CLASS_H

#define ARC_MAX_LEN_COMMENT 32

class TArcPak: public TArchive
{
private:
	//Nachalo zagolovka
	typedef struct __ARCPAK_BLOCK_HEAD
	{
		BYTE ID;
		BYTE METHOD;
	} ARCPAK_BLOCK_HEAD;

	//Zagolovok fayla v arhive
	typedef struct __ARCPAK_FILE_HEAD
	{
		BYTE NAME1;
		BYTE NAME2;
		BYTE NAME3;
		BYTE NAME4;
		BYTE NAME5;
		BYTE NAME6;
		BYTE NAME7;
		BYTE NAME8;
		BYTE NAME9;
		BYTE NAME10;
		BYTE NAME11;
		BYTE NAME12;
		BYTE NAME13;
		BYTE PACK_SIZE1;
		BYTE PACK_SIZE2;
		BYTE PACK_SIZE3;
		BYTE PACK_SIZE4;
		BYTE FTIME1;
		BYTE FTIME2;
		BYTE FTIME3;
		BYTE FTIME4;
		BYTE CRC1;
		BYTE CRC2;
		BYTE ORIG_SIZE1;
		BYTE ORIG_SIZE2;
		BYTE ORIG_SIZE3;
		BYTE ORIG_SIZE4;
	} ARCPAK_FILE_HEAD;

	//strukturi dlya chteniya arhiva
	ARCPAK_BLOCK_HEAD m_BLOCK_HEAD;	//zagolovok bloka
	ARCPAK_FILE_HEAD  m_FILE_HEAD;	//zagolovok fayla

	//vnutrennie funkcii
	int AnalyzeFileHead(ARCPAK_BLOCK_HEAD&);	//obrabotka zagolovka fayla

	//prochitat' kommentariy arhiva
	int ReadComment();

public:
	TArcPak(DataForArchive& dfa, LanguageMessages& langmsg, LanguageResults& langres, char* namearch = "ARCPAK"): TArchive(dfa, langmsg, langres, namearch) {;}
	~TArcPak() {;}

	int TestFile        (char*);	//fayl yavlyaetsya arhivom ARC/PAK
	int AnalyzeInfoOfArc(char*);	//opredelit' parametri (info) arhiva

	ULONGLONG GetUnpackSizeFiles  () {return m_UnpackSizeFiles;}	//vozvrat real'nogo razmera faylov v BYTEs
	ULONGLONG GetPackSizeFiles    () {return m_PackSizeFiles;}		//vozvrat sjatogo razmera faylov v BYTEs
	ULONGLONG GetArchiveSize      () {return m_ArchiveSize;}		//vozvrat razmera arhiva v BYTEs
	unsigned  GetNumberFiles      () {return m_NumberFiles;}		//vozvrat kolichestvo faylov v arhive
	unsigned  GetNumberFolders    () {return m_NumberFolders;}		//vozvrat kolichestvo papok v arhive (only ARC)
	short     GetMethodPack       () {return m_MethodPack;}			//vozvrat metod sjatiya | 0x01 - Unpacked | 0x02 - Unpacked | 0x03 - Packed RLE | 0x04 - Squeezed | 0x05 - Crunched | 0x06 - Crunched | 0x07 - Crunched | 0x08 - Crunched | 0x09 - Squashed | 0x0A - Crushed | 0x0B - Distilled
	double    GetRatioArchiveSize () {return m_RatioArchiveSize;}	//sjatie ot razmera arhiva (%)
	double    GetRatioPackFileSize() {return m_RatioPackFileSize;}	//sjatie ot upak razmera faylov (%)
	BOOL      GetMainComment      () {return m_MainComment;}		//TRUE - arhiv imeet kommentariy FALSE - net
	BOOL      GetArchiveType      () {return m_ArchiveType;}		//TRUE - PAK arhiv FALSE - ARC arhiv
	char*     GetTextComment      () {return m_pTextComment;}		//vernut' ukazatel' na kommentariy arhiva
};

#endif	//_TARCPAK_CLASS_H