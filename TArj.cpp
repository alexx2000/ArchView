/////////////////////////////////////////////////////////////////////////////
//TArj.cpp
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TArj.h"

/////////////////////////////////////////////////////////////////////////////
//FUNCTIONS PRIVATE
/////////////////////////////////////////////////////////////////////////////

//prochitat' kommentariy arhiva
//return
//	MSG - resul'tat vipolneniya
int TArj::
ReadComment()
{
	//predustanovka ukazatelya
	if (m_pTextComment) { delete[] m_pTextComment; m_pTextComment = 0; }
	m_pTextComment = new char[m_CommentSize+1];
	//skopirovat' kommentariy
	strcpy(m_pTextComment, m_buffer_cmt);
	return TMESSAGE_OK;
}

//opredelenie nomera toma dlya mnogotomnogo arhiva
//imya fayla arhiva
void TArj::
DefineNumberVolume(char* path)
{
	//esli est' original'noe imya arhiva, to opredelim po nemu
	if (strlen(m_pOriginalName) != 0) 
		path = m_pOriginalName;

	char* strnum = 0;
	char* substr = 0;
	int   size   = strlen(path);

	//obichniy sposob numeracii tomov *.aXX
	if (size < 2) return;		//proverka dlini imeni
	strnum = path + size - 2;	//videlenie poslednih 2h simvolov
	//proverka na rasshirenie ARJ
	if ((strcmp(strnum, "RJ") == 0) ||
		(strcmp(strnum, "Rj") == 0) ||
		(strcmp(strnum, "rJ") == 0) ||
		(strcmp(strnum, "rj") == 0))
		m_VolumeNumber += 1;
	else
	{
		m_VolumeNumber = atoi(strnum);
		//esli m_VolumeNumber = 0 znachit ne opredeleno
		if (m_VolumeNumber != 0)	//0 - error ili "00"
			m_VolumeNumber += 1;
		else
			if (strnum = "00") m_VolumeNumber = 1;
	}

	//noviy sposob numeracii tomov *.arj.XXX
	substr = strstr(path, ".arj.");
	if (!substr) return;
	substr = substr + strlen(".arj.");
	if (strlen(substr) >= 3)		//minimum ostavshegosya (001) = 3
	{
		m_VolumeNumber = atoi(strnum);
	}
}

//analiz zagolovka arhiva
//head - struktura zagolovka bloka
//return
//	MSG - resul'tat vipolneniya
int TArj::
AnalyzeArcHead(ARJ_BLOCK_HEAD& head)
{
	DWORD TIME_CREATED  = 0;
	DWORD TIME_MODIFIED = 0;
	
	//proverit' HEADER_ID
	if (head.HEADER_ID != 0xEA60) return TERROR_FORMAT;
	
	//mnogotomnost'
	if (head.FLAGS & 0x04) m_MultiArc           = TRUE;
	//informaciya dlya vosstanovleniya
	if (head.FLAGS & 0x08) m_ARJ_RecoveryRecord = TRUE;
	//arhiv zablokirovan
	if (head.FLAGS & 0x40) m_AuthVerification   = TRUE;
	if (head.FLAGS & 0x02) m_AuthVerification   = TRUE; //old (obsolete)

	//sobrat' vremya sozdaniya i posledney modifikacii arhiva
	TIME_CREATED  = TakeDWord(head.DATE1, head.DATE2);
	TIME_MODIFIED = TakeDWord(head.PACK_SIZE1, head.PACK_SIZE2);
	DateTimeDosFormat(TIME_CREATED, m_DateCreated, m_TimeCreated);
	DateTimeDosFormat(TIME_MODIFIED, m_DateModified, m_TimeModified);

	//dannie po elektronnoy podpisi
	m_ARJ_SecurityPosition = TakeDWord(head.CRC1, head.CRC2);
	m_ARJ_SecurityLength   = head.ACCESS;

	double Ver;
	m_HostOS = head.HOST_OS;
	Ver      = (double)head.VER_CREATED / 10.0;
	if (m_PackVersion < Ver)   m_PackVersion   = Ver;
	Ver      = (double)head.VER_EXTRACT / 10.0;
	if (m_UnpackVersion < Ver) m_UnpackVersion = Ver;

	//propustit' nedochiatnnoe + 4 (HEADER_ID[WORD] + HEAD_SIZE[WORD])
	m_loffset = head.HDR_SIZE + 4 - sizeof(head);
	MSG = SeekArc(m_loffset, NULL);
	if (MSG != TMESSAGE_OK) return MSG;

	int      len     = MAX_LEN_STR;
	int      toffset = 0;
	LONGLONG temp    = 0;
	//propustit' imya arhiva
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
	//propustit' kommentariy arhiva
	len = ARJ_MAX_LEN_COMMENT;
	{
		temp = m_ArchiveSize - GetArcPointer();
		if (temp < len) len = (int)temp;

		MSG = ReadArc(m_buffer_cmt, len);
		if (MSG != TMESSAGE_OK) return MSG;

		//dlina stroki (stroka zakanchivaetsya \0 (0x00))
		toffset = strlen(m_buffer_cmt);
		//commentariy arhiva
		if (toffset != 0)
		{
			m_MainComment = TRUE;
			//zapomnit; kommentariy
			m_CommentSize = toffset;
			ReadComment();
		}
		//mojet eto bol'she chem pozvoleno
		if (toffset > len) return TERROR_FORMAT;

		//skol'ko prochitano nujnogo (nado vernut')
		m_loffset = - len + 1 + toffset;
		MSG = SeekArc(m_loffset, NULL);
		if (MSG != TMESSAGE_OK) return MSG;
	}

	//konec zagolovka
	char endbuf[6] = {NULL};
	MSG = ReadArc(endbuf, 6);
	if (MSG != TMESSAGE_OK) return MSG;
	//propustit' konec zgolovka
	m_loffset = TakeWord(endbuf[4], endbuf[5]);
	return TMESSAGE_OK;
}

