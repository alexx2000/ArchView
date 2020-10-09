/////////////////////////////////////////////////////////////////////////////
//TArchive.cpp
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TArchive.h"
#include <time.h>
#include <string.h>

/////////////////////////////////////////////////////////////////////////////
//Construction/Destruction
/////////////////////////////////////////////////////////////////////////////

//dfa      - utochnyayushie dannie
//	PathLF   - imya logfile (esli net to ne sozdavat')
//	NewLF    - rejim zapisi logfile 0 - prodoljit' 1 - noviy
//	DetailLF - detalizaciya logfayla 1 - lowdetail 2 - highdetail
//	SFX      - 1 - proveryat' SFX arhivi 0 - net
//namearch - imya typa archiva kotoriy rabotaet
TArchive::
TArchive(DataForArchive& dfa, LanguageMessages& langmsg, LanguageResults& langres, char* namearch):
	m_LangMsg(langmsg), m_LangRes(langres), m_h(0),
	m_pOriginalName(0), m_pAuthVerification(0), m_pTextComment(0),
	m_LogFile(0), m_hWnd(0), m_hEventEnd(0), m_BeginThread(0),
	m_CheckSFX(dfa.SFX), m_MAX_LEN_SFX(dfa.SizeSFX),
	m_InvertRatio(dfa.InvertRatio),
	m_pTargetFile(dfa.pTargetFile), m_pUnArchiveDll(dfa.pUnArchiveDll),
	m_pInternalName(0)

{
	//infa svyazannaya s logfaylom
	int size = strlen(dfa.PathLF);
	if (size > 0)
	{
		//detalizaciya logfayla
		if (dfa.DetailLF == 2)
			m_DetailLF = dfa.DetailLF;
		else
			m_DetailLF = 1;
		//sozdaem logfayl
		m_LogFile = new MLogFile(m_LangRes, dfa.pTargetFile, namearch, dfa.WidthAuthorName, dfa.WidthMessage);
		if (!m_LogFile->OpenLogFile(dfa.PathLF, dfa.NewLF)) m_DetailLF = 0;
	}
	else m_DetailLF = 0;

	//proverki
	if (!m_pTargetFile)   m_pTargetFile   = "";
	if (!m_pUnArchiveDll) m_pUnArchiveDll = "";
}

TArchive::
~TArchive()
{
	try
	{
		if (m_h)
			{ ::CloseHandle(m_h); m_h = 0; }
		if (m_pOriginalName)
			{ delete[] m_pOriginalName; m_pOriginalName = 0; }
		if (m_pAuthVerification)
			{ delete[] m_pAuthVerification; m_pAuthVerification = 0; }
		if (m_pTextComment)
			{ delete[] m_pTextComment; m_pTextComment = 0; }
		if (m_LogFile)
			{ m_LogFile->CloseLogFile(); delete m_LogFile; m_LogFile = 0; }
		if (m_pInternalName)
			{ delete[] m_pInternalName; m_pInternalName = 0; }

		m_hWnd        = 0;
		m_hEventEnd   = 0;
		m_BeginThread = 0;
	}
	catch (...) {;}
}

/////////////////////////////////////////////////////////////////////////////
//FUNCTIONS PROTECTED
/////////////////////////////////////////////////////////////////////////////

