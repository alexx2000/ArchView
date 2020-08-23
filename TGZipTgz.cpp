/////////////////////////////////////////////////////////////////////////////
//TGZipTgz.cpp
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TGZipTgz.h"

/////////////////////////////////////////////////////////////////////////////
//FUNCTIONS PRIVATE
/////////////////////////////////////////////////////////////////////////////

//obrabotka zagolovka fayla
//head - struktura zagolovka bloka
//return
//	MSG - resul'tat vipolneniya
int TGZipTgz::
AnalyzeFileHead(GZIPTGZ_FILE_HEAD& head)
{
	//proverit' signaturu
	if ((head.ID1 != 0x1F) || (head.ID2 != 0x8B)) return TERROR_FORMAT;

	//metod upakovki
	m_MethodPack    = head.METHOD;
	m_MethodPackAdd = head.XFLAGS;

	//Host OS
	m_HostOS = head.HOST_OS;

	//est' li oblast' EXTRA
	WORD len_extra;
	if (head.FLAGS & 0x04)
	{
		//prochitat' dlinu oblasti EXTRA
		MSG = ReadArc(&len_extra, sizeof(len_extra));
		if (MSG != TMESSAGE_OK) return MSG;
		//propustit' oblast'
		MSG = SeekArc(len_extra, NULL);
		if (MSG != TMESSAGE_OK) return MSG;
	}

	int      len     = MAX_LEN_STR;
	int      toffset = 0;
	LONGLONG temp    = 0;

	//esli est' imya fayla originala
	if (head.FLAGS & 0x08)
	//propustit' imya fayla originala
	{
		temp = m_ArchiveSize - GetArcPointer();
		if (temp < len) len = (int)temp;

		MSG = ReadArc(m_buffer, len);
		if (MSG != TMESSAGE_OK) return MSG;

		//dlina stroki (stroka zakanchivaetsya \0 (0x00))
		toffset = strlen(m_buffer);
		//mojet eto bol'she chem pozvoleno
		if (toffset > len) return TERROR_FORMAT;

		//imya arhiva pri sozdanii
		if (m_pOriginalName) {delete[] m_pOriginalName; m_pOriginalName = 0;}
		m_pOriginalName = new char[toffset+1];
		strcpy(m_pOriginalName, m_buffer);

		//skol'ko prochitano nujnogo (nado vernut')
		m_loffset = - len + 1 + toffset;
		MSG = SeekArc(m_loffset, NULL);
		if (MSG != TMESSAGE_OK) return MSG;
	}

	//esli est' kommentariy fayla/arhiva
	if (head.FLAGS & 0x10)
	//propustit' kommentariy fayla
	{
		//est' kommentariy
		m_MainComment = TRUE;
		
		temp = m_ArchiveSize - GetArcPointer();
		if (temp < len) len = (int)temp;

		MSG = ReadArc(m_buffer, len);
		if (MSG != TMESSAGE_OK) return MSG;

		//dlina stroki (stroka zakanchivaetsya \0 (0x00))
		toffset = strlen(m_buffer);
		//commentariy arhiva
		if (toffset != 0) m_MainComment = TRUE;
		//mojet eto bol'she chem pozvoleno
		if (toffset > len) return TERROR_FORMAT;

		//skol'ko prochitano nujnogo (nado vernut')
		m_loffset = - len + 1 + toffset;
		MSG = SeekArc(m_loffset, NULL);
		if (MSG != TMESSAGE_OK) return MSG;
	}

	//esli est' pole CRC16
	WORD CRC16;
	if (head.FLAGS & 0x02)
	{
		//prochitat' dlinu oblasti EXTRA
		MSG = ReadArc(&CRC16, sizeof(CRC16));
		if (MSG != TMESSAGE_OK) return MSG;
	}

	//zapomnim posiciyu nachala zaarhivirivannogo fayla
	LONGLONG Start = GetArcPointer();

	//smestitsya i prochitat' konec arhiva
	m_loffset = 0 - sizeof(m_ARC_END);
	MSG = SeekArc(m_loffset, NULL, FILE_END);
	if (MSG != TMESSAGE_OK) return MSG;
	MSG = ReadArc(&m_ARC_END, sizeof(m_ARC_END));
	if (MSG != TMESSAGE_OK) return MSG;

	//rasmeri
	m_PackSizeFiles   += GetArcPointer() - Start - sizeof(m_ARC_END);
	m_UnpackSizeFiles += m_ARC_END.ORIG_SIZE;
	m_NumberFiles      = 1;
	return TMESSAGE_OK;
}

/////////////////////////////////////////////////////////////////////////////
//FUNCTIONS PUBLIC
/////////////////////////////////////////////////////////////////////////////

//fayl yavlyaetsya arhivom GZIP
//path - imya fayla dlya testirovaniya
//return
//	TMESSAGE_OK     - ok
//	ERROR_OPEN_FILE - oshibka otckritiya fayla
//	ERROR_READ_FILE - oshibka chteniya fayla
//	ERROR_SEEK_FILE - oshibka smesheniya fayla
//	SUDDEN_END_FILE - vnezapniy konec fayla
//	ERROR_FORMAT    - oshibka formata fayla
int TGZipTgz::
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
	WORD ID = 0;
	MSG = ReadArc(&ID, sizeof(ID));
	if (MSG != TMESSAGE_OK) { if (m_h) { ::CloseHandle(m_h); m_h = 0; } return MSG; }

	//proverka zagolovka arhiva
	if (ID != 0x8B1F) MSG = TERROR_FORMAT;
	else MSG = TMESSAGE_OK;
	if (m_DetailLF == 2)
		m_LogFile->WriteMessage(m_LangMsg.SignatureArchive, MSG);
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
int TGZipTgz::
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
	int flag;
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
	if (flag != TMESSAGE_OK) { if (m_h) { ::CloseHandle(m_h); m_h = 0; } return flag; }

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

	MSG = AnalyzeFileHead(m_FILE_HEAD);
	if (m_DetailLF == 2)
	{
		m_LogFile->WriteMessage(m_LangMsg.BlockFile, MSG);
		if (MSG == TMESSAGE_OK)
			m_LogFile->WriteMessage(m_LangMsg.BlockArchiveEnd, MSG);
	}
	if (MSG != TMESSAGE_OK) { if (m_h) { ::CloseHandle(m_h); m_h = 0; } return MSG; }

	CountRatioArchiveSize();
	CountRatioPackFileSize();
	if (m_h) { ::CloseHandle(m_h); m_h = 0; }
	return TMESSAGE_OK;
}