//analiz zagolovka fayla/papki
//head - struktura zagolovka bloka
//return
//	MSG - resul'tat vipolneniya
int TArj::
AnalyzeFileHead(ARJ_BLOCK_HEAD& head)
{
	DWORD PACK_SIZE = 0;
	DWORD ORIG_SIZE = 0;
	DWORD CRC       = 0;

	//sobrat' dannie
	PACK_SIZE = TakeDWord(head.PACK_SIZE1, head.PACK_SIZE2);
	ORIG_SIZE = TakeDWord(head.ORIG_SIZE1, head.ORIG_SIZE2);
	CRC       = TakeDWord(head.CRC1, head.CRC2);
	
	//proverit' HEADER_ID
	if (head.HEADER_ID != 0xEA60) return TERROR_FORMAT;
	
	//parol'
	if (head.FLAGS & 0x01) m_Password = TRUE;
	//mnogotomnost'
	if (head.FLAGS & 0x04) m_MultiArc = TRUE;
	//mnogotomnost'
	if (head.FLAGS & 0x08) m_MultiArc = TRUE;

	double Ver;
	BYTE Method;
	m_PackSizeFiles   += PACK_SIZE;
	m_UnpackSizeFiles += ORIG_SIZE;
	m_HostOS           = head.HOST_OS;
	Ver = (double)head.VER_CREATED / 10.0;
	if (m_PackVersion < Ver)   m_PackVersion   = Ver;
	Ver = (double)head.VER_EXTRACT / 10.0;
	if (m_UnpackVersion < Ver) m_UnpackVersion = Ver;
	Method = head.METHOD;
	if (m_MethodPack < Method) m_MethodPack    = Method;

	//eshe 1 papka ili fayl
	if (head.FILE_TYPE == 0x03) m_NumberFolders++;
	else m_NumberFiles++;

	//propustit' nedochiatnnoe + 4 (HEADER_ID[WORD] + HEAD_SIZE[WORD])
	m_loffset = head.HDR_SIZE + 4 - sizeof(head);
	MSG = SeekArc(m_loffset, NULL);
	if (MSG != TMESSAGE_OK) return MSG;

	int      len     = MAX_LEN_STR;
	int      toffset = 0;
	LONGLONG temp    = 0;
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
	//propustit' kommentariy fayla
	len = ARJ_MAX_LEN_COMMENT;
	{
		temp = m_ArchiveSize - GetArcPointer();
		if (temp < len) len = (int)temp;

		MSG = ReadArc(m_buffer_cmt, len);
		if (MSG != TMESSAGE_OK) return MSG;

		//dlina stroki (stroka zakanchivaetsya \0 (0x00))
		toffset = strlen(m_buffer_cmt);
		//commentariy fayla
		if (toffset != 0) m_FileComment = TRUE;
		//mojet eto bol'she chem pozvoleno
		if (toffset > len) return TERROR_FORMAT;

		//skol'ko prochitano nujnogo (nado vernut')
		m_loffset = - len + 1 + toffset;
		MSG = SeekArc(m_loffset, NULL);
		if (MSG != TMESSAGE_OK) return MSG;
	}

	//konec zagolovka
	char endbuf[6] = {NULL};
	MSG = ReadArc(endbuf, 6);
	if (MSG != TMESSAGE_OK) return MSG;
	//propustit' konec zgolovka
	m_loffset = TakeWord(endbuf[4], endbuf[5]);
	MSG = SeekArc(m_loffset, NULL);
	if (MSG != TMESSAGE_OK) return MSG;

	m_loffset = PACK_SIZE;
	return TMESSAGE_OK;
}

