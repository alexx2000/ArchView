/////////////////////////////////////////////////////////////////////////////
//TCab.cpp
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TCab.h"

/////////////////////////////////////////////////////////////////////////////
//FUNCTIONS PRIVATE
/////////////////////////////////////////////////////////////////////////////

//proverka zagolovka arhiva
//head - struktura zagolovka bloka
//return
//	MSG - resul'tat vipolneniya
int TCab::
AnalyzeArcHead(CAB_ARC_HEAD& head)
{
	//proverka signaturi
	if (head.SIGNATURE != 'FCSM') return TERROR_FORMAT;

	//kolichestvo papok i faylov vmeste
	m_NumberEntries = head.NUMBER_FILES;

	//versii upakovki/raspakovki
	m_PackVersion   = head.VERSION_PACK;
	m_UnpackVersion = head.VERSION_UNPACK;

	//mnogotomnost'
	if (head.FLAGS != 0) m_MultiArc = TRUE;

	//nomer toma mnogotomnogo arhiva
	//+1 t.k. v nam nado s 1 a ne s 0
	m_VolumeNumber = head.VOLUME_NUM + 1;

	//smestitsya i prochitat' konec zagolovka arhiva
	m_loffset = head.OFFSET - sizeof(m_ARC_HEAD) - sizeof(m_ARC_ADD);
	MSG = SeekArc(m_loffset, NULL);
	if(MSG != TMESSAGE_OK) return MSG;
	//chtenie zagolovka arhiva
	MSG = ReadArc(&m_ARC_ADD, sizeof(m_ARC_ADD));
	if (MSG != TMESSAGE_OK) return MSG;

	//metod sjatiya
	m_MethodPack    = m_ARC_ADD.METHOD;
	//metod sjatiya dopolnitel'no
	m_MethodPackAdd = m_ARC_ADD.METHOD_ADD;

	//razmer bloka
	m_loffset = head.OFFSET;

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
int TCab::
AnalyzeFileHead(CAB_FILE_HEAD& head, int& BlockType)
{
	//raspakovanniy razmer
	m_UnpackSizeFiles += head.UNP_SIZE;

	//eshe 1 fayl ili papka
	if (head.ATTR & 0x0010)
	{
		BlockType = 2;
		m_NumberFolders++;
	}
	else
	{
		BlockType = 1;
		m_NumberFiles++;
	}

	//est' li parol'
	if (head.ATTR & 0x0008) m_Password++;

	int len = MAX_LEN_STR;
	int toffset = 0;
	LONGLONG temp;
	//propustit' put' i imya fayla
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

	m_loffset = 0;
	return TMESSAGE_OK;
}

/////////////////////////////////////////////////////////////////////////////
//FUNCTIONS PUBLIC
/////////////////////////////////////////////////////////////////////////////

//fayl yavlyaetsya arhivom CAB
//path - imya fayla dlya testirovaniya
//return
//	TMESSAGE_OK     - ok
//	ERROR_OPEN_FILE - oshibka otckritiya fayla
//	ERROR_READ_FILE - oshibka chteniya fayla
//	ERROR_SEEK_FILE - oshibka smesheniya fayla
//	SUDDEN_END_FILE - vnezapniy konec fayla
//	ERROR_FORMAT    - oshibka formata fayla
int TCab::
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
	if (MSG != TMESSAGE_OK)
	{
		//mojet eto SFX arhiv
		if (m_CheckSFX)
		{
			BOOL CheckSFX      = FALSE;		//bilali proverka na SFX modul
			int  Flag          = TERROR_FORMAT;
			WORD EXECUTABLE_ID = 0;

			//vstaem v nachalo fayla
			MSG = SeekArc(0, 0, FILE_BEGIN);
			if (MSG != TMESSAGE_OK) { if (m_h) { ::CloseHandle(m_h); m_h = 0; } return MSG; }
			MSG = ReadArc(&EXECUTABLE_ID, sizeof(EXECUTABLE_ID));
			if (MSG != TMESSAGE_OK) { if (m_h) { ::CloseHandle(m_h); m_h = 0; } return MSG; }
			if (EXECUTABLE_ID == 'ZM')
			{
				int  len      = m_MAX_LEN_SFX;
				BYTE *buffer  = 0;
				LONGLONG temp = 0;

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
				int i;
				for (i=0; i < len-7; i++)
				{
					if ((buffer[i]   == 'M')  &&
						(buffer[i+1] == 'S')  &&
						(buffer[i+2] == 'C')  &&
						(buffer[i+3] == 'F')  &&
						(buffer[i+4] == 0x00) &&
						(buffer[i+5] == 0x00) &&
						(buffer[i+6] == 0x00) &&
						(buffer[i+7] == 0x00))
					{
						Flag = TMESSAGE_OK;
						//smeshenie dlya dalneyshego razbora
						m_SfxModule = i + sizeof(EXECUTABLE_ID);

						//vernutsya na tockku nachala arhiva
						m_loffset = - len + i;
						MSG = SeekArc(m_loffset, NULL);
						if (MSG != TMESSAGE_OK) Flag = MSG;

						//chtenie zagolovka arhiva
						MSG = ReadArc(&m_ARC_HEAD, sizeof(m_ARC_HEAD));
						if (MSG != TMESSAGE_OK) Flag = MSG;
						//proverka zagolovka arhiva
						MSG = AnalyzeArcHead(m_ARC_HEAD);
						if (MSG != TMESSAGE_OK) Flag = MSG;
						m_loffset += m_SfxModule;

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
					m_LogFile->WriteMessage(m_LangMsg.BlockArhiveHeader, Flag);
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
int TCab::
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
	for (int i=0; i<m_NumberEntries; i++)
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

	CountRatioArchiveSize();
	CountRatioPackFileSize();
	if (m_h) { ::CloseHandle(m_h); m_h = 0; }
	return TMESSAGE_OK;
}
