/////////////////////////////////////////////////////////////////////////////
//TRar.h
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TArchive.h"
#include "UnRar.h"

#ifndef _TRAR_CLASS_H
#define _TRAR_CLASS_H

#define RAR_MAX_LEN_COMMENT 62000	//tak v help for RAR

//analiz RAR arhiva
class TRar: public TArchive
{
private:
	int						m_ver;
	RARHeaderDataEx			m_HeaderDataEx;
	RAROpenArchiveDataEx	m_OpenArchiveData;

	//Zagolovok lyubogo bloka
	typedef struct __RAR_BLOCK_HEAD
	{
		BYTE HEAD_CRC1;		//HEAD_CRC[WORD] razlojili na 2*[BYTE]
		BYTE HEAD_CRC2;		//HEAD_FLAGS[WORD] HEAD_SIZE[WORD] takje
		BYTE HEAD_TYPE;		//chtobi sizeof(RAR_BLOCK_HEAD) = 7
		BYTE HEAD_FLAGS1;	//inache pochemuti budet 8
		BYTE HEAD_FLAGS2;
		BYTE HEAD_SIZE1;
		BYTE HEAD_SIZE2;
	} RAR_BLOCK_HEAD;

	/*Zagolovok fayla v arhive bez "Zagolovok lyubogo bloka"
	vnachale (t.k dalee mogut sledovat' drugie bloki)
	i bez HIGH_PACK_SIZE[DWORD], HIGH_UNP_SIZE[DWORD],
	FILE_NAME[len(NAME_SIZE)], SALT[8*BYTE], EXT_TIME[?]
	v konce (oni ne vsegda prisutstvuyut)*/
	typedef struct __RAR_FILE_HEAD
	{
		BYTE PACK_SIZE1;	//PACK_SIZE[DWORD] razlojili na 4*[BYTE]
		BYTE PACK_SIZE2;	//UNP_SIZE[DWORD] FILE_CRC[DWORD]
		BYTE PACK_SIZE3;	//FTIME[DWORD] NAME_SIZE[WORD]
		BYTE PACK_SIZE4;	//ATTR[DWORD] takje
		BYTE UNP_SIZE1;		//chtobi sizeof(RAR_BLOCK_HEAD) = 25
		BYTE UNP_SIZE2;		//inache pochemuti budet 26
		BYTE UNP_SIZE3;
		BYTE UNP_SIZE4;
		BYTE HOST_OS;
		BYTE FILE_CRC1;
		BYTE FILE_CRC2;
		BYTE FILE_CRC3;
		BYTE FILE_CRC4;
		BYTE FTIME1;
		BYTE FTIME2;
		BYTE FTIME3;
		BYTE FTIME4;
		BYTE UNP_VER;
		BYTE METHOD;
		BYTE NAME_SIZE1;
		BYTE NAME_SIZE2;
		BYTE ATTR1;
		BYTE ATTR2;
		BYTE ATTR3;
		BYTE ATTR4;
	} RAR_FILE_HEAD;

	//strukturi dlya chteniya arhiva
	RAR_BLOCK_HEAD m_BLOCK_HEAD;//zagolovok lyubogo bloka
	RAR_FILE_HEAD  m_FILE_HEAD;	//FILE_HEAD bez BLOCK_HEAD i ? koncevih

	HMODULE LoadPlugin(LPCSTR lpLibFileName);

