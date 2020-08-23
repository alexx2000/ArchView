/////////////////////////////////////////////////////////////////////////////
//TCpio.cpp
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TCpio.h"

/////////////////////////////////////////////////////////////////////////////
//FUNCTIONS PRIVATE
/////////////////////////////////////////////////////////////////////////////

//obrabotka zagolovka fayla/papki
//head - struktura zagolovka bloka
//return
//	BlockType - tip bloka
//		0 - neizvestno
//		1 - fayl
//		2 - papka
//	MSG - resul'tat vipolneniya
int TCpio::
AnalyzeFileHead(CPIO_FILE_HEAD& head, int& BlockType)
{
	DWORD FILE_SIZE = 0;
	DWORD NAME_SIZE = 0;
	DWORD ATTR      = 0;
	DWORD tdwrd     = 0;
	
	char* ttt;
	ttt = head.FILD01;
	ttt = head.ATTR;
	ttt = head.FILD03;
	ttt = head.FILD04;
	ttt = head.TYPE;
	ttt = head.FILD06;
	ttt = head.FILE_SIZE;
	ttt = head.FILD08;
	ttt = head.FILD09;
	ttt = head.FILD10;
	ttt = head.FILD11;
	ttt = head.NAME_SIZE;
	ttt = head.FILD13;

	//razmer fayla
	head.FILE_SIZE[CPIO_SIZE_FILD_DIGI_SIZE] = '\0';
	sscanf(head.FILE_SIZE, "%x", &FILE_SIZE);

	//eshe 1 fayl ili papka
	head.ATTR[CPIO_SIZE_FILD_DIGI_SIZE] = '\0';
	sscanf(head.ATTR, "%x", &ATTR);
	if ((tdwrd=(ATTR & 0x0000FF00)) == 0x00004100)
	{
		BlockType = 2;
		m_NumberFolders++;
	}
	else
	{
		if (ATTR != 0x00000000)
		{
			BlockType = 1;
			m_NumberFiles++;
		}
	}

	//rasmeri
	m_PackSizeFiles   += FILE_SIZE;
	m_UnpackSizeFiles += FILE_SIZE;

	//razmer imeni fayla
	head.NAME_SIZE[CPIO_SIZE_FILD_DIGI_SIZE] = '\0';
	sscanf(head.NAME_SIZE, "%x", &NAME_SIZE);

	//propustit' imya fayla
	MSG = SeekArc(NAME_SIZE, NULL);
	if (MSG != TMESSAGE_OK) return MSG;

	//sam fayl
	m_loffset = FILE_SIZE;

	return TMESSAGE_OK;
}

/////////////////////////////////////////////////////////////////////////////
//FUNCTIONS PUBLIC
/////////////////////////////////////////////////////////////////////////////

