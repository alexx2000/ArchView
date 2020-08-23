/////////////////////////////////////////////////////////////////////////////
//TAce.cpp
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TAce.h"
#include "UnAce.h"

/////////////////////////////////////////////////////////////////////////////
//FUNCTIONS FOR UNACEV2.DLL
/////////////////////////////////////////////////////////////////////////////

char MyAceTextComment[ACE_MAX_LEN_COMMENT] = {0};
int  MyAceReadComment = 0;

INT __stdcall InfoProc(pACEInfoCallbackProcStruc Info)
{
	return ACE_CALLBACK_RETURN_OK;
}

INT __stdcall ErrorProc(pACEErrorCallbackProcStruc Error)
{
	return ACE_CALLBACK_RETURN_CANCEL;
}

INT __stdcall RequestProc(pACERequestCallbackProcStruc Request)
{
	return ACE_CALLBACK_RETURN_CANCEL;
}

INT __stdcall StateProc(pACEStateCallbackProcStruc State)
{
	if ((State->Archive.GlobalData->Comment.BufSize) && (!MyAceReadComment))
	{
		MyAceReadComment = 1;
		strcpy(MyAceTextComment, State->Archive.GlobalData->Comment.Buf);
	}
	return ACE_CALLBACK_RETURN_CANCEL;
}

/////////////////////////////////////////////////////////////////////////////
//FUNCTIONS PRIVATE
/////////////////////////////////////////////////////////////////////////////

//prochitat' kommentariy arhiva
//return
//	MSG - resul'tat vipolneniya
int TAce::
ReadComment()
{
	//ispol'zuyutsya storonnie plagini na vsyakiy sluchay
	try
	{
		//chitat' kommentariy s pomoch'yu unacev2.dll
		HINSTANCE DllModule                  = 0;
		INT (__stdcall *ACEInitDllProc)        (pACEInitDllStruc DllData);
		INT (__stdcall *ACEReadArchiveDataProc)(LPSTR ArchiveName, pACEReadArchiveDataStruc ArchiveData);
		INT (__stdcall *ACEListProc)           (LPSTR ArchiveName, pACEListStruc List);
		char pUnAceDll [MAX_PATH]            = {0};
		char CommentBuf[ACE_MAX_LEN_COMMENT] = {0};
		char FileList  [32768]               = {0};
		//schitaem kommentariy esche ne prochitannim
		MyAceReadComment = 0;

		//chtob put' bil real'nim bez vremennih okrujeniya
		if(!::ExpandEnvironmentStrings(m_pUnArchiveDll, pUnAceDll, MAX_PATH))
			strncpy(pUnAceDll, m_pUnArchiveDll, MAX_PATH);

		//zagrujaem DLL
		if (DllModule = ::LoadLibrary(pUnAceDll))
		{
			//nahodim nujnie funkcii
			ACEInitDllProc         = (INT(__stdcall*)(pACEInitDllStruc DllData))                               GetProcAddress(DllModule, "ACEInitDll");
			ACEReadArchiveDataProc = (INT(__stdcall*)(LPSTR ArchiveName, pACEReadArchiveDataStruc ArchiveData))GetProcAddress(DllModule, "ACEReadArchiveData");
			ACEListProc            = (INT(__stdcall*)(LPSTR ArchiveName, pACEListStruc List))                  GetProcAddress(DllModule, "ACEList");
			if ((ACEInitDllProc) && (ACEReadArchiveDataProc) && (ACEListProc))
			{
				//inicicalizaciya DLL
				tACEInitDllStruc DllData = {0};

				DllData.GlobalData.MaxArchiveTestBytes = m_MAX_LEN_SFX;
				DllData.GlobalData.MaxFileBufSize      = 0x2ffFF;
				DllData.GlobalData.Comment.BufSize     = ACE_MAX_LEN_COMMENT;
				DllData.GlobalData.Comment.Buf         = CommentBuf;
				DllData.GlobalData.TempDir             = "C:\\";
				DllData.GlobalData.InfoCallbackProc    = InfoProc;
				DllData.GlobalData.ErrorCallbackProc   = ErrorProc;
				DllData.GlobalData.RequestCallbackProc = RequestProc;
				DllData.GlobalData.StateCallbackProc   = StateProc;

				ACEInitDllProc(&DllData);

				//chtenie dannih DLL
				tACEReadArchiveDataStruc ArchiveData = {0};
				ACEReadArchiveDataProc(m_pTargetFile, &ArchiveData);

				//list arhiva
				tACEListStruc List = {0};
				List.Files.SourceDir   = "";
				List.Files.FileList    = FileList;
				List.Files.ExcludeList = "";
				List.Files.FullMatch   = 0;
				ACEListProc(m_pTargetFile, &List);
				if (MyAceReadComment)
				{
					if (m_pTextComment) { delete[] m_pTextComment; m_pTextComment = 0; }
					m_pTextComment = new char[strlen(MyAceTextComment)+1];
					strcpy(m_pTextComment, MyAceTextComment);
					if (DllModule) { ::FreeLibrary(DllModule); DllModule = 0; }
					return TMESSAGE_OK;
				}
				else
				{
					if (m_pTextComment) { delete[] m_pTextComment; m_pTextComment = 0; }
					if (DllModule) { ::FreeLibrary(DllModule); DllModule = 0; }
					return TERROR_DLL;
				}
			}
			else
			{
				if (DllModule) { ::FreeLibrary(DllModule); DllModule = 0; }
				return TERROR_DLL;
			}
		}
		else return TERROR_NOT_DLL;
	}
	catch (...)
	{
		return TERROR_DLL;
	}
}

