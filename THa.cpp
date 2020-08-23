/////////////////////////////////////////////////////////////////////////////
//THa.cpp
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "THa.h"

/////////////////////////////////////////////////////////////////////////////
//FUNCTIONS PRIVATE
/////////////////////////////////////////////////////////////////////////////

//proverka zagolovka arhiva
//head - struktura zagolovka bloka
//return
//	MSG - resul'tat vipolneniya
int THa::
AnalyzeArcHead(HA_HEAD& head)
{
	if (head.ID != 'AH') return TERROR_FORMAT;
	return TMESSAGE_OK;
}

//obrabotka zagolovka fayla/papki
//head - struktura zagolovka bloka
//return
//	BlockType - tip bloka
//		0 - neizvestno
//		1 - fayl
//		2 - papka
//	MSG - resul'tat vipolneniya
int THa::
AnalyzeFileHead(HA_FILE_HEAD& head, int& BlockType)
{
	DWORD PACK_SIZE = 0;
	DWORD ORIG_SIZE = 0;
	DWORD FTIME     = 0;
	
	//sabrat' dannie iz BYTEs
	PACK_SIZE = TakeDWord(
		TakeWord(head.PACK_SIZE1, head.PACK_SIZE2),
		TakeWord(head.PACK_SIZE3, head.PACK_SIZE4));
	ORIG_SIZE = TakeDWord(
		TakeWord(head.ORIG_SIZE1, head.ORIG_SIZE2),
		TakeWord(head.ORIG_SIZE3, head.ORIG_SIZE4));
	FTIME     = TakeDWord(
		TakeWord(head.FTIME1, head.FTIME2),
		TakeWord(head.FTIME3, head.FTIME4));

	int      len     = MAX_LEN_STR;
	int      toffset = 0;
	LONGLONG temp    = 0;
	//propustit' put' k faylu
	{
		temp = m_ArchiveSize - GetArcPointer();
		if (temp < len) len = (int)temp;

		MSG = ReadArc(m_buffer, len);
		if (MSG != TMESSAGE_OK) return MSG;

		//dlina stroki (stroka zakanchivaetsya \0 (0x00))
		toffset = strlen(m_buffer);
		//mojet eto bol'she chem pozvoleno
		if (toffset > len) return TERROR_FORMAT;

		//skol'ko prochitano nujnogo (nado vernut')
		m_loffset = - len + 1 + toffset;
		MSG = SeekArc(m_loffset, NULL);
		if (MSG != TMESSAGE_OK) return MSG;
	}
	//propustit' imya fayla
	{
		temp = m_ArchiveSize - GetArcPointer();
		if (temp < len) len = (int)temp;

		MSG = ReadArc(m_buffer, len);
		if (MSG != TMESSAGE_OK) return MSG;

		//dlina stroki (stroka zakanchivaetsya \0 (0x00))
		toffset = strlen(m_buffer);
		//mojet eto bol'she chem pozvoleno
		if (toffset > len) return TERROR_FORMAT;

		//skol'ko prochitano nujnogo (nado vernut')
		m_loffset = - len + 1 + toffset;
		MSG = SeekArc(m_loffset, NULL);
		if (MSG != TMESSAGE_OK) return MSG;
	}

	//dobavka k zagolovku fayla v arhive
	MSG = ReadArc(&m_FILE_ADD, sizeof(m_FILE_ADD));
	if (MSG != TMESSAGE_OK) return MSG;

	//sam upakovanniy fayl
	m_loffset = PACK_SIZE;

	BYTE Method = 0;
	m_PackSizeFiles += PACK_SIZE;
	m_UnpackSizeFiles += ORIG_SIZE;
	m_HostOS = m_FILE_ADD.HOST_OS;
	
	//metod upakovki
	Method = head.METHOD & 0x0F;
	switch (Method)
	{
		case 0x0E:	//directoriya
					BlockType = 2;
					m_NumberFolders++;
					break;
		case 0x0F:	break;
		default:	//fayl
					if (m_MethodPack < Method) m_MethodPack = Method;
					BlockType = 1;
					m_NumberFiles++;
					break;
	}

	return TMESSAGE_OK;
}

/////////////////////////////////////////////////////////////////////////////
//FUNCTIONS PUBLIC
/////////////////////////////////////////////////////////////////////////////

//fayl yavlyaetsya arhivom HA
//path - imya fayla dlya testirovaniya
//return
//	TMESSAGE_OK     - ok
//	ERROR_OPEN_FILE - oshibka otckritiya fayla
//	ERROR_READ_FILE - oshibka chteniya fayla
//	ERROR_SEEK_FILE - oshibka smesheniya fayla
//	SUDDEN_END_FILE - vnezapniy konec fayla
//	ERROR_FORMAT    - oshibka formata fayla
int THa::
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
	m_loffset = sizeof(m_ARC_HEAD);

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
int THa::
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

		m_BlockType = 0;
		MSG = AnalyzeFileHead(m_FILE_HEAD, m_BlockType);
		if (m_DetailLF == 2)
		{
			switch (m_BlockType)
			{
			case 1: m_LogFile->WriteMessage(m_LangMsg.BlockFile, MSG);
					break;
			case 2: m_LogFile->WriteMessage(m_LangMsg.BlockFolder, MSG);
					break;
			default: m_LogFile->WriteMessage(m_LangMsg.BlockUnknown, MSG);
					break;
			}
		}
		if (MSG != TMESSAGE_OK) { if (m_h) { ::CloseHandle(m_h); m_h = 0; } return MSG; }
	}
}