//obnulenie dannih
void TArchive::
InitialParametrs()
{
	//informaciya ob arhive
	m_UnpackSizeFiles   = 0;	//nezaarhivir razmer faylov
	m_PackSizeFiles     = 0;	//zaarhivir razmer faylov
	m_ArchiveSize       = 0;	//razmer arhiva
	m_NumberFiles       = 0;	//kolichestvo faylov
	m_NumberFolders     = 0;	//kolichestvo papok
	m_NumberChapters    = 0;	//kolichestvo glav (dlya ARJ)
	m_DictionarySize    = 0;	//razmer slovarya
	if (m_pOriginalName) { delete[] m_pOriginalName; m_pOriginalName = 0; }	//imya arhiva(dlya ARJ) fayla(dlya GZIP) magic(dlya TAR) pri sozdanii
	m_HostOS            = -1;	//OS arhivirovaniya
	m_ExtractOS         = -1;	//OS dlya raspakovki
	m_MethodPack        = -1;	//metod sjatiya
	m_MethodPackAdd     = -1;	//metod sjatiya (dlya ZIP/JAR/CAB)
	m_MemorySize        = 0;	//vozvrat ob'em ispol'zuemoy operativnoy pamyati v Mb(dlya PPM)
	m_RatioArchiveSize  = 0.0;	//sjatie ot razmera arhiva
	m_RatioPackFileSize = 0.0;	//sjatie ot upak razmera faylov
	m_PackVersion       = 0.0;	//versiya zapakovki
	m_UnpackVersion     = 0.0;	//versiya dlya raspakovki
	m_SfxModule         = 0;	//modul' SFX (*.exe)
	m_RecoveryRecord    = 0;	//est' infa dlya vosstanovleniya
	m_Solid             = FALSE;//neprerivniy arhiv
	m_MultiArc          = FALSE;//mnogotomniy arhiv
	m_VolumeNumber      = 0;	//nomer toma mnogotomnogo arhiva
	m_MainComment       = FALSE;//est' comentariy arhiva
	m_FileComment       = FALSE;//est' comentarii faylov
	m_ChapterComment    = FALSE;//est' comentarii chapter'a (dlya ARJ)
	m_Password          = FALSE;//est' parol'
	m_ArchiveLock       = FALSE;//arhiv zablokirovan
	m_AuthVerification  = FALSE;//est' electronnaya podpis'
	if (m_pAuthVerification) { delete[] m_pAuthVerification; m_pAuthVerification = 0; }	//stroka electronnaya podpis'
	m_ArchiveType       = FALSE;/*FALSE or TRUE
								ARC ili PAK arhiv (dlya ARC/PAK)
								LHA ili LZH arhiv (dlya LHA/LZH)
								ZIP ili JAR arhiv (dlya ZIP/JAR)*/

	if(m_pTextComment) { delete[] m_pTextComment; m_pTextComment = 0; }	//ukazatel' na text - kommentariy

	//vremya raboti
	m_TestTime          = 0;	//vremya raboti TestFile
	m_AnalyzeTime       = 0;	//vremya raboti AnalyzeInfoOfArc
	m_AllTime           = 0;	//polnoe vremya raboti

	m_CommentSize       = 0;	//dlya zapisi dlini kommentariya (v simvolah)
	m_CommentOffset     = 0;	//dlya zapisi smesheniya kommentariya ot nachala arhiva

	//vnutrennie peremennie
	::CloseHandle(m_h);	m_h = 0;//dlya raboti s faylami
	m_loffset           = 0;	//smeshenie v fayle
	m_hoffset           = 0;	//smeshenie v fayle (high-order DWORD)
	m_BigOffset         = FALSE;//bol'shoe smeshenie v fayle vpered, kogda starshie 4 byte smesheniya eshe = 0x0, a starshie uje doshli do bita-znaka (chtob znat' chto eto chislo polojitel'noe)
	m_Unicode           = FALSE;//imena faulov v yunikode
	MSG                 = 0;	//soobsheniya funkciy
	m_BlockType         = 0;	//dlya detalizacii vida obrabotannogo bloka arhiva

	//dlya realizacii v rabochem potoke (tol'ko funkciya Start())
	m_hWnd              = 0;	//okno dlya vozvrata soobsheniy ot potoka
	m_hEventEnd         = 0;	//sobitie zakritiya potoka
	m_BeginThread       = 0;	//zapusk v rabochem potoke ili net
	m_Percent           = 0;	//procent vipolneniya raboti
}

//schitat' sjatie ot razmera arhiva
void TArchive::
CountRatioArchiveSize()
{
	LONGLONG as  = m_ArchiveSize;
	LONGLONG usf = m_UnpackSizeFiles;
	if (usf == 0)
		m_RatioArchiveSize = 0;
	else
		m_RatioArchiveSize = ((double)as / (double)usf) * 100.0;
	if (m_InvertRatio)
		m_RatioArchiveSize = 100 - m_RatioArchiveSize;
}

//schitat' sjatie ot upak razmera faylov
void TArchive::
CountRatioPackFileSize()
{
	LONGLONG psf = m_PackSizeFiles;
	LONGLONG usf = m_UnpackSizeFiles;
	if (usf == 0)
		m_RatioPackFileSize = 0;
	else
		m_RatioPackFileSize = ((double)psf / (double)usf) * 100.0;
	if (m_InvertRatio)
		m_RatioPackFileSize = 100 - m_RatioPackFileSize;
}

