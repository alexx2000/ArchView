/////////////////////////////////////////////////////////////////////////////
//TArcPak.cpp
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TArcPak.h"
#include <string.h>

/////////////////////////////////////////////////////////////////////////////
//FUNCTIONS PRIVATE
/////////////////////////////////////////////////////////////////////////////

//prochitat' kommentariy arhiva
//return
//	MSG - resul'tat vipolneniya
int TArcPak::
ReadComment()
{
	//proverka i predustanovka ukazatelya
	if (m_pTextComment) { delete[] m_pTextComment; m_pTextComment = 0; }
	m_pTextComment = new char[m_CommentSize+1];
	//chitat' kommentariy
	//smestitsya na 32 BYTE (s konca zapisey faylov)
	//no uje prochitana m_BLOCK_HEAD (2 BYTE)
	m_loffset = m_CommentSize - sizeof(m_BLOCK_HEAD);
	MSG = SeekArc(m_loffset, 0);
	if (MSG != TMESSAGE_OK) return MSG;
	MSG = ReadArc(m_pTextComment, m_CommentSize);
	if (MSG != TMESSAGE_OK)
	{
		if (m_pTextComment) {delete[] m_pTextComment; m_pTextComment = 0;}
		return MSG;
	}
	else	//stroka zakanchivaetsya 0x00
		m_pTextComment[m_CommentSize] = '\0';
	return TMESSAGE_OK;
}

//obrabotka zagolovka fayla
//head - struktura zagolovka bloka
//return
//	MSG - resul'tat vipolneniya
int TArcPak::
AnalyzeFileHead(ARCPAK_BLOCK_HEAD& head)
{
	DWORD PACK_SIZE = 0;
	DWORD ORIG_SIZE = 0;
	DWORD FTIME     = 0;
	
	MSG = ReadArc(&m_FILE_HEAD, sizeof(m_FILE_HEAD));
	if (MSG != TMESSAGE_OK)
		{ if (m_h) { ::CloseHandle(m_h); m_h = 0; } return MSG; }
	//sabrat' dannie iz BYTEs
	PACK_SIZE = TakeDWord(
		TakeWord(m_FILE_HEAD.PACK_SIZE1, m_FILE_HEAD.PACK_SIZE2),
		TakeWord(m_FILE_HEAD.PACK_SIZE3, m_FILE_HEAD.PACK_SIZE4));
	ORIG_SIZE = TakeDWord(
		TakeWord(m_FILE_HEAD.ORIG_SIZE1, m_FILE_HEAD.ORIG_SIZE2),
		TakeWord(m_FILE_HEAD.ORIG_SIZE3, m_FILE_HEAD.ORIG_SIZE4));
	FTIME = TakeDWord(
		TakeWord(m_FILE_HEAD.FTIME1, m_FILE_HEAD.FTIME2),
		TakeWord(m_FILE_HEAD.FTIME3, m_FILE_HEAD.FTIME4));

	//eshe fayl ili papka (papka only ARC)
	if (head.METHOD == 0x1E) m_NumberFolders++;
	else
	{
		m_NumberFiles++;
		m_loffset          = PACK_SIZE;
		m_PackSizeFiles   += PACK_SIZE;
		m_UnpackSizeFiles += ORIG_SIZE;
	}

	//metod sjatiya
	if (head.METHOD <= 0x0B)
	{
		BYTE Method = head.METHOD;
		if (m_MethodPack < Method) m_MethodPack = Method;
	}

	return TMESSAGE_OK;
}

/////////////////////////////////////////////////////////////////////////////
//FUNCTIONS PUBLIC
/////////////////////////////////////////////////////////////////////////////

