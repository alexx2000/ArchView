/////////////////////////////////////////////////////////////////////////////
//TZoo.cpp
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TZoo.h"

/////////////////////////////////////////////////////////////////////////////
//FUNCTIONS PRIVATE
/////////////////////////////////////////////////////////////////////////////

//prochitat' kommenariy arhiva
//return
//	MSG - resul'tat vipolneniya
int TZoo::
ReadComment()
{
	//esli est' kommentariy prochitat' ego
	if (m_MainComment)
	{
		MSG = SeekArc(m_CommentOffset, NULL, FILE_BEGIN);
		if (MSG != TMESSAGE_OK) { if (m_h) { ::CloseHandle(m_h); m_h = 0; } return MSG; }
		//proverka i predustanovka ukazatelya
		if (m_pTextComment) { delete[] m_pTextComment; m_pTextComment = 0; }
		m_pTextComment = new char[m_CommentSize+1];
		//chitat' kommentariy
		MSG = ReadArc(m_pTextComment, m_CommentSize);
		if (MSG != TMESSAGE_OK)
		{
			if (m_pTextComment) {delete[] m_pTextComment; m_pTextComment = 0;}
			return MSG;
		}
		else	//stroka zakanchivaetsya 0x00
		{
			m_pTextComment[m_CommentSize] = '\0';
			return TMESSAGE_OK;
		}
	}
	return TMESSAGE_OK;
}

//obrabotka zagolovka arhiva
//head - struktura zagolovka bloka
//return
//	MSG - resul'tat vipolneniya
int TZoo::
AnalyzeArcHead(ZOO_HEAD& head)
{
	DWORD ID             = 0;
	DWORD OFFSET         = 0;
	DWORD COMMENT_OFFSET = 0;
	WORD  VER_CREATED    = 0;
	WORD  COMMENT_SIZE   = 0;
	
	//sabrat' dannie iz BYTEs
	ID             = TakeDWord(
		TakeWord(head.ID1, head.ID2),
		TakeWord(head.ID3, head.ID4));
	VER_CREATED = TakeWord(head.VER_CREATED1, head.VER_CREATED2);
	OFFSET         = TakeDWord(
		TakeWord(head.OFFSET1, head.OFFSET2),
		TakeWord(head.OFFSET3, head.OFFSET4));
	COMMENT_OFFSET = TakeDWord(
		TakeWord(head.COMMENT_OFFSET1, head.COMMENT_OFFSET2),
		TakeWord(head.COMMENT_OFFSET3, head.COMMENT_OFFSET4));
	COMMENT_SIZE   = TakeWord(head.COMMENT_SIZE1, head.COMMENT_SIZE2);

	//proverka signaturi
	if (ID != 0xFDC4A7DC) return TERROR_FORMAT;

	//versiya upakovshika
	m_PackVersion = (double)VER_CREATED;

	//versiya dlya raspkovki
	m_UnpackVersion = (double)head.VER_EXTRACT;

	//est' li kommentariy
	if (COMMENT_SIZE > 0)
	{
		m_MainComment   = TRUE;
		//zapomnit' dlinu kommentariya (v simvolah)
		//i ego smeshenie ot nachala fayla
		m_CommentSize   = COMMENT_SIZE;
		m_CommentOffset = COMMENT_OFFSET;
	}

	//razmer zagolovka arhiva
	m_loffset = OFFSET;

	return TMESSAGE_OK;
}

//obrabotka zagolovka fayla
//head - struktura zagolovka bloka
//return
//	MSG - resul'tat vipolneniya
int TZoo::
AnalyzeFileHead(ZOO_FILE_HEAD& head)
{
	DWORD ID           = 0;
	DWORD PACK_SIZE    = 0;
	DWORD UNP_SIZE     = 0;
	DWORD FTIME        = 0;
	DWORD OFFSET       = 0;
	WORD  COMMENT_SIZE = 0;
	
	//sabrat' dannie iz BYTEs
	ID           = TakeDWord(
		TakeWord(head.ID1, head.ID2),
		TakeWord(head.ID3, head.ID4));
	PACK_SIZE    = TakeDWord(
		TakeWord(head.PACK_SIZE1, head.PACK_SIZE2),
		TakeWord(head.PACK_SIZE3, head.PACK_SIZE4));
	UNP_SIZE     = TakeDWord(
		TakeWord(head.UNP_SIZE1, head.UNP_SIZE2),
		TakeWord(head.UNP_SIZE3, head.UNP_SIZE4));
	FTIME        = TakeDWord(
		TakeWord(head.FTIME1, head.FTIME2),
		TakeWord(head.FTIME3, head.FTIME4));
	OFFSET       = TakeDWord(
		TakeWord(head.OFFSET1, head.OFFSET2),
		TakeWord(head.OFFSET3, head.OFFSET4));
	COMMENT_SIZE = TakeWord(head.COMMENT_SIZE1, head.COMMENT_SIZE2);

	//proverka signaturi
	if (ID != 0xFDC4A7DC) return TERROR_FORMAT;

	//metod sjatiya
	BYTE Method = head.METHOD;
	if (m_MethodPack < Method) m_MethodPack = Method;

	//eshe 1 fayl
	m_NumberFiles++;

	//est' li kommentariy
	if (COMMENT_SIZE > 0) m_FileComment = TRUE;

	//razmeri
	m_PackSizeFiles   += PACK_SIZE;
	m_UnpackSizeFiles += UNP_SIZE;

	//nedochitannoe
	m_loffset = (unsigned)(OFFSET - GetArcPointer());

	return TMESSAGE_OK;
}

/////////////////////////////////////////////////////////////////////////////
//FUNCTIONS PUBLIC
/////////////////////////////////////////////////////////////////////////////

//fayl yavlyaetsya arhivom ZOO
//path - imya fayla dlya testirovaniya
//return
//	TMESSAGE_OK     - ok
//	ERROR_OPEN_FILE - oshibka otckritiya fayla
//	ERROR_READ_FILE - oshibka chteniya fayla
//	ERROR_SEEK_FILE - oshibka smesheniya fayla
//	SUDDEN_END_FILE - vnezapniy konec fayla
//	ERROR_FORMAT    - oshibka formata fayla
int TZoo::
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
	MSG = AnalyzeArcHead(m_ARC_HEAD);
	if (m_DetailLF == 2)
		m_LogFile->WriteMessage(m_LangMsg.BlockArhiveHeader, MSG);
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
int TZoo::
AnalyzeInfoOfArc(char* path)
{
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
		if ((m_FILE_HEAD.OFFSET1 == 0) &&
			(m_FILE_HEAD.OFFSET2 == 0) &&
			(m_FILE_HEAD.OFFSET3 == 0) &&
			(m_FILE_HEAD.OFFSET4 == 0))
		{
			CountRatioArchiveSize();
			CountRatioPackFileSize();
			//esli est' kommentariy prochitat' ego
			if (m_MainComment)
			{
				MSG = ReadComment();
				if (m_DetailLF == 2)
					m_LogFile->WriteMessage(m_LangMsg.BlockArchiveComment, MSG);
				if (MSG != TMESSAGE_OK) { if (m_h) { ::CloseHandle(m_h); m_h = 0; } return MSG; }
			}
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