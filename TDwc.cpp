/////////////////////////////////////////////////////////////////////////////
//TDwc.cpp
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TDwc.h"

/////////////////////////////////////////////////////////////////////////////
//FUNCTIONS PRIVATE
/////////////////////////////////////////////////////////////////////////////

//obrabotka zagolovka arhiva
//head - struktura zagolovka bloka
//return
//	MSG - resul'tat vipolneniya
int TDwc::
AnalyzeArcEnd(DWC_END& head)
{
	DWORD ENTRIES_COUNT = 0;
	WORD HEAD_SIZE      = 0;
	WORD ENTRIES_SIZE   = 0;
	
	//sabrat' dannie iz BYTEs
	ENTRIES_COUNT = TakeDWord(
		TakeWord(head.ENTRIES_COUNT1, head.ENTRIES_COUNT2),
		TakeWord(head.ENTRIES_COUNT3, head.ENTRIES_COUNT4));
	HEAD_SIZE = TakeWord(head.HEAD_SIZE1, head.HEAD_SIZE2);
	ENTRIES_SIZE = TakeWord(head.ENTRIES_SIZE1, head.ENTRIES_SIZE2);

	//kolichestvo faylov v arhive
	m_NumberFiles = ENTRIES_COUNT;

	//proverka signaturi
	if ((head.ID1 != 'D') ||
		(head.ID2 != 'W') ||
		(head.ID3 != 'C')) return TERROR_FORMAT;

	//proverka znacheniya razmera bloka konca
	if (HEAD_SIZE != 0x1B) return TERROR_FORMAT;

	//proverka znacheniya razmera bloka directorii
	if (ENTRIES_SIZE != 0x22) return TERROR_FORMAT;

	//smeshenie dlya dal'neyshego razbora
	//s konca razmer bloka konca + razmer bloka fayla * kol etih blokov
	m_loffset = (unsigned)GetArchSize() - HEAD_SIZE;
	for (unsigned i=0; i<ENTRIES_COUNT; i++)
		m_loffset -= ENTRIES_SIZE;

	//proverka poluchennogo smesheniya
	if (m_loffset < 0) return TERROR_FORMAT;

	return TMESSAGE_OK;
}

//obrabotka zagolovka fayla
//head - struktura zagolovka bloka
//return
//	MSG - resul'tat vipolneniya
int TDwc::
AnalyzeFileHead(DWC_FILE_HEAD& head)
{
	DWORD PACK_SIZE = 0;
	DWORD ORIG_SIZE = 0;
	DWORD FTIME     = 0;
	DWORD OFFSET    = 0;
	
	//sabrat' dannie iz BYTEs
	PACK_SIZE = TakeDWord(
		TakeWord(head.PACK_SIZE1, head.PACK_SIZE2),
		TakeWord(head.PACK_SIZE3, head.PACK_SIZE4));
	ORIG_SIZE = TakeDWord(
		TakeWord(head.ORIG_SIZE1, head.ORIG_SIZE2),
		TakeWord(head.ORIG_SIZE3, head.ORIG_SIZE4));
	FTIME = TakeDWord(
		TakeWord(head.FTIME1, head.FTIME2),
		TakeWord(head.FTIME3, head.FTIME4));
	OFFSET = TakeDWord(
		TakeWord(head.OFFSET1, head.OFFSET2),
		TakeWord(head.OFFSET3, head.OFFSET4));

	//metod sjatiya
	BYTE Method = (head.METHOD & 0x0F);
	//predustanovka m_MethodPack dlya pervogo prohoda
	if (m_MethodPack == -1) m_MethodPack = 2;
	if (m_MethodPack > Method) m_MethodPack = Method;

	//esty' li parol'
	if (head.METHOD & 0x40) m_Password = TRUE;

	//est' li SFX modul'
	if (m_FirstFile == FALSE)
	{
		if (OFFSET != 0) m_SfxModule = OFFSET;
		m_FirstFile = TRUE;
	}

	//est' li kommentariy
	if (head.COMMENT_SIZE > 0) m_FileComment = TRUE;

	//razmeri
	m_PackSizeFiles   += PACK_SIZE;
	m_UnpackSizeFiles += ORIG_SIZE;

	return TMESSAGE_OK;
}