//analiz zagolovka glavi
//head - struktura zagolovka bloka
//return
//	MSG - resul'tat vipolneniya
int TArj::
AnalyzeChapterHead(ARJ_BLOCK_HEAD& head)
{
	//proverit' HEADER_ID
	if (head.HEADER_ID != 0xEA60) return TERROR_FORMAT;
	
	//parol'
	if (head.FLAGS & 0x01) m_Password = TRUE;
	//mnogotomnost'
	if (head.FLAGS & 0x04) m_MultiArc = TRUE;
	//mnogotomnost'
	if (head.FLAGS & 0x08) m_MultiArc = TRUE;

	//eshe odna glava
	m_NumberChapters++;

	double Ver    = 0;
	BYTE   Method = 0;
	m_HostOS = head.HOST_OS;
	Ver = (double)head.VER_CREATED / 10.0;
	if (m_PackVersion < Ver)   m_PackVersion   = Ver;
	Ver = (double)head.VER_EXTRACT;
	if (m_UnpackVersion < Ver) m_UnpackVersion = Ver;
	Method = head.METHOD;
	if (m_MethodPack < Method) m_MethodPack    = Method;

	//propustit' nedochiatnnoe + 4 (HEADER_ID[WORD] + HEAD_SIZE[WORD])
	m_loffset = head.HDR_SIZE + 4 - sizeof(head);
	MSG = SeekArc(m_loffset, NULL);
	if (MSG != TMESSAGE_OK) return MSG;

	int      len     = MAX_LEN_STR;
	int      toffset = 0;
	LONGLONG temp    = 0;
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
	//propustit' kommentariy chapter'a
	len = ARJ_MAX_LEN_COMMENT;
	{
		temp = m_ArchiveSize - GetArcPointer();
		if (temp < len) len = (int)temp;

		MSG = ReadArc(m_buffer_cmt, len);
		if (MSG != TMESSAGE_OK) return MSG;

		//dlina stroki (stroka zakanchivaetsya \0 (0x00))
		toffset = strlen(m_buffer_cmt);
		//commentariy fayla
		if (toffset != 0) m_ChapterComment = TRUE;
		//mojet eto bol'she chem pozvoleno
		if (toffset > len) return TERROR_FORMAT;

		//skol'ko prochitano nujnogo (nado vernut')
		m_loffset = - len + 1 + toffset;
		MSG = SeekArc(m_loffset, NULL);
		if (MSG != TMESSAGE_OK) return MSG;
	}

	//konec zagolovka
	char endbuf[6] = {NULL};
	MSG = ReadArc(endbuf, 6);
	if (MSG != TMESSAGE_OK) return MSG;
	//propustit' konec zgolovka
	m_loffset = TakeWord(endbuf[4], endbuf[5]);
	return TMESSAGE_OK;
}

/////////////////////////////////////////////////////////////////////////////
//FUNCTIONS PUBLIC
/////////////////////////////////////////////////////////////////////////////