//proverka zagolovka arhiva
//head - struktura zagolovka bloka
//return
//	MSG - resul'tat vipolneniya
int TAce::
AnalyzeArcHead(ACE_BLOCK_HEAD& head)
{
	BYTE  SIZE_AV      = 0;
	WORD  HEAD_SIZE    = 0;
	WORD  HEAD_FLAGS   = 0;
	DWORD TIME_CREATED = 0;

	//soberem HEAD_SIZE i HEAD_FLAGS
	HEAD_SIZE  = TakeWord(head.HEAD_SIZE1,  head.HEAD_SIZE2);
	HEAD_FLAGS = TakeWord(head.HEAD_FLAGS1, head.HEAD_FLAGS2);
	
	//eto blok zagolovka arhiva?
	if (head.HEAD_TYPE != 0x00) return TERROR_FORMAT;

	MSG = ReadArc(&m_ARC_HEAD, sizeof(m_ARC_HEAD));
	if (MSG != TMESSAGE_OK) return MSG;

	//imeetsya li stroka **ACE**
	if ((m_ARC_HEAD.ACESIGN1 != '*') ||
		(m_ARC_HEAD.ACESIGN2 != '*') ||
		(m_ARC_HEAD.ACESIGN3 != 'A') ||
		(m_ARC_HEAD.ACESIGN4 != 'C') ||
		(m_ARC_HEAD.ACESIGN5 != 'E') ||
		(m_ARC_HEAD.ACESIGN6 != '*') ||
		(m_ARC_HEAD.ACESIGN7 != '*')) return TERROR_FORMAT;

	//soberem vremya sozdaniya arhiva (format DOS)
	TIME_CREATED = TakeDWord(
		TakeWord(m_ARC_HEAD.TIME_CREATED1, m_ARC_HEAD.TIME_CREATED2),
		TakeWord(m_ARC_HEAD.TIME_CREATED3, m_ARC_HEAD.TIME_CREATED4));
	DateTimeDosFormat(TIME_CREATED, m_DateCreated, m_TimeCreated);

	//komentariy
	if (HEAD_FLAGS & 0x0002) m_MainComment      = TRUE;
	//mnogotomniy arhiv
	if (HEAD_FLAGS & 0x0800) m_MultiArc         = TRUE;
	//elektronnaya podpis'
	if (HEAD_FLAGS & 0x1000) m_AuthVerification = TRUE;
	//est' infa dlya vosstanovleniya
	if (HEAD_FLAGS & 0x2000) m_RecoveryRecord   = TRUE;
	//arhiv zablokirovan
	if (HEAD_FLAGS & 0x4000) m_ArchiveLock      = TRUE;
	//arhiv neprerivniy
	if (HEAD_FLAGS & 0x8000) m_Solid            = TRUE;

	//nomer toma mnogotomnogo arhiva
	//+1 t.k. v nam nado s 1 a ne s 0
	m_VolumeNumber  = m_ARC_HEAD.VOLUME_NUM + 1;

	//versiya dlya raspakovki
	m_UnpackVersion = (double)m_ARC_HEAD.VER_EXTRACT / 10.0;
	//versiya zapakovki
	m_PackVersion   = (double)m_ARC_HEAD.VER_CREATED / 10.0;
	//Host OS
	m_HostOS        = m_ARC_HEAD.HOST_CREATED;

	//razmer zagolovka arhiva + 4,
	//t.k. HEAD_CRC, HEAD_SIZE ne uchitivayutsya v HEAD_SIZE
	m_loffset = HEAD_SIZE + 4;

	//esli est' Authentity Verification to prochitat'
	if (m_AuthVerification)
	{
		//razmer stroki AV
		MSG = ReadArc(&SIZE_AV, sizeof(SIZE_AV));
		if (MSG != TMESSAGE_OK) return MSG;

		if (m_pAuthVerification) { delete[] m_pAuthVerification; m_pAuthVerification = 0; }
		m_pAuthVerification = new char[SIZE_AV+1];
		m_pAuthVerification[SIZE_AV] = '\0';
		MSG = ReadArc(m_pAuthVerification, SIZE_AV);
		if (MSG != TMESSAGE_OK) return MSG;
	}

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
int TAce::
AnalyzeFileHead(ACE_BLOCK_HEAD& head, int& BlockType)
{
	WORD  HEAD_FLAGS = 0;
	WORD  FNAME_SIZE = 0;
	DWORD PACK_SIZE  = 0;
	DWORD ORIG_SIZE  = 0;
	DWORD FTIME      = 0;
	DWORD ATTR       = 0;
	DWORD CRC        = 0;

	MSG = ReadArc(&m_FILE_HEAD, sizeof(m_FILE_HEAD));
	if (MSG != TMESSAGE_OK) return MSG;
	//prochitana eshe structura FILE_HEAD
	m_loffset -= sizeof(m_FILE_HEAD);

	//sabrat' dannie iz BYTEs
	HEAD_FLAGS = TakeWord(head.HEAD_FLAGS1, head.HEAD_FLAGS2);
	FNAME_SIZE = TakeWord(m_FILE_HEAD.FNAME_SIZE1, m_FILE_HEAD.FNAME_SIZE2);
	PACK_SIZE  = TakeDWord(
		TakeWord(m_FILE_HEAD.PACK_SIZE1, m_FILE_HEAD.PACK_SIZE2),
		TakeWord(m_FILE_HEAD.PACK_SIZE3, m_FILE_HEAD.PACK_SIZE4));
	ORIG_SIZE  = TakeDWord(
		TakeWord(m_FILE_HEAD.ORIG_SIZE1, m_FILE_HEAD.ORIG_SIZE2),
		TakeWord(m_FILE_HEAD.ORIG_SIZE3, m_FILE_HEAD.ORIG_SIZE4));
	FTIME      = TakeDWord(
		TakeWord(m_FILE_HEAD.FTIME1,     m_FILE_HEAD.FTIME2),
		TakeWord(m_FILE_HEAD.FTIME3,     m_FILE_HEAD.FTIME4));
	ATTR       = TakeDWord(
		TakeWord(m_FILE_HEAD.ATTR1,      m_FILE_HEAD.ATTR2),
		TakeWord(m_FILE_HEAD.ATTR3,      m_FILE_HEAD.ATTR4));
	CRC        = TakeDWord(
		TakeWord(m_FILE_HEAD.CRC1,       m_FILE_HEAD.CRC2),
		TakeWord(m_FILE_HEAD.CRC3,       m_FILE_HEAD.CRC4));

	//est' li komentariy fayla?
	if (HEAD_FLAGS & 0x0002) m_FileComment = TRUE;
	//est' li parol'?
	if (HEAD_FLAGS & 0x4000) m_Password    = TRUE;
	//solid
	if (HEAD_FLAGS & 0x8000) m_Solid       = TRUE;
	//razmer slovarya
	unsigned DictSize = 0;
	if (m_FILE_HEAD.TECH_INFO3 == 0x05) DictSize = 32;
	if (m_FILE_HEAD.TECH_INFO3 == 0x06) DictSize = 64;
	if (m_FILE_HEAD.TECH_INFO3 == 0x07) DictSize = 128;
	if (m_FILE_HEAD.TECH_INFO3 == 0x08) DictSize = 256;
	if (m_FILE_HEAD.TECH_INFO3 == 0x09) DictSize = 512;
	if (m_FILE_HEAD.TECH_INFO3 == 0x0A) DictSize = 1024;
	if (m_FILE_HEAD.TECH_INFO3 == 0x0B) DictSize = 2048;
	if (m_FILE_HEAD.TECH_INFO3 == 0x0C) DictSize = 4096;
	if (m_DictionarySize < DictSize) m_DictionarySize = DictSize;
	
	//v arhive eshe 1 fayl ili 1 papka
	if (ATTR & 0x00000010)
	{
		BlockType = 2;
		m_NumberFolders++;
	}
	else
	{
		BlockType = 1;
		m_NumberFiles++;
	}

	//propustit' ostavshuyusyu infu bloka
	MSG = SeekArc(m_loffset, NULL);
	if (MSG != TMESSAGE_OK) return MSG;
	
	//sam upakovanniy fayl
	m_loffset = PACK_SIZE;
	//zapomnim chto chislo bol'shoe (razmer - smeshenie dlya >2Gb <4Gb)
	if (m_loffset < 0) m_BigOffset = TRUE;
	
	BYTE Method = 0;
	m_PackSizeFiles   += PACK_SIZE;
	m_UnpackSizeFiles += ORIG_SIZE;
	Method = m_FILE_HEAD.TECH_INFO2;
	if (m_MethodPack < Method) m_MethodPack = Method;

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
int TAce::
AnalyzeFileHead_64(ACE_BLOCK_HEAD& head, int& BlockType)
{
	WORD  HEAD_FLAGS = 0;
	WORD  FNAME_SIZE = 0;
	DWORD FTIME      = 0;
	DWORD ATTR       = 0;
	DWORD CRC        = 0;
	LARGE_INTEGER PACK_SIZE;
	LARGE_INTEGER ORIG_SIZE;
	PACK_SIZE.QuadPart = 0;
	ORIG_SIZE.QuadPart = 0;

	MSG = ReadArc(&m_FILE_HEAD_64, sizeof(m_FILE_HEAD_64));
	if (MSG != TMESSAGE_OK) return MSG;
	//prochitana eshe structura FILE_HEAD
	m_loffset -= sizeof(m_FILE_HEAD_64);

	//sabrat' dannie iz BYTEs
	HEAD_FLAGS = TakeWord(head.HEAD_FLAGS1, head.HEAD_FLAGS2);
	FNAME_SIZE = TakeWord(m_FILE_HEAD_64.FNAME_SIZE1, m_FILE_HEAD_64.FNAME_SIZE2);
	PACK_SIZE.LowPart  = TakeDWord(
		TakeWord(m_FILE_HEAD_64.PACK_SIZE1, m_FILE_HEAD_64.PACK_SIZE2),
		TakeWord(m_FILE_HEAD_64.PACK_SIZE3, m_FILE_HEAD_64.PACK_SIZE4));
	PACK_SIZE.HighPart = TakeDWord(
		TakeWord(m_FILE_HEAD_64.PACK_SIZE5, m_FILE_HEAD_64.PACK_SIZE6),
		TakeWord(m_FILE_HEAD_64.PACK_SIZE7, m_FILE_HEAD_64.PACK_SIZE8));
	ORIG_SIZE.LowPart  = TakeDWord(
		TakeWord(m_FILE_HEAD_64.ORIG_SIZE1, m_FILE_HEAD_64.ORIG_SIZE2),
		TakeWord(m_FILE_HEAD_64.ORIG_SIZE3, m_FILE_HEAD_64.ORIG_SIZE4));
	ORIG_SIZE.HighPart = TakeDWord(
		TakeWord(m_FILE_HEAD_64.ORIG_SIZE5, m_FILE_HEAD_64.ORIG_SIZE6),
		TakeWord(m_FILE_HEAD_64.ORIG_SIZE7, m_FILE_HEAD_64.ORIG_SIZE8));
	FTIME      = TakeDWord(
		TakeWord(m_FILE_HEAD_64.FTIME1,     m_FILE_HEAD_64.FTIME2),
		TakeWord(m_FILE_HEAD_64.FTIME3,     m_FILE_HEAD_64.FTIME4));
	ATTR       = TakeDWord(
		TakeWord(m_FILE_HEAD_64.ATTR1,      m_FILE_HEAD_64.ATTR2),
		TakeWord(m_FILE_HEAD_64.ATTR3,      m_FILE_HEAD_64.ATTR4));
	CRC        = TakeDWord(
		TakeWord(m_FILE_HEAD_64.CRC1,       m_FILE_HEAD_64.CRC2),
		TakeWord(m_FILE_HEAD_64.CRC3,       m_FILE_HEAD_64.CRC4));

	//est' li komentariy fayla?
	if (HEAD_FLAGS & 0x0002) m_FileComment = TRUE;
	//est' li parol'?
	if (HEAD_FLAGS & 0x4000) m_Password    = TRUE;
	//solid
	if (HEAD_FLAGS & 0x8000) m_Solid       = TRUE;
	//razmer slovarya
	unsigned DictSize = 0;
	if (m_FILE_HEAD.TECH_INFO3 == 0x05) DictSize = 32;
	if (m_FILE_HEAD.TECH_INFO3 == 0x06) DictSize = 64;
	if (m_FILE_HEAD.TECH_INFO3 == 0x07) DictSize = 128;
	if (m_FILE_HEAD.TECH_INFO3 == 0x08) DictSize = 256;
	if (m_FILE_HEAD.TECH_INFO3 == 0x09) DictSize = 512;
	if (m_FILE_HEAD.TECH_INFO3 == 0x0A) DictSize = 1024;
	if (m_FILE_HEAD.TECH_INFO3 == 0x0B) DictSize = 2048;
	if (m_FILE_HEAD.TECH_INFO3 == 0x0C) DictSize = 4096;
	if (m_DictionarySize < DictSize) m_DictionarySize = DictSize;
	
	//v arhive eshe 1 fayl ili 1 papka
	if (ATTR & 0x00000010)
	{
		BlockType = 2;
		m_NumberFolders++;
	}
	else
	{
		BlockType = 1;
		m_NumberFiles++;
	}

	//propustit' ostavshuyusyu infu bloka
	MSG = SeekArc(m_loffset, NULL);
	if (MSG != TMESSAGE_OK) return MSG;
	
	//sam upakovanniy fayl
	m_loffset = PACK_SIZE.LowPart;
	m_hoffset = PACK_SIZE.HighPart;
	//zapomnim chto chislo bol'shoe (razmer - smeshenie dlya >2Gb <4Gb)
	m_BigOffset = TRUE;
	
	BYTE Method = 0;
	m_PackSizeFiles   += PACK_SIZE.QuadPart;
	m_UnpackSizeFiles += ORIG_SIZE.QuadPart;
	Method = m_FILE_HEAD.TECH_INFO2;
	if (m_MethodPack < Method) m_MethodPack = Method;

	return TMESSAGE_OK;
}

//obrabotka zapisi dlya vosstanovleniya
//head - struktura zagolovka bloka
//return
//	MSG - resul'tat vipolneniya
int TAce::
AnalyzeRecovRec(ACE_BLOCK_HEAD& head)
{
	DWORD SizeRR = 0;

	MSG = ReadArc(&SizeRR, sizeof(SizeRR));
	if (MSG != TMESSAGE_OK) return MSG;

	//prochitana eshe structura ADDSIZE
	m_loffset -= sizeof(SizeRR);
	//sam upakovanniy fayl
	m_loffset = SizeRR;

	m_RecoveryRecord = SizeRR;
	return TMESSAGE_OK;
}

//obrabotka neizvestnogo bloka
//head - struktura zagolovka bloka
//return
//	MSG - resul'tat vipolneniya
int TAce::
UnknownBlock(ACE_BLOCK_HEAD& head)
{
	WORD  HEAD_FLAGS = 0;
	DWORD ADDSIZE    = 0;

	//sabrat' dannie iz BYTEs
	HEAD_FLAGS = TakeWord(head.HEAD_FLAGS1, head.HEAD_FLAGS2);
	//est' chtoto dal'she?
	if (HEAD_FLAGS & 0x0002)
	{
		MSG = ReadArc(&ADDSIZE, sizeof(ADDSIZE));
		if (MSG != TMESSAGE_OK) return MSG;

		//prochitana eshe structura ADDSIZE
		m_loffset -= sizeof(ADDSIZE);
		//sam upakovanniy fayl
		m_loffset = ADDSIZE;
	}
	return TMESSAGE_OK;
}

/////////////////////////////////////////////////////////////////////////////
//FUNCTIONS PUBLIC
/////////////////////////////////////////////////////////////////////////////

//fayl yavlyaetsya arhivom ACE
//path - imya fayla dlya testirovaniya
//return
//	TMESSAGE_OK     - ok
//	ERROR_OPEN_FILE - oshibka otckritiya fayla
//	ERROR_READ_FILE - oshibka chteniya fayla
//	ERROR_SEEK_FILE - oshibka smesheniya fayla
//	SUDDEN_END_FILE - vnezapniy konec fayla
//	ERROR_FORMAT    - oshibka formata fayla
int TAce::
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
	if (MSG != TMESSAGE_OK) { if (m_h) { ::CloseHandle(m_h); m_h = 0; } return MSG; }

	//proverka markera fayla MARK_HEAD
	MSG = AnalyzeArcHead(m_BLOCK_HEAD);
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
			if (MSG != TMESSAGE_OK) 
				{ if (m_h) { ::CloseHandle(m_h); m_h = 0; } return MSG; }
			MSG = ReadArc(&EXECUTABLE_ID, sizeof(EXECUTABLE_ID));
			if (MSG != TMESSAGE_OK) { if (m_h) { ::CloseHandle(m_h); m_h = 0; } return MSG; }
			if (EXECUTABLE_ID == 'ZM')
			{
				int            len    = m_MAX_LEN_SFX;
				BYTE*          buffer = 0;
				LONGLONG       temp   = 0;
				ACE_BLOCK_HEAD head;

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
				for (int i=0; i < len-13; i++)
				{
					if ((buffer[i+4]  == 0x00) &&
						(buffer[i+7]  == '*')  &&
						(buffer[i+8]  == '*')  &&
						(buffer[i+9]  == 'A')  &&
						(buffer[i+10] == 'C')  &&
						(buffer[i+11] == 'E')  &&
						(buffer[i+12] == '*')  &&
						(buffer[i+13] == '*'))
					{
						Flag = TMESSAGE_OK;

						head.HEAD_CRC1   = buffer[i];
						head.HEAD_CRC2   = buffer[i+1];
						head.HEAD_SIZE1  = buffer[i+2];
						head.HEAD_SIZE2  = buffer[i+3];
						head.HEAD_TYPE   = buffer[i+4];
						head.HEAD_FLAGS1 = buffer[i+5];
						head.HEAD_FLAGS2 = buffer[i+6];

						//skol'ko prochitano nujnogo (nado vernut')
						//sizeof(head) eto (ot i do i+13)
						m_loffset = - len + i + sizeof(head);
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
int TAce::
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
	WORD HEAD_SIZE = 0;
	int  flag      = 0;
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
		if ((m_hoffset == 0) && (!m_BigOffset))
		{
			MSG = SeekArc(m_loffset, NULL);
			if(MSG != TMESSAGE_OK) { if (m_h) { ::CloseHandle(m_h); m_h = 0; } return MSG; }
		}
		else
		{
			MSG = SeekArc(m_loffset, &m_hoffset);
			if(MSG != TMESSAGE_OK) { if (m_h) { ::CloseHandle(m_h); m_h = 0; } return MSG; }
			//sbrosim flag (razmer - smeshenie dlya >2Gb <4Gb)
			m_BigOffset = FALSE;
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
			//esli est' kommentariy arhiva, to prochitat' ego
			if (m_MainComment)
			{
				MSG = ReadComment();
				if (m_DetailLF == 2)
					m_LogFile->WriteMessage(m_LangMsg.DLLReadArchiveComment, MSG);
			}
			return TMESSAGE_OK;
		}

		HEAD_SIZE = TakeWord(m_BLOCK_HEAD.HEAD_SIZE1, m_BLOCK_HEAD.HEAD_SIZE2);

		//ostalos' neprochitano v bloke + 4,
		//t.k. HEAD_CRC, HEAD_SIZE ne uchitivayutsya v HEAD_SIZE
		m_loffset = HEAD_SIZE + 4 - sizeof(m_BLOCK_HEAD);
		//zagolovok kakogo bloka
		switch (m_BLOCK_HEAD.HEAD_TYPE)
		{
			//zagolovok fayla
			case 0x01:	m_BlockType = 0;
						MSG = AnalyzeFileHead(m_BLOCK_HEAD, m_BlockType);
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
						break;
			//zagolovok fayla > 4 GB
			case 0x03:	m_BlockType = 0;
						MSG = AnalyzeFileHead_64(m_BLOCK_HEAD, m_BlockType);
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
						break;
			//informaciya dlya vosstanovleniya (stariy 0x02)
			case 0x02:
			//informaciya dlya vosstanovleniya
			case 0x05:	MSG = AnalyzeRecovRec(m_BLOCK_HEAD);
						if (m_DetailLF == 2)
							m_LogFile->WriteMessage(m_LangMsg.BlockRecoveryRecord, MSG);
						if (MSG != TMESSAGE_OK)
							{ if (m_h) { ::CloseHandle(m_h); m_h = 0; } return MSG; }
						CountRatioArchiveSize();
						CountRatioPackFileSize();
						if (m_h) { ::CloseHandle(m_h); m_h = 0; }
						//esli est' kommentariy arhiva, to prochitat' ego
						if (m_MainComment)
						{
							MSG = ReadComment();
							if (m_DetailLF == 2)
								m_LogFile->WriteMessage(m_LangMsg.DLLReadArchiveComment, MSG);
						}
						return TMESSAGE_OK;
						break;
			default:	MSG = UnknownBlock(m_BLOCK_HEAD);
						if (m_DetailLF == 2)
							m_LogFile->WriteMessage(m_LangMsg.BlockUnknown, MSG);
						if (MSG != TMESSAGE_OK)
							{ if (m_h) { ::CloseHandle(m_h); m_h = 0; } return MSG; }
						break;
		}
	}
}
