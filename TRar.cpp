//TRar.cpp

#include "stdafx.h"
#include "TRar.h"
#include "UnRar.h"

/////////////////////////////////////////////////////////////////////////////
//FUNCTIONS PRIVATE
/////////////////////////////////////////////////////////////////////////////

//prochitat' kommentariy arhiva
//return
//	MSG - resul'tat vipolneniya
int TRar::
ReadComment()
{
	//ispol'zuyutsya storonnie plagini na vsyakiy sluchay
	try
	{
		//chitat' kommentariy s pomoch'yu unrar.dll
		HINSTANCE        DllModule        = 0;
		RAROPENARCHIVEEX RAROpenArchiveEx = 0;
		RARCLOSEARCHIVE  RARCloseArchive  = 0;
		char             pUnRarDll[MAX_PATH] = {0};

		//chtob put' bil real'nim bez vremennih okrujeniya
		if(!::ExpandEnvironmentStrings(m_pUnArchiveDll, pUnRarDll, MAX_PATH))
			strncpy(pUnRarDll, m_pUnArchiveDll, MAX_PATH);

		//zagrujaem DLL
		if (DllModule = ::LoadLibrary(pUnRarDll))
		{
			//nahodim nujnie funkcii
			RAROpenArchiveEx = (RAROPENARCHIVEEX)::GetProcAddress(DllModule, "RAROpenArchiveEx");
			RARCloseArchive  = (RARCLOSEARCHIVE) ::GetProcAddress(DllModule, "RARCloseArchive");
			if ((RAROpenArchiveEx) && (RARCloseArchive))
			{
				//proverka i predustanovka ukazatelya
				if (m_pTextComment) { delete[] m_pTextComment; m_pTextComment = 0; }
				m_pTextComment = new char[RAR_MAX_LEN_COMMENT+1];
				m_pTextComment[0]                   = '\0';
				m_pTextComment[RAR_MAX_LEN_COMMENT] = '\0';

				HANDLE               hArcData        = 0;
				RAROpenArchiveDataEx OpenArchiveData = {0};

				OpenArchiveData.ArcName    = m_pTargetFile;
				OpenArchiveData.CmtBuf     = m_pTextComment;
				OpenArchiveData.CmtBufSize = RAR_MAX_LEN_COMMENT;
				OpenArchiveData.OpenMode   = RAR_OM_LIST;

				hArcData = RAROpenArchiveEx(&OpenArchiveData);
				if (hArcData) { RARCloseArchive(hArcData); hArcData = 0; }

				//proverka resul'tata
				if (OpenArchiveData.OpenResult)
				{
					if (m_pTextComment) { delete[] m_pTextComment; m_pTextComment = 0; }
					if (DllModule) { ::FreeLibrary(DllModule); DllModule = 0; }
					return TERROR_DLL;
				}
				else
				{
					if (DllModule) { ::FreeLibrary(DllModule); DllModule = 0; }
					return TMESSAGE_OK;
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

//opredelenie nomera toma dlya mnogotomnogo arhiva
//path - imya faila arhiva
void TRar::
DefineNumberVolume(char* path)
{
	char* strnum = 0;
	char* substr = 0;
	int   size   = strlen(path);

	//stariy sposob numeracii tomov *.rXX
	if (size < 2) return;		//proverka dlini imeni
	strnum = path + size - 2;	//videlenie poslednih 2h simvolov
	//proverka na rasshirenie RAR
	if ((strcmp(strnum, "AR") == 0) ||
		(strcmp(strnum, "Ar") == 0) ||
		(strcmp(strnum, "aR") == 0) ||
		(strcmp(strnum, "ar") == 0))
		m_VolumeNumber += 1;
	else
	{
		m_VolumeNumber = atoi(strnum);
		//esli m_VolumeNumber = 0 znachit ne opredeleno
		if (m_VolumeNumber != 0)	//0 - error ili "00"
			m_VolumeNumber += 2;
		else
			if (strnum = "00") m_VolumeNumber = 2;
	}

	//noviy sposob numeracii tomov *.partN.rar
	substr = strstr(path, ".part");
	if (!substr) return;
	substr = substr + strlen(".part");
	if (strlen(substr) >= 5)		//minimum ostavshegosya (1.rar) = 6
	{
		strnum = strstr(substr, ".");	//ishem tochu pered rasshireniem
		if (substr)
		{
			int len  = strnum - substr; //kolichestvo cifr (.part1) = 1
			int back = strlen(strnum);	//simvolov posle cifr (.rar) = 4
			if (len)
			{
				strnum = new char[len+1];
				for (int i=0; i<len; i++)	//kopiruem cifru
					strnum[i] = substr[strlen(substr) - back - len + i];
				strnum[len] = '\0';

				m_VolumeNumber = atoi(strnum);
				if (strnum) {delete[] strnum; strnum = 0;}
			}
		}
	}
}

//proverka markera fayla MARK_HEAD
//head - structura zagolovka bloka (marker)
//return
//	MSG - resul'tat vipolneniya
int TRar::
AnalyzeMarkHead(RAR_BLOCK_HEAD& head)
{
	if ((head.HEAD_CRC1   != 0x52) ||
		(head.HEAD_CRC2   != 0x61) ||
		(head.HEAD_TYPE   != 0x72) ||
		(head.HEAD_FLAGS1 != 0x21) ||
		(head.HEAD_FLAGS2 != 0x1a) ||
		(head.HEAD_SIZE1  != 0x07) ||
		(head.HEAD_SIZE2  != 0x00)) return TERROR_FORMAT;
	return TMESSAGE_OK;
}

//obrabotka zagolovka arhiva MAIN_HEAD
//head - struktura zagolovka bloka
//return
//	MSG - resul'tat vipolneniya
int TRar::
AnalyzeMainHead(RAR_BLOCK_HEAD& head)
{
	WORD HEAD_FLAGS = 0;

	if (head.HEAD_TYPE != 0x73) return TERROR_FORMAT;

	//soberem HEAD_FLAGS
	HEAD_FLAGS = TakeWord(head.HEAD_FLAGS1, head.HEAD_FLAGS2);

	//mnogotomniy arhiv
	if (HEAD_FLAGS & 0x0001) m_MultiArc         = TRUE;
	//komentariy
	if (HEAD_FLAGS & 0x0002) m_MainComment      = TRUE;
	//arhiv zablokirovan
	if (HEAD_FLAGS & 0x0004) m_ArchiveLock      = TRUE;
	//arhiv neprerivniy
	if (HEAD_FLAGS & 0x0008) m_Solid            = TRUE;
	//est' elektronnaya podpis'
	if (HEAD_FLAGS & 0x0020) m_AuthVerification = TRUE;
	//est' infa dlya vosstanovleniya
	if (HEAD_FLAGS & 0x0040) m_RecoveryRecord   = TRUE;

	m_loffset = 6; //RESERVED1(WORD) + RESERVED2(DWORD)

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
int TRar::
AnalyzeFileHead(RAR_BLOCK_HEAD& head, int& BlockType)
{
	WORD  twrd           = 0;
	WORD  HEAD_FLAGS     = 0;
	WORD  NAME_SIZE      = 0;
	DWORD PACK_SIZE      = 0;
	DWORD UNP_SIZE       = 0;
	DWORD FTIME          = 0;
	DWORD ATTR           = 0;
	DWORD HIGH_PACK_SIZE = 0;
	DWORD HIGH_UNP_SIZE  = 0;

	MSG = ReadArc(&m_FILE_HEAD, sizeof(m_FILE_HEAD));
	if (MSG != TMESSAGE_OK) return MSG;
	//prochitana eshe structura FILE_HEAD
	m_loffset -= sizeof(m_FILE_HEAD);

	//sabrat' dannie iz BYTEs
	HEAD_FLAGS = TakeWord(head.HEAD_FLAGS1, head.HEAD_FLAGS2);
	NAME_SIZE  = TakeWord(m_FILE_HEAD.NAME_SIZE1, m_FILE_HEAD.NAME_SIZE2);
	PACK_SIZE  = TakeDWord(
					TakeWord(m_FILE_HEAD.PACK_SIZE1, m_FILE_HEAD.PACK_SIZE2),
					TakeWord(m_FILE_HEAD.PACK_SIZE3, m_FILE_HEAD.PACK_SIZE4));
	UNP_SIZE   = TakeDWord(
					TakeWord(m_FILE_HEAD.UNP_SIZE1, m_FILE_HEAD.UNP_SIZE2),
					TakeWord(m_FILE_HEAD.UNP_SIZE3, m_FILE_HEAD.UNP_SIZE4));
	FTIME      = TakeDWord(
					TakeWord(m_FILE_HEAD.FTIME1, m_FILE_HEAD.FTIME2),
					TakeWord(m_FILE_HEAD.FTIME3, m_FILE_HEAD.FTIME4));
	ATTR       = TakeDWord(
					TakeWord(m_FILE_HEAD.ATTR1, m_FILE_HEAD.ATTR2),
					TakeWord(m_FILE_HEAD.ATTR3, m_FILE_HEAD.ATTR4));

	//est' li parol'?
	if (HEAD_FLAGS & 0x0004) m_Password = TRUE;
	//est' li komentariy fayla?
	if (HEAD_FLAGS & 0x0008) m_FileComment = TRUE;
	//razmer slovarya
	//ispol'zuem promejutochnoe twrd t.k.
	//v chislah po neskol'ko edinic v dvoichnom vide
	//i mojet vozniknut' oshibka esli sovpadet tol'ko po odnoy edinice
	unsigned DictSize = 64;
	if (HEAD_FLAGS & 0x0020)                    DictSize = 128;
	if (HEAD_FLAGS & 0x0040)                    DictSize = 256;
	if ((twrd=(HEAD_FLAGS & 0x0060)) == 0x0060) DictSize = 512;
	if (HEAD_FLAGS & 0x0080)                    DictSize = 1024;
	if ((twrd=(HEAD_FLAGS & 0x00A0)) == 0x00A0) DictSize = 2048;
	if ((twrd=(HEAD_FLAGS & 0x00C0)) == 0x00C0) DictSize = 4096;
	if ((twrd=(HEAD_FLAGS & 0x00E0)) == 0x00E0)
	{
		DictSize  = 0;
		BlockType = 2;
		m_NumberFolders++;	//znachit v arhive eshe 1 papka
	}
	else
	{
		BlockType = 1;
		m_NumberFiles++;	//znachit v arhive eshe 1 fayl
	}
	if (m_DictionarySize < DictSize) m_DictionarySize = DictSize;
	//est' li polya HIGH_SIZE
	if (HEAD_FLAGS & 0x0100)
	{
		MSG = ReadArc(&HIGH_PACK_SIZE, sizeof(HIGH_PACK_SIZE));
		if (MSG != TMESSAGE_OK) return MSG;
		//prochitana eshe structura HIGH_PACK_SIZE
		m_loffset -= sizeof(HIGH_PACK_SIZE);
		MSG = ReadArc(&HIGH_UNP_SIZE, sizeof(HIGH_UNP_SIZE));
		if (MSG != TMESSAGE_OK) return MSG;
		//prochitana eshe structura HIGH_UNP_SIZE
		m_loffset -= sizeof(HIGH_UNP_SIZE);
		//zapomnim chto chislo bol'shoe (razmer - smeshenie dlya >2Gb <4Gb)
		m_BigOffset = TRUE;
	}
	//imena faylov v yunikode?
	if (HEAD_FLAGS & 0x0200) m_Unicode = TRUE;

	//propustit' ostavshuyusyu infu bloka
	MSG = SeekArc(m_loffset, NULL);
	if(MSG != TMESSAGE_OK) return MSG;

	//sam upakovanniy fayl
	m_loffset = PACK_SIZE;
	m_hoffset = HIGH_PACK_SIZE;
	
	double UnpVer = 0;
	BYTE   Method = 0;

	m_PackSizeFiles   += TakeLong(PACK_SIZE, HIGH_PACK_SIZE);
	m_UnpackSizeFiles += TakeLong(UNP_SIZE, HIGH_UNP_SIZE);
	m_HostOS          = m_FILE_HEAD.HOST_OS;
	UnpVer            = (double)m_FILE_HEAD.UNP_VER / 10.0;
	if (m_UnpackVersion < UnpVer) m_UnpackVersion = UnpVer;
	Method            = m_FILE_HEAD.METHOD;
	if (m_MethodPack < Method) m_MethodPack = Method;

	return TMESSAGE_OK;
}

//obrabotka bloka kommentariya (stariy)
//head - struktura zagolovka bloka
//return
//	MSG - resul'tat vipolneniya
int TRar::
AnalyzeComment75h(RAR_BLOCK_HEAD& head)
{
	m_MainComment = TRUE;
	//propustim kak neizvestniy
	return UnknownBlock(head);
}

//obrabotka elektronnoy podpisi (stariy)
//head - struktura zagolovka bloka
//return
//	MSG - resul'tat vipolneniya
int TRar::
AnalyzeAuthVer76h(RAR_BLOCK_HEAD& head)
{
	//infu:
	// -imya arhiva
	// -data sozdaniya
	// -vremya sozdaniya
	// -imya avtora
	//ne viudit' tak kak ona zajata/zashifrovana
	m_AuthVerification = TRUE;
	//propustim kak neizvestniy
	return UnknownBlock(head);
}

//obrabotka elektronnoy podpisi (stariy)
//head - struktura zagolovka bloka
//return
//	MSG - resul'tat vipolneniya
int TRar::
AnalyzeAuthVer79h(RAR_BLOCK_HEAD& head)
{
	char  DateAV[11];
	char  TimeAV[9];
	char* FileName         = 0;
	char* AuthorName       = 0;
	WORD  SIZE             = 0;
	WORD  SIZE_FILE_NAME   = 0;
	WORD  SIZE_AUTHOR_NAME = 0;
	DWORD FTIME            = 0;

	//prochitat' vremya i datu elektronnoy podpisi
	MSG = ReadArc(&FTIME, sizeof(FTIME));
	if (MSG != TMESSAGE_OK) return MSG;
	m_loffset -= sizeof(FTIME);
	DateTimeDosFormat(FTIME, DateAV, TimeAV);

	//prochitaem razmer imeni fayla arhiva
	MSG = ReadArc(&SIZE_FILE_NAME, sizeof(SIZE_FILE_NAME));
	if (MSG != TMESSAGE_OK) return MSG;
	m_loffset -= sizeof(SIZE_FILE_NAME);

	//prochitaem razmer imeni avtora fayla arhiva
	MSG = ReadArc(&SIZE_AUTHOR_NAME, sizeof(SIZE_AUTHOR_NAME));
	if (MSG != TMESSAGE_OK) return MSG;
	m_loffset -= sizeof(SIZE_AUTHOR_NAME);

	//prochitaem imya fayla arhiva
	FileName = new char[SIZE_FILE_NAME+1];
	FileName[SIZE_FILE_NAME] = '\0';
	MSG = ReadArc(FileName, SIZE_FILE_NAME);
	if (MSG != TMESSAGE_OK)
	{
		if (FileName) { delete[] FileName; FileName = 0; }
		return MSG;
	}
	m_loffset -= SIZE_FILE_NAME;

	//prochitaem imya avtora fayla arhiva
	AuthorName = new char[SIZE_AUTHOR_NAME+1];
	AuthorName[SIZE_AUTHOR_NAME] = '\0';
	MSG = ReadArc(AuthorName, SIZE_AUTHOR_NAME);
	if (MSG != TMESSAGE_OK)
	{
		if (FileName) { delete[] FileName; FileName = 0; }
		if (AuthorName) { delete[] AuthorName; AuthorName = 0; }
		return MSG;
	}
	m_loffset -= SIZE_AUTHOR_NAME;

	//sostavim stroku elektronnoy podpisi
	SIZE =  strlen(FileName) +
		1 + strlen(DateAV)   +
		1 + strlen(TimeAV)   +
		1 + strlen(AuthorName);
	if (m_pAuthVerification) { delete[] m_pAuthVerification; m_pAuthVerification = 0; }
	m_pAuthVerification = new char[SIZE+1];
	strcpy(m_pAuthVerification, FileName);
	strcat(m_pAuthVerification, "\n");
	strcat(m_pAuthVerification, DateAV);
	strcat(m_pAuthVerification, "\n");
	strcat(m_pAuthVerification, TimeAV);
	strcat(m_pAuthVerification, "\n");
	strcat(m_pAuthVerification, AuthorName);

	m_AuthVerification = TRUE;

	if (FileName)   { delete[] FileName; FileName = 0; }
	if (AuthorName) { delete[] AuthorName; AuthorName = 0; }

	return TMESSAGE_OK;
}

//obrabotka bloka dlya vosstanovleniya (stariy)
//head - struktura zagolovka bloka
//return
//	MSG - resul'tat vipolneniya
int TRar::
AnalyzeRecovRec78h(RAR_BLOCK_HEAD& head)
{
	WORD  HEAD_FLAGS = 0;
	DWORD ADD_SIZE   = 0;

	//sabrat' HEAD_FLAGS iz BYTEs
	HEAD_FLAGS = TakeWord(head.HEAD_FLAGS1, head.HEAD_FLAGS2);

	//est' li pole ADD_SIZE?
	if (HEAD_FLAGS & 0x8000)
	{
		MSG = ReadArc(&ADD_SIZE, sizeof(ADD_SIZE));
		if (MSG != TMESSAGE_OK) return MSG;
		//prochitano eshe WORD
		m_loffset -= sizeof(ADD_SIZE);
		//dobavlenie k razmeru bloka
		m_loffset += ADD_SIZE;
	}
	m_RecoveryRecord = ADD_SIZE;
	return TMESSAGE_OK;
}

//obrabotka subbloka (stariy)
//head - struktura zagolovka bloka
//return
//	MSG - resul'tat vipolneniya
int TRar::
AnalyzeSubblok77h(RAR_BLOCK_HEAD& head, int& BlockType)
{
	//propustim kak neizvestniy
	return UnknownBlock(head);
}

//obrabotka subbloka
//head - struktura zagolovka bloka
//return
//	BlockType - tip subbloka
//		0 - neizvestno
//		1 - elektronnaya podpis'
//		2 - zapis' dlya vosstanovleniya
//		3 - arhivniy kommentariy
//		4 - kommentariy fayla
//	MSG - resul'tat vipolneniya
int TRar::
AnalyzeSubblok7Ah(RAR_BLOCK_HEAD& head, int& BlockType)
{
	WORD  HEAD_FLAGS = 0;
	DWORD ADD_SIZE   = 0;
	DWORD FTIME      = 0;

	//sabrat' HEAD_FLAGS iz BYTEs
	HEAD_FLAGS = TakeWord(head.HEAD_FLAGS1, head.HEAD_FLAGS2);

	//est' li pole ADD_SIZE?
	if (HEAD_FLAGS & 0x8000)
	{
		MSG = ReadArc(&m_FILE_HEAD, sizeof(m_FILE_HEAD));
		if (MSG != TMESSAGE_OK) return MSG;
		//prochitano eshe strlen(m_FILE_HEAD)
		m_loffset -= sizeof(m_FILE_HEAD);

		//soberem ADD_SIZE
		ADD_SIZE = TakeDWord(
					TakeWord(m_FILE_HEAD.PACK_SIZE1, m_FILE_HEAD.PACK_SIZE2),
					TakeWord(m_FILE_HEAD.PACK_SIZE3, m_FILE_HEAD.PACK_SIZE4));
		//dobavlenie k razmeru bloka
		m_loffset += ADD_SIZE;

		const int len = 2;
		char buffer[len] = {NULL};
		MSG = ReadArc(buffer, len);
		if (MSG != TMESSAGE_OK) return MSG;
		//prochitano eshe "len"
		m_loffset -= len;

		//podtip subbloka electronnaya podpis' "AV"
		if ((buffer[0] == 'A') && (buffer[1] == 'V'))
		{
			BlockType = 1;

			char  DateAV[11];
			char  TimeAV[9];
			char* FileName   = 0;
			char* AuthorName = 0;
			WORD  SIZE       = 0;

			//soberem vremya i datu elektronnoy podpisi
			FTIME = TakeDWord(
					TakeWord(m_FILE_HEAD.FTIME1, m_FILE_HEAD.FTIME2),
					TakeWord(m_FILE_HEAD.FTIME3, m_FILE_HEAD.FTIME4));
			DateTimeDosFormat(FTIME, DateAV, TimeAV);

			//prochitaem imya fayla arhiva
			MSG = ReadArc(&SIZE, sizeof(SIZE));
			if (MSG != TMESSAGE_OK) return MSG;
			m_loffset -= sizeof(SIZE);
			FileName = new char[SIZE+1];
			FileName[SIZE] = '\0';
			MSG = ReadArc(FileName, SIZE);
			if (MSG != TMESSAGE_OK)
			{
				if (FileName) { delete[] FileName; FileName = 0; }
				return MSG;
			}
			m_loffset -= SIZE;

			//eto propusk 2 BYTEs
			MSG = ReadArc(&SIZE, sizeof(SIZE));
			if (MSG != TMESSAGE_OK)
			{
				if (FileName) { delete[] FileName; FileName = 0; }
				return MSG;
			}
			m_loffset -= sizeof(SIZE);

			//prochitaem imya avtora fayla arhiva
			MSG = ReadArc(&SIZE, sizeof(SIZE));
			if (MSG != TMESSAGE_OK)
			{
				if (FileName) { delete[] FileName; FileName = 0; }
				return MSG;
			}
			m_loffset -= sizeof(SIZE);
			AuthorName = new char[SIZE+1];
			AuthorName[SIZE] = '\0';
			MSG = ReadArc(AuthorName, SIZE);
			if (MSG != TMESSAGE_OK)
			{
				if (FileName) { delete[] FileName; FileName = 0; }
				if (AuthorName) { delete[] AuthorName; AuthorName = 0; }
				return MSG;
			}
			m_loffset -= SIZE;

			//sostavim stroku elektronnoy podpisi
			SIZE =  strlen(FileName) +
				1 + strlen(DateAV)   +
				1 + strlen(TimeAV)   +
				1 + strlen(AuthorName);
			if (m_pAuthVerification) { delete[] m_pAuthVerification; m_pAuthVerification = 0; }
			m_pAuthVerification = new char[SIZE+1];
			strcpy(m_pAuthVerification, FileName);
			strcat(m_pAuthVerification, "\n");
			strcat(m_pAuthVerification, DateAV);
			strcat(m_pAuthVerification, "\n");
			strcat(m_pAuthVerification, TimeAV);
			strcat(m_pAuthVerification, "\n");
			strcat(m_pAuthVerification, AuthorName);

			m_AuthVerification = TRUE;

			if (FileName)   { delete[] FileName; FileName = 0; }
			if (AuthorName) { delete[] AuthorName; AuthorName = 0; }
		}
		//podtip subbloka informaciya dlya vosstanovleniya "RRProtect+"
		if ((buffer[0] == 'R') && (buffer[1] == 'R'))
		{
			BlockType        = 2;
			m_RecoveryRecord = ADD_SIZE;
		}
		//podtip subbloka kommentariy arhiva ili fayla "CMT"
		if ((buffer[0] == 'C') && (buffer[1] == 'M'))
		{
			if ((m_NumberFiles > 0) || (m_NumberFolders > 0))
			{
				BlockType     = 4;
				m_FileComment = TRUE;
			}
			else
			{
				BlockType     = 3;
				m_MainComment = TRUE;
			}
		}
	}
	return TMESSAGE_OK;
}

//obrabotka neizvestnogo bloka
//head - struktura zagolovka bloka
//return
//	MSG - resul'tat vipolneniya
int TRar::
UnknownBlock(RAR_BLOCK_HEAD& head)
{
	WORD  HEAD_FLAGS = 0;
	DWORD ADD_SIZE   = 0;

	//sabrat' HEAD_FLAGS iz BYTEs
	HEAD_FLAGS = TakeWord(head.HEAD_FLAGS1, head.HEAD_FLAGS2);

	//est' li pole ADD_SIZE?
	if (HEAD_FLAGS & 0x8000)
	{
		MSG = ReadArc(&ADD_SIZE, sizeof(ADD_SIZE));
		if (MSG != TMESSAGE_OK) return MSG;
		//prochitano eshe WORD
		m_loffset -= sizeof(ADD_SIZE);
		//dobavlenie k razmeru bloka
		m_loffset += ADD_SIZE;
	}
	return TMESSAGE_OK;
}

/////////////////////////////////////////////////////////////////////////////
//FUNCTIONS PUBLIC
/////////////////////////////////////////////////////////////////////////////

//fayl yavlyaetsya arhivom RAR
//path - fayla dlya testirovaniya
//return
//	TMESSAGE_OK     - ok
//	ERROR_OPEN_FILE - oshibka otckritiya fayla
//	ERROR_READ_FILE - oshibka chteniya fayla
//	ERROR_SEEK_FILE - oshibka smesheniya fayla
//	SUDDEN_END_FILE - vnezapniy konec fayla
//	ERROR_FORMAT    - oshibka formata fayla
int TRar::
TestFile(char* path)
{
	//obnulenie dannih
	InitialParametrs();
	
	//otkrit' fayl dlya chteniya
	m_h = CreateFile(path, GENERIC_READ, FILE_SHARE_READ, 
			NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (m_h == INVALID_HANDLE_VALUE) 
		{ if (m_h) { ::CloseHandle(m_h); m_h = 0; } return TERROR_OPEN_FILE; }

	//chtenie zagolovka Arhiva
	MSG = ReadArc(&m_BLOCK_HEAD, sizeof(m_BLOCK_HEAD));
	if (MSG != TMESSAGE_OK) { if (m_h) { ::CloseHandle(m_h); m_h = 0; } return MSG; }
	m_loffset = sizeof(m_BLOCK_HEAD);

	//proverka markera fayla MARK_HEAD
	MSG = AnalyzeMarkHead(m_BLOCK_HEAD);
	if (m_DetailLF == 2)
		m_LogFile->WriteMessage(m_LangMsg.MarkerArchive, MSG);
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
				char     *buffer = 0;
				LONGLONG temp    = 0;

				CheckSFX = TRUE;

				//ne perebrat' razmer arhiva
				temp = GetArchSize() - GetArcPointer();
				if (temp < len) len = (int)temp;

				buffer = new char[len];
				MSG    = ReadArc(buffer, len);
				if (MSG != TMESSAGE_OK)
				{
					if (buffer) { delete[] buffer; buffer = 0; }
					if (m_h) { ::CloseHandle(m_h); m_h = 0; } return MSG;
				}
				
				//iskat' metku arhiva
				for (int i=0; i < len-6; i++)
				{
					if ((buffer[i]   == 0x52) &&
						(buffer[i+1] == 0x61) &&
						(buffer[i+2] == 0x72) &&
						(buffer[i+3] == 0x21) &&
						(buffer[i+4] == 0x1a) &&
						(buffer[i+5] == 0x07) &&
						(buffer[i+6] == 0x00))
					{
						Flag = TMESSAGE_OK;
						//smeshenie dlya dalneyshego razbora
						m_loffset  += i + sizeof(EXECUTABLE_ID);
						m_SfxModule = i + sizeof(EXECUTABLE_ID);
						if (buffer) { delete[] buffer; buffer = 0; }
						break;
					}
				}
				if (buffer) { delete[] buffer; buffer = 0; }
			}

			//SFX ili net
			if ((m_DetailLF == 2) && (CheckSFX))
			{
				m_LogFile->WriteMessage(m_LangMsg.SFXModule, Flag);
				if (Flag == TMESSAGE_OK)
					m_LogFile->WriteMessage(m_LangMsg.MarkerArchive, Flag);
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
//path - fayla dlya analiza
//return
//	TTERMINATE_THREAD - iskustvennoe zavershenie
//	TMESSAGE_OK       - ok
//	ERROR_OPEN_FILE   - oshibka otckritiya fayla
//	ERROR_READ_FILE   - oshibka chteniya fayla
//	ERROR_SEEK_FILE   - oshibka smesheniya fayla
//	SUDDEN_END_FILE   - vnezapniy konec fayla
//	ERROR_FORMAT      - oshibka formata fayla
int TRar::
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

		//prochitat' sleduyushiy zagolovok bloka
		flag = ReadArc(&m_BLOCK_HEAD, sizeof(m_BLOCK_HEAD));
		if (flag == TERROR_READ_FILE) { if (m_h) { ::CloseHandle(m_h); m_h = 0; } return flag; }
		//mojet fayl zakonchilsya
		if (flag == TSUDDEN_END_FILE)
		{
			CountRatioArchiveSize();
			CountRatioPackFileSize();
			if (m_h) { ::CloseHandle(m_h); m_h = 0; }
			//esli arhiv mnogotomniy opredelit' nomer toma
			if (m_MultiArc) DefineNumberVolume(path);
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
		
		//ostalos' neprochitano v bloke
		m_loffset = HEAD_SIZE - sizeof(m_BLOCK_HEAD);

		switch (m_BLOCK_HEAD.HEAD_TYPE)
		{
			//zagolovok arhiva
			case 0x73:	MSG = AnalyzeMainHead(m_BLOCK_HEAD);
						if (m_DetailLF == 2)
							m_LogFile->WriteMessage(m_LangMsg.BlockArhiveHeader, MSG);
						if (MSG != TMESSAGE_OK)
							{ if (m_h) { ::CloseHandle(m_h); m_h = 0; } return MSG; }
						break;
			//zagolovok fayla
			case 0x74:	m_BlockType = 0;
						MSG = AnalyzeFileHead(m_BLOCK_HEAD, m_BlockType);
						if (m_DetailLF == 2)
						{
							switch(m_BlockType)
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
			//kommentariy (stariy)
			case 0x75:	MSG = AnalyzeComment75h(m_BLOCK_HEAD);
						if (m_DetailLF == 2)
							m_LogFile->WriteMessage(m_LangMsg.BlockArchiveComment, MSG);
						if (MSG != TMESSAGE_OK)
							{ if (m_h) { ::CloseHandle(m_h); m_h = 0; } return MSG; }
						break;
			//elektronnya podpis' (stariy)
			case 0x76:	MSG = AnalyzeAuthVer76h(m_BLOCK_HEAD);
						if (m_DetailLF == 2)
							m_LogFile->WriteMessage(m_LangMsg.BlockAuthentityVerification, MSG);
						if (MSG != TMESSAGE_OK)
							{ if (m_h) { ::CloseHandle(m_h); m_h = 0; } return MSG; }
						break;
			//subblok (stariy)
			case 0x77:	m_BlockType = 0;
						MSG = AnalyzeSubblok77h(m_BLOCK_HEAD, m_BlockType);
						if (m_DetailLF == 2)
							m_LogFile->WriteMessage(m_LangMsg.SubBlockUnknown, MSG);
						if (MSG != TMESSAGE_OK)
							{ if (m_h) { ::CloseHandle(m_h); m_h = 0; } return MSG; }
						break;
			//informaciya dlya vosstanovleniya (stariy)
			case 0x78:	MSG = AnalyzeRecovRec78h(m_BLOCK_HEAD);
						if (m_DetailLF == 2)
							m_LogFile->WriteMessage(m_LangMsg.BlockRecoveryRecord, MSG);
						if (MSG != TMESSAGE_OK)
							{ if (m_h) { ::CloseHandle(m_h); m_h = 0; } return MSG; }
						break;
			//elektronnya podpis' (stariy)
			case 0x79:	MSG = AnalyzeAuthVer79h(m_BLOCK_HEAD);
						if (m_DetailLF == 2)
							m_LogFile->WriteMessage(m_LangMsg.BlockAuthentityVerification, MSG);
						if (MSG != TMESSAGE_OK)
							{ if (m_h) { ::CloseHandle(m_h); m_h = 0; } return MSG; }
						break;
			//subblok
			case 0x7A:	m_BlockType = 0;
						MSG = AnalyzeSubblok7Ah(m_BLOCK_HEAD, m_BlockType);
						if (m_DetailLF == 2)
						{
							switch(m_BlockType)
							{
							case 1: m_LogFile->WriteMessage(m_LangMsg.SubBlockAuthentityVerification, MSG);
								    break;
							case 2: m_LogFile->WriteMessage(m_LangMsg.SubBlockRecoveryRecord, MSG);
								    break;
							case 3: m_LogFile->WriteMessage(m_LangMsg.SubBlockArchiveComment, MSG);
								    break;
							case 4: m_LogFile->WriteMessage(m_LangMsg.SubBlockFileComment, MSG);
								    break;
							default: m_LogFile->WriteMessage(m_LangMsg.SubBlockUnknown, MSG);
								    break;
							}
						}
						if (MSG != TMESSAGE_OK)
							{ if (m_h) { ::CloseHandle(m_h); m_h = 0; } return MSG; }
						break;
			//konec arhiva
			case 0x7B:	CountRatioArchiveSize();
						CountRatioPackFileSize();
						if (m_DetailLF == 2)
							m_LogFile->WriteMessage(m_LangMsg.BlockArchiveEnd, TMESSAGE_OK);
						if (m_h) { ::CloseHandle(m_h); m_h = 0; }
						//esli arhiv mnogotomniy opredelit' nomer toma
						if (m_MultiArc) DefineNumberVolume(path);
						//esli est' kommentariy arhiva, to prochitat' ego
						if (m_MainComment)
						{
							MSG = ReadComment();
							if (m_DetailLF == 2)
								m_LogFile->WriteMessage(m_LangMsg.DLLReadArchiveComment, MSG);
						}
						return TMESSAGE_OK;
						break;
			//neizvesniy blok
			default:	MSG = UnknownBlock(m_BLOCK_HEAD);
						if (m_DetailLF == 2)
							m_LogFile->WriteMessage(m_LangMsg.BlockUnknown, MSG);
						if (MSG != TMESSAGE_OK)
							{ if (m_h) { ::CloseHandle(m_h); m_h = 0; } return MSG; }
						break;
		}
	}
}