//fayl yavlyaetsya arhivom ARJ
//path - imya fayla dlya testirovaniya
//return
//	TMESSAGE_OK     - ok
//	ERROR_OPEN_FILE - oshibka otckritiya fayla
//	ERROR_READ_FILE - oshibka chteniya fayla
//	ERROR_SEEK_FILE - oshibka smesheniya fayla
//	SUDDEN_END_FILE - vnezapniy konec fayla
//	ERROR_FORMAT    - oshibka formata fayla
int TArj::
TestFile(char* path)
{
	//obnulenie dannih
	InitialParametrs();
	m_ARJ_RecoveryRecord = FALSE;
	
	//otkrit' fayl dlya chteniya
	m_h = CreateFile(path, GENERIC_READ, FILE_SHARE_READ, 
			NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (m_h == INVALID_HANDLE_VALUE) 
		{ if (m_h) { ::CloseHandle(m_h); m_h = 0; } return TERROR_OPEN_FILE; }

	//chtenie signaturi
	WORD HEADER_ID = 0;
	MSG = ReadArc(&HEADER_ID, sizeof(HEADER_ID));
	if (MSG != TMESSAGE_OK) { if (m_h) { ::CloseHandle(m_h); m_h = 0; } return MSG; }

	//proverka signaturi
	if (HEADER_ID != 0xEA60) MSG = TERROR_FORMAT;
	else MSG = TMESSAGE_OK;
	if (m_DetailLF == 2)
		m_LogFile->WriteMessage(m_LangMsg.SignatureArchive, MSG);
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
				int      len    = m_MAX_LEN_SFX;
				BYTE*    buffer = 0;
				LONGLONG temp   = 0;

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
					if ((buffer[i]    == 0x60) &&
						(buffer[i+1]  == 0xEA) &&
						(buffer[i+10] == 0x02) &&
						(TakeWord(buffer[i+2], buffer[i+3]) <= 2600))
					{
						Flag = TMESSAGE_OK;
						//smeshenie dlya dalneyshego razbora
						m_loffset  += i + sizeof(EXECUTABLE_ID);
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
int TArj::
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
		//prochitat' sleduyushuyu signaturu
		flag = ReadArc(&m_BLOCK_HEAD, sizeof(m_BLOCK_HEAD));
		if (flag == TERROR_READ_FILE) { if (m_h) { ::CloseHandle(m_h); m_h = 0; } return flag; }
		//mojet fayl zakonchilsya
		if (flag == TERROR_SEEK_FILE)
		{
			CountRatioArchiveSize();
			CountRatioPackFileSize();
			if (m_h) { ::CloseHandle(m_h); m_h = 0; }
			//esli arhiv mnogotomniy opredelit' nomer toma
			if (m_MultiArc)
				DefineNumberVolume(path);
			return TMESSAGE_OK;
		}

		//blok konca arhiva (dal'she infa dlya vosstanovleniya)
		if (m_BLOCK_HEAD.HEAD_SIZE == 0x0000)
		{
			CountRatioArchiveSize();
			CountRatioPackFileSize();
			//esli arhiv mnogotomniy opredelit' nomer toma
			if (m_MultiArc)
				DefineNumberVolume(path);
			//est'li elektronnaya podpis'
			if (m_AuthVerification)
			{
				//arhiv s elektronnoy podpis'yu yavlyaetsya zablokirovannim
				m_ArchiveLock = TRUE;
				//vstanem na nachalo blocka elektronnoy podpisi
				MSG = SeekArc(m_ARJ_SecurityPosition, NULL, FILE_BEGIN);
				if(MSG != TMESSAGE_OK) { if (m_h) { ::CloseHandle(m_h); m_h = 0; } return MSG; }
				if (m_DetailLF == 2)
					m_LogFile->WriteMessage(m_LangMsg.BlockAuthentityVerification, TMESSAGE_OK);
				//propustim block elektronnoy podpisi
				MSG = SeekArc(m_ARJ_SecurityLength, NULL);
				if(MSG != TMESSAGE_OK) { if (m_h) { ::CloseHandle(m_h); m_h = 0; } return MSG; }
				//posle elektronnoy podpisi mojet idti recovery record
				if (m_ArchiveSize - GetArcPointer())
				{
					//metka recovery record
					const char* PSigx = "PSigx";
					const int   len   = 6;
					char  label[7] = {NULL};
					MSG = ReadArc(label, len);
					if(MSG != TMESSAGE_OK) { if (m_h) { ::CloseHandle(m_h); m_h = 0; } return MSG; }
					if (strcmp(label, PSigx) == 0)
					{
						m_RecoveryRecord     = (unsigned)(m_ArchiveSize - GetArcPointer());
						m_ARJ_RecoveryRecord = FALSE;
						if (m_DetailLF == 2)
							m_LogFile->WriteMessage(m_LangMsg.BlockRecoveryRecord, TMESSAGE_OK);
					}
					else
					{
						if (m_DetailLF == 2)
							m_LogFile->WriteMessage(m_LangMsg.BlockUnknown, TMESSAGE_OK);
					}
				}
			}
			//esli est' Recovery Record opredelit' razmer
			if (m_ARJ_RecoveryRecord)
			{
				//bila prochitana lishka structuroy m_BLOCK_HEAD
				m_loffset = sizeof(m_BLOCK_HEAD) - 4;
				MSG       = SeekArc(-m_loffset, NULL);
				if(MSG != TMESSAGE_OK) { if (m_h) { ::CloseHandle(m_h); m_h = 0; } return MSG; }
				//metka recovery record
				const char* PSigx = "PSigx";
				const int   len   = 6;
				char  label[7] = {NULL};
				MSG = ReadArc(label, len);
				if(MSG != TMESSAGE_OK) { if (m_h) { ::CloseHandle(m_h); m_h = 0; } return MSG; }
				if (strcmp(label, PSigx) == 0)
				{
					m_RecoveryRecord     = (unsigned)(m_ArchiveSize - GetArcPointer());
					m_ARJ_RecoveryRecord = FALSE;
					if (m_DetailLF == 2)
						m_LogFile->WriteMessage(m_LangMsg.BlockRecoveryRecord, TMESSAGE_OK);
				}
				else
				{
					if (m_DetailLF == 2)
						m_LogFile->WriteMessage(m_LangMsg.BlockRecoveryRecord, TERROR_FORMAT);
				}
			}
			if (m_h) { ::CloseHandle(m_h); m_h = 0; }
			return TMESSAGE_OK;
		}

		//zagolovok kakogo bloka
		switch (m_BLOCK_HEAD.FILE_TYPE)
		{
			//zagolovok fayla v arhive
			case 0x00:	MSG = AnalyzeFileHead(m_BLOCK_HEAD);
						if (m_DetailLF == 2)
							m_LogFile->WriteMessage(m_LangMsg.BlockFile, MSG);
						if (MSG != TMESSAGE_OK)
							{ if (m_h) { ::CloseHandle(m_h); m_h = 0; } return MSG; }
						break;
			//zagolovok fayla v arhive
			case 0x01:	MSG = AnalyzeFileHead(m_BLOCK_HEAD);
						if (m_DetailLF == 2)
							m_LogFile->WriteMessage(m_LangMsg.BlockFile, MSG);
						if (MSG != TMESSAGE_OK)
							{ if (m_h) { ::CloseHandle(m_h); m_h = 0; } return MSG; }
						break;
			//zagolovok arhiva
			case 0x02:	MSG = AnalyzeArcHead(m_BLOCK_HEAD);
						if (m_DetailLF == 2)
							m_LogFile->WriteMessage(m_LangMsg.BlockArhiveHeader, MSG);
						if (MSG != TMESSAGE_OK)
							{ if (m_h) { ::CloseHandle(m_h); m_h = 0; } return MSG; }
						break;
			//zagolovok fayla v arhive (directory)
			case 0x03:	MSG = AnalyzeFileHead(m_BLOCK_HEAD);
						if (m_DetailLF == 2)
							m_LogFile->WriteMessage(m_LangMsg.BlockFolder, MSG);
						if (MSG != TMESSAGE_OK)
							{ if (m_h) { ::CloseHandle(m_h); m_h = 0; } return MSG; }
						break;
			//zagolovok fayla v arhive
			case 0x04:	MSG = AnalyzeFileHead(m_BLOCK_HEAD);
						if (m_DetailLF == 2)
							m_LogFile->WriteMessage(m_LangMsg.BlockVolumeLabel, MSG);
						if (MSG != TMESSAGE_OK)
							{ if (m_h) { ::CloseHandle(m_h); m_h = 0; } return MSG; }
						break;
			//chapter header
			case 0x05:	MSG = AnalyzeChapterHead(m_BLOCK_HEAD);
						if (m_DetailLF == 2)
							m_LogFile->WriteMessage(m_LangMsg.BlockChapter, MSG);
						if (MSG != TMESSAGE_OK)
							{ if (m_h) { ::CloseHandle(m_h); m_h = 0; } return MSG; }
						break;
			default:	if (m_DetailLF == 2)
							m_LogFile->WriteMessage(m_LangMsg.BlockUnknown, TERROR_FORMAT);
						if (m_h) { ::CloseHandle(m_h); m_h = 0; }
						return TERROR_FORMAT;
						break;
		}
	}
}