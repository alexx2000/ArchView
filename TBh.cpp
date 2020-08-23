/////////////////////////////////////////////////////////////////////////////
//TBh.cpp
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TBh.h"

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
int TBh::
AnalyzeFileHead(BH_FILE_HEAD& head, int& BlockType)
{
	WORD  twrd         = 0;
	DWORD ID           = 0;
	DWORD PACK_SIZE    = 0;
	DWORD ORIG_SIZE    = 0;
	DWORD FTIME        = 0;
	WORD  ATTR         = 0;
	WORD  HEAD_SIZE    = 0;
	WORD  FLAGS        = 0;
	WORD  NAME_SIZE    = 0;
	WORD  COMMENT_SIZE = 0;
	
	//sabrat' dannie iz BYTEs
	ID = TakeDWord(
		TakeWord(head.ID1, head.ID2),
		TakeWord(head.ID3, head.ID4));
	PACK_SIZE = TakeDWord(
		TakeWord(head.PACK_SIZE1, head.PACK_SIZE2),
		TakeWord(head.PACK_SIZE3, head.PACK_SIZE4));
	ORIG_SIZE = TakeDWord(
		TakeWord(head.ORIG_SIZE1, head.ORIG_SIZE2),
		TakeWord(head.ORIG_SIZE3, head.ORIG_SIZE4));
	FTIME = TakeDWord(
		TakeWord(head.FTIME1, head.FTIME2),
		TakeWord(head.FTIME3, head.FTIME4));
	ATTR         = TakeWord(head.ATTR1, head.ATTR2);
	HEAD_SIZE    = TakeWord(head.HEAD_SIZE1, head.HEAD_SIZE2);
	FLAGS        = TakeWord(head.FLAGS1, head.FLAGS2);
	NAME_SIZE    = TakeWord(head.NAME_SIZE1, head.NAME_SIZE2);
	COMMENT_SIZE = TakeWord(head.COMMENT_SIZE1, head.COMMENT_SIZE2);

	//proverka signaturi
	if (ID != 0x07054842) return TERROR_FORMAT;

	//versiya zapakovki raspakovki
	BYTE Ver = head.VER_CREATED;
	if (m_PackVersion < Ver) m_PackVersion = Ver;
	Ver = head.VER_EXTRACT;
	if (m_UnpackVersion < Ver) m_UnpackVersion = Ver;

	//est' commentariy
	if (COMMENT_SIZE != 0) m_FileComment = TRUE;

	//est' parol'
	if (FLAGS & 0x0001) m_Password = TRUE;

	//metod sjatiya
	//ispol'zuem promejutochnoe twrd t.k.
	//v chislah po neskol'ko edinic v dvoichnom vide
	//i mojet vozniknut' oshibka esli sovpadet tol'ko po odnoy edinice
	BYTE Method = -1;
	if (FLAGS & 0x0800)                  Method = 0x03;
	if (twrd=(FLAGS & 0x0802) == 0x0802) Method = 0x04;
	if (twrd=(FLAGS & 0x0804) == 0x0804) Method = 0x01;
	if (twrd=(FLAGS & 0x0300) == 0x0300) Method = 0x02;
	if (twrd=(FLAGS & 0x0302) == 0x0302) Method = 0x02;
	if (twrd=(FLAGS & 0x0304) == 0x0304) Method = 0x02;
	if (twrd=(FLAGS & 0x0306) == 0x0306) Method = 0x02;
	if (m_MethodPack < Method) m_MethodPack = Method;

	//v arhive eshe 1 fayl ili 1 papka
	if ((twrd=(ATTR & 0x0010)) == 0x0010)
	{
		BlockType = 2;
		m_NumberFolders++;
	}
	else
	{
		BlockType = 1;
		m_NumberFiles++;
	}

	//razmeri faylov
	m_PackSizeFiles   += PACK_SIZE;
	m_UnpackSizeFiles += ORIG_SIZE;

	//propustit' ostavsheesya v zagolovke
	m_loffset = HEAD_SIZE - sizeof(head);
	MSG = SeekArc(m_loffset, NULL);
	if (MSG != TMESSAGE_OK) return MSG;

	//sam upakovanniy fayl
	m_loffset = PACK_SIZE;

	return TMESSAGE_OK;
}

/////////////////////////////////////////////////////////////////////////////
//FUNCTIONS PUBLIC
/////////////////////////////////////////////////////////////////////////////

//fayl yavlyaetsya arhivom BH
//path - imya fayla dlya testirovaniya
//return
//	TMESSAGE_OK     - ok
//	ERROR_OPEN_FILE - oshibka otckritiya fayla
//	ERROR_READ_FILE - oshibka chteniya fayla
//	ERROR_SEEK_FILE - oshibka smesheniya fayla
//	SUDDEN_END_FILE - vnezapniy konec fayla
//	ERROR_FORMAT    - oshibka formata fayla
int TBh::
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
	DWORD ID = 0;
	MSG = ReadArc(&ID, sizeof(ID));
	if (MSG != TMESSAGE_OK) { if (m_h) { ::CloseHandle(m_h); m_h = 0; } return MSG; }

	//proverka signaturi
	if (ID != 0x07054842) MSG = TERROR_FORMAT;
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
				for (int i=0; i < len-10; i++)
				{
					if ((buffer[i]   == 'B') &&
						(buffer[i+1] == 'H') &&
						(buffer[i+2] == 0x05) &&
						(buffer[i+3] == 0x07))
					{
						Flag = TMESSAGE_OK;
						//smeshenie dlya dalneyshego razbora
						m_loffset = i + sizeof(EXECUTABLE_ID);
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
int TBh::
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