//fayl yavlyaetsya arhivom CPIO
//path - imya fayla dlya testirovaniya
//return
//	TMESSAGE_OK     - ok
//	ERROR_OPEN_FILE - oshibka otckritiya fayla
//	ERROR_READ_FILE - oshibka chteniya fayla
//	ERROR_SEEK_FILE - oshibka smesheniya fayla
//	SUDDEN_END_FILE - vnezapniy konec fayla
//	ERROR_FORMAT    - oshibka formata fayla
int TCpio::
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
	MSG = ReadArc(m_SIGNATURE, CPIO_SIZE_FILD_SIGNATURE);
	if (MSG != TMESSAGE_OK) { if (m_h) { ::CloseHandle(m_h); m_h = 0; } return MSG; }
	m_SIGNATURE[CPIO_SIZE_FILD_SIGNATURE] = '\0';

	//proverka zagolovka arhiva
	if (atoi(m_SIGNATURE) != 70701) MSG = TERROR_FORMAT; //"070701"
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
int TCpio::
AnalyzeInfoOfArc(char* path)
{
	//otkrit' fayl dlya chteniya
	m_h = CreateFile(path, GENERIC_READ, FILE_SHARE_READ, 
			NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (m_h == INVALID_HANDLE_VALUE) 
		{ if (m_h) { ::CloseHandle(m_h); m_h = 0; } return TERROR_OPEN_FILE; }

	//razmer fayla arhiva
	m_ArchiveSize = GetArchSize();

	//koncevoy blok arhiva vsegda takoy (size = 104 (13*8))
	//eto nujno tol'ko dlya vedeniya logfile
	char* endblock = 0;
	if (m_DetailLF == 2)
		endblock = "00000000000000000000000000000000000000010000000000000000000000000000000000000000000000000000000b00000000";
	else endblock = "";

	//razbor arhiva
	int   flag      = 0;
	DWORD SIGNATURE = 0;
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

		//prochitat' signaturu
		flag = ReadArc(m_SIGNATURE, CPIO_SIZE_FILD_SIGNATURE);
		if (flag == TERROR_READ_FILE) { if (m_h) { ::CloseHandle(m_h); m_h = 0; } return flag; }
		//mojet fayl zakonchilsya
		if (flag == TSUDDEN_END_FILE)
		{
			CountRatioArchiveSize();
			CountRatioPackFileSize();
			if (m_h) { ::CloseHandle(m_h); m_h = 0; }
			return TMESSAGE_OK;
		}
		m_SIGNATURE[CPIO_SIZE_FILD_SIGNATURE] = '\0';
		SIGNATURE = 0;
		sscanf(m_SIGNATURE, "%x", &SIGNATURE);

		//proverit' signaturu
		if (SIGNATURE != 0x070701) //"070701"
		{
			//mojet nado proizvesti viravnivanie
			//peremeshaemsya na signaturu nazad
			m_loffset = - CPIO_SIZE_FILD_SIGNATURE;
			MSG = SeekArc(m_loffset, NULL);
			if (MSG != TMESSAGE_OK) { if (m_h) { ::CloseHandle(m_h); m_h = 0; } return MSG; }
			int      len     = 16;	//dlya viravnivaniya
			BYTE     *buffer = 0;
			LONGLONG temp    = 0;
			int Flag = TERROR_FORMAT;

			//ne perebrat' razmer arhiva
			temp = GetArchSize() - GetArcPointer();
			if (temp < len) len = (int)temp;

			buffer = new BYTE[len];
			MSG = ReadArc(buffer, len);
			if (MSG != TMESSAGE_OK)
			{
				if (buffer) delete []buffer; buffer = 0;
				if (m_h) { ::CloseHandle(m_h); m_h = 0; } return MSG;
			}
			
			//iskat' metku arhiva
			for (int i=0; i < len-5; i++)
			{
				if ((buffer[i]   == 0x30) &&
					(buffer[i+1] == 0x37) &&
					(buffer[i+2] == 0x30) &&
					(buffer[i+3] == 0x37) && 
					(buffer[i+4] == 0x30) &&
					(buffer[i+5] == 0x31))
				{
					Flag = TMESSAGE_OK;
					//smestitsya na tochku posle signaturi
					m_loffset += -len + i + 2*CPIO_SIZE_FILD_SIGNATURE;
					MSG = SeekArc(m_loffset, NULL);
					if (MSG != TMESSAGE_OK) Flag = MSG;

					if (buffer) delete []buffer;
					buffer = 0;
					break;
				}
			}
			if (buffer) delete []buffer;
			buffer = 0;
			//virovnyalis' ili net
			if (Flag != TMESSAGE_OK) { if (m_h) { ::CloseHandle(m_h); m_h = 0; } return Flag; }
		}

		//prochitat' blok fayla
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
		m_FILE_HEAD.FILD13[CPIO_SIZE_FILD_DIGI_SIZE] = '\0';

		m_BlockType = 0;
		MSG = AnalyzeFileHead(m_FILE_HEAD, m_BlockType);
		if (m_DetailLF == 2)
		{
			if (strcmp(m_FILE_HEAD.FILD01, endblock) == 0)
				m_LogFile->WriteMessage(m_LangMsg.BlockArchiveEnd, MSG);
			else
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
		}
		if (MSG != TMESSAGE_OK)
			{ if (m_h) { ::CloseHandle(m_h); m_h = 0; } return MSG; }
	}
}
