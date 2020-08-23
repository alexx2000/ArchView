/////////////////////////////////////////////////////////////////////////////
//TLhaLzh.cpp
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TLhaLzh.h"

//obrabotka zagolovka fayla/papki
//head - struktura zagolovka bloka
//return
//	BlockType - tip bloka
//		0 - neizvestno
//		1 - fayl
//		2 - papka
//	MSG - resul'tat vipolneniya
int TLhaLzh::
AnalyzeFileHead(LHALZH_FILE_HEAD& head, int& BlockType)
{
	DWORD PACK_SIZE = 0;
	DWORD UNP_SIZE  = 0;
	DWORD FTIME     = 0;
	
	//sabrat' dannie iz BYTEs
	PACK_SIZE = TakeDWord(
		TakeWord(head.PACK_SIZE1, head.PACK_SIZE2),
		TakeWord(head.PACK_SIZE3, head.PACK_SIZE4));
	UNP_SIZE  = TakeDWord(
		TakeWord(head.UNP_SIZE1, head.UNP_SIZE2),
		TakeWord(head.UNP_SIZE3, head.UNP_SIZE4));
	FTIME     = TakeDWord(
		TakeWord(head.FTIME1, head.FTIME2),
		TakeWord(head.FTIME3, head.FTIME4));

	//proverka zagolovka fayla
	if ((head.METHOD1 != '-') ||
		(head.METHOD2 != 'l') ||
		(head.METHOD5 != '-')) return TERROR_FORMAT;

	//metod sjatiya i razmer slovarya
	BYTE Method   = -1;
	BYTE DictSize = 0;
	if ((head.METHOD3 == 'h') && (head.METHOD4 == '0'))
		{Method = 0x00; DictSize = 0;}
	if ((head.METHOD3 == 'h') && (head.METHOD4 == '1'))
		{Method = 0x01; DictSize = 4;}
	if ((head.METHOD3 == 'h') && (head.METHOD4 == '2'))
		{Method = 0x02; DictSize = 8;}
	if ((head.METHOD3 == 'h') && (head.METHOD4 == '3'))
		{Method = 0x03; DictSize = 8;}
	if ((head.METHOD3 == 'h') && (head.METHOD4 == '4'))
		{Method = 0x04; DictSize = 4;}
	if ((head.METHOD3 == 'h') && (head.METHOD4 == '5'))
		{Method = 0x05; DictSize = 8;}
	if ((head.METHOD3 == 'h') && (head.METHOD4 == '6'))
		{Method = 0x06; DictSize = 32;}
	if ((head.METHOD3 == 'h') && (head.METHOD4 == '7'))
		{Method = 0x07; DictSize = 64;}
	if ((head.METHOD3 == 'z') && (head.METHOD4 == 's'))
		{Method = 0x08; DictSize = 2;}
	if ((head.METHOD3 == 'z') && (head.METHOD4 == '4'))
		{Method = 0x09; DictSize = 0;}
	if ((head.METHOD3 == 'z') && (head.METHOD4 == '5'))
		{Method = 0x0A; DictSize = 4;}
	if (m_MethodPack < Method) m_MethodPack = Method;
	if (m_DictionarySize < DictSize) m_DictionarySize = DictSize;

	//eshe 1 fayl ili 1 papka
	if ((head.METHOD3 == 'h') && (head.METHOD4 == 'd'))
	{
		BlockType = 2;
		m_NumberFolders++;
	}
	else
	{
		BlockType = 1;
		m_NumberFiles++;
	}

	//razmeri
	m_PackSizeFiles   += PACK_SIZE;
	m_UnpackSizeFiles += UNP_SIZE;

	//nedochitannoe (po raznomu dlya zagolovkov raznogo urovnya)
	//dlya 0x02 head.HEAD_SIZE vklyuchaet (samogo sebya [BYTE] + head.HEAD_CHECKSUM [BYTE])
	if (head.HEAD_LEVEL == 0x02)
		m_loffset = head.HEAD_SIZE - sizeof(head);
	//dlya 0x00 i 0x01 head.HEAD_SIZE ne vklyuchaet (samogo sebya [BYTE] + head.HEAD_CHECKSUM 1 [BYTE])
	else
		m_loffset = head.HEAD_SIZE + 2 - sizeof(head);

	//propustit' ostavshuyusyu infu bloka
	MSG = SeekArc(m_loffset, NULL);
	if (MSG != TMESSAGE_OK) return MSG;
	
	//sam upakovanniy fayl
	m_loffset = PACK_SIZE;

	return TMESSAGE_OK;
}

/////////////////////////////////////////////////////////////////////////////
//FUNCTIONS PUBLIC
/////////////////////////////////////////////////////////////////////////////