	//vnutrennie funkcii
	int  AnalyzeMarkHead   (RAR_BLOCK_HEAD&);		//proverka markera fayla MARK_HEAD
	int  AnalyzeMainHead   (RAR_BLOCK_HEAD&);		//obrabotka zagolonka arhiva MAIN_HEAD
	int  AnalyzeFileHead   (RAR_BLOCK_HEAD&, int&);	//obrabotka zagolovka fayla/papki
	int  AnalyzeComment75h (RAR_BLOCK_HEAD&);		//obrabotka bloka kommentariya (stariy)
	int  AnalyzeAuthVer76h (RAR_BLOCK_HEAD&);		//obrabotka elektronnoy podpisi (stariy)
	int  AnalyzeAuthVer79h (RAR_BLOCK_HEAD&);		//obrabotka elektronnoy podpisi (stariy)
	int  AnalyzeRecovRec78h(RAR_BLOCK_HEAD&);		//obrabotka bloka dlya vosstanovleniya (stariy)
	int  AnalyzeSubblok77h (RAR_BLOCK_HEAD&, int&);	//obrabotka subbloka (stariy)
	int  AnalyzeSubblok7Ah (RAR_BLOCK_HEAD&, int&);	//obrabotka subbloka
	int  UnknownBlock      (RAR_BLOCK_HEAD&);		//obrabotka neizvestnogo bloka

	//opredelenie nomera toma dlya mnogotomnogo arhiva
	void DefineNumberVolume(char*);

	//prochitat' kommentariy arhiva
	int ReadComment();

	int TestFile4(char*);	//fayl yavlyaetsya arhivom RAR
	int AnalyzeInfoOfArc4(char*);	//opredelit' parametri (info) arhiva

	int AnalyzeFileHeadEx(RARHeaderDataEx&, int&);
	int TestFile5(char*);	//fayl yavlyaetsya arhivom RAR
	int AnalyzeInfoOfArc5(char*,bool);	//opredelit' parametri (info) arhiva

public:
	TRar(DataForArchive& dfa, LanguageMessages& langmsg, LanguageResults& langres, char* namearch = "RAR"): TArchive(dfa, langmsg, langres, namearch) {;}
	~TRar() {;}

	int TestFile        (char*);	//fayl yavlyaetsya arhivom RAR
	int AnalyzeInfoOfArc(char*);	//opredelit' parametri (info) arhiva

	ULONGLONG GetUnpackSizeFiles       () {return m_UnpackSizeFiles;}	//vozvrat real'nogo razmera faylov v BYTEs
	ULONGLONG GetPackSizeFiles         () {return m_PackSizeFiles;}		//vozvrat sjatogo razmera faylov v BYTEs
	ULONGLONG GetArchiveSize           () {return m_ArchiveSize;}		//vozvrat razmera arhiva v BYTEs
	unsigned  GetNumberFiles           () {return m_NumberFiles;}		//vozvrat kolichestvo faylov v arhive
	unsigned  GetNumberFolders         () {return m_NumberFolders;}		//vozvrat kolichestvo papok v arhive
	unsigned  GetDictionarySize        () {return m_DictionarySize;}	//vozvrat razmer slovarya | 0x00 - 64 Kb | 0x02 - 128 Kb | 0x04 - 256 Kb | 0x06 - 512 Kb | 0x08 - 1024 Kb | 0x0A - 2048 Kb | 0x0C - 4096 Kb | 0x0E - folder
	short     GetHostOS                () {return m_HostOS;}			//vozvrat ispolzuemaya pri arhivirovanii OS | 0x00 - MS-DOS | 0x01 - OS/2 | 0x02 - Win32 | 0x03 - Unix | 0x04 - Mac OS | 0x05 - BeOS
	short     GetMethodPack            () {return m_MethodPack;}		//vozvrat metod sjatiya | 0x30 - bez szhatiya (Stored) | 0x31 - skorostnoe (Fastest) | 0x32 - bystroe (Fast) | 0x33 - obychnoe (Normal) | 0x34 - horoshee (Good) | 0x35 - maksimal'noe (Best)
	double    GetRatioArchiveSize      () {return m_RatioArchiveSize;}	//sjatie ot razmera arhiva (%)
	double    GetRatioPackFileSize     () {return m_RatioPackFileSize;}	//sjatie ot upak razmera faylov (%)
	double    GetUnpackVersion         () {return m_UnpackVersion;}		//vozvarat versiay dlya raspakovki (X.X)
	unsigned  GetSfxModule             () {return m_SfxModule;}			//razmer modulya SFX (*.exe) esli 0 to net
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
};

#endif	//_TRAR_CLASS_H