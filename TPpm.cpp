/////////////////////////////////////////////////////////////////////////////
//TPpm.cpp
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TPpm.h"

/////////////////////////////////////////////////////////////////////////////
//FUNCTIONS PUBLIC
/////////////////////////////////////////////////////////////////////////////

//fayl yavlyaetsya arhivom PPM
//path - imya fayla dlya testirovaniya
//return
//	TMESSAGE_OK     - ok
//	ERROR_OPEN_FILE - oshibka otckritiya fayla
//	ERROR_READ_FILE - oshibka chteniya fayla
//	ERROR_SEEK_FILE - oshibka smesheniya fayla
//	SUDDEN_END_FILE - vnezapniy konec fayla
//	ERROR_FORMAT    - oshibka formata fayla
int TPpm::
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
	if ((m_ARC_HEAD.SIGNATURE1 != 'P') ||
		(m_ARC_HEAD.SIGNATURE2 != 'P') ||
		(m_ARC_HEAD.SIGNATURE3 != 'M') ||
		(m_ARC_HEAD.SIGNATURE4 != 'D') ||
		(m_ARC_HEAD.SIGNATURE5 != '_') ||
		(m_ARC_HEAD.SIGNATURE6 != 'A')) MSG = TERROR_FORMAT;
	else MSG = TMESSAGE_OK;
	if (m_DetailLF == 2)
		m_LogFile->WriteMessage(m_LangMsg.SignatureArchive, MSG);
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
int TPpm::
AnalyzeInfoOfArc(char* path)
{
	//otkrit' fayl dlya chteniya
	m_h = CreateFile(path, GENERIC_READ, FILE_SHARE_READ, 
			NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (m_h == INVALID_HANDLE_VALUE) 
		{ if (m_h) { ::CloseHandle(m_h); m_h = 0; } return TERROR_OPEN_FILE; }

	//razmer fayla arhiva
	m_ArchiveSize = GetArchSize();

	int flag = 0;
	//prochitat' zagolovok sleduyushego bloka
	flag = ReadArc(&m_ARC_HEAD, sizeof(m_ARC_HEAD));
	if (flag != TMESSAGE_OK) { if (m_h) { ::CloseHandle(m_h); m_h = 0; } return MSG; }

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
	if ((m_ARC_HEAD.SIGNATURE1 != 'P') ||
		(m_ARC_HEAD.SIGNATURE2 != 'P') ||
		(m_ARC_HEAD.SIGNATURE3 != 'M') ||
		(m_ARC_HEAD.SIGNATURE4 != 'D') ||
		(m_ARC_HEAD.SIGNATURE5 != '_') ||
		(m_ARC_HEAD.SIGNATURE6 != 'A')) MSG = TERROR_FORMAT;
	else MSG = TMESSAGE_OK;
	if (m_DetailLF == 2)
		m_LogFile->WriteMessage(m_LangMsg.BlockFile, MSG);
	if (MSG != TMESSAGE_OK)
		{ if (m_h) { ::CloseHandle(m_h); m_h = 0; } return MSG; }

	//metod sjatiya
	m_MethodPack = m_ARC_HEAD.METHOD;

	//ob'eam ispol'zuemoy pamyati
	m_MemorySize = m_ARC_HEAD.MEMORY_SIZE + 1;

	//soberem dannoe
	DWORD ORIG_SIZE;
	ORIG_SIZE = TakeDWord(
		TakeWord(m_ARC_HEAD.ORIG_SIZE1, m_ARC_HEAD.ORIG_SIZE2),
		TakeWord(m_ARC_HEAD.ORIG_SIZE3, m_ARC_HEAD.ORIG_SIZE4));

	//razmer fayla do sjatiya
	m_UnpackSizeFiles = ORIG_SIZE;

	m_NumberFiles = 1;
	//-5 konec arhiva CRC[4 BYTE] + END_ARC[1 BYTE[0x00]]
	m_PackSizeFiles = m_ArchiveSize - sizeof(m_ARC_HEAD) - 5;

	//rascheti sjatiya
	CountRatioArchiveSize();
	CountRatioPackFileSize();
	if (m_h) { ::CloseHandle(m_h); m_h = 0; }
	return TMESSAGE_OK;
}