//fayl yavlyaetsya arhivom ARC/PAK
//path - imya fayla dlya testirovaniya
//return
//	TMESSAGE_OK     - ok
//	ERROR_OPEN_FILE - oshibka otckritiya fayla
//	ERROR_READ_FILE - oshibka chteniya fayla
//	ERROR_SEEK_FILE - oshibka smesheniya fayla
//	SUDDEN_END_FILE - vnezapniy konec fayla
//	ERROR_FORMAT    - oshibka formata fayla
int TArcPak::
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
	MSG = ReadArc(&m_BLOCK_HEAD, sizeof(m_BLOCK_HEAD));
	if (MSG != TMESSAGE_OK)
		{ if (m_h) { ::CloseHandle(m_h); m_h = 0; } return MSG; }

	//proverka id arhiva
	if (m_BLOCK_HEAD.ID != 0x1A) MSG = TERROR_FORMAT;
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
int TArcPak::
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
			if(MSG != TMESSAGE_OK)
				{ if (m_h) { ::CloseHandle(m_h); m_h = 0; } return MSG; }
		}
		else
		{
			MSG = SeekArc(m_loffset, &m_hoffset);
			if(MSG != TMESSAGE_OK)
				{ if (m_h) { ::CloseHandle(m_h); m_h = 0; } return MSG; }
		}
		m_loffset = 0;
		m_hoffset = 0;
		//prochitat' zagolovok sleduyushego bloka
		flag = ReadArc(&m_BLOCK_HEAD, sizeof(m_BLOCK_HEAD));
		if (flag == TERROR_READ_FILE) { if (m_h) { ::CloseHandle(m_h); m_h = 0; } return flag; }
		//mojet fayl zakonchilsya
		if (flag == TSUDDEN_END_FILE)
		{
			CountRatioArchiveSize();
			CountRatioPackFileSize();
			if (m_h) { ::CloseHandle(m_h); m_h = 0; }
			return TMESSAGE_OK;
		}

		switch (m_BLOCK_HEAD.METHOD)
		{
			//konec arhiva
			case 0x00:	CountRatioArchiveSize();
						CountRatioPackFileSize();
						if (m_DetailLF == 2)
							m_LogFile->WriteMessage(m_LangMsg.BlockArchiveEnd, TMESSAGE_OK);
						//fayl eshe ne zakonchilsya
						if (GetArcPointer() < GetArchSize())
						{
							//prochitat' zagolovok sleduyushego bloka
							flag = ReadArc(&m_BLOCK_HEAD, sizeof(m_BLOCK_HEAD));
							if (flag == TERROR_READ_FILE) { if (m_h) { ::CloseHandle(m_h); m_h = 0; } return flag; }
							//libo eto PAK arhiv libo ARC s kommentariem
							if ((m_BLOCK_HEAD.ID == 0xFE) && (m_BLOCK_HEAD.METHOD = 0x02))
								m_ArchiveType = TRUE;	//PAK
							else
							{
								m_MainComment = TRUE;
								//kommentariy vsegda 32 simvola
								m_CommentSize = ARC_MAX_LEN_COMMENT;
								MSG = ReadComment();
								if (m_DetailLF == 2)
									m_LogFile->WriteMessage(m_LangMsg.BlockArchiveComment, MSG);
								if (MSG != TMESSAGE_OK)
									{ if (m_h) { ::CloseHandle(m_h); m_h = 0; } return MSG; }
							}
						}
						if (m_h) { ::CloseHandle(m_h); m_h = 0; }
						return TMESSAGE_OK;
						break;
			//blok otkritiya novoy papki (only ARC)
			case 0x1E:	MSG = AnalyzeFileHead(m_BLOCK_HEAD);
						if (m_DetailLF == 2)
							m_LogFile->WriteMessage(m_LangMsg.BlockFolderBegin, MSG);
						if (MSG != TMESSAGE_OK)
							{ if (m_h) { ::CloseHandle(m_h); m_h = 0; } return MSG; }
						break;
			//marker konca papki (only ARC)
			case 0x1F:	if (m_DetailLF == 2)
							m_LogFile->WriteMessage(m_LangMsg.BlockFolderEnd, TMESSAGE_OK);
						break;
			default:	MSG = AnalyzeFileHead(m_BLOCK_HEAD);
						if (m_DetailLF == 2)
						{
							if (m_BLOCK_HEAD.METHOD <= 0x13)
								m_LogFile->WriteMessage(m_LangMsg.BlockFile, MSG);
							else
							{
								if (m_BLOCK_HEAD.METHOD <= 0x1D)
									m_LogFile->WriteMessage(m_LangMsg.BlockInformationalItems, MSG);
								else
									m_LogFile->WriteMessage(m_LangMsg.BlockControlItems, MSG);
							}
						}
						if (MSG != TMESSAGE_OK)
							{ if (m_h) { ::CloseHandle(m_h); m_h = 0; } return MSG; }
						break;
		}
	}
}