//ustanovka dati i vremeni sozdaniya archiva iz DOS formata
//
//  31 30 29 28 27 26 25 24 23 22 21 20 19 18 17 16
// |<---- year-1980 --->|<- month ->|<--- day ---->|
//
//  15 14 13 12 11 10  9  8  7  6  5  4  3  2  1  0
// |<--- hour --->|<---- minute --->|<- second/2 ->|
//
//datetime - chislo predstavlyayushee datu i vremya (format vverhu)
//date     - strokovoe predstavlenie dati
//time     - strokovoe predstavlenie vremeni
void TArchive::
DateTimeDosFormat(DWORD datetime, char* date, char* time)
{
	WORD hour   = 0;
	WORD minute = 0;
	WORD second = 0;
	WORD year   = 0;
	WORD month  = 0;
	WORD day    = 0;

	//razbor dati i vremeni
	second = (datetime & 0x1F) * 2;
	minute = (datetime >> 5) & 0x3F;
	hour = (datetime >> 11) & 0x1F;
	day = (datetime >> 16) & 0x1F;
	month = (datetime >> 21) & 0x0F;
	year = ((datetime >> 25) & 0x7F) + 1980;

	char buf[4];

	//sostavlenie stroki dati
	if (day < 10)
	{
		strcpy(date, "0");
		strcat(date, _itoa(day, buf, 10));
	}
	else strcpy(date, _itoa(day, buf, 10));
	strcat(date, "/");
	if (month < 10)
	{
		strcat(date, "0");
		strcat(date, _itoa(month, buf, 10));
	}
	else strcat(date, _itoa(month, buf, 10));
	strcat(date, "/");
	strcat(date, _itoa(year, buf, 10));

	//sostavlenie stroki vremeni
	if (hour < 10)
	{
		strcpy(time, "0");
		strcpy(time, _itoa(hour, buf, 10));
	}
	else strcpy(time, _itoa(hour, buf, 10));
	strcat(time, ":");
	if (minute < 10)
	{
		strcat(time, "0");
		strcat(time, _itoa(minute, buf, 10));
	}
	else strcat(time, _itoa(minute, buf, 10));
	strcat(time, ":");
	if (second < 10)
	{
		strcat(time, _itoa(second, buf, 10));
		strcat(time, "0");
	}
	else strcat(time, _itoa(second, buf, 10));
}

//razmer fayla arhiva
LONGLONG  TArchive::
GetArchSize()
{
	DWORD FileSize     = 0;
	DWORD FileSizeHigh = 0;

	FileSize = ::GetFileSize(m_h, &FileSizeHigh);
	return TakeLong(FileSize, FileSizeHigh);
}

//sobrat' LONGLONG iz LONGs
//ll - mladshiy LONG
//hl - starshiy LONG
LONGLONG  TArchive::
TakeLong(DWORD ll, LONG hl)
{
	LARGE_INTEGER ull;

	ull.HighPart = hl;
	ull.LowPart  = ll;
	return ull.QuadPart;
}

//sobrat' WORD iz BYTEs
//lb - mladshiy BYTE
//hb - starshiy BYTE
WORD TArchive::
TakeWord(BYTE lb, BYTE hb)
{
	return (WORD)lb | ((WORD)hb) << 8;
}

//sobrat' DWORD iz WORDs
//lw - mladshiy WORD
//hw - starshiy WORD
DWORD TArchive::
TakeDWord(WORD lw, WORD hw)
{
	return (DWORD)lw | ((DWORD)hw) << 16;
}

/////////////////////////////////////////////////////////////////////////////
//FUNCTIONS PUBLIC
/////////////////////////////////////////////////////////////////////////////

//chtenie iz fayla
//lpBuffer - bufer kuda chitat'
//uBytes   - kolichestvo BYTE dlya chteniya
//return
//	TERROR_READ_FILE - oshibka chteniya
//	TMESSAGE_OK      - ok
int TArchive::
ReadArc(LPVOID lpBuffer, UINT uBytes)
{
	DWORD NumBytesRead = 0;

	//prochita' dannie
	if (!::ReadFile(m_h, lpBuffer, uBytes, &NumBytesRead, NULL)) 
		return TERROR_READ_FILE;

	//esli nado ukazat' skol'ko bayt prochitano
	if (uBytes != NumBytesRead) return TSUDDEN_END_FILE; //(uBytes - NumBytesRead);

	return TMESSAGE_OK;
}

