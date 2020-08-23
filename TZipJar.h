/////////////////////////////////////////////////////////////////////////////
//TZipJar.h
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TArchive.h"

#ifndef _TZIPJAR_CLASS_H
#define _TZIPJAR_CLASS_H

#define MAX_LEN_END_RECORD 65562	//(2^16 = 65536 maximal'naya dlina kommentariya)
									//(sizeof(m_END_RECORD) = 22)
									//(sizeof(m_JAR64_END_RECORD) = 26)
									//65536 + 26 = 65562

class TZipJar: public TArchive
{
private:
	/*Lokal'niy zagolovok fayla v arhive bez FILE_NAME[len(NAME_SIZE)],
	EXTRA_FILD[len(EXTRA_SIZE)] v konce (oni ne vsegda prisutstvuyut)*/
	typedef struct __ZIPJAR_LOCAL_FILE_HEAD
	{
		WORD UNP_VER;		//PACK_SIZE[DWORD] razlojili na 2*[WORD]
		WORD BIT_FLAG;		//UNP_SIZE[DWORD] CRC[DWORD]
		WORD PACK_METHOD;
		WORD FILE_TIME;
		WORD FILE_DATE;
		WORD CRC1;
		WORD CRC2;
		WORD PACK_SIZE1;
		WORD PACK_SIZE2;
		WORD UNP_SIZE1;
		WORD UNP_SIZE2;
		WORD NAME_SIZE;
		WORD EXTRA_SIZE;
	} ZIPJAR_LOCAL_FILE_HEAD;

	/*Zagolovok fayla v arhive bez FILE_NAME[len(NAME_SIZE)],
	EXTRA_FILD[len(EXTRA_SIZE)], FILE_COMMENT[len(COMMENT_SIZE)]
	v konce (oni ne vsegda prisutstvuyut)*/
	typedef struct __ZIPJAR_FILE_HEAD
	{
		WORD MADE_BY;		//EXTERN_ATTR[DWORD] razlojili na 2*[WORD]
		WORD UNP_VER;		//OFFSET[DWORD]
		WORD BIT_FLAG;
		WORD PACK_METHOD;
		WORD FILE_TIME;
		WORD FILE_DATE;
		WORD CRC1;
		WORD CRC2;
		WORD PACK_SIZE1;
		WORD PACK_SIZE2;
		WORD UNP_SIZE1;
		WORD UNP_SIZE2;
		WORD NAME_SIZE;
		WORD EXTRA_SIZE;
		WORD COMMENT_SIZE;
		WORD DISK_START;
		WORD INTERN_ATTR;
		WORD EXTERN_ATTR1;
		WORD EXTERN_ATTR2;
		WORD OFFSET1;
		WORD OFFSET2;
	} ZIPJAR_FILE_HEAD;

	/*Blok END_OF_CENTRAL_DIRECTORY_RECORD bez
	FILE_COMMENT[len(COMMENT_SIZE)]*/
	typedef struct __ZIPJAR_END_RECORD
	{
		WORD DISK;			//SIZE[DWORD] razlojili na 2*[WORD]
		WORD DISK_START;	//OFFSET[DWORD]
		WORD ENTRIES_DISK;
		WORD ENTRIES_DIR;
		WORD SIZE1;
		WORD SIZE2;
		WORD OFFSET1;
		WORD OFFSET2;
		WORD COMMENT_SIZE;
	} ZIPJAR_END_RECORD;

	/*Blok ZIP64_END_OF_CENTRAL_DIRECTORY_RECORD bez
	ZIP64_EXTENSIBLE_DATA_SECTOR[?] v konce ne vsegda*/
	typedef struct __ZIPJAR_ZIP64_END_RECORD
	{
		WORD ZIP64_SIZE1;	//ZIP64_SIZE[ULONGLONG] razlojili na 4*[WORD]
		WORD ZIP64_SIZE2;	//DISK[DWORD]
		WORD ZIP64_SIZE3;	//DISK_START[DWORD]
		WORD ZIP64_SIZE4;	//ENTRIES_DISK[ULONGLONG]
		WORD MADE_BY;		//ENTRIES_DIR[ULONGLONG]
		WORD UNP_VER;		//SIZE[ULONGLONG]
		WORD DISK1;			//OFFSET[ULONGLONG]
		WORD DISK2;
		WORD DISK_START1;
		WORD DISK_START2;
		WORD ENTRIES_DISK1;
		WORD ENTRIES_DISK2;
		WORD ENTRIES_DISK3;
		WORD ENTRIES_DISK4;
		WORD ENTRIES_DIR1;
		WORD ENTRIES_DIR2;
		WORD ENTRIES_DIR3;
		WORD ENTRIES_DIR4;
		WORD SIZE1;
		WORD SIZE2;
		WORD SIZE3;
		WORD SIZE4;
		WORD OFFSET1;
		WORD OFFSET2;
		WORD OFFSET3;
		WORD OFFSET4;
	} ZIPJAR_ZIP64_END_RECORD;

