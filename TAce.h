/////////////////////////////////////////////////////////////////////////////
//TAce.h
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TArchive.h"

#ifndef _TACE_CLASS_H
#define _TACE_CLASS_H

#define ACE_MAX_LEN_COMMENT 32768	//tak v ishodnikah example.c (unace.h)

class TAce: public TArchive
{
private:
	//Zagolovok lyubogo bloka
	typedef struct __ACE_BLOCK_HEAD
	{
		BYTE HEAD_CRC1;		//HEAD_CRC[WORD] razlojili na 2*[BYTE]
		BYTE HEAD_CRC2;		//HEAD_FLAGS[WORD] HEAD_SIZE[WORD] takje
		BYTE HEAD_SIZE1;	//chtobi sizeof(RAR_BLOCK_HEAD) = 7
		BYTE HEAD_SIZE2;	//inache pochemuti budet 8
		BYTE HEAD_TYPE;
		BYTE HEAD_FLAGS1;
		BYTE HEAD_FLAGS2;
	} ACE_BLOCK_HEAD;

	/*Zagolovok arhiva
	bez AV_SIZE[BYTE], AV[len(FNAME_SIZE)], 
	COMM_SIZE[WORD)], COMMENT[len(COMM_SIZE)], RESERVED[?]
	v konce (oni ne vsegda prisutstvuyut)*/
	typedef struct __ACE_HEAD
	{	
		BYTE ACESIGN1;
		BYTE ACESIGN2;
		BYTE ACESIGN3;
		BYTE ACESIGN4;
		BYTE ACESIGN5;
		BYTE ACESIGN6;
		BYTE ACESIGN7;
		BYTE VER_EXTRACT;
		BYTE VER_CREATED;
		BYTE HOST_CREATED;
		BYTE VOLUME_NUM;
		BYTE TIME_CREATED1;
		BYTE TIME_CREATED2;
		BYTE TIME_CREATED3;
		BYTE TIME_CREATED4;
		BYTE RESERVED1;
		BYTE RESERVED2;
		BYTE RESERVED3;
		BYTE RESERVED4;
		BYTE RESERVED5;
		BYTE RESERVED6;
		BYTE RESERVED7;
		BYTE RESERVED8;
	} ACE_HEAD;

	/*Zagolovok fayla v arhive bez "Zagolovok lyubogo bloka"
	vnachale (t.k dalee mogut sledovat' drugie bloki)
	i bez FNAME[len(FNAME_SIZE)], COMM_SIZE[WORD)],
	COMMENT[len(COMM_SIZE)], RESERVED[?]
	v konce (oni ne vsegda prisutstvuyut)*/
	typedef struct __ACE_FILE_HEAD
	{
		BYTE PACK_SIZE1;	//PACK_SIZE[DWORD] razlojili na 4*[BYTE]
		BYTE PACK_SIZE2;	//ORIG_SIZE[DWORD] CRC[DWORD]
		BYTE PACK_SIZE3;	//FTIME[DWORD] FNAME_SIZE[WORD]
		BYTE PACK_SIZE4;	//ATTR[DWORD] takje
		BYTE ORIG_SIZE1;
		BYTE ORIG_SIZE2;
		BYTE ORIG_SIZE3;
		BYTE ORIG_SIZE4;
		BYTE FTIME1;
		BYTE FTIME2;
		BYTE FTIME3;
		BYTE FTIME4;
		BYTE ATTR1;
		BYTE ATTR2;
		BYTE ATTR3;
		BYTE ATTR4;
		BYTE CRC1;
		BYTE CRC2;
		BYTE CRC3;
		BYTE CRC4;
		BYTE TECH_INFO1;
		BYTE TECH_INFO2;
		BYTE TECH_INFO3;
		BYTE TECH_INFO4;
		BYTE RESERVED1;
		BYTE RESERVED2;
		BYTE FNAME_SIZE1;
		BYTE FNAME_SIZE2;
	} ACE_FILE_HEAD;