//smeshenie v arhive
//loffset - mladshee smeshenie
//hoffset - starshee smeshenie
//MoveMetod:
//	FILE_BEGIN The starting point is zero or the beginning of the file. 
//	FILE_CURRENT The starting point is the current value of the file pointer. 
//	FILE_END The starting point is the current end-of-file position. 
//return
//	TERROR_SEEK_FILE - oshibka smesheniya
//	TMESSAGE_OK      - ok
int TArchive::
SeekArc(LONG loffset, PLONG hoffset, DWORD MoveMethod)
{
	LARGE_INTEGER Offset;

	if (hoffset == NULL)
		Offset.QuadPart = loffset;
	else
	{
		Offset.LowPart = loffset;
		Offset.HighPart = *hoffset;
	}
	Offset.LowPart = ::SetFilePointer(m_h, Offset.LowPart, &Offset.HighPart, MoveMethod);
	if (Offset.LowPart == 0xFFFFFFFF && GetLastError() != NO_ERROR)
		return TERROR_SEEK_FILE;
	else
		return TMESSAGE_OK;
}

//opredelit' smeshenie v arhive
LONGLONG TArchive::
GetArcPointer()
{
	LARGE_INTEGER ull;
	ull.LowPart  = 0;
	ull.HighPart = 0;

	ull.LowPart = ::SetFilePointer(m_h, 0, &ull.HighPart, FILE_CURRENT);
	if (ull.LowPart == 0xFFFFFFFF && GetLastError() != NO_ERROR)
	{
		ull.QuadPart = -1;
	}

	return ull.QuadPart;
}

/////////////////////////////////////////////////////////////////////////////
//FUNCTIONS PUBLIC
/////////////////////////////////////////////////////////////////////////////

//vizov testirovaniya ili sbora informacii
//lpDataForThread - dannie peredavaemie v potok (mojno i bez potoka)
//return
//TTERMINATE_THREAD - iskustvennoe zavershenie
//TMESSAGE_OK       - ok
//ERROR_OPEN_FILE   - oshibka otckritiya fayla
//ERROR_READ_FILE   - oshibka chteniya fayla
//ERROR_SEEK_FILE   - oshibka smesheniya fayla
//SUDDEN_END_FILE   - vnezapniy konec fayla
//ERROR_FORMAT      - oshibka formata fayla
int TArchive::
Start(LPVOID lpDataForThread)
{
	DataForThread* dft = (DataForThread*)lpDataForThread;

	//nujni tol'ko dlya zapuska v rabochem potoke
	m_hWnd        = dft->hWnd;
	m_hEventEnd   = dft->hEventEnd;
	m_BeginThread = dft->BeginThread;

	//dlya vremeni
	clock_t start  = 0;
	clock_t finish = 0;
	start = clock();

	if (dft->Flag == TEST_ARC)
	{
		MSG = TestFile(dft->Path);
		if (m_DetailLF >= 1)
			m_LogFile->WriteMessage(m_LangMsg._TestArchive, MSG);
	}
	if (dft->Flag == ANALYZE_ARC)
	{
		MSG = AnalyzeInfoOfArc(dft->Path);
		if (m_DetailLF >= 1)
			m_LogFile->WriteMessage(m_LangMsg._AnalyzeArchive, MSG);
	}
	
	finish = clock();

	//podschet vremeni
	if (dft->Flag == TEST_ARC)
		m_TestTime = ((double)(finish - start) / CLOCKS_PER_SEC);
	if (dft->Flag == ANALYZE_ARC)
		m_AnalyzeTime = ((double)(finish - start) / CLOCKS_PER_SEC);

	m_AllTime = m_TestTime + m_AnalyzeTime;

	//esli rabochiy potok to poslat' soobshenie
	if (m_BeginThread)
		::PostMessage(m_hWnd, WM_THREAD_FINISHED, 0, (LPARAM)MSG);
	return MSG;
}
