/////////////////////////////////////////////////////////////////////////////
//TArj.h
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TArchive.h"

#ifndef _TARJ_CLASS_H
#define _TARJ_CLASS_H

#define ARJ_MAX_LEN_COMMENT 2000 + 1/*25 line
									80 byte in 1 line
									(79 char and 1 byte endline 0x0A)
									1 byte endstr 0x00
									*/

class TArj: public TArchive
{
private:
	/*Zagolovok bloka v arhive bez FILE_NAME[len(NAME_SIZE)],
	MAIN_COMMENT[len(COMMENT_SIZE)] CRC[DWORD] EXTENDED[?]*/
	typedef struct __ARJ_BLOCK_HEAD
	{
		WORD HEADER_ID;		//DATE[DWORD] razlojili na 2*[WORD]
		WORD HEAD_SIZE;		//PACK_SIZE[DWORD] ORIG_SIZE[DWORD]
		BYTE HDR_SIZE;		//CRC[DWORD]
		BYTE VER_CREATED;
		BYTE VER_EXTRACT;
		BYTE HOST_OS;
		BYTE FLAGS;
		BYTE METHOD;
		BYTE FILE_TYPE;
		BYTE RESERVED;
		WORD DATE1;
		WORD DATE2;
		WORD PACK_SIZE1;
		WORD PACK_SIZE2;
		WORD ORIG_SIZE1;
		WORD ORIG_SIZE2;
		WORD CRC1;
		WORD CRC2;
		WORD FILESPEC_POS;
		WORD ACCESS;
		BYTE FIRST_CHAPTER;
		BYTE LAST_CHAPTER;
	} ARJ_BLOCK_HEAD;

	//strukturi dlya chteniya arhiva
	ARJ_BLOCK_HEAD m_BLOCK_HEAD;

	char  m_buffer    [MAX_LEN_STR];
	char  m_buffer_cmt[ARJ_MAX_LEN_COMMENT];
	BOOL  m_ARJ_RecoveryRecord;		//est'li recovery record
	DWORD m_ARJ_SecurityPosition;	//posiciya elektronnoy podpisi
	WORD  m_ARJ_SecurityLength;		//razmer blocka elektronnoy podpisi

	//vnutrennie funkcii
	int AnalyzeArcHead    (ARJ_BLOCK_HEAD&);	//analizirovat' zagolovok arhiva
	int AnalyzeFileHead   (ARJ_BLOCK_HEAD&);	//blok fayla / papki
	int AnalyzeChapterHead(ARJ_BLOCK_HEAD&);	//blok glava

	//opredelenie nomera toma dlya mnogotomnogo arhiva
	void DefineNumberVolume(char*);

	//prochitat' kommentariy arhiva
	int ReadComment();

public:
	TArj(DataForArchive& dfa, LanguageMessages& langmsg, LanguageResults& langres, char* namearch = "ARJ"): TArchive(dfa, langmsg, langres, namearch) {;}
	~TArj() {;}

	int TestFile        (char*);	//fayl yavlyaetsya arhivom ARJ
	int AnalyzeInfoOfArc(char*);	//opredelit' parametri (info) arhiva

	ULONGLONG GetUnpackSizeFiles  () {return m_UnpackSizeFiles;}	//vozvrat real'nogo razmera faylov v BYTEs
	ULONGLONG GetPackSizeFiles    () {return m_PackSizeFiles;}		//vozvrat sjatogo razmera faylov v BYTEs
	ULONGLONG GetArchiveSize      () {return m_ArchiveSize;}		//vozvrat razmera arhiva v BYTEs
	unsigned  GetNumberFiles      () {return m_NumberFiles;}		//vozvrat kolichestvo faylov v arhive
	unsigned  GetNumberFolders    () {return m_NumberFolders;}		//vozvrat kolichestvo papok v arhive
	unsigned  GetNumberChapters   () {return m_NumberChapters;}		//vozvrat kolichestvo glav v arhive
	char*     GetOriginalName     () {return m_pOriginalName;}		//vozvrat imya arhiva pri sozdanii
	short     GetHostOS           () {return m_HostOS;}				//vozvrat ispolzuemaya pri arhivirovanii OS | 0x00 - MS-DOS | 0x01 - Primos | 0x02 - Unix | 0x03 - Amiga | 0x04 - Mac OS | 0x05 - OS/2 | 0x06 - Apple GS | 0x07 - Atari ST | 0x08 - Next | 0x09 - Vax VMS | 0x0A - Win95 | 0x0B - Win32
	short     GetMethodPack       () {return m_MethodPack;}			//vozvrat metod sjatiya | 0x00 - bez szhatiya (Stored) | 0x01 - horoshee (Good) | 0x02 - obychnoe (Normal) | 0x03 - bystroe (Fast) | 0x04 - skorostnoe (Fastest)
	double    GetRatioArchiveSize () {return m_RatioArchiveSize;}	//sjatie ot razmera arhiva (%)
	double    GetRatioPackFileSize() {return m_RatioPackFileSize;}	//sjatie ot upak razmera faylov (%)
	double    GetPackVersion      () {return m_PackVersion;}		//vozvarat versiay zapakovki (X.X)
	double    GetUnpackVersion    () {return m_UnpackVersion;}		//vozvarat versiay dlya raspakovki (X.X)
	unsigned  GetSfxModule        () {return m_SfxModule;}			//razmer modulya SFX (*.exe) esli 0 to net
	unsigned  GetRecoveryRecord   () {return m_RecoveryRecord;}		//razmer v BYTEs informacii dlya vosstanovleniya
	BOOL      GetMultiArc         () {return m_MultiArc;}			//TRUE - arhiv mnogotomniy FALSE - net
	unsigned  GetVolumeNumber     () {return m_VolumeNumber;}		//nomer toma mnogotomnogo arhiva (0 - ?)
	BOOL      GetMainComment      () {return m_MainComment;}		//TRUE - arhiv imeet kommentariy FALSE - net
	BOOL      GetFileComment      () {return m_FileComment;}		//TRUE - fayli imeyut kommentarii FALSE - net
	BOOL      GetChapterComment   () {return m_ChapterComment;}		//TRUE - chapter'i imeyut kommentarii FALSE - net
	BOOL      GetPassword         () {return m_Password;}			//TRUE - arhiv zashishen parolem FALSE - net
	BOOL      GetArchiveLock      () {return m_ArchiveLock;}		//TRUE - arhiv zablokirovan FALSE - net
	BOOL      GetAuthVerification () {return m_AuthVerification;}	//TRUE - elektronnaya podpis' FALSE - net
	char*     GetTextComment      () {return m_pTextComment;}		//vernut' ukazatel' na kommentariy arhiva
	char*     GetDateCreated      () {return m_DateCreated;}		//data sozdaniya arhiva
	char*     GetTimeCreated      () {return m_TimeCreated;}		//vremya sozdaniya arhiva
	char*     GetDateModified     () {return m_DateModified;}		//data modifikacii arhiva
	char*     GetTimeModified     () {return m_TimeModified;}		//vremya modifikacii arhiva
};

#endif	//_TARJ_CLASS_H