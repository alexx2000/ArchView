/////////////////////////////////////////////////////////////////////////////
//TSqz.cpp
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TSqz.h"

/////////////////////////////////////////////////////////////////////////////
//FUNCTIONS PRIVATE
/////////////////////////////////////////////////////////////////////////////

//analiz zagolovka arhiva
//head - struktura zagolovka bloka
//return
//	MSG - resul'tat vipolneniya
int TSqz::
AnalyzeArcHead(SQZ_ARC_HEAD& head)
{
	//proverit' signaturu
	if ((head.ID1 != 'H') ||
		(head.ID2 != 'L') ||
		(head.ID3 != 'S') ||
		(head.ID4 != 'Q') ||
		(head.ID5 != 'Z')) return TERROR_FORMAT;

	//versiya dlya raspakovki
	m_UnpackVersion = head.UNP_VER / 10.0;
	//OS sozdaniya arhiva
	m_HostOS        = head.HOST_OS;

	//format vremeni
	if (head.FLAGS & 0x02) m_TimeFormat  = 1;	//DOS
	else m_TimeFormat = 0;	//UNIX
	//arhiv zablokirovan
	if (head.FLAGS & 0x04) m_ArchiveLock = TRUE;
	//est' parol'
	if (head.FLAGS & 0x08) m_Password    = TRUE;

	//razmer zagolovka arhiva
	m_loffset = sizeof(head);
	return TMESSAGE_OK;
}

//analiz zagolovka fayla
//head - struktura zagolovka bloka
//return
//	MSG - resul'tat vipolneniya
int TSqz::
AnalyzeFileHead(BYTE size)
{
	DWORD PACK_SIZE = 0;
	DWORD ORIG_SIZE = 0;
	DWORD FTIME     = 0;

	//prochitat' zagolovok fayla
	MSG = ReadArc(&m_FILE_HEAD, sizeof(m_FILE_HEAD));
	if (MSG != TMESSAGE_OK) return MSG;

	//sobrat' dannie
	PACK_SIZE = TakeDWord(
		TakeWord(m_FILE_HEAD.PACK_SIZE1, m_FILE_HEAD.PACK_SIZE2),
		TakeWord(m_FILE_HEAD.PACK_SIZE3, m_FILE_HEAD.PACK_SIZE4));
	ORIG_SIZE = TakeDWord(
		TakeWord(m_FILE_HEAD.ORIG_SIZE1, m_FILE_HEAD.ORIG_SIZE2),
		TakeWord(m_FILE_HEAD.ORIG_SIZE3, m_FILE_HEAD.ORIG_SIZE4));
	FTIME     = TakeDWord(
		TakeWord(m_FILE_HEAD.FTIME1, m_FILE_HEAD.FTIME2),
		TakeWord(m_FILE_HEAD.FTIME3, m_FILE_HEAD.FTIME4));
	
	//metod sjatiya
	BYTE Method = m_FILE_HEAD.METHOD & 0x0F;
	if (m_MethodPack < Method) m_MethodPack = Method;
	
	//rzmeri
	m_PackSizeFiles   += PACK_SIZE;
	m_UnpackSizeFiles += ORIG_SIZE;

	//eshe 1 fayl
	m_NumberFiles++;

	//propustit' nedochiatnnoe + 1 (HEAD_SUM[BYTE])
	m_loffset = size + 1 - sizeof(m_FILE_HEAD);
	MSG = SeekArc(m_loffset, NULL);
	if (MSG != TMESSAGE_OK) return MSG;

	m_loffset = PACK_SIZE;
	return TMESSAGE_OK;
}

/////////////////////////////////////////////////////////////////////////////
//FUNCTIONS PUBLIC
/////////////////////////////////////////////////////////////////////////////

