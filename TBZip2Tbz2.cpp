/////////////////////////////////////////////////////////////////////////////
//TBZip2Tbz2.cpp
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TBZip2Tbz2.h"

/////////////////////////////////////////////////////////////////////////////
//FUNCTIONS PRIVATE
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//FUNCTIONS PUBLIC
/////////////////////////////////////////////////////////////////////////////

//fayl yavlyaetsya arhivom BZIP2
//path - imya fayla dlya testirovaniya
//return
//	TMESSAGE_OK     - ok
//	ERROR_OPEN_FILE - oshibka otckritiya fayla
//	ERROR_READ_FILE - oshibka chteniya fayla
//	ERROR_SEEK_FILE - oshibka smesheniya fayla
//	SUDDEN_END_FILE - vnezapniy konec fayla
//	ERROR_FORMAT    - oshibka formata fayla
int TBZip2Tbz2::
TestFile(char* path)
{
	//obnulenie dannih
	InitialParametrs();
	
	//otkrit' fayl dlya chteniya
	m_h = CreateFile(path, GENERIC_READ, FILE_SHARE_READ, 
			NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (m_h == INVALID_HANDLE_VALUE) 
		{ if (m_h) { ::CloseHandle(m_h); m_h = 0; } return TERROR_OPEN_FILE; }

	//chtenie zagolovka arhiva
	MSG = ReadArc(&m_ARC_HEAD, sizeof(m_ARC_HEAD));
	if (MSG != TMESSAGE_OK) { if (m_h) { ::CloseHandle(m_h); m_h = 0; } return MSG; }

	//proverka zagolovka arhiva
	if ((m_ARC_HEAD.SIGNATURE1 != 'B') ||
		(m_ARC_HEAD.SIGNATURE2 != 'Z') ||
		(m_ARC_HEAD.SIGNATURE3 != 'h')) MSG = TERROR_FORMAT;
	else MSG = TMESSAGE_OK;
	if (m_DetailLF == 2)
		m_LogFile->WriteMessage(m_LangMsg.BlockArhiveHeader, MSG);
	if (MSG != TMESSAGE_OK)
		{ if (m_h) { ::CloseHandle(m_h); m_h = 0; } return MSG; }

	//zakritie fayla
	if (m_h) { ::CloseHandle(m_h); m_h = 0; }
	return TMESSAGE_OK;
}

//opredelit' parametri (info) arhiva
//path - imya fayla dlya analiza
//return
//	TTERMINATE_TRHEAD - iskustvennoe zavershenie potoka
//	TMESSAGE_OK       - ok
//	ERROR_OPEN_FILE   - oshibka otckritiya fayla
//	ERROR_READ_FILE   - oshibka chteniya fayla
//	ERROR_SEEK_FILE   - oshibka smesheniya fayla
//	SUDDEN_END_FILE   - vnezapniy konec fayla
//	ERROR_FORMAT      - oshibka formata fayla
int TBZip2Tbz2::
AnalyzeInfoOfArc(char* path)
{
	//otkrit' fayl dlya chteniya
	m_h = CreateFile(path, GENERIC_READ, FILE_SHARE_READ, 
			NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (m_h == INVALID_HANDLE_VALUE) 
		{ if (m_h) { ::CloseHandle(m_h); m_h = 0; } return TERROR_OPEN_FILE; }

	//razmer fayla arhiva
	m_ArchiveSize = GetArchSize();

	//nachalo razbora arhiva

	int flag = 0;
	//prochitat' zagolovok sleduyushego bloka
	flag = ReadArc(&m_ARC_HEAD, sizeof(m_ARC_HEAD));
	if (flag == TERROR_READ_FILE) { if (m_h) { ::CloseHandle(m_h); m_h = 0; } return flag; }
	//mojet fayl zakonchilsya
	if (flag == TSUDDEN_END_FILE)
	{
		if (m_h) { ::CloseHandle(m_h); m_h = 0; }
		return TMESSAGE_OK;
	}

	//esli rabochiy potok (v principe ne nujen NET CIKLA)
	if (m_BeginThread)
	{
		//procent vipolneniya raboti
		LONGLONG percent = 0;
		if (m_ArchiveSize) percent = 100 * GetArcPointer() / m_ArchiveSize;
		if (m_Percent != percent)	//chtob ne poslat' lishnie soobsheniya (oni tormozyat rabotu)
		{
			m_Percent = percent;
			::PostMessage(m_hWnd, WM_THREAD_PERCENT, 0, (LPARAM)m_Percent);
		}
		//net lis sobitiya zavershit' rabotu potoka
		if(::WaitForSingleObject(m_hEventEnd, 0) == WAIT_OBJECT_0)
		{ if (m_h) { ::CloseHandle(m_h); m_h = 0; } return TTERMINATE_THREAD; }
	}

	//proverka zagolovka arhiva
	if ((m_ARC_HEAD.SIGNATURE1 != 'B') ||
		(m_ARC_HEAD.SIGNATURE2 != 'Z') ||
		(m_ARC_HEAD.SIGNATURE3 != 'h')) MSG = TERROR_FORMAT;
	else MSG = TMESSAGE_OK;
	if (m_DetailLF == 2)
		m_LogFile->WriteMessage(m_LangMsg.BlockFile, MSG);
	if (MSG != TMESSAGE_OK)
		{ if (m_h) { ::CloseHandle(m_h); m_h = 0; } return MSG; }

	//metod sjatiya
	m_MethodPack = m_ARC_HEAD.METHOD;

	m_NumberFiles = 1;
	m_PackSizeFiles = m_ArchiveSize - sizeof(m_ARC_HEAD);
	if (m_h) { ::CloseHandle(m_h); m_h = 0; }
	return TMESSAGE_OK;
}
