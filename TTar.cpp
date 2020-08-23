/////////////////////////////////////////////////////////////////////////////
//TTar.cpp
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TTar.h"

/////////////////////////////////////////////////////////////////////////////
//FUNCTIONS PRIVATE
/////////////////////////////////////////////////////////////////////////////

//obrabotka zagolovka fayla/papki
//head - struktura zagolovka bloka
//return
//	MSG - resul'tat vipolneniya
int TTar::
AnalyzeFileHead(TAR_FILE_HEAD& head)
{
	LARGE_INTEGER FILE_SIZE;
	FILE_SIZE.QuadPart = 0;
	DWORD CRC       = 0;
	DWORD FTIME     = 0;

	//razmer fayla
	sscanf(head.size,   "%o", &FILE_SIZE.QuadPart);
	//CRC fayla
	sscanf(head.chksum, "%o", &CRC);
	//vreamya poslednego izmeneniya fayla
	sscanf(head.mtime,  "%o", &FTIME);

	//sscanf("%o") mojet preobrazovivat' chsla do 32 bit
	//esli chislo bol'she to svishe 32 bit vse otbrositsya
	//dlya proverki preobrazuem nashe SIZE obratno v stroku
	//sprintf("%o") i sravnim stroki, esli oni ne ravni to
	//dobavit' v HighPart 0x01
	
	//original'naya stroka (ostavim tol'ko cifri)
	//bivayut nenujnie 0x00('\0') 0x20(' ') 0x30 ('0')
	char StrSize1[12] = {NULL};
	BOOL BeginNumber = FALSE;
	for(int i=0, j=0; i<11; i++)	//berem do 11 t.k. 12 simvol vsegda razdelitel'
	{
		if (head.size[i] == 0x00)	//'\0'
			continue;
		if (head.size[i] == 0x20)	//' '
			continue;
		if ((head.size[i] == 0x30) && !BeginNumber)	//'0' i chislo eshe ne nachalos'
			continue;

		BeginNumber = TRUE;
		StrSize1[j++] = head.size[i];
	}
	StrSize1[j] = '\0';	//zakonchim stroku
	//esli razmer bil 0 to dlina stroki poluchilas' 0
	//sdelaem togda prosto chislo 0 v stroke "0"
	if (strlen(StrSize1) == 0)
	{
		StrSize1[0] = '0';
		StrSize1[1] = '\0';
	}

	//nashe obratnopreobrazovannoe v stroku chsilo
	char StrSize2[12] = {NULL};
	sprintf(StrSize2, "%o", FILE_SIZE.LowPart);
	
	//sravnim i esli neravenstvo to uvelichim HighPart
	if (strcmp(StrSize1, StrSize2) != 0)
		FILE_SIZE.HighPart++;

	//znachenie polya magic
	int size = strlen(head.magic);
	if (m_pOriginalName) {delete[] m_pOriginalName; m_pOriginalName = 0;}
	m_pOriginalName = new char[size+1];
	strcpy(m_pOriginalName, head.magic);

	//eshe 1 fayl ili papka
	if ((BYTE)head.linkflag == 0x35)
	{
		m_NumberFolders++;
	}
	if (((BYTE)head.linkflag == 0x00) || ((BYTE)head.linkflag == 0x30))
	{
		m_NumberFiles++;
		//rasmeri
		m_PackSizeFiles   += FILE_SIZE.QuadPart;
		m_UnpackSizeFiles += FILE_SIZE.QuadPart;
	}

	//vipolnit' ravnenie po granice 0x200
	LONGLONG offset  = (DWORD)GetArcPointer();
	LONGLONG of1     = offset + FILE_SIZE.QuadPart;
	LONGLONG toffset = offset + FILE_SIZE.QuadPart;
	while ((toffset % 0x200) != 0)
	{
		toffset++;
	}
	FILE_SIZE.QuadPart += toffset - offset - FILE_SIZE.QuadPart;

	m_loffset = FILE_SIZE.LowPart;
	m_hoffset = FILE_SIZE.HighPart;
	if ((LONG)FILE_SIZE.LowPart < 0) m_BigOffset = TRUE;

	return TMESSAGE_OK;
}