//fayl yavlyaetsya arhivom SQZ
//path - imya fayla dlya testirovaniya
//return
//	TMESSAGE_OK     - ok
//	ERROR_OPEN_FILE - oshibka otckritiya fayla
//	ERROR_READ_FILE - oshibka chteniya fayla
//	ERROR_SEEK_FILE - oshibka smesheniya fayla
//	SUDDEN_END_FILE - vnezapniy konec fayla
//	ERROR_FORMAT    - oshibka formata fayla
int TSqz::
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
				SQZ_ARC_HEAD head;

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
				for (int i=0; i < len-7; i++)
				{
					if ((buffer[i]   == 'H') &&
						(buffer[i+1] == 'L') &&
						(buffer[i+2] == 'S') &&
						(buffer[i+3] == 'Q') &&
						(buffer[i+4] == 'Z'))
					{
						Flag = TMESSAGE_OK;

						head.ID1     = buffer[i];
						head.ID2     = buffer[i+1];
						head.ID3     = buffer[i+2];
						head.ID4     = buffer[i+3];
						head.ID5     = buffer[i+4];
						head.UNP_VER = buffer[i+5];
						head.HOST_OS = buffer[i+6];
						head.FLAGS   = buffer[i+7];

						//skol'ko prochitano nujnogo (nado vernut')
						m_loffset = - len + i + sizeof(m_ARC_HEAD);
						MSG = SeekArc(m_loffset, NULL);
						if (MSG != TMESSAGE_OK) Flag = MSG;
						m_loffset = 0;

						MSG = AnalyzeArcHead(head);
						if (MSG != TMESSAGE_OK) Flag = MSG;
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
int TSqz::
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
	int  flag = 0;
	BYTE ID   = 0;
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
		//prochitat' sleduyushiy ID
		flag = ReadArc(&ID, sizeof(ID));
		if (flag == TERROR_READ_FILE) { if (m_h) { ::CloseHandle(m_h); m_h = 0; } return flag; }
		//mojet fayl zakonchilsya
		if (flag == TSUDDEN_END_FILE)
		{
			CountRatioArchiveSize();
			CountRatioPackFileSize();
			if (m_h) { ::CloseHandle(m_h); m_h = 0; }
			return TMESSAGE_OK;
		}

		//eto blok fayla
		if (ID > 18)
		{
			MSG = AnalyzeFileHead(ID);
			if (m_DetailLF == 2)
				m_LogFile->WriteMessage(m_LangMsg.BlockFile, MSG);
			if (MSG != TMESSAGE_OK) { if (m_h) { ::CloseHandle(m_h); m_h = 0; } return MSG; }
		}
		//zagolovok kakogo bloka
		else
		{
			WORD size = 0;
			switch (ID)
			{
				//END_OF_ARC
				case 0x00:	CountRatioArchiveSize();
							CountRatioPackFileSize();
							if (m_DetailLF == 2)
								m_LogFile->WriteMessage(m_LangMsg.BlockArchiveEnd, TMESSAGE_OK);
							if (m_h) { ::CloseHandle(m_h); m_h = 0; }
							return TMESSAGE_OK;
							break;
				//blok COMMENT stariy
				case 0x01:	m_MainComment = TRUE;
							//chtenie razmera bloka
							MSG = ReadArc(&size, sizeof(size));
							if (m_DetailLF == 2)
								m_LogFile->WriteMessage(m_LangMsg.BlockArchiveComment, MSG);
							if (MSG != TMESSAGE_OK)
								{ if (m_h) { ::CloseHandle(m_h); m_h = 0; } return MSG; }
							m_loffset = size;
							break;
				//blok PASSWORD
				case 0x02:	m_Password = TRUE;
							//chtenie razmera bloka
							MSG = ReadArc(&size, sizeof(size));
							if (m_DetailLF == 2)
								m_LogFile->WriteMessage(m_LangMsg.BlockPassword, MSG);
							if (MSG != TMESSAGE_OK)
								{ if (m_h) { ::CloseHandle(m_h); m_h = 0; } return MSG; }
							m_loffset = size;
							break;
				//blok POSTFIX
				case 0x03:	//chtenie razmera bloka
							MSG = ReadArc(&size, sizeof(size));
							if (m_DetailLF == 2)
								m_LogFile->WriteMessage(m_LangMsg.BlockPostfix, MSG);
							if (MSG != TMESSAGE_OK)
								{ if (m_h) { ::CloseHandle(m_h); m_h = 0; } return MSG; }
							m_loffset = size;
							break;
				//blok SECURITY ENVELOPE
				case 0x04:	m_ArchiveLock = TRUE;
							//chtenie razmera bloka
							MSG = ReadArc(&size, sizeof(size));
							if (m_DetailLF == 2)
								m_LogFile->WriteMessage(m_LangMsg.BlockSecurityEnvelope, MSG);
							if (MSG != TMESSAGE_OK)
								{ if (m_h) { ::CloseHandle(m_h); m_h = 0; } return MSG; }
							m_loffset = size;
							break;
				//blok VOLUME LABEL
				case 0x05:	m_MultiArc = TRUE;
							//chtenie razmera bloka
							MSG = ReadArc(&size, sizeof(size));
							if (m_DetailLF == 2)
								m_LogFile->WriteMessage(m_LangMsg.BlockVolumeLabel, MSG);
							if (MSG != TMESSAGE_OK)
								{ if (m_h) { ::CloseHandle(m_h); m_h = 0; } return MSG; }
							m_loffset = size;
							break;
				//blok DIRECTORY
				case 0x06:	m_NumberFolders++;
							//chtenie razmera bloka
							MSG = ReadArc(&size, sizeof(size));
							if (m_DetailLF == 2)
								m_LogFile->WriteMessage(m_LangMsg.BlockFolder, MSG);
							if (MSG != TMESSAGE_OK)
								{ if (m_h) { ::CloseHandle(m_h); m_h = 0; } return MSG; }
							m_loffset = size;
							break;
				//blok COMMENT
				case 0x07:	m_MainComment = TRUE;
							//chtenie razmera bloka
							MSG = ReadArc(&size, sizeof(size));
							if (m_DetailLF == 2)
								m_LogFile->WriteMessage(m_LangMsg.BlockArchiveComment, MSG);
							if (MSG != TMESSAGE_OK)
								{ if (m_h) { ::CloseHandle(m_h); m_h = 0; } return MSG; }
							m_loffset = size;
							break;
				//bloki OTHERWISE
				default:	//chtenie razmera bloka
							MSG = ReadArc(&size, sizeof(size));
							if (m_DetailLF == 2)
								m_LogFile->WriteMessage(m_LangMsg.BlockUnknown, MSG);
							if (MSG != TMESSAGE_OK)
								{ if (m_h) { ::CloseHandle(m_h); m_h = 0; } return MSG; }
							m_loffset = size;
							break;
			}
		}
	}
}