	/*Blok ZIP64_END_OF_CENTRAL_DIRECTORY_LOCATOR*/
	typedef struct __ZIPJAR_ZIP64_END_LOCATOR
	{
		DWORD DISK_START;		//OFFSET[ULONGLONG] razlojili na 2*[DWORD]
		DWORD OFFSET1;
		DWORD OFFSET2;
		DWORD DISK;
	} ZIPJAR_ZIP64_END_LOCATOR;

	/*Blok JAR64_END_OF_CENTRAL_DIRECTORY_RECORD bez
	FILE_COMMENT[len(COMMENT_SIZE)]*/
	typedef struct __JAR_JAR64_END_RECORD
	{
		WORD DISK;			//SIZE[DWORD] razlojili na 2*[WORD]
		WORD DISK_START;	//OFFSET[ULONGLONG]
		WORD ENTRIES_DISK;
		WORD ENTRIES_DIR;
		WORD SIZE1;
		WORD SIZE2;
		WORD OFFSET1;
		WORD OFFSET2;
		WORD OFFSET3;
		WORD OFFSET4;
		WORD COMMENT_SIZE;
	} JAR_JAR64_END_RECORD;

	/*Blok EXTRA_FILD_HEADER*/
	typedef struct __ZIPJAR_EXTRA_FILD_HEADER
	{
		WORD HEADER_ID;		//0x0001 - ZIP64_EXTRA_FILD
		WORD DATA_SIZE;
	} ZIPJAR_EXTRA_FILD_HEADER;

	/*Blok ZIP64_EXTRA_FILD
	voobshe nekotorie polya mogut otsutstvovat'*/
	typedef struct __ZIPJAR_ZIP64_EXTRA_FILD
	{
		LONGLONG ORIG_SIZE;
		LONGLONG PACK_SIZE;
	//	LONGLONG OFFSET;
	//	DWORD DISK_START;	//chashe vsego ego net
	} ZIPJAR_ZIP64_EXTRA_FILD;

	/*Blok JAR64_EXTRA_FILD_1
	voobshe nekotorie polya mogut otsutstvovat'*/
	typedef struct __JAR_JAR64_EXTRA_FILD_1
	{
		WORD HIGH_ORIG_SIZE;
		WORD HIGH_PACK_SIZE;
		WORD HIGH_OFFSET;
	} JAR_JAR64_EXTRA_FILD_1;

	/*Blok JAR64_EXTRA_FILD_2
	voobshe nekotorie polya mogut otsutstvovat'*/
	typedef struct __JAR_JAR64_EXTRA_FILD_2
	{
		WORD HIGH_ORIG_SIZE;
		WORD HIGH_PACK_SIZE;
	} JAR_JAR64_EXTRA_FILD_2;

	//strukturi dlya chteniya arhiva
	ZIPJAR_LOCAL_FILE_HEAD   m_LOCAL_FILE_HEAD;
	ZIPJAR_FILE_HEAD         m_FILE_HEAD;
	ZIPJAR_END_RECORD        m_END_RECORD;
	ZIPJAR_ZIP64_END_RECORD  m_ZIP64_END_RECORD;
	ZIPJAR_ZIP64_END_LOCATOR m_ZIP64_END_LOCATOR;
	JAR_JAR64_END_RECORD     m_JAR64_END_RECORD;
	ZIPJAR_EXTRA_FILD_HEADER m_EXTRA_FILD_HEADER;
	ZIPJAR_ZIP64_EXTRA_FILD  m_ZIP64_EXTRA_FILD;
	JAR_JAR64_EXTRA_FILD_1   m_JAR64_EXTRA_FILD_1;
	JAR_JAR64_EXTRA_FILD_2   m_JAR64_EXTRA_FILD_2;


	//chtobi ne dublirovat' dannie LocalFileHeader i FileHeader
	BOOL m_zlNumFileFold;
	BOOL m_zlPackUnpSize;

	//vnutrennie funkcii
	int AnalyzeLocalFileHead(int&);	//obrabotka lokal'nogo bloka fayla/papki
	int AnalyzeFileHead(int&);		//obrabotka bloka fayla/papki
	int AnalyzeDigitalSign();		//vozmojno blok elektronnoy podpisi
	int AnalyzeZip64EndRecord();	//zip64 directory record block
	int AnalyzeZip64EndLocator();	//zip64 directory locator block
	int AnalyzeJar64EndRecord();	//jar64 directory record block
	int AnalyzeEndRecord();			//archive end block
	int CheckEndRecordZIP64();		//archive end block na nalichie ZIP64

	//prochitat' kommentariy arhiva
	int ReadComment();

public:
	TZipJar(DataForArchive& dfa, LanguageMessages& langmsg, LanguageResults& langres, char* namearch = "ZIP/JAR"): TArchive(dfa, langmsg, langres, namearch) {;}
	~TZipJar() {;}