/////////////////////////////////////////////////////////////////////////////
//FUNCTIONS PUBLIC
/////////////////////////////////////////////////////////////////////////////

//fayl yavlyaetsya arhivom TAR
//path - imya fayla dlya testirovaniya
//return
//	TMESSAGE_OK     - ok
//	ERROR_OPEN_FILE - oshibka otckritiya fayla
//	ERROR_READ_FILE - oshibka chteniya fayla
//	ERROR_SEEK_FILE - oshibka smesheniya fayla
//	SUDDEN_END_FILE - vnezapniy konec fayla
//	ERROR_FORMAT    - oshibka formata fayla
int TTar::
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
	MSG = ReadArc(&m_FILE_HEAD, sizeof(m_FILE_HEAD));
	if (MSG != TMESSAGE_OK) { if (m_h) { ::CloseHandle(m_h); m_h = 0; } return MSG; }

	//linkflag imeet znachenie ot '0' do '7', '\0', 'D', 'L', 'M', 'V'
	if (((BYTE)m_FILE_HEAD.linkflag != 0x00) &&	//'\0' (LF_OLDNORMAL) Normal disk file, Unix compatible
		((BYTE)m_FILE_HEAD.linkflag != 0x30) &&	//'0'  (LF_NORMAL) Normal disk file
		((BYTE)m_FILE_HEAD.linkflag != 0x31) &&	//'1'  (LF_LINK) Link to previously dumped file
		((BYTE)m_FILE_HEAD.linkflag != 0x32) &&	//'2'  (LF_SYMLINK) Symbolic link
		((BYTE)m_FILE_HEAD.linkflag != 0x33) &&	//'3'  (LF_CHR) Character special file
		((BYTE)m_FILE_HEAD.linkflag != 0x34) &&	//'4'  (LF_BLK) Block special file
		((BYTE)m_FILE_HEAD.linkflag != 0x35) &&	//'5'  (LF_DIR) Directory
		((BYTE)m_FILE_HEAD.linkflag != 0x36) &&	//'6'  (LF_FIFO) FIFO special file
		((BYTE)m_FILE_HEAD.linkflag != 0x37) &&	//'7'  (LF_CONTIG) Contiguous file
		((BYTE)m_FILE_HEAD.linkflag != 0x44) &&	//'D'  (LF_DUMPDIR)
		((BYTE)m_FILE_HEAD.linkflag != 0x4C) &&	//'L'  (LF_LONGLINK) Length file name > 100 symbols
		((BYTE)m_FILE_HEAD.linkflag != 0x4D) &&	//'M'  (LF_MULTIVOL)
		((BYTE)m_FILE_HEAD.linkflag != 0x56))	//'V'  (LF_VOLHDR)
	{
		if (m_DetailLF == 2)
			m_LogFile->WriteMessage(m_LangMsg.BlockArhiveHeader, TERROR_FORMAT);
		if (m_h) { ::CloseHandle(m_h); m_h = 0; }
		return TERROR_FORMAT;
	}

	//magic ravno "ustar  " libo "ustar00" libo "GNUtar " libo "GNUtar0" libo ""
	if ((strcmp(m_FILE_HEAD.magic, "ustar  ") != 0) &&
		(strcmp(m_FILE_HEAD.magic, "GNUtar ") != 0) &&
		(strcmp(m_FILE_HEAD.magic, "ustar00") != 0)	&&	//videl takoe
		(strcmp(m_FILE_HEAD.magic, "GNUtar0") != 0) &&	//po analogii s predidushim
		(strcmp(m_FILE_HEAD.magic, "")        != 0))	//videl i voobshe nezapolnennoe
	{
		if (m_DetailLF == 2)
			m_LogFile->WriteMessage(m_LangMsg.BlockArhiveHeader, TERROR_FORMAT);
		if (m_h) { ::CloseHandle(m_h); m_h = 0; }
		return TERROR_FORMAT;
	}

	if (m_DetailLF == 2)
		m_LogFile->WriteMessage(m_LangMsg.BlockArhiveHeader, TMESSAGE_OK);

	//arhiv TAR doljen bit' viravnen blokom 512 BYTE (0x200)
	//i zakanchivatsya lokom 512 BYTE (0x200) nule (0x00) eto ne vsegda
	//poetomu proverim razmer arhiva na viravnivanie
	if ((GetArchSize() % 0x200) != 0)
	{
		if (m_h) { ::CloseHandle(m_h); m_h = 0; }
		return TERROR_FORMAT;
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
int TTar::
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

		//TAR arhiv doljen zakanchivatsya blokom 512 BYTE (0x200) nuleqy (0x00)
		//no eto uslovie vidimo neobyazatel'no ili ne vse ego vipolnyayut
		//poetomu proverim mojet mi uje stoim na granice arhiva
		if (GetArchSize() == GetArcPointer())
		{
			CountRatioArchiveSize();
			CountRatioPackFileSize();
			if (m_h) { ::CloseHandle(m_h); m_h = 0; }
			return TMESSAGE_OK;
		}

		//prochitat' zagolovok fayla
		flag = ReadArc(&m_FILE_HEAD, sizeof(m_FILE_HEAD));
		if (flag != TMESSAGE_OK) { if (m_h) { ::CloseHandle(m_h); m_h = 0; } return flag; }

		//linkflag imeet znachenie ot '0' do '7', '\0', 'D', 'L', 'M', 'V'
		//inache mojet konec arhiva
		if (//((BYTE)m_FILE_HEAD.linkflag != 0x00) &&	//'\0' (LF_OLDNORMAL) Normal disk file, Unix compatible
			((BYTE)m_FILE_HEAD.linkflag != 0x30) &&	//'0'  (LF_NORMAL) Normal disk file
			((BYTE)m_FILE_HEAD.linkflag != 0x31) &&	//'1'  (LF_LINK) Link to previously dumped file
			((BYTE)m_FILE_HEAD.linkflag != 0x32) &&	//'2'  (LF_SYMLINK) Symbolic link
			((BYTE)m_FILE_HEAD.linkflag != 0x33) &&	//'3'  (LF_CHR) Character special file
			((BYTE)m_FILE_HEAD.linkflag != 0x34) &&	//'4'  (LF_BLK) Block special file
			((BYTE)m_FILE_HEAD.linkflag != 0x35) &&	//'5'  (LF_DIR) Directory
			((BYTE)m_FILE_HEAD.linkflag != 0x36) &&	//'6'  (LF_FIFO) FIFO special file
			((BYTE)m_FILE_HEAD.linkflag != 0x37) &&	//'7'  (LF_CONTIG) Contiguous file
			((BYTE)m_FILE_HEAD.linkflag != 0x44) &&	//'D'  (LF_DUMPDIR)
			((BYTE)m_FILE_HEAD.linkflag != 0x4C) &&	//'L'  (LF_LONGLINK) Length file name > 100 symbols
			((BYTE)m_FILE_HEAD.linkflag != 0x4D) &&	//'M'  (LF_MULTIVOL)
			((BYTE)m_FILE_HEAD.linkflag != 0x56))	//'V'  (LF_VOLHDR)
		{
			BOOL END_ARCHIVE = TRUE;
			//konec arhiva 0x200 nuley 0x00
			for (int i=0; i<RECORDSIZE; i++)
			{
				if (m_FILE_HEAD.charptr[i] != 0x00)
				{
					END_ARCHIVE = FALSE;
					break;
				}
			}
			if (m_h) { ::CloseHandle(m_h); m_h = 0; }
			if (END_ARCHIVE)
			{
				CountRatioArchiveSize();
				CountRatioPackFileSize();
				if (m_DetailLF == 2)
					m_LogFile->WriteMessage(m_LangMsg.BlockArchiveEnd, TMESSAGE_OK);
				if (m_h) { ::CloseHandle(m_h); m_h = 0; }
				return TMESSAGE_OK;
			}
			else { if (m_h) { ::CloseHandle(m_h); m_h = 0; } return TERROR_FORMAT; }
		}

		//magic ravno "ustar  " libo "ustar00" libo "GNUtar " libo "GNUtar0"
		//inache mojet konec arhiva
		if ((strcmp(m_FILE_HEAD.magic, "ustar  ") != 0) &&
			(strcmp(m_FILE_HEAD.magic, "GNUtar ") != 0) &&
			(strcmp(m_FILE_HEAD.magic, "ustar00") != 0)	&&	//videl takoe
			(strcmp(m_FILE_HEAD.magic, "GNUtar0") != 0) &&	//po analogii s predidushim
			(strcmp(m_FILE_HEAD.magic, "")        != 0))	//videl i voobshe nezapolnennoe
		{
			BOOL END_ARCHIVE = TRUE;
			//konec arhiva 0x200 nuley 0x00
			for (int i=0; i<RECORDSIZE; i++)
			{
				if (m_FILE_HEAD.charptr[i] != 0x00)
				{
					END_ARCHIVE = FALSE;
					break;
				}
			}
			if (m_h) { ::CloseHandle(m_h); m_h = 0; }
			if (END_ARCHIVE)
			{
				CountRatioArchiveSize();
				CountRatioPackFileSize();
				if (m_DetailLF == 2)
					m_LogFile->WriteMessage(m_LangMsg.BlockArchiveEnd, TMESSAGE_OK);
				if (m_h) { ::CloseHandle(m_h); m_h = 0; }
				return TMESSAGE_OK;
			}
			else { if (m_h) { ::CloseHandle(m_h); m_h = 0; } return TERROR_FORMAT; }
		}

		MSG = AnalyzeFileHead(m_FILE_HEAD);
		if (m_DetailLF == 2)
		{
			switch ((BYTE)m_FILE_HEAD.linkflag)
			{
			case 0x00:	m_LogFile->WriteMessage(m_LangMsg.BlockFile, MSG);		//'\0' (LF_OLDNORMAL) Normal disk file, Unix compatible
						break;
			case 0x30:	m_LogFile->WriteMessage(m_LangMsg.BlockFile, MSG);		//'0'  (LF_NORMAL) Normal disk file
						break;
			case 0x31:	m_LogFile->WriteMessage(m_LangMsg.BlockUnknown, MSG);	//'1'  (LF_LINK) Link to previously dumped file
						break;
			case 0x32:	m_LogFile->WriteMessage(m_LangMsg.BlockUnknown, MSG);	//'2'  (LF_SYMLINK) Symbolic link
						break;
			case 0x33:	m_LogFile->WriteMessage(m_LangMsg.BlockUnknown, MSG);	//'3'  (LF_CHR) Character special file
						break;
			case 0x34:	m_LogFile->WriteMessage(m_LangMsg.BlockUnknown, MSG);	//'4'  (LF_BLK) Block special file
						break;
			case 0x35:	m_LogFile->WriteMessage(m_LangMsg.BlockFolder, MSG);	//'5'  (LF_DIR) Directory
						break;
			case 0x36:	m_LogFile->WriteMessage(m_LangMsg.BlockUnknown, MSG);	//'6'  (LF_FIFO) FIFO special file
						break;
			case 0x37:	m_LogFile->WriteMessage(m_LangMsg.BlockUnknown, MSG);	//'7'  (LF_CONTIG) Contiguous file
						break;
			case 0x44:	m_LogFile->WriteMessage(m_LangMsg.BlockUnknown, MSG);	//'D'  (LF_DUMPDIR)
						break;
			case 0x4C:	m_LogFile->WriteMessage(m_LangMsg.BlockLongNameFile, MSG);	//'L'  (LF_LONGLINK) Length file name > 100 symbols
						break;
			case 0x4D:	m_LogFile->WriteMessage(m_LangMsg.BlockUnknown, MSG);	//'M'  (LF_MULTIVOL)
						break;
			case 0x56:	m_LogFile->WriteMessage(m_LangMsg.BlockUnknown, MSG);	//'V'  (LF_VOLHDR)
						break;
			default:	m_LogFile->WriteMessage(m_LangMsg.BlockUnknown, MSG);
						if (m_h) { ::CloseHandle(m_h); m_h = 0; }
						return TERROR_FORMAT;
						break;
			}
		}
		if (MSG != TMESSAGE_OK) { if (m_h) { ::CloseHandle(m_h); m_h = 0; } return MSG; }
	}
}
