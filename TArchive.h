/////////////////////////////////////////////////////////////////////////////
//TArchive.h
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MLogFile.h"
#include <string.h>
#include <stdlib.h>

#ifndef _TARCHIVE_CLASS_H
#define _TARCHIVE_CLASS_H

#define MAX_LEN_STR     1024 + 1	//+1 t.k. stroka zakanchivaetsya BYTE = 0x00

//analiz arhiva (lyubogo, vse klassi ego nasledniki)
class TArchive
{
	protected:
		//informaciya ob arhive
		LONGLONG m_UnpackSizeFiles;		//nezaarhivir razmer faylov
		LONGLONG m_PackSizeFiles;		//zaarhivir razmer faylov
		LONGLONG m_ArchiveSize;			//razmera arhiva
		unsigned m_NumberFiles;			//kolichestvo faylov
		unsigned m_NumberFolders;		//kolichestvo papok
		unsigned m_NumberChapters;		//kolichestvo glav (dlya ARJ)
		unsigned m_DictionarySize;		//razmer slovarya
		char*    m_pOriginalName;		//imya arhiva(dlya ARJ) fayla(dlya GZIP) magic(dlya TAR) pri sozdanii
		short    m_HostOS;				//OS arhivirovaniya
		short    m_ExtractOS;			//OS dlya raspakovki
		short    m_MethodPack;			//metod sjatiya
		short    m_MethodPackAdd;		//metod sjatiya (dlya ZIP/JAR/CAB/GZIP)
		BYTE     m_MemorySize;			//vozvrat ob'em ispol'zuemoy operativnoy pamyati v Mb(dlya PPM)
		double   m_RatioArchiveSize;	//sjatie ot razmera arhiva
		double   m_RatioPackFileSize;	//sjatie ot upak razmera faylov
		double   m_PackVersion;			//versiya zapakovki
		double   m_UnpackVersion;		//versiya dlya raspakovki
		unsigned m_SfxModule;			//modul' SFX (*.exe)
		unsigned m_RecoveryRecord;		//est' infa dlya vosstanovleniya
		BOOL     m_Solid;				//neprerivniy arhiv
		BOOL     m_MultiArc;			//mnogotomniy arhiv
		unsigned m_VolumeNumber;		//nomer toma mnogotomnogo arhiva
		BOOL     m_MainComment;			//est' comentariy arhiva
		BOOL     m_FileComment;			//est' comentarii faylov
		BOOL     m_ChapterComment;		//est' comentarii chapter'a (dlya ARJ)
		BOOL     m_Password;			//est' parol'
		BOOL     m_ArchiveLock;			//arhiv zablokirovan
		BOOL     m_AuthVerification;	//est' electronnaya podpis'
		char*    m_pAuthVerification;	//stroka elektronnaya podpis'
		BOOL     m_ArchiveType;			/*FALSE or TRUE
										ARC ili PAK arhiv (dlya ARC/PAK)
										LHA ili LZH arhiv (dlya LHA/LZH)
										ZIP ili JAR arhiv (dlya ZIP/JAR)*/
		char     m_DateCreated[11];		//data sozdaniya arhiva
		char     m_TimeCreated[9];		//vreamya sozdaniya arhiva
		char     m_DateModified[11];	//data modifikacii arhiva
		char     m_TimeModified[9];		//vreamya modifikacii arhiva

		char*    m_pTextComment;		//ukazatel' na text - kommentariy

		//vremya raboti
		double   m_TestTime;			//vremya testirovaniya arhiva
		double   m_AnalyzeTime;			//vremya analiza arhiva
		double   m_AllTime;				//polnoe vremya analiza arhiva

		unsigned m_CommentSize;			//dlya zapisi dlini kommentariya (v simvolah)
		unsigned m_CommentOffset;		//dlya zapisi smesheniya kommentariya ot nachala arhiva

