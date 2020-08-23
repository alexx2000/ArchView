/////////////////////////////////////////////////////////////////////////////
//TSqz.h
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TArchive.h"

#ifndef _TSQZ_CLASS_H
#define _TSQZ_CLASS_H

class TSqz: public TArchive
{
private:
	//Zagolovok arhiva
	typedef struct __SQZ_ARC_HEAD
	{
		BYTE ID1;
		BYTE ID2;
		BYTE ID3;
		BYTE ID4;
		BYTE ID5;
		BYTE UNP_VER;
		BYTE HOST_OS;
		BYTE FLAGS;
	} SQZ_ARC_HEAD;

	//Zagolovok fayla v arhive
	typedef struct __SQZ_FILE_HEAD
	{
		BYTE HEAD_SUM;
		BYTE METHOD;
		BYTE PACK_SIZE1;
		BYTE PACK_SIZE2;
		BYTE PACK_SIZE3;
		BYTE PACK_SIZE4;
		BYTE ORIG_SIZE1;
		BYTE ORIG_SIZE2;
		BYTE ORIG_SIZE3;
		BYTE ORIG_SIZE4;
		BYTE FTIME1;
		BYTE FTIME2;
		BYTE FTIME3;
		BYTE FTIME4;
		BYTE ATTR;
		BYTE CRC1;
		BYTE CRC2;
		BYTE CRC3;
		BYTE CRC4;
	} SQZ_FILE_HEAD;

	//strukturi dlya chteniya arhiva
	SQZ_ARC_HEAD  m_ARC_HEAD;
	SQZ_FILE_HEAD m_FILE_HEAD;

	//vnutrennie funkcii
	int AnalyzeArcHead (SQZ_ARC_HEAD&);
	int AnalyzeFileHead(BYTE);

	//vnutrennie funkcii
	BYTE m_TimeFormat;	//Unix=0/Dos=1 format vremeni

public:
	TSqz(DataForArchive& dfa, LanguageMessages& langmsg, LanguageResults& langres, char* namearch = "SQZ"): TArchive(dfa, langmsg, langres, namearch) {;}
	~TSqz() {;}

	int TestFile        (char*);	//fayl yavlyaetsya arhivom SQZ
	int AnalyzeInfoOfArc(char*);	//opredelit' parametri (info) arhiva

	ULONGLONG GetUnpackSizeFiles  () {return m_UnpackSizeFiles;}	//vozvrat real'nogo razmera faylov v BYTEs
	ULONGLONG GetPackSizeFiles    () {return m_PackSizeFiles;}		//vozvrat sjatogo razmera faylov v BYTEs
	ULONGLONG GetArchiveSize      () {return m_ArchiveSize;}		//vozvrat razmera arhiva v BYTEs
	unsigned  GetNumberFiles      () {return m_NumberFiles;}		//vozvrat kolichestvo faylov v arhive
	unsigned  GetNumberFolders    () {return m_NumberFolders;}		//vozvrat kolichestvo papok v arhive
	short     GetHostOS           () {return m_HostOS;}				//ozvrat ispolzuemaya pri arhivirovanii OS | 0x00 - MS-DOS | 0x01 - OS/2 | 0x02 - Vax VMS | 0x03 - OS/2 | 0x04 - Amiga | 0x05 - Mac OS | 0x06 - Unix
	short     GetMethodPack       () {return m_MethodPack;}			//ozvrat metod sjatiya | 0x00 - bez szhatiya (Stored) | 0x01 - bystroe (Fast) | 0x02 - obychnoe (Normal) | 0x03 - horoshee (Good) | 0x04 - maximal'noe (Best)
	double    GetRatioArchiveSize () {return m_RatioArchiveSize;}	//sjatie ot razmera arhiva (%)
	double    GetRatioPackFileSize() {return m_RatioPackFileSize;}	//sjatie ot upak razmera faylov (%)
	double    GetUnpackVersion    () {return m_UnpackVersion;}		//vozvarat versiay dlya raspakovki (X.X)
	unsigned  GetSfxModule        () {return m_SfxModule;}			//razmer modulya SFX (*.exe) esli 0 to net
	BOOL      GetMultiArc         () {return m_MultiArc;}			//TRUE - arhiv mnogotomniy FALSE - net
	BOOL      GetMainComment      () {return m_MainComment;}		//TRUE - arhiv imeet kommentariy FALSE - net
	BOOL      GetPassword         () {return m_Password;}			//TRUE - arhiv zashishen parolem FALSE - net
	BOOL      GetArchiveLock      () {return m_ArchiveLock;}		//TRUE - arhiv zablokirovan FALSE - net
};

#endif	//_TSQZ_CLASS_H