	int TestFile        (char*);	//fayl yavlyaetsya arhivom ZIP/JAR
	int AnalyzeInfoOfArc(char*);	//opredelit' parametri (info) arhiva

	ULONGLONG GetUnpackSizeFiles  () {return m_UnpackSizeFiles;}	//vozvrat real'nogo razmera faylov v BYTEs
	ULONGLONG GetPackSizeFiles    () {return m_PackSizeFiles;}		//vozvrat sjatogo razmera faylov v BYTEs
	ULONGLONG GetArchiveSize      () {return m_ArchiveSize;}		//vozvrat razmera arhiva v BYTEs
	unsigned  GetNumberFiles      () {return m_NumberFiles;}		//vozvrat kolichestvo faylov v arhive
	unsigned  GetNumberFolders    () {return m_NumberFolders;}		//vozvrat kolichestvo papok v arhive
	short     GetHostOS           () {return m_HostOS;}				//vozvrat ispolzuemaya pri arhivirovanii OS | 0x00 - MS-DOS | 0x01 - Amiga | 0x02 - Vax VMS | 0x03 - Unix | 0x04 - VM/CMS | 0x05 - Atari ST | 0x06 - OS/2 | 0x07 - Mac OS | 0x08 - Z-System | 0x09 - CP/M | 0x0A - TOPS-20 | 0x0B - WinNT | 0x0C - SMS/QDOS | 0x0D - Acorn RISC | 0x0E - VFAT | 0x0F - MVS | 0x10 - BeOS | 0x11 - Tandem
	short     GetExtractOS        () {return m_ExtractOS;}			//vozvrat OS dlya raspakovki | 0x00 - MS-DOS | 0x01 - Amiga | 0x02 - Vax VMS | 0x03 - Unix | 0x04 - VM/CMS | 0x05 - Atari ST | 0x06 - OS/2 | 0x07 - Mac OS | 0x08 - Z-System | 0x09 - CP/M | 0x0A - TOPS-20 | 0x0B - WinNT | 0x0C - SMS/QDOS | 0x0D - Acorn RISC | 0x0E - VFAT | 0x0F - MVS | 0x10 - BeOS | 0x11 - Tandem
	short     GetMethodPack       () {return m_MethodPack;}			//vozvrat metod sjatiya | 0x00 - bez szhatiya (Stored) | 0x01 - (Shrunk) | 0x02 - (Reduced with compression factor 1) | 0x03 - (Reduced with compression factor 2) | 0x04 - (Reduced with compression factor 3) | 0x05 - (Reduced with compression factor 4) | 0x06 - (Imploded) | 0x07 - (Reserved for Tokenizing compression algorithm) | 0x08 - (Deflated) | 0x09 - (Enhanced Deflating using Deflated64(tm)) | 0x0A - (PKWARE Date Compression Library Imploding) | 0x0B - (?) | 0x0C - (BZip2)
	short     GetMethodPackAdd    () {return m_MethodPackAdd;}		//vozvrat metod sjatiya | 0x00 - normal'noe (Normal) | 0x02 - maksimal'noe (Best) | 0x04 - bistroe (Fast) | 0x06 - skorostnoe (Superfast)
	double    GetRatioArchiveSize () {return m_RatioArchiveSize;}	//sjatie ot razmera arhiva (%)
	double    GetRatioPackFileSize() {return m_RatioPackFileSize;}	//sjatie ot upak razmera faylov (%)
	double    GetPackVersion      () {return m_PackVersion;}		//vozvarat versiay zapakovki (X.X)
	double    GetUnpackVersion    () {return m_UnpackVersion;}		//vozvarat versiay dlya raspakovki (X.X)
	unsigned  GetSfxModule        () {return m_SfxModule;}			//razmer modulya SFX (*.exe) esli 0 to net
	BOOL      GetMultiArc         () {return m_MultiArc;}			//TRUE - arhiv mnogotomniy FALSE - net
	unsigned  GetVolumeNumber     () {return m_VolumeNumber;}		//nomer toma mnogotomnogo arhiva (0 - ?)
	BOOL      GetMainComment      () {return m_MainComment;}		//TRUE - arhiv imeet kommentariy FALSE - net
	BOOL      GetFileComment      () {return m_FileComment;}		//TRUE - fayli imeyut kommentarii FALSE - net
	BOOL      GetPassword         () {return m_Password;}			//TRUE - arhiv zashishen parolem FALSE - net
	BOOL      GetArchiveType      () {return m_ArchiveType;}		//TRUE - JAR arhiv FALSE - ZIP arhiv
	char*     GetTextComment      () {return m_pTextComment;}		//vernut' ukazatel' na kommentariy arhiva
};

#endif	//_TZIPJAR_CLASS_H