//fayl yavlyaetsya arhivom LHA/LZH
//path - imya fayla dlya testirovaniya
//return
//	TMESSAGE_OK     - ok
//	ERROR_OPEN_FILE - oshibka otckritiya fayla
//	ERROR_READ_FILE - oshibka chteniya fayla
//	ERROR_SEEK_FILE - oshibka smesheniya fayla
//	SUDDEN_END_FILE - vnezapniy konec fayla
//	ERROR_FORMAT    - oshibka formata fayla
int TLhaLzh::
TestFile(char* path)
{
	//obnulenie dannih
	InitialParametrs();

	//eto LHA ili LZH arhiv
	char* ext  = 0;
	int   size = strlen(path);
	ext = path + size - 3;
	ext = _strlwr(ext);
	if ((strcmp(ext, "lzh") == 0) ||
		(strcmp(ext, "lzs") == 0))
		m_ArchiveType = TRUE;	//LZH
	
	//otkrit' fayl dlya chteniya
	m_h = CreateFile(path, GENERIC_READ, FILE_SHARE_READ, 
			NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (m_h == INVALID_HANDLE_VALUE) 
		{ if (m_h) { ::CloseHandle(m_h); m_h = 0; } return TERROR_OPEN_FILE; }

	//chtenie zagolovka arhiva
	MSG = ReadArc(&m_FILE_HEAD, sizeof(m_FILE_HEAD));
	if (MSG != TMESSAGE_OK) { if (m_h) { ::CloseHandle(m_h); m_h = 0; } return MSG; }

	//proverka zagolovka fayla
	if ((m_FILE_HEAD.METHOD1 != '-') ||
		(m_FILE_HEAD.METHOD2 != 'l') ||
		(m_FILE_HEAD.METHOD5 != '-')) MSG = TERROR_FORMAT;
	else MSG = TMESSAGE_OK;
	if (m_DetailLF == 2)
		m_LogFile->WriteMessage(m_LangMsg.SignatureArchive, MSG);
	if (MSG != TMESSAGE_OK)
	{
		//mojet eto SFX arhiv
		if (m_CheckSFX)
		{
			BOOL CheckSFX      = FALSE;	//bilali proverka na SFX modul
			int  Flag          = TERROR_FORMAT;
			WORD EXECUTABLE_ID = 0;

			//vstaem v nachalo fayla
			MSG = SeekArc(0, 0, FILE_BEGIN);
			if (MSG != TMESSAGE_OK) { if (m_h) { ::CloseHandle(m_h); m_h = 0; } return MSG; }
			MSG = ReadArc(&EXECUTABLE_ID, sizeof(EXECUTABLE_ID));
			if (MSG != TMESSAGE_OK) { if (m_h) { ::CloseHandle(m_h); m_h = 0; } return MSG; }
			if (EXECUTABLE_ID == 'ZM')
			{
				int      len     = m_MAX_LEN_SFX;
				BYTE     *buffer = 0;
				LONGLONG temp    = 0;

				CheckSFX = TRUE;

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
				for (int i=0; i < len-20; i++)
				{
					if ((buffer[i+2]  == '-')  &&
						(buffer[i+3]  == 'l')  &&
						(buffer[i+6]  == '-')  &&
						(buffer[i+20] >= 0x00) &&
						(buffer[i+20] <= 0x02))
					{
						Flag = TMESSAGE_OK;
						//smeshenie dlya dalneyshego razbora
						m_loffset += i + sizeof(EXECUTABLE_ID);
						m_SfxModule = i + sizeof(EXECUTABLE_ID);
						if (buffer) delete []buffer;
						buffer = 0;
						break;
					}
				}
				if (buffer) delete []buffer;
				buffer = 0;
			}

			//SFX ili net
			if ((m_DetailLF == 2) && (CheckSFX))
			{
				m_LogFile->WriteMessage(m_LangMsg.SFXModule, Flag);
				if (Flag == TMESSAGE_OK)
					m_LogFile->WriteMessage(m_LangMsg.SignatureArchive, Flag);
			}
			if (Flag != TMESSAGE_OK) { if (m_h) { ::CloseHandle(m_h); m_h = 0; } return Flag; }
		}
		//ne proveryaem na SFX
		else
		{
			//zakritie fayla
			if (m_h) { ::CloseHandle(m_h); m_h = 0; }
			return TERROR_FORMAT;
		}
	}

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
int TLhaLzh::
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
			
		//zagolovok fayla
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
		if (MSG != TMESSAGE_OK)
			{ if (m_h) { ::CloseHandle(m_h); m_h = 0; } return MSG; }
	}
}