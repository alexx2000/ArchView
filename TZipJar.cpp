/////////////////////////////////////////////////////////////////////////////
//TZipJar.cpp
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TZipJar.h"

/////////////////////////////////////////////////////////////////////////////
//FUNCTIONS PRIVATE
/////////////////////////////////////////////////////////////////////////////

//prochitat' kommentariy arhiva
//return
//	MSG - resul'tat vipolneniya
int TZipJar::
ReadComment()
{
	//proverka i predustanovka ukazatelya
	if (m_pTextComment) { delete[] m_pTextComment; m_pTextComment = 0; }
	m_pTextComment = new char[m_CommentSize+1];
	//chitat' kommentariy
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

//obrabotka lokal'nogo bloka fayla/papki
//return
//	BlockType - tip bloka
//		0 - neizvestno
//		1 - fayl
//		2 - papka
//	MSG - resul'tat vipolneniya
int TZipJar::
AnalyzeLocalFileHead(int& BlockType)
{
	WORD  HEAD_FLAGS = 0;
	DWORD PACK_SIZE  = 0;
	DWORD UNP_SIZE   = 0;
	LARGE_INTEGER ZIP64_PACK_SIZE;
	LARGE_INTEGER ZIP64_UNP_SIZE;
	ZIP64_PACK_SIZE.QuadPart = 0;
	ZIP64_UNP_SIZE.QuadPart  = 0;
	
	//chitat' blok
	MSG = ReadArc(&m_LOCAL_FILE_HEAD, sizeof(m_LOCAL_FILE_HEAD));
	if (MSG != TMESSAGE_OK) return MSG;

	//sobrat' PACK_SIZE
	PACK_SIZE = TakeDWord(m_LOCAL_FILE_HEAD.PACK_SIZE1,
						m_LOCAL_FILE_HEAD.PACK_SIZE2);
	UNP_SIZE  = TakeDWord(m_LOCAL_FILE_HEAD.UNP_SIZE1,
						m_LOCAL_FILE_HEAD.UNP_SIZE2);
	
	HEAD_FLAGS = m_LOCAL_FILE_HEAD.BIT_FLAG;
	//est' li parol'?
	if (HEAD_FLAGS & 0x0001) m_Password = TRUE;
	//v arhive eshe 1 fayl ili 1 papka
	if (PACK_SIZE == 0 && UNP_SIZE == 0)
	{
		BlockType = 2;
		m_NumberFolders++;
		m_zlNumFileFold = TRUE;
	}
	else
	{
		BlockType = 1;
		m_NumberFiles++;
		m_zlNumFileFold = TRUE;
	}

	double Ver;
	WORD Method;
	Method      = (m_LOCAL_FILE_HEAD.UNP_VER & 0xFF00) / 0x100;
	m_ExtractOS = (BYTE)Method;
	Method      = m_LOCAL_FILE_HEAD.UNP_VER & 0x00FF;
	Ver = (double)Method / 10.0;
	if (m_UnpackVersion < Ver) m_UnpackVersion = Ver;
	Method = m_LOCAL_FILE_HEAD.PACK_METHOD;
	if (m_MethodPack < Method) m_MethodPack = (BYTE)Method;
	Method = m_LOCAL_FILE_HEAD.BIT_FLAG & 0x0006;
	if (m_MethodPackAdd < Method) m_MethodPackAdd = (BYTE)Method;

	//propustit' imya fayla/papki
	MSG = SeekArc(m_LOCAL_FILE_HEAD.NAME_SIZE, NULL);
	if(MSG != TMESSAGE_OK) return MSG;

	//obrabotka EXTRA_FILDS
	BOOL UseBigSizes = FALSE;
	if (m_LOCAL_FILE_HEAD.EXTRA_SIZE > 0)
	{
		m_loffset = m_LOCAL_FILE_HEAD.EXTRA_SIZE;
		while (m_loffset > 0)
		{
			//chitat' zagolovok EXTRA_FILD_HEADER
			MSG = ReadArc(&m_EXTRA_FILD_HEADER, sizeof(m_EXTRA_FILD_HEADER));
			if (MSG != TMESSAGE_OK) return MSG;
			//prochitan zagolovok
			m_loffset -= sizeof(m_EXTRA_FILD_HEADER);
			//obrabativaem nujnie
			switch (m_EXTRA_FILD_HEADER.HEADER_ID)
			{
			//ZIP64_EXTRA_FILD
			case 0x0001:
				if (m_EXTRA_FILD_HEADER.DATA_SIZE >= sizeof(m_ZIP64_EXTRA_FILD))
				{
					UseBigSizes = TRUE;
					//chitat' ZIP64_EXTRA_FILD
					MSG = ReadArc(&m_ZIP64_EXTRA_FILD, sizeof(m_ZIP64_EXTRA_FILD));
					if (MSG != TMESSAGE_OK) return MSG;
					//sobrat' ZIP64_PACK_SIZE i ZIP64_UNP_SIZE
					ZIP64_PACK_SIZE.QuadPart = m_ZIP64_EXTRA_FILD.PACK_SIZE;
					ZIP64_UNP_SIZE.QuadPart  = m_ZIP64_EXTRA_FILD.ORIG_SIZE;
					//prochitan ZIP64_EXTRA_FILD
					m_loffset -= sizeof(m_ZIP64_EXTRA_FILD);
					m_EXTRA_FILD_HEADER.DATA_SIZE -= sizeof(m_ZIP64_EXTRA_FILD);
				}
				//propustit' ostavsheesya
				m_loffset -= m_EXTRA_FILD_HEADER.DATA_SIZE;
				MSG = SeekArc(m_EXTRA_FILD_HEADER.DATA_SIZE, NULL);
				if(MSG != TMESSAGE_OK) return MSG;
				break;
			//JAR64_EXTRA_FILD
			case 0x0064:
				switch (m_EXTRA_FILD_HEADER.DATA_SIZE)
				{
				case sizeof(m_JAR64_EXTRA_FILD_1):
					UseBigSizes = TRUE;
					//chitat' JAR64_EXTRA_FILD_1
					MSG = ReadArc(&m_JAR64_EXTRA_FILD_1, sizeof(m_JAR64_EXTRA_FILD_1));
					if (MSG != TMESSAGE_OK) return MSG;
					//sobrat' ZIP64_PACK_SIZE i ZIP64_UNP_SIZE
					ZIP64_PACK_SIZE.LowPart  = PACK_SIZE;
					ZIP64_PACK_SIZE.HighPart = m_JAR64_EXTRA_FILD_1.HIGH_PACK_SIZE;
					ZIP64_UNP_SIZE.LowPart   = UNP_SIZE;
					ZIP64_UNP_SIZE.HighPart  = m_JAR64_EXTRA_FILD_1.HIGH_ORIG_SIZE;
					//prochitan JAR64_EXTRA_FILD_1
					m_loffset -= sizeof(m_JAR64_EXTRA_FILD_1);
					m_EXTRA_FILD_HEADER.DATA_SIZE -= sizeof(m_JAR64_EXTRA_FILD_1);
					break;
				case sizeof(m_JAR64_EXTRA_FILD_2):
					UseBigSizes = TRUE;
					//chitat' JAR64_EXTRA_FILD_2
					MSG = ReadArc(&m_JAR64_EXTRA_FILD_2, sizeof(m_JAR64_EXTRA_FILD_2));
					if (MSG != TMESSAGE_OK) return MSG;
					//sobrat' ZIP64_PACK_SIZE i ZIP64_UNP_SIZE
					ZIP64_PACK_SIZE.LowPart  = PACK_SIZE;
					ZIP64_PACK_SIZE.HighPart = m_JAR64_EXTRA_FILD_2.HIGH_PACK_SIZE;
					ZIP64_UNP_SIZE.LowPart   = UNP_SIZE;
					ZIP64_UNP_SIZE.HighPart  = m_JAR64_EXTRA_FILD_2.HIGH_ORIG_SIZE;
					//prochitan JAR64_EXTRA_FILD_2
					m_loffset -= sizeof(m_JAR64_EXTRA_FILD_2);
					m_EXTRA_FILD_HEADER.DATA_SIZE -= sizeof(m_JAR64_EXTRA_FILD_2);
					break;
				}
				//propustit' ostavsheesya
				m_loffset -= m_EXTRA_FILD_HEADER.DATA_SIZE;
				MSG = SeekArc(m_EXTRA_FILD_HEADER.DATA_SIZE, NULL);
				if(MSG != TMESSAGE_OK) return MSG;
				break;
			default:
				MSG = SeekArc(m_EXTRA_FILD_HEADER.DATA_SIZE, NULL);
				if(MSG != TMESSAGE_OK) return MSG;
				//propusheno m_EXTRA_FILD_HEADER.DATA_SIZE
				m_loffset -= m_EXTRA_FILD_HEADER.DATA_SIZE;
				break;
			}
		}
	}

	//razmeri faylov
	if ((PACK_SIZE != 0) || (UNP_SIZE != 0)) m_zlPackUnpSize = TRUE;
	if (UseBigSizes)
	{
		m_PackSizeFiles   += ZIP64_PACK_SIZE.QuadPart;
		m_UnpackSizeFiles += ZIP64_UNP_SIZE.QuadPart;
		//propustit' dannie
		m_loffset = ZIP64_PACK_SIZE.LowPart;
		m_hoffset = ZIP64_PACK_SIZE.HighPart;
		//zapomnim chto chislo bol'shoe (razmer - smeshenie dlya >2Gb <4Gb)
		if (m_loffset < 0) m_BigOffset = TRUE;
	}
	else
	{
		m_PackSizeFiles   += PACK_SIZE;
		m_UnpackSizeFiles += UNP_SIZE;
		//propustit' dannie
		m_loffset = PACK_SIZE;
		m_hoffset = 0;
		//zapomnim chto chislo bol'shoe (razmer - smeshenie dlya >2Gb <4Gb)
		if (m_loffset < 0) m_BigOffset = TRUE;
	}

	return TMESSAGE_OK;
}

//obrabotka bloka fayla/papki
//return
//	BlockType - tip bloka
//		0 - neizvestno
//		1 - fayl
//		2 - papka
//	MSG - resul'tat vipolneniya
int TZipJar::
AnalyzeFileHead(int& BlockType)
{
	WORD  HEAD_FLAGS = 0;
	DWORD CRC        = 0;
	DWORD PACK_SIZE  = 0;
	DWORD UNP_SIZE   = 0;
	LARGE_INTEGER ZIP64_PACK_SIZE;
	LARGE_INTEGER ZIP64_UNP_SIZE;
	ZIP64_PACK_SIZE.QuadPart = 0;
	ZIP64_UNP_SIZE.QuadPart  = 0;
	
	//chitat' blok
	MSG = ReadArc(&m_FILE_HEAD, sizeof(m_FILE_HEAD));
	if (MSG != TMESSAGE_OK) return MSG;

	//sobrat' CRC, PACK_SIZE, UNP_SIZE
	CRC = TakeDWord(m_FILE_HEAD.CRC1, m_FILE_HEAD.CRC2);
	PACK_SIZE = TakeDWord(m_FILE_HEAD.PACK_SIZE1, m_FILE_HEAD.PACK_SIZE2);
	UNP_SIZE = TakeDWord(m_FILE_HEAD.UNP_SIZE1, m_FILE_HEAD.UNP_SIZE2);

	HEAD_FLAGS = m_FILE_HEAD.BIT_FLAG;
	//est' li parol'?
	if (HEAD_FLAGS & 0x0001) m_Password = TRUE;
	//est' li komentariy fayla?
	if (m_FILE_HEAD.COMMENT_SIZE != 0) m_FileComment = TRUE;
	//v arhive eshe 1 fayl ili 1 papka
	if (PACK_SIZE == 0 && UNP_SIZE == 0)
	{
		BlockType = 2;
		if (!m_zlNumFileFold) m_NumberFolders++;
	}
	else
	{
		BlockType = 1;
		if (!m_zlNumFileFold) m_NumberFiles++;
	}

	double Ver;
	WORD Method;
	Method      = (m_FILE_HEAD.MADE_BY & 0xFF00) / 0x100;
	m_HostOS    = (BYTE)Method;
	Method      = (m_FILE_HEAD.UNP_VER & 0xFF00) / 0x100;
	m_ExtractOS = (BYTE)Method;
	Method      = m_FILE_HEAD.MADE_BY & 0x00FF;
	Ver = (double)Method / 10.0;
	if (m_PackVersion < Ver) m_PackVersion = Ver;
	Method = m_FILE_HEAD.UNP_VER & 0x00FF;
	Ver = (double)Method / 10.0;
	if (m_UnpackVersion < Ver) m_UnpackVersion = Ver;
	Method = m_FILE_HEAD.PACK_METHOD;
	if (m_MethodPack < Method) m_MethodPack = (BYTE)Method;
	Method = m_FILE_HEAD.BIT_FLAG & 0x0006;
	if (m_MethodPackAdd < Method) m_MethodPackAdd = (BYTE)Method;

	//propustit' imya fayla/papki
	MSG = SeekArc(m_FILE_HEAD.NAME_SIZE, NULL);
	if(MSG != TMESSAGE_OK) return MSG;

	//obrabotka EXTRA_FILDS
	BOOL UseBigSizes = FALSE;
	if (m_FILE_HEAD.EXTRA_SIZE > 0)
	{
		m_loffset = m_FILE_HEAD.EXTRA_SIZE;
		while (m_loffset > 0)
		{
			//chitat' zagolovok EXTRA_FILD_HEADER
			MSG = ReadArc(&m_EXTRA_FILD_HEADER, sizeof(m_EXTRA_FILD_HEADER));
			if (MSG != TMESSAGE_OK) return MSG;
			//prochitan zagolovok
			m_loffset -= sizeof(m_EXTRA_FILD_HEADER);
			//obrabativaem nujnie
			switch (m_EXTRA_FILD_HEADER.HEADER_ID)
			{
			//ZIP64_EXTRA_FILD
			case 0x0001:
				if (m_EXTRA_FILD_HEADER.DATA_SIZE >= sizeof(m_ZIP64_EXTRA_FILD))
				{
					UseBigSizes = TRUE;
					//chitat' ZIP64_EXTRA_FILD
					MSG = ReadArc(&m_ZIP64_EXTRA_FILD, sizeof(m_ZIP64_EXTRA_FILD));
					if (MSG != TMESSAGE_OK) return MSG;
					//sobrat' ZIP64_PACK_SIZE i ZIP64_UNP_SIZE
					ZIP64_PACK_SIZE.QuadPart = m_ZIP64_EXTRA_FILD.PACK_SIZE;
					ZIP64_UNP_SIZE.QuadPart  = m_ZIP64_EXTRA_FILD.ORIG_SIZE;
					//prochitan ZIP64_EXTRA_FILD
					m_loffset -= sizeof(m_ZIP64_EXTRA_FILD);
					m_EXTRA_FILD_HEADER.DATA_SIZE -= sizeof(m_ZIP64_EXTRA_FILD);
				}
				//propustit' ostavsheesya
				m_loffset -= m_EXTRA_FILD_HEADER.DATA_SIZE;
				MSG = SeekArc(m_EXTRA_FILD_HEADER.DATA_SIZE, NULL);
				if(MSG != TMESSAGE_OK) return MSG;
				break;
			//JAR64_EXTRA_FILD
			case 0x0064:
				switch (m_EXTRA_FILD_HEADER.DATA_SIZE)
				{
				case sizeof(m_JAR64_EXTRA_FILD_1):
					UseBigSizes = TRUE;
					//chitat' JAR64_EXTRA_FILD_1
					MSG = ReadArc(&m_JAR64_EXTRA_FILD_1, sizeof(m_JAR64_EXTRA_FILD_1));
					if (MSG != TMESSAGE_OK) return MSG;
					//sobrat' ZIP64_PACK_SIZE i ZIP64_UNP_SIZE
					ZIP64_PACK_SIZE.LowPart  = PACK_SIZE;
					ZIP64_PACK_SIZE.HighPart = m_JAR64_EXTRA_FILD_1.HIGH_PACK_SIZE;
					ZIP64_UNP_SIZE.LowPart   = UNP_SIZE;
					ZIP64_UNP_SIZE.HighPart  = m_JAR64_EXTRA_FILD_1.HIGH_ORIG_SIZE;
					//prochitan JAR64_EXTRA_FILD_1
					m_loffset -= sizeof(m_JAR64_EXTRA_FILD_1);
					m_EXTRA_FILD_HEADER.DATA_SIZE -= sizeof(m_JAR64_EXTRA_FILD_1);
					break;
				case sizeof(m_JAR64_EXTRA_FILD_2):
					UseBigSizes = TRUE;
					//chitat' JAR64_EXTRA_FILD_2
					MSG = ReadArc(&m_JAR64_EXTRA_FILD_2, sizeof(m_JAR64_EXTRA_FILD_2));
					if (MSG != TMESSAGE_OK) return MSG;
					//sobrat' ZIP64_PACK_SIZE i ZIP64_UNP_SIZE
					ZIP64_PACK_SIZE.LowPart  = PACK_SIZE;
					ZIP64_PACK_SIZE.HighPart = m_JAR64_EXTRA_FILD_2.HIGH_PACK_SIZE;
					ZIP64_UNP_SIZE.LowPart   = UNP_SIZE;
					ZIP64_UNP_SIZE.HighPart  = m_JAR64_EXTRA_FILD_2.HIGH_ORIG_SIZE;
					//prochitan JAR64_EXTRA_FILD_2
					m_loffset -= sizeof(m_JAR64_EXTRA_FILD_2);
					m_EXTRA_FILD_HEADER.DATA_SIZE -= sizeof(m_JAR64_EXTRA_FILD_2);
					break;
				}
				//propustit' ostavsheesya
				m_loffset -= m_EXTRA_FILD_HEADER.DATA_SIZE;
				MSG = SeekArc(m_EXTRA_FILD_HEADER.DATA_SIZE, NULL);
				if(MSG != TMESSAGE_OK) return MSG;
				break;
			default:
				MSG = SeekArc(m_EXTRA_FILD_HEADER.DATA_SIZE, NULL);
				if(MSG != TMESSAGE_OK) return MSG;
				//propusheno m_EXTRA_FILD_HEADER.DATA_SIZE
				m_loffset -= m_EXTRA_FILD_HEADER.DATA_SIZE;
				break;
			}
		}
	}

	//razmeri faylov
	if (!m_zlPackUnpSize)
	{
		if (UseBigSizes)
		{
			m_PackSizeFiles   += ZIP64_PACK_SIZE.QuadPart;
			m_UnpackSizeFiles += ZIP64_UNP_SIZE.QuadPart;
		}
		else
		{
			m_PackSizeFiles   += PACK_SIZE;
			m_UnpackSizeFiles += UNP_SIZE;
		}
	}

	//propustit' kommentariya fayla/papki
	m_loffset = m_FILE_HEAD.COMMENT_SIZE;

	return TMESSAGE_OK;
}

//vozmojno blok elektronnoy podpisi
//return
//	MSG - resul'tat vipolneniya
int TZipJar::
AnalyzeDigitalSign()
{
	WORD DATA_SIZE = 0;
	
	//chitat' blok
	MSG = ReadArc(&DATA_SIZE, sizeof(DATA_SIZE));
	if (MSG != TMESSAGE_OK) return MSG;

	m_loffset = DATA_SIZE;
	return TMESSAGE_OK;
}

//zip64 directory record block
//return
//	MSG - resul'tat vipolneniya
int TZipJar::
AnalyzeZip64EndRecord()
{
	LARGE_INTEGER SIZE;
	SIZE.QuadPart = 0;

	//chitat' blok
	MSG = ReadArc(&m_ZIP64_END_RECORD, sizeof(m_ZIP64_END_RECORD));
	if (MSG != TMESSAGE_OK) return MSG;

	//versiya upakovi, raspakovki, OS sozdaniya i raspakovki
	double Ver;
	WORD Method;
	Method      = (m_ZIP64_END_RECORD.MADE_BY & 0xFF00) / 0x100;
	m_HostOS    = (BYTE)Method;
	Method      = (m_ZIP64_END_RECORD.UNP_VER & 0xFF00) / 0x100;
	m_ExtractOS = (BYTE)Method;
	Method      = m_ZIP64_END_RECORD.MADE_BY & 0x00FF;
	Ver = (double)Method / 10.0;
	if (m_PackVersion < Ver) m_PackVersion = Ver;
	Method = m_ZIP64_END_RECORD.UNP_VER & 0x00FF;
	Ver = (double)Method / 10.0;
	if (m_UnpackVersion < Ver) m_UnpackVersion = Ver;

	SIZE.LowPart  = TakeDWord(m_ZIP64_END_RECORD.ZIP64_SIZE1, m_ZIP64_END_RECORD.ZIP64_SIZE2);
	SIZE.HighPart = TakeDWord(m_ZIP64_END_RECORD.ZIP64_SIZE3, m_ZIP64_END_RECORD.ZIP64_SIZE4);

	//skol'ko ostalos' neprochitano v etom bloke
	SIZE.QuadPart -= (sizeof(m_ZIP64_END_RECORD) - sizeof(SIZE));	//prochitali (v SIZE razmer bez ucheta samogo SIZE)

	m_loffset = SIZE.LowPart;
	m_hoffset = SIZE.HighPart;
	return TMESSAGE_OK;
}

//zip64 directory locator block
//return
//	MSG - resul'tat vipolneniya
int TZipJar::
AnalyzeZip64EndLocator() 
{
	//chitat' blok
	MSG = ReadArc(&m_ZIP64_END_LOCATOR, sizeof(m_ZIP64_END_LOCATOR));
	if (MSG != TMESSAGE_OK) return MSG;

	return TMESSAGE_OK;
}

//jar64 directory record block
//return
//	MSG - resul'tat vipolneniya
int TZipJar::
AnalyzeJar64EndRecord()
{
	//chitat' blok
	MSG = ReadArc(&m_JAR64_END_RECORD, sizeof(m_JAR64_END_RECORD));
	if (MSG != TMESSAGE_OK) return MSG;

	//proverka na mnogotomnost'
	if (m_JAR64_END_RECORD.DISK != 0)
	{
		m_MultiArc     = TRUE;
		m_VolumeNumber = m_JAR64_END_RECORD.DISK;
	}

	if (m_JAR64_END_RECORD.COMMENT_SIZE != 0)
	{
		//est' kommentariy
		m_MainComment = TRUE;
		//prochitat' kommentariy
		m_CommentSize = m_JAR64_END_RECORD.COMMENT_SIZE;
		MSG = ReadComment();
		if (MSG != TMESSAGE_OK) return MSG;
	}

	return TMESSAGE_OK;
}

//archive end block
//return
//	MSG - resul'tat vipolneniya
int TZipJar::
AnalyzeEndRecord()
{
	//chitat' blok
	MSG = ReadArc(&m_END_RECORD, sizeof(m_END_RECORD));
	if (MSG != TMESSAGE_OK) return MSG;

	//proverka na mnogotomnost'
	if (m_END_RECORD.DISK != 0)
	{
		m_MultiArc     = TRUE;
		m_VolumeNumber = m_END_RECORD.DISK;
	}

	if (m_END_RECORD.COMMENT_SIZE != 0)
	{
		//est' kommentariy
		m_MainComment = TRUE;
		//prochitat' kommentariy
		m_CommentSize = m_END_RECORD.COMMENT_SIZE;
		MSG = ReadComment();
		if (MSG != TMESSAGE_OK) return MSG;
	}

	return TMESSAGE_OK;
}

//archive end block na nalichie ZIP64
//return
//	MSG - resul'tat vipolneniya
int TZipJar::
CheckEndRecordZIP64()
{
	LARGE_INTEGER temp;
	LARGE_INTEGER ttemp;
	temp.QuadPart   = 0;
	ttemp.QuadPart  = 0;
	DWORD SIGNATURE = 0;
	int   toffset   = 0;

	//proverim arhiva na ZIP64 (tam m_loffset i m_hoffset budut drugie)
	//pered (SIGNATURE[4]+END_RECORD[18])[22] ishem (SIGNATURE[4]+ZIP64_END_LOCATOR[16])[20]
	toffset = sizeof(SIGNATURE) + sizeof(m_END_RECORD) +
				sizeof(SIGNATURE) + sizeof(m_ZIP64_END_LOCATOR);
	ttemp.QuadPart = GetArcPointer();
	if (ttemp.QuadPart > toffset)
	{
		ttemp.QuadPart = ttemp.QuadPart - toffset;
		MSG = SeekArc(ttemp.LowPart, &ttemp.HighPart, FILE_BEGIN);
		if (MSG != TMESSAGE_OK) return MSG;
		MSG = ReadArc(&SIGNATURE, sizeof(SIGNATURE));
		if (MSG != TMESSAGE_OK) return MSG;
		//proverka signaturi
		if (SIGNATURE == 0x07064b50)
		{
			//chtenie ZIP64_END_LOCATOR
			MSG = ReadArc(&m_ZIP64_END_LOCATOR, sizeof(m_ZIP64_END_LOCATOR));
			if (m_DetailLF == 2)
				m_LogFile->WriteMessage(m_LangMsg.BlockZip64DirectoryLocator, MSG);
			if (MSG != TMESSAGE_OK) return MSG;

			//poluchim ZIP64_END_RECORD
			ttemp.LowPart  = m_ZIP64_END_LOCATOR.OFFSET1;
			ttemp.HighPart = m_ZIP64_END_LOCATOR.OFFSET2;
			MSG = SeekArc(ttemp.LowPart, &ttemp.HighPart, FILE_BEGIN);
			if (MSG != TMESSAGE_OK) return MSG;
			temp.QuadPart = GetArcPointer();
			MSG = ReadArc(&SIGNATURE, sizeof(SIGNATURE));
			if (MSG != TMESSAGE_OK) return MSG;
			//proverka signaturi
			if (SIGNATURE == 0x06064b50)
			{
				//chtenie ZIP64_END_RECORD
				MSG = ReadArc(&m_ZIP64_END_RECORD, sizeof(m_ZIP64_END_RECORD));
				if (m_DetailLF == 2)
					m_LogFile->WriteMessage(m_LangMsg.BlockZip64DirectoryRecord, MSG);
				if (MSG != TMESSAGE_OK) return MSG;
				LARGE_INTEGER SIZE;
				SIZE.LowPart  = TakeDWord(m_ZIP64_END_RECORD.SIZE1, m_ZIP64_END_RECORD.SIZE2);
				SIZE.HighPart = TakeDWord(m_ZIP64_END_RECORD.SIZE3, m_ZIP64_END_RECORD.SIZE4);
				temp.QuadPart = temp.QuadPart - SIZE.QuadPart;
				m_loffset = temp.LowPart;
				m_hoffset = temp.HighPart;
				if (m_loffset < 0) m_BigOffset = TRUE;	//zapomnim chto chislo bol'shoe (razmer - smeshenie dlya >2Gb <4Gb)
			}
		}
	}

	return TMESSAGE_OK;
}

/////////////////////////////////////////////////////////////////////////////
//FUNCTIONS PUBLIC
/////////////////////////////////////////////////////////////////////////////

//fayl yavlyaetsya arhivom ZIP/JAR
//path - imya fayla dlya testirovaniya
//return
//	TMESSAGE_OK     - ok
//	ERROR_OPEN_FILE - oshibka otckritiya fayla
//	ERROR_READ_FILE - oshibka chteniya fayla
//	ERROR_SEEK_FILE - oshibka smesheniya fayla
//	SUDDEN_END_FILE - vnezapniy konec fayla
//	ERROR_FORMAT    - oshibka formata fayla
int TZipJar::
TestFile(char* path)
{
	//obnulenie dannih
	InitialParametrs();
	m_zlNumFileFold = FALSE;
	m_zlPackUnpSize = FALSE;

	//eto ZIP ili JAR arhiv
	char* ext  = 0;
	int   size = strlen(path);
	ext = path + size - 3;
	ext = _strlwr(ext);
	if (strcmp(ext, "jar") == 0) m_ArchiveType = TRUE;	//JAR

	//otkrit' fayl dlya chteniya
	m_h = CreateFile(path, GENERIC_READ, FILE_SHARE_READ, 
			NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (m_h == INVALID_HANDLE_VALUE) 
		{ if (m_h) { ::CloseHandle(m_h); m_h = 0; } return TERROR_OPEN_FILE; }

	DWORD SIGNATURE = 0;
	//chtenie signaturi
	MSG = ReadArc(&SIGNATURE, sizeof(SIGNATURE));
	if (MSG != TMESSAGE_OK) { if (m_h) { ::CloseHandle(m_h); m_h = 0; } return MSG; }

	if (SIGNATURE == 0x08074B50) m_MultiArc = TRUE;

	BOOL ThisZIP = FALSE;
	//proverka signaturi
	if ((SIGNATURE != 0x04034B50) &&
		(SIGNATURE != 0x08074B50)) MSG = TERROR_FORMAT;
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
				for (int i=0; i < len-9; i++)
				{
					if ((buffer[i]   == 0x50) &&
						(buffer[i+1] == 0x4B) &&
						(buffer[i+2] == 0x03) &&
						(buffer[i+3] == 0x04) && 
						(buffer[i+4] >  0x00) &&
						(buffer[i+4] <  0x30) &&
						(buffer[i+5] <  0x15) &&
						(buffer[i+8] <  0x10) &&
						(buffer[i+9] == 0x00))
					{
						ThisZIP = TRUE;
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

				//SFX ili net
				if ((m_DetailLF == 2) && (CheckSFX))
				{
					m_LogFile->WriteMessage(m_LangMsg.SFXModule, Flag);
					if (Flag == TMESSAGE_OK)
						m_LogFile->WriteMessage(m_LangMsg.SignatureArchive, Flag);
				}
			}
		}
	}
	else ThisZIP = TRUE;

	//dlya arhivov > 2Gb
	LARGE_INTEGER temp;
	LARGE_INTEGER ttemp;
	temp.QuadPart  = 0;
	ttemp.QuadPart = 0;

	int toffset = 0;
	//Teper' poprobovat' nayti koncevoy blok arhiva
	//(SIGNATURE[4]+END_RECORD[18])[22]
	//a tocnee ego signaturu (SIGNATURE[4])
	toffset = sizeof(SIGNATURE) + sizeof(m_END_RECORD);
	ttemp.QuadPart = GetArchSize();
	ttemp.QuadPart = ttemp.QuadPart - toffset;
	MSG = SeekArc(ttemp.LowPart, &ttemp.HighPart, FILE_BEGIN);
	if (MSG != TMESSAGE_OK) { if (m_h) { ::CloseHandle(m_h); m_h = 0; } return MSG; }
	temp.QuadPart = GetArcPointer();
	MSG = ReadArc(&SIGNATURE, sizeof(SIGNATURE));
	if (MSG != TMESSAGE_OK) { if (m_h) { ::CloseHandle(m_h); m_h = 0; } return MSG; }

	//proverka signaturi
	if (SIGNATURE == 0x06054b50)
	{
		//chtenie koncevogo bloka
		MSG = ReadArc(&m_END_RECORD, sizeof(m_END_RECORD));
		if (MSG != TMESSAGE_OK)
		{
			if (m_DetailLF == 2)
				m_LogFile->WriteMessage(m_LangMsg.BlockArchiveEnd, MSG);
			if (m_h) { ::CloseHandle(m_h); m_h = 0; }
			return MSG;
		}
		DWORD SIZE;
		SIZE = TakeDWord(m_END_RECORD.SIZE1, m_END_RECORD.SIZE2);
		if (m_END_RECORD.DISK == m_END_RECORD.DISK_START)	//CENTRAL_DIRECTORY smotrim na nemnogotomnom arhive inache medlenno s nachala fayla
		{
			temp.QuadPart = temp.QuadPart - SIZE;
			m_loffset = temp.LowPart;
			m_hoffset = temp.HighPart;
			if (m_loffset < 0) m_BigOffset = TRUE;	//zapomnim chto chislo bol'shoe (razmer - smeshenie dlya >2Gb <4Gb)
			if (m_DetailLF == 2)
				m_LogFile->WriteMessage(m_LangMsg.BlockArchiveEnd, TMESSAGE_OK);

			//proverim arhiva na ZIP64 (tam m_loffset i m_hoffset budut drugie)
			//pered (SIGNATURE[4]+END_RECORD[18])[22] ishem (SIGNATURE[4]+ZIP64_END_LOCATOR[16])[20]
			MSG = CheckEndRecordZIP64();
			if (MSG != TMESSAGE_OK) { if (m_h) { ::CloseHandle(m_h); m_h = 0; } return MSG; }

			if (m_h) { ::CloseHandle(m_h); m_h = 0; }
			return TMESSAGE_OK;
		}
	}

	//Togda poprobovat' nayti koncevoy blok arhiva JAR64
	//(SIGNATURE[4]+JAR64_END_RECORD[22])[26]
	//a tocnee ego signaturu (SIGNATURE[4])
	toffset = sizeof(SIGNATURE) + sizeof(m_JAR64_END_RECORD);
	ttemp.QuadPart = GetArchSize();
	ttemp.QuadPart = ttemp.QuadPart - toffset;
	MSG = SeekArc(ttemp.LowPart, &ttemp.HighPart, FILE_BEGIN);
	if (MSG != TMESSAGE_OK) { if (m_h) { ::CloseHandle(m_h); m_h = 0; } return MSG; }
	temp.QuadPart = GetArcPointer();
	MSG = ReadArc(&SIGNATURE, sizeof(SIGNATURE));
	if (MSG != TMESSAGE_OK) { if (m_h) { ::CloseHandle(m_h); m_h = 0; } return MSG; }

	//proverka signaturi
	if (SIGNATURE == 0x06055b50)
	{
		//chtenie koncevogo bloka
		MSG = ReadArc(&m_JAR64_END_RECORD, sizeof(m_JAR64_END_RECORD));
		if (MSG != TMESSAGE_OK)
		{
			if (m_DetailLF == 2)
				m_LogFile->WriteMessage(m_LangMsg.BlockJar64DirectoryRecord, MSG);
			if (m_h) { ::CloseHandle(m_h); m_h = 0; }
			return MSG;
		}
		DWORD SIZE;
		SIZE = TakeDWord(m_JAR64_END_RECORD.SIZE1, m_JAR64_END_RECORD.SIZE2);
		if (m_JAR64_END_RECORD.DISK == m_JAR64_END_RECORD.DISK_START)	//CENTRAL_DIRECTORY smotrim na nemnogotomnom arhive inache medlenno s nachala fayla
		{
			temp.QuadPart = temp.QuadPart - SIZE;
			m_loffset = temp.LowPart;
			m_hoffset = temp.HighPart;
			if (m_loffset < 0) m_BigOffset = TRUE;	//zapomnim chto chislo bol'shoe (razmer - smeshenie dlya >2Gb <4Gb)
			if (m_DetailLF == 2)
				m_LogFile->WriteMessage(m_LangMsg.BlockJar64DirectoryRecord, TMESSAGE_OK);
			if (m_h) { ::CloseHandle(m_h); m_h = 0; }

			m_ArchiveType = TRUE;	//JAR
			return TMESSAGE_OK;
		}
	}

	//Vozmojno u arhiva est' kommentariy posle koncevogo bloka
	//poprobovat' nayti koncevoy blok arhiva
	//(SIGNATURE[4]+END_RECORD[18])[22]
	//ili flya JAR64 (SIGNATURE[4]+JAR64_END_RECORD[22])[26]
	//(berem 2e t.k. 26>22)
	//a tocnee ego signaturu (SIGNATURE[4]) pered kommentariem
	BOOL ErrorFindEndBlock = FALSE;
	int  len               = MAX_LEN_END_RECORD;
	BYTE *buffer           = 0;

	//ne perebrat' razmer arhiva
	temp.QuadPart = GetArchSize();
	if (temp.QuadPart < len) len = (int)temp.QuadPart;

	buffer = new BYTE[len];
	//vstaem v konec fayla i smeshaemsya  k nachalu na len byte
	ttemp.QuadPart = GetArchSize();
	ttemp.QuadPart = ttemp.QuadPart - len;
	MSG = SeekArc(ttemp.LowPart, &ttemp.HighPart, FILE_BEGIN);
	if (MSG != TMESSAGE_OK)
	{
		if (buffer) { delete []buffer; buffer = 0; }
		if (m_h) { ::CloseHandle(m_h); m_h = 0; } return MSG;
	}
	MSG = ReadArc(buffer, len);
	if (MSG != TMESSAGE_OK)
	{
		if (buffer) { delete []buffer; buffer = 0; }
		if (m_h) { ::CloseHandle(m_h); m_h = 0; } return MSG;
	}
	
	//iskat' metku koncevogo bloka
	//for (int i=len-22; i >= 0; i--)
	for (int i=len-26; i >= 0; i--)	//pereshli na offset(JAR64)=26 poetomu delaem vezde +4
	{
		//ZIP, JAR, ZIP64
		if ((buffer[i+4] == 0x50)                &&
			(buffer[i+5] == 0x4B)                &&
			(buffer[i+6] == 0x05)                &&
			(buffer[i+7] == 0x06)                &&
			(buffer[i+8] == buffer[i+10])        &&	//DISK = DISK_START
			(buffer[i+9] == buffer[i+11])        &&	//CENTRAL_DIRECTORY smotrim na nemnogotomnom arhive inache medlenno s nachala fayla
			(buffer[i+24] + buffer[i+25] != 0x00))	//kommentariy est' (t.k. seychas ishodim iz etogo)
		{
			ThisZIP = TRUE;

			//Teper' poprobovat' takoy je razbor chto i vishe
			//ttemp.QuadPart = temp.QuadPart - len + i;
			ttemp.QuadPart = temp.QuadPart - len + i + 4;
			MSG = SeekArc(ttemp.LowPart, &ttemp.HighPart, FILE_BEGIN);
			if (MSG != TMESSAGE_OK) { ErrorFindEndBlock = TRUE; break; }
			temp.QuadPart = GetArcPointer();
			MSG = ReadArc(&SIGNATURE, sizeof(SIGNATURE));
			if (MSG != TMESSAGE_OK) { ErrorFindEndBlock = TRUE; break; }

			//proverka signaturi
			if (SIGNATURE == 0x06054b50)
			{
				//chtenie koncevogo bloka
				MSG = ReadArc(&m_END_RECORD, sizeof(m_END_RECORD));
				if (MSG != TMESSAGE_OK)
				{
					if (m_DetailLF == 2)
						m_LogFile->WriteMessage(m_LangMsg.BlockArchiveEnd, MSG);
					ErrorFindEndBlock = TRUE;
					break;
				}
				DWORD SIZE;
				SIZE = TakeDWord(m_END_RECORD.SIZE1, m_END_RECORD.SIZE2);
				if (m_END_RECORD.DISK == m_END_RECORD.DISK_START)	//CENTRAL_DIRECTORY smotrim na nemnogotomnom arhive inache medlenno s nachala fayla
				{
					temp.QuadPart = temp.QuadPart - SIZE;
					m_loffset = temp.LowPart;
					m_hoffset = temp.HighPart;
					if (m_loffset < 0) m_BigOffset = TRUE;	//zapomnim chto chislo bol'shoe (razmer - smeshenie dlya >2Gb <4Gb)
					if (m_DetailLF == 2)
						m_LogFile->WriteMessage(m_LangMsg.BlockArchiveEnd, TMESSAGE_OK);

					//proverim arhiva na ZIP64 (tam m_loffset i m_hoffset budut drugie)
					//pered (SIGNATURE[4]+END_RECORD[18])[22] ishem (SIGNATURE[4]+ZIP64_END_LOCATOR[16])[20]
					MSG = CheckEndRecordZIP64();
					if (MSG != TMESSAGE_OK) { ErrorFindEndBlock = TRUE; break; }

					break;
				}
			}
		}
		//JAR64
		if ((buffer[i]   == 0x50)                &&
			(buffer[i+1] == 0x5B)                &&
			(buffer[i+2] == 0x05)                &&
			(buffer[i+3] == 0x06)                &&
			(buffer[i+4] == buffer[i+6])         &&	//DISK = DISK_START
			(buffer[i+5] == buffer[i+7])         &&	//CENTRAL_DIRECTORY smotrim na nemnogotomnom arhive inache medlenno s nachala fayla
			(buffer[i+24] + buffer[i+25] != 0x00))	//kommentariy est' (t.k. seychas ishodim iz etogo)
		{
			ThisZIP = TRUE;

			//Teper' poprobovat' takoy je razbor chto i vishe
			ttemp.QuadPart = temp.QuadPart - len + i;
			MSG = SeekArc(ttemp.LowPart, &ttemp.HighPart, FILE_BEGIN);
			if (MSG != TMESSAGE_OK) { ErrorFindEndBlock = TRUE; break; }
			temp.QuadPart = GetArcPointer();
			MSG = ReadArc(&SIGNATURE, sizeof(SIGNATURE));
			if (MSG != TMESSAGE_OK) { ErrorFindEndBlock = TRUE; break; }

			//proverka signaturi
			if (SIGNATURE == 0x06055b50)
			{
				//chtenie koncevogo bloka
				MSG = ReadArc(&m_JAR64_END_RECORD, sizeof(m_JAR64_END_RECORD));
				if (MSG != TMESSAGE_OK)
				{
					if (m_DetailLF == 2)
						m_LogFile->WriteMessage(m_LangMsg.BlockJar64DirectoryRecord, MSG);
					ErrorFindEndBlock = TRUE;
					break;
				}
				DWORD SIZE;
				SIZE = TakeDWord(m_JAR64_END_RECORD.SIZE1, m_JAR64_END_RECORD.SIZE2);
				if (m_JAR64_END_RECORD.DISK == m_JAR64_END_RECORD.DISK_START)	//CENTRAL_DIRECTORY smotrim na nemnogotomnom arhive inache medlenno s nachala fayla
				{
					temp.QuadPart = temp.QuadPart - SIZE;
					m_loffset = temp.LowPart;
					m_hoffset = temp.HighPart;
					if (m_loffset < 0) m_BigOffset = TRUE;	//zapomnim chto chislo bol'shoe (razmer - smeshenie dlya >2Gb <4Gb)
					if (m_DetailLF == 2)
						m_LogFile->WriteMessage(m_LangMsg.BlockJar64DirectoryRecord, TMESSAGE_OK);

					m_ArchiveType = TRUE;	//JAR
					break;
				}
			}
		}
	}
	if (buffer) delete []buffer;
	buffer = 0;
	if (ErrorFindEndBlock)
		{ if (m_h) { ::CloseHandle(m_h); m_h = 0; } return MSG; }

	if (!ThisZIP) 
		{ if (m_h) { ::CloseHandle(m_h); m_h = 0; } return TERROR_FORMAT; }

	//pomoemu eto lishnee, a zachem napisal ne pomnyu
	//if (m_loffset < 0)
	//	{ if (m_h) { ::CloseHandle(m_h); m_h = 0; } return TERROR_FORMAT; }

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
int TZipJar::
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
	DWORD SIGNATURE = 0;
	int   flag      = 0;
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

		//prochitat' sleduyushuyu signaturu
		flag = ReadArc(&SIGNATURE, sizeof(SIGNATURE));
		if (flag == TERROR_READ_FILE) { if (m_h) { ::CloseHandle(m_h); m_h = 0; } return flag; }
		//mojet fayl zakonchilsya
		if (flag == TSUDDEN_END_FILE)
		{
			m_MultiArc = TRUE;
			CountRatioArchiveSize();
			CountRatioPackFileSize();
			if (m_h) { ::CloseHandle(m_h); m_h = 0; }
			if (m_MultiArc) m_VolumeNumber++;
			return TMESSAGE_OK;
		}

		//zagolovok kakogo bloka
		switch (SIGNATURE)
		{
			//lockal'niy zagolovok fayla (bivaet u odnogo iz vidov mnogotomnih arhivov)
			case 0x08074B50:	m_MultiArc = TRUE; break;
			//lockal'niy zagolovok fayla (local file header)
			case 0x04034b50:	m_BlockType = 0;
								MSG = AnalyzeLocalFileHead(m_BlockType);
								if (m_DetailLF == 2)
								{
									switch (m_BlockType)
									{
									case 1: m_LogFile->WriteMessage(m_LangMsg.BlockLocalFile, MSG);
											break;
									case 2: m_LogFile->WriteMessage(m_LangMsg.BlockLocalFolder, MSG);
											break;
									default: m_LogFile->WriteMessage(m_LangMsg.BlockUnknown, MSG);
											break;
									}
								}
								if (MSG != TMESSAGE_OK)
								{ if (m_h) { ::CloseHandle(m_h); m_h = 0; } return MSG; }
								break;
			//zagolovok fayla (file data)
			case 0x02014b50:	m_BlockType = 0;
								MSG = AnalyzeFileHead(m_BlockType);
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
			//elektronnya podpis' (Digital signature)
			case 0x05054b50:	MSG = AnalyzeDigitalSign();
								if (m_DetailLF == 2)
									m_LogFile->WriteMessage(m_LangMsg.BlockAuthentityVerification, MSG);
								if (MSG != TMESSAGE_OK)
								{ if (m_h) { ::CloseHandle(m_h); m_h = 0; } return MSG; }
								break;
			//Zip64 end of central directory record
			case 0x06064b50:	MSG = AnalyzeZip64EndRecord();
								if (m_DetailLF == 2)
									m_LogFile->WriteMessage(m_LangMsg.BlockZip64DirectoryRecord, MSG);
								if (MSG != TMESSAGE_OK)
								{ if (m_h) { ::CloseHandle(m_h); m_h = 0; } return MSG; }
								break;
			//Zip64 end of central directory locator
			case 0x07064b50:	MSG = AnalyzeZip64EndLocator();
								if (m_DetailLF == 2)
									m_LogFile->WriteMessage(m_LangMsg.BlockZip64DirectoryLocator, MSG);
								if (MSG != TMESSAGE_OK)
								{ if (m_h) { ::CloseHandle(m_h); m_h = 0; } return MSG; }
								break;
			//Jar64 end of central directory record
			case 0x06055b50:	m_ArchiveType = TRUE;	//JAR
								MSG = AnalyzeJar64EndRecord();
								if (m_DetailLF == 2)
									m_LogFile->WriteMessage(m_LangMsg.BlockJar64DirectoryRecord, MSG);
								if (MSG != TMESSAGE_OK)
								{ if (m_h) { ::CloseHandle(m_h); m_h = 0; } return MSG; }
								CountRatioArchiveSize();
								CountRatioPackFileSize();
								if (m_MultiArc) m_VolumeNumber++;
								if (m_h) { ::CloseHandle(m_h); m_h = 0; }
								return TMESSAGE_OK;
								break;
			//koncevoy blok (end of central directory locator)
			case 0x06054b50:	MSG = AnalyzeEndRecord();
								if (m_DetailLF == 2)
									m_LogFile->WriteMessage(m_LangMsg.BlockArchiveEnd, MSG);
								if (MSG != TMESSAGE_OK)
								{ if (m_h) { ::CloseHandle(m_h); m_h = 0; } return MSG; }
								CountRatioArchiveSize();
								CountRatioPackFileSize();
								if (m_MultiArc) m_VolumeNumber++;
								if (m_h) { ::CloseHandle(m_h); m_h = 0; }
								return TMESSAGE_OK;
								break;
			default:			if (m_DetailLF == 2)
									m_LogFile->WriteMessage(m_LangMsg.BlockUnknown, TERROR_FORMAT);
								if (m_h) { ::CloseHandle(m_h); m_h = 0; }
								return TERROR_FORMAT;
								break;
		}
	}
}