	/*Zagolovok fayla v arhive bez "Zagolovok lyubogo bloka"
	vnachale (t.k dalee mogut sledovat' drugie bloki)
	i bez FNAME[len(FNAME_SIZE)], COMM_SIZE[WORD)],
	COMMENT[len(COMM_SIZE)], RESERVED[?]
	v konce (oni ne vsegda prisutstvuyut)*/
	typedef struct __ACE_FILE_HEAD_64
	{
		BYTE PACK_SIZE1;	//PACK_SIZE[LONGLONG] razlojili na 8*[BYTE]
		BYTE PACK_SIZE2;	//ORIG_SIZE[LONGLONG] CRC[DWORD]
		BYTE PACK_SIZE3;	//FTIME[DWORD] FNAME_SIZE[WORD]
		BYTE PACK_SIZE4;	//ATTR[DWORD] takje
		BYTE PACK_SIZE5;
		BYTE PACK_SIZE6;
		BYTE PACK_SIZE7;
		BYTE PACK_SIZE8;
		BYTE ORIG_SIZE1;
		BYTE ORIG_SIZE2;
		BYTE ORIG_SIZE3;
		BYTE ORIG_SIZE4;
		BYTE ORIG_SIZE5;
		BYTE ORIG_SIZE6;
		BYTE ORIG_SIZE7;
		BYTE ORIG_SIZE8;
		BYTE FTIME1;
		BYTE FTIME2;
		BYTE FTIME3;
		BYTE FTIME4;
		BYTE ATTR1;
		BYTE ATTR2;
		BYTE ATTR3;
		BYTE ATTR4;
		BYTE CRC1;
		BYTE CRC2;
		BYTE CRC3;
		BYTE CRC4;
		BYTE TECH_INFO1;
		BYTE TECH_INFO2;
		BYTE TECH_INFO3;
		BYTE TECH_INFO4;
		BYTE RESERVED1;
		BYTE RESERVED2;
		BYTE FNAME_SIZE1;
		BYTE FNAME_SIZE2;
	} ACE_FILE_HEAD_64;

	//strukturi dlya chteniya arhiva
	ACE_BLOCK_HEAD    m_BLOCK_HEAD;		//zagolovok lyubogo bloka
	ACE_HEAD          m_ARC_HEAD;		//zagolovok arhiva
	ACE_FILE_HEAD     m_FILE_HEAD;		//FILE_HEAD bez BLOCK_HEAD i ? koncevih
	ACE_FILE_HEAD_64  m_FILE_HEAD_64;	//FILE_HEAD_64 bez BLOCK_HEAD i ? koncevih

	//vnutrennie funkcii
	int AnalyzeArcHead (ACE_BLOCK_HEAD&);			//proverka zagolovka arhiva
	int AnalyzeFileHead(ACE_BLOCK_HEAD&, int&);		//obrabotka zagolovka fayla/papki
	int AnalyzeFileHead_64(ACE_BLOCK_HEAD&, int&);	//obrabotka zagolovka fayla > 4GB
	int AnalyzeRecovRec(ACE_BLOCK_HEAD&);			//obrabotka zapisi dlya vosstanovleniya
	int UnknownBlock   (ACE_BLOCK_HEAD&);			//obrabotka neizvestnogo bloka

	//prochitat' kommentariy arhiva
	int ReadComment();

public:
	TAce(DataForArchive& dfa, LanguageMessages& langmsg, LanguageResults& langres, char* namearch = "ACE"): TArchive(dfa, langmsg, langres, namearch) {;}
	~TAce() {;}

	int TestFile        (char*);	//fayl yavlyaetsya arhivom ACE
	int AnalyzeInfoOfArc(char*);	//opredelit' parametri (info) arhiva