		//vnutrennie peremennie
		HANDLE    m_h;			//dlya raboti s faylami
		LONG      m_loffset;	//smeshenie v fayle
		LONG      m_hoffset;	//smeshenie v fayle (high-order DWORD)
		BOOL      m_BigOffset;	//bol'shoe smeshenie v fayle vpered, kogda starshie 4 byte smesheniya eshe = 0x0, a starshie uje doshli do bita-znaka (chtob znat' chto eto chislo polojitel'noe)
		BOOL      m_Unicode;	//imena faylov v yunikode
		int       MSG;			//soobsheniya funkciy
		int       m_BlockType;	//dlya detalizacii vida obrabotannogo bloka arhiva

		//dlya realizacii v rabochem potoke (tol'ko funkciya Start())
		HWND      m_hWnd;		//okno dlya vozvrata soobsheniy ot potoka
		HANDLE    m_hEventEnd;	//sobitie zakritiya potoka
		BOOL      m_BeginThread;//zapusk v rabochem potoke ili net
		LONGLONG  m_Percent;	//procent vipolneniya raboti

		//eti parametri inicializiruyutsya v KONSTRUKTORE
		MLogFile* m_LogFile;	//dlya raboti s logfile
		int       m_DetailLF;	//detalizaciya logfile 0 - net 1 - malo 2 - podrobno
		int       m_CheckSFX;	//1 - proveryat' SFX arhivi 0 - net
		unsigned  m_MAX_LEN_SFX;//maksimal'niya razmer SFX modulya
		unsigned  m_InvertRatio;//kak rasschitivat' sjatie (0: ratio = puck/unpack; 1: ratio = 1 - puck/unpack)

		char*     m_pTargetFile;	//fayl dlya razbora (delete NENADO)
		char*     m_pUnArchiveDll;	//DLL dlya dannogo tipa arhiva (chitat' kommentariy) (delete NENADO)

		//multiyazichnost'
		LanguageMessages& m_LangMsg;	//yazik dlya soobsheniy logfile
		LanguageResults&  m_LangRes;	//yazik dlya resultatov logfile

		//poluchennoe imya arhiva/plagina
		char* m_pInternalName;

		//vnutrennie funkcii
		void     InitialParametrs      ();					//obnulenie dannih
		void     CountRatioArchiveSize ();					//schitat' sjatie ot razmera arhiva
		void     CountRatioPackFileSize();					//schitat' sjatie ot upak razmera faylov
		void     DateTimeDosFormat     (DWORD, char*,char*);//ustanovka dati i vremeni sozdaniya archiva iz DOS formata
		LONGLONG GetArchSize           ();					//razmer fayla arhiva
		LONGLONG TakeLong              (DWORD, LONG);		//sobrat' LONGLONG iz LONGs
		WORD     TakeWord              (BYTE, BYTE);		//sobrat' WORD iz BYTEs
		DWORD    TakeDWord             (WORD, WORD);		//sobrat' DWORD iz WORDs

		int      ReadArc               (LPVOID, UINT);		//chtenie i podscheti (return > 0 = prochitano BYTEs )
		int      SeekArc               (LONG, PLONG, DWORD MoveMethod = FILE_CURRENT);//smeshenie v arhive
		LONGLONG GetArcPointer         ();					//opredelit' smeshenie v arhive

	public:
		TArchive(DataForArchive& dfa, LanguageMessages& langmsg, LanguageResults& langres, char* namearch= "MSG");
		~TArchive();

		virtual int TestFile        (char*) = 0;//testirovat' fayl na sootvetstvuyushiy vid arhiva
		virtual int AnalyzeInfoOfArc(char*) = 0;//opredelit' parametri (info) arhiva
		int         Start           (LPVOID);	//vizov testirovaniya ili sbora informacii

		const char* GetInternalName () {return m_pInternalName;}//v bazovom klasse ne ispol'zuetsya
		
		double      GetTestTime     () {return m_TestTime;}		//vremya raboti TestFile()
		double      GetAnalyzeTime  () {return m_AnalyzeTime;}	//vremya raboti AnalyzeInfoOfArc()
		double      GetAllTime      () {return m_AllTime;}		//polnoe vremya analiza
};

#endif	//_TARCHIVE_CLASS_H