/////////////////////////////////////////////////////////////////////////////
//FUNCTIONS PUBLIC
/////////////////////////////////////////////////////////////////////////////

//fayl yavlyaetsya arhivom DWC
//path - imya fayla dlya testirovaniya
//return
//	TMESSAGE_OK     - ok
//	ERROR_OPEN_FILE - oshibka otckritiya fayla
//	ERROR_READ_FILE - oshibka chteniya fayla
//	ERROR_SEEK_FILE - oshibka smesheniya fayla
//	SUDDEN_END_FILE - vnezapniy konec fayla
//	ERROR_FORMAT    - oshibka formata fayla
int TDwc::
TestFile(char* path)
{
	//obnulenie dannih
	InitialParametrs();
	
	//otkrit' fayl dlya chteniya
	m_h = CreateFile(path, GENERIC_READ, FILE_SHARE_READ, 
			NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (m_h == INVALID_HANDLE_VALUE) 
		{ if (m_h) { ::CloseHandle(m_h); m_h = 0; } return TERROR_OPEN_FILE; }

	//smestimsya chtobi prochitat' konec arhiva
	MSG = SeekArc((unsigned)GetArchSize() - sizeof(m_ARC_END), NULL);
	if (MSG != TMESSAGE_OK) { if (m_h) { ::CloseHandle(m_h); m_h = 0; } return MSG; }

	//chtenie zagolovka arhiva
	MSG = ReadArc(&m_ARC_END, sizeof(m_ARC_END));
	if (MSG != TMESSAGE_OK) { if (m_h) { ::CloseHandle(m_h); m_h = 0; } return MSG; }

	//proverka zagolovka arhiva
	MSG = AnalyzeArcEnd(m_ARC_END);
	if (m_DetailLF == 2)
		m_LogFile->WriteMessage(m_LangMsg.BlockArchiveEnd, MSG);
	if (MSG != TMESSAGE_OK) { if (m_h) { ::CloseHandle(m_h); m_h = 0; } return MSG; }

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
int TDwc::
AnalyzeInfoOfArc(char* path)
{
	m_FirstFile = FALSE;

	//otkrit' fayl dlya chteniya
	m_h = CreateFile(path, GENERIC_READ, FILE_SHARE_READ, 
			NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (m_h == INVALID_HANDLE_VALUE) 
		{ if (m_h) { ::CloseHandle(m_h); m_h = 0; } return TERROR_OPEN_FILE; }

	//razmer fayla arhiva
	m_ArchiveSize = GetArchSize();

	//razbor arhiva
	int flag = 0;
	while(TRUE)
	{
		//esli rabochiy potok
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

		//sdelat' smeshenie
		if (m_hoffset == 0)
		{
			MSG = SeekArc(m_loffset, NULL);
			if(MSG != TMESSAGE_OK) { if (m_h) { ::CloseHandle(m_h); m_h = 0; } return MSG; }
		}
		else
		{
			MSG = SeekArc(m_loffset, &m_hoffset);
			if(MSG != TMESSAGE_OK) { if (m_h) { ::CloseHandle(m_h); m_h = 0; } return MSG; }
		}
		m_loffset = 0;
		m_hoffset = 0;
		//prochitat' zagolovok sleduyushego bloka
		flag = ReadArc(&m_FILE_HEAD, sizeof(m_FILE_HEAD));
		if (flag == TERROR_READ_FILE) { if (m_h) { ::CloseHandle(m_h); m_h = 0; } return flag; }
		//mojet fayl zakonchilsya
		if (flag == TSUDDEN_END_FILE)
		{
			CountRatioArchiveSize();
			CountRatioPackFileSize();
			if (m_h) { ::CloseHandle(m_h); m_h = 0; }
			return TMESSAGE_OK;
		}
			
		//zagolovok fayla
		MSG = AnalyzeFileHead(m_FILE_HEAD);
		if (m_DetailLF == 2)
			m_LogFile->WriteMessage(m_LangMsg.BlockFile, MSG);
		if (MSG != TMESSAGE_OK) { if (m_h) { ::CloseHandle(m_h); m_h = 0; } return MSG; }
	}
}