	ULONGLONG GetUnpackSizeFiles       () {return m_UnpackSizeFiles;}	//vozvrat real'nogo razmera faylov v BYTEs
	ULONGLONG GetPackSizeFiles         () {return m_PackSizeFiles;}		//vozvrat sjatogo razmera faylov v BYTEs
	ULONGLONG GetArchiveSize           () {return m_ArchiveSize;}		//vozvrat razmera arhiva v BYTEs
	unsigned  GetNumberFiles           () {return m_NumberFiles;}		//vozvrat kolichestvo faylov v arhive
	unsigned  GetNumberFolders         () {return m_NumberFolders;}		//vozvrat kolichestvo papok v arhive
	unsigned  GetDictionarySize        () {return m_DictionarySize;}	//vozvrat razmer slovarya | 0x05 - 32 Kb | 0x06 - 64 Kb | 0x07 - 128 Kb | 0x08 - 256 Kb | 0x09 - 512 Kb | 0x0A - 1024 Kb | 0x0B - 2048 Kb | 0x0C - 4096 Kb
	short     GetHostOS                () {return m_HostOS;}			//vozvrat ispolzuemaya pri arhivirovanii OS | 0x00 - MS-DOS | 0x01 - OS/2 | 0x02 - Win32 | 0x03 - Unix | 0x04 - Mac OS | 0x05 - WinNT | 0x06 - Primos | 0x07 - Apple GS | 0x08 - Atari ST | 0x09 - Vax VMS | 0x0A - Amiga | 0x0B - Next
	short     GetMethodPack            () {return m_MethodPack;}		//vozvrat metod sjatiya | 0x00 - bez szhatiya (Stored) | 0x01 - skorostnoe (Fastest) | 0x02 - bystroe (Fast) | 0x03 - obychnoe (Normal) | 0x04 - horoshee (Good) | 0x05 - maksimal'noe (Best)
	double    GetRatioArchiveSize      () {return m_RatioArchiveSize;}	//sjatie ot razmera arhiva      (%)
	double    GetRatioPackFileSize     () {return m_RatioPackFileSize;}	//sjatie ot upak razmera faylov      (%)
	double    GetPackVersion           () {return m_PackVersion;}		//vozvarat versiay zapakovki      (X.X)
	double    GetUnpackVersion         () {return m_UnpackVersion;}		//vozvarat versiay dlya raspakovki      (X.X)
	unsigned  GetSfxModule             () {return m_SfxModule;}			//razmer modulya SFX      (*.exe) esli 0 to net
	unsigned  GetRecoveryRecord        () {return m_RecoveryRecord;}	//razmer v BYTEs informacii dlya vosstanovleniya
	BOOL      GetSolid                 () {return m_Solid;}				//TRUE - arhiv neprerivniy FALSE - net
	BOOL      GetMultiArc              () {return m_MultiArc;}			//TRUE - arhiv mnogotomniy FALSE - net
	unsigned  GetVolumeNumber          () {return m_VolumeNumber;}		//nomer toma mnogotomnogo arhiva (0 - ?)
	BOOL      GetMainComment           () {return m_MainComment;}		//TRUE - arhiv imeet kommentariy FALSE - net
	BOOL      GetFileComment           () {return m_FileComment;}		//TRUE - fayli imeyut kommentarii FALSE - net
	BOOL      GetPassword              () {return m_Password;}			//TRUE - arhiv zashishen parolem FALSE - net
	BOOL      GetArchiveLock           () {return m_ArchiveLock;}		//TRUE - arhiv zablokirovan FALSE - net
	BOOL      GetAuthVerification      () {return m_AuthVerification;}	//TRUE - est' elektronnaya podpis' FALSE - net
	char*     GetStringAuthVerification() {return m_pAuthVerification;}	//elektronnaya podpis'
	char*     GetTextComment           () {return m_pTextComment;}		//vernut' ukazatel' na kommentariy arhiva
	char*     GetDateCreated           () {return m_DateCreated;}		//data sozdaniya arhiva
	char*     GetTimeCreated           () {return m_TimeCreated;}		//vremya sozdaniya arhiva
};

#endif	//_TACE_CLASS_H