/////////////////////////////////////////////////////////////////////////////
//TWcxPluginTC.cpp
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TWcxPluginTC.h"

/////////////////////////////////////////////////////////////////////////////
//Construction/Destruction
/////////////////////////////////////////////////////////////////////////////

TWcxPluginTC::
TWcxPluginTC(DataForArchive& dfa, LanguageMessages& langmsg, LanguageResults& langres, char* namearch): TArchive(dfa, langmsg, langres, namearch)
{
	if (m_pInternalName)
		{ delete[] m_pInternalName; m_pInternalName = 0; }
	char WcxPlugin[]   = "WCXPLUGIN";
	int  len           = strlen(namearch);
	if (len)
	{
		m_pInternalName = new char[len+1];
		strcpy(m_pInternalName, namearch);
	}
	else
	{
		m_pInternalName = new char[strlen(WcxPlugin)+1];
		strcpy(m_pInternalName, WcxPlugin);
	}
	//otmetim v logfile chto eto plugin (use plugin filepath (name))
	if (m_DetailLF >= 1)
	{
		char* msg       = 0;
		char  space[]   = " ";
		char  left[]    = "(";
		char  right[]   = ")";
		char  newline[] = "\n";

		unsigned len  =
			strlen(langmsg.UsePlugin) +
			strlen(space) +
			strlen(m_pUnArchiveDll) +
			strlen(space) +
			strlen(left) +
			strlen(m_pInternalName) +
			strlen(right) +
			strlen(newline);

		msg = new char[len+1];
		strcpy(msg, langmsg.UsePlugin);
		strcat(msg, space);
		strcat(msg, m_pUnArchiveDll);
		strcat(msg, space);
		strcat(msg, left);
		strcat(msg, m_pInternalName);
		strcat(msg, right);
		strcat(msg, newline);

		m_LogFile->WriteString(msg);
		if (msg) { delete[] msg; msg = 0; }
	}
}

/////////////////////////////////////////////////////////////////////////////
//FUNCTIONS PRIVATE
/////////////////////////////////////////////////////////////////////////////

HMODULE TWcxPluginTC::
LoadPlugin(LPCSTR lpLibFileName)
{
#ifdef _M_AMD64
	size_t l = strlen(lpLibFileName);
	if ((l > 2) && (strncmp(lpLibFileName + (l - 2), "64", 2) != 0))
	{
		char lpLibFileName64[MAX_PATH] = { 0 };
		snprintf(lpLibFileName64, sizeof(lpLibFileName64), "%s64", lpLibFileName);
		if (GetFileAttributes(lpLibFileName64) != INVALID_FILE_ATTRIBUTES)
		{
			return LoadLibrary(lpLibFileName64);
		}
	}
#endif
	return LoadLibrary(lpLibFileName);
}

//obrabotka zagolovka fayla/papki
//head - struktura zagolovka fayla/papki
//return
//	BlockType - tip bloka
//		0 - neizvestno
//		1 - fayl
//		2 - papka
//	MSG - resul'tat vipolneniya
int TWcxPluginTC::
AnalyzeFileHead(tHeaderData& head, int& BlockType)
{
	//est' li komentariy fayla?
	if (head.CmtSize) m_FileComment = TRUE;
	
	//v arhive eshe 1 fayl ili 1 papka
	if (head.FileAttr & 0x10)
	{
		BlockType = 2;
		m_NumberFolders++;
	}
	else
	{
		BlockType = 1;
		m_NumberFiles++;
	}

	double UnpVer        = 0;
	m_PackSizeFiles   += head.PackSize;
	m_UnpackSizeFiles += head.UnpSize;
	if (m_MethodPack < head.Method) m_MethodPack = head.Method;
	if (m_HostOS < head.HostOS)     m_HostOS     = head.HostOS;
	UnpVer            = (double)head.UnpVer / 10.0;
	if (m_UnpackVersion < UnpVer) m_UnpackVersion = UnpVer;

	return TMESSAGE_OK;
}

//obrabotka zagolovka fayla/papki
//head - struktura zagolovka fayla/papki
//return
//	BlockType - tip bloka
//		0 - neizvestno
//		1 - fayl
//		2 - papka
//	MSG - resul'tat vipolneniya
int TWcxPluginTC::
AnalyzeFileHeadEx(tHeaderDataEx& head, int& BlockType)
{
	//est' li komentariy fayla?
	if (head.CmtSize) m_FileComment = TRUE;

	//v arhive eshe 1 fayl ili 1 papka
	if (head.FileAttr & 0x10)
	{
		BlockType = 2;
		m_NumberFolders++;
	}
	else
	{
		BlockType = 1;
		m_NumberFiles++;
	}

	double UnpVer = 0;
	m_PackSizeFiles += TakeLong(head.PackSize, head.PackSizeHigh);
	m_UnpackSizeFiles += TakeLong(head.UnpSize, head.UnpSizeHigh);
	if (m_MethodPack < head.Method) m_MethodPack = head.Method;
	if (m_HostOS < head.HostOS)     m_HostOS = head.HostOS;
	UnpVer = (double)head.UnpVer / 10.0;
	if (m_UnpackVersion < UnpVer) m_UnpackVersion = UnpVer;

	return TMESSAGE_OK;
}

//opredelit' smeshenie v arhive
LONGLONG TWcxPluginTC::
GetArcPointer(HANDLE hand)
{
	LARGE_INTEGER ull;
	ull.LowPart  = 0;
	ull.HighPart = 0;

	ull.LowPart = ::SetFilePointer(hand, 0, &ull.HighPart, FILE_CURRENT);
	if (ull.LowPart == 0xFFFFFFFF && GetLastError() != NO_ERROR)
	{
		ull.QuadPart = -1;
	}

	return ull.QuadPart;
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
int TWcxPluginTC::
TestFile(char* path)
{
	//ispol'zuyutsya storonnie plagini na vsyakiy sluchay
	try
	{
		//obnulenie dannih
		InitialParametrs();
		
		//otkrit' fayl dlya chteniya
		m_h = CreateFile(path, GENERIC_READ, FILE_SHARE_READ, 
				NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if (m_h == INVALID_HANDLE_VALUE) 
			{ if (m_h) { ::CloseHandle(m_h); m_h = 0; } return TERROR_OPEN_FILE; }

		//mojet eto SFX arhiv (nado li proveryat')
		WORD EXECUTABLE_ID = 0;
		MSG = ReadArc(&EXECUTABLE_ID, sizeof(EXECUTABLE_ID));
		if (m_h) { ::CloseHandle(m_h); m_h = 0; }
		if (MSG != TMESSAGE_OK) return MSG;
		if (EXECUTABLE_ID == 'ZM')
		{
			m_SfxModule = 1;
			if (!m_CheckSFX) return TERROR_FORMAT;
		}

		//teper' dll
		HINSTANCE               DllModule               = 0;
		WCXOPENARCHIVE          OpenArchive             = 0;
		WCXREADHEADER           ReadHeader              = 0;
		WCXREADHEADEREX         ReadHeaderEx            = 0;
		WCXPROCESSFILE          ProcessFile             = 0;
		WCXCLOSEARCHIVE         CloseArchive            = 0;
		WCXCANYOUHANDLETHISFILE CanYouHandleThisFile    = 0;
		char                    pWcxPluginDll[MAX_PATH] = {0};

		//chtob put' bil real'nim bez vremennih okrujeniya
		if(!::ExpandEnvironmentStrings(m_pUnArchiveDll, pWcxPluginDll, MAX_PATH))
			strncpy_s(pWcxPluginDll, m_pUnArchiveDll, MAX_PATH);

		//zagrujaem DLL
		if (DllModule = LoadPlugin(pWcxPluginDll))
		{
			//nahodim nujnie funkcii
			OpenArchive          = (WCXOPENARCHIVE)         ::GetProcAddress(DllModule, "OpenArchive");
			ReadHeader           = (WCXREADHEADER)          ::GetProcAddress(DllModule, "ReadHeader");
			ReadHeaderEx         = (WCXREADHEADEREX)        ::GetProcAddress(DllModule, "ReadHeaderEx");
			ProcessFile          = (WCXPROCESSFILE)         ::GetProcAddress(DllModule, "ProcessFile");
			CloseArchive         = (WCXCLOSEARCHIVE)        ::GetProcAddress(DllModule, "CloseArchive");
			CanYouHandleThisFile = (WCXCANYOUHANDLETHISFILE)::GetProcAddress(DllModule, "CanYouHandleThisFile");

			//uznaem a hochet li voobshe plagin (DLL) popitatsya
			//rabotat' s nashim faylom, esli eto vozmojno
			if (CanYouHandleThisFile)
			{
				if (!CanYouHandleThisFile(path))
				{
					if (m_DetailLF == 2)
						m_LogFile->WriteMessage(m_LangMsg.SignatureArchive, TERROR_FORMAT);
					if (DllModule) { ::FreeLibrary(DllModule); DllModule = 0; }
					return TERROR_FORMAT;
				}
			}

			if ((OpenArchive) && (ReadHeader) && (ProcessFile) && (CloseArchive))
			{
				HANDLE hArcData = 0;

				m_OpenArchiveData.ArcName    = path;
				m_OpenArchiveData.OpenMode   = PK_OM_LIST;
				m_OpenArchiveData.OpenResult = 0;
				m_OpenArchiveData.CmtBuf     = 0;
				m_OpenArchiveData.CmtBufSize = 0;
				m_OpenArchiveData.CmtSize    = 0;
				m_OpenArchiveData.CmtState   = 0;

				hArcData = OpenArchive(&m_OpenArchiveData);
				if (!hArcData)
				{
					if (hArcData)  { CloseArchive(hArcData);   hArcData  = 0; }
					if (DllModule) { ::FreeLibrary(DllModule); DllModule = 0; }
					return TERROR_FORMAT;
				}
				//proverka resul'tata
				if (m_OpenArchiveData.OpenResult)
				{
					if (m_OpenArchiveData.OpenResult == E_UNKNOWN_FORMAT)
					{
						if (m_DetailLF == 2)
							m_LogFile->WriteMessage(m_LangMsg.SignatureArchive, TERROR_FORMAT);
						if (hArcData)  { CloseArchive(hArcData);   hArcData  = 0; }
						if (DllModule) { ::FreeLibrary(DllModule); DllModule = 0; }
						return TERROR_FORMAT;
					}
					else
					{
						if (m_DetailLF == 2)
							m_LogFile->WriteMessage(m_LangMsg.SignatureArchive, TERROR_DLL);
						if (hArcData)  { CloseArchive(hArcData);   hArcData  = 0; }
						if (DllModule) { ::FreeLibrary(DllModule); DllModule = 0; }
						return TERROR_DLL;
					}
				}
				else
				{
					//sdelaem probnoe chtenie t.k. nekotorie plagini
					//vsegda vozvrashayut pri proverke 0
					//(ne deleyut proverku formata dlya OpenArchive())
					//potom sdelaem propusk t.k. na vsyakiy sluchay
					int Result;
					//vibrat' fayl/papku
					if (ReadHeaderEx)
					{ 
						memset(&m_HeaderDataEx, 0, sizeof(tHeaderDataEx));
						m_HeaderDataEx.HostOS = -1;
						m_HeaderDataEx.Method = -1;
						Result = ReadHeaderEx(hArcData, &m_HeaderDataEx);
					}
					else
					{
						memset(&m_HeaderData, 0, sizeof(tHeaderData));
						m_HeaderData.HostOS = -1;	//kak v TArchive::InitialParametrs() class TArchive
						m_HeaderData.Method = -1;	//kak v TArchive::InitialParametrs() class TArchive
						Result = ReadHeader(hArcData, &m_HeaderData);
					}
					if (!Result)
					{
						//propustit' fayl/papku
						Result = ProcessFile(hArcData, PK_SKIP, "", "");
						if (!Result)
						{
							if (hArcData)  { CloseArchive(hArcData);   hArcData  = 0; }
							if (DllModule) { ::FreeLibrary(DllModule); DllModule = 0; }
							if (m_DetailLF == 2)
								m_LogFile->WriteMessage(m_LangMsg.SignatureArchive, TMESSAGE_OK);
							return TMESSAGE_OK;
						}
						else
						{
							if (hArcData)  { CloseArchive(hArcData);   hArcData  = 0; }
							if (DllModule) { ::FreeLibrary(DllModule); DllModule = 0; }
							if (m_DetailLF == 2)
								m_LogFile->WriteMessage(m_LangMsg.SignatureArchive, TERROR_FORMAT);
							return TERROR_FORMAT;
						}
					}
					//oshibki
					//esli srazu konec arhiva to eto toje oshibka
					//t.k. nekotorie plagini nalyuboy fayl tut vozvrashayut E_END_ARCHIVE
					else
					{
						if (hArcData)  { CloseArchive(hArcData);   hArcData  = 0; }
						if (DllModule) { ::FreeLibrary(DllModule); DllModule = 0; }
						if (m_DetailLF == 2)
							m_LogFile->WriteMessage(m_LangMsg.SignatureArchive, TERROR_FORMAT);
						return TERROR_FORMAT;
					}
				}
			}
			else
			{
				if (DllModule) { ::FreeLibrary(DllModule); DllModule = 0; }
				if (m_DetailLF == 2)
					m_LogFile->WriteMessage(m_LangMsg.SignatureArchive, TERROR_DLL);
				return TERROR_DLL;
			}
		}
		else return TERROR_NOT_DLL;
	}
	catch (...)
	{
		if (m_DetailLF == 2)
			m_LogFile->WriteMessage(m_LangMsg.SignatureArchive, TERROR_DLL);
		return TERROR_DLL;
	}
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
int TWcxPluginTC::
AnalyzeInfoOfArc(char* path)
{
	//ispol'zuyutsya storonnie plagini na vsyakiy sluchay
	try
	{
		//otkrit' fayl dlya chteniya
		m_h = CreateFile(path, GENERIC_READ, FILE_SHARE_READ, 
				NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if (m_h == INVALID_HANDLE_VALUE) 
			{ if (m_h) { ::CloseHandle(m_h); m_h = 0; } return TERROR_OPEN_FILE; }

		//razmer fayla arhiva
		m_ArchiveSize = GetArchSize();
		if (m_h) { ::CloseHandle(m_h); m_h = 0; }

		//teper' dll
		HINSTANCE        DllModule               = 0;
		WCXOPENARCHIVE   OpenArchive             = 0;
		WCXREADHEADER    ReadHeader              = 0;
		WCXREADHEADEREX  ReadHeaderEx            = 0;
		WCXPROCESSFILE   ProcessFile             = 0;
		WCXCLOSEARCHIVE  CloseArchive            = 0;
		char             pWcxPluginDll[MAX_PATH] = {0};

		//chtob put' bil real'nim bez vremennih okrujeniya
		if(!::ExpandEnvironmentStrings(m_pUnArchiveDll, pWcxPluginDll, MAX_PATH))
			strncpy(pWcxPluginDll, m_pUnArchiveDll, MAX_PATH);

		//zagrujaem DLL
		if (DllModule = LoadPlugin(pWcxPluginDll))
		{
			//nahodim nujnie funkcii
			OpenArchive  = (WCXOPENARCHIVE) ::GetProcAddress(DllModule, "OpenArchive");
			ReadHeader   = (WCXREADHEADER)  ::GetProcAddress(DllModule, "ReadHeader");
			ReadHeaderEx = (WCXREADHEADEREX)::GetProcAddress(DllModule, "ReadHeaderEx");
			ProcessFile  = (WCXPROCESSFILE) ::GetProcAddress(DllModule, "ProcessFile");
			CloseArchive = (WCXCLOSEARCHIVE)::GetProcAddress(DllModule, "CloseArchive");
			if ((OpenArchive) && (ReadHeader) && (ProcessFile) && (CloseArchive))
			{
				//proverka i predustanovka ukazatelya
				if (m_pTextComment) { delete[] m_pTextComment; m_pTextComment = 0; }
				m_pTextComment = new char[WCX_MAX_LEN_COMMENT+1];
				m_pTextComment[0]                   = '\0';
				m_pTextComment[WCX_MAX_LEN_COMMENT] = '\0';

				HANDLE hArcData = 0;

				m_OpenArchiveData.ArcName    = path;
				m_OpenArchiveData.OpenMode   = PK_OM_LIST;
				m_OpenArchiveData.OpenResult = 0;
				m_OpenArchiveData.CmtBuf     = m_pTextComment;
				m_OpenArchiveData.CmtBufSize = WCX_MAX_LEN_COMMENT;
				m_OpenArchiveData.CmtSize    = 0;
				m_OpenArchiveData.CmtState   = 0;

				//otkrit' arhiv
				hArcData = OpenArchive(&m_OpenArchiveData);
				if (!hArcData)
				{
					if (m_pTextComment) { delete[] m_pTextComment; m_pTextComment = 0; }
					if (hArcData)  { CloseArchive(hArcData);   hArcData  = 0; }
					if (DllModule) { ::FreeLibrary(DllModule); DllModule = 0; }
					return TERROR_FORMAT;
				}
				//proverka resul'tata
				if (m_OpenArchiveData.OpenResult)
				{
					if (m_pTextComment) { delete[] m_pTextComment; m_pTextComment = 0; }
					if (m_OpenArchiveData.OpenResult == E_UNKNOWN_FORMAT)
					{
						if (m_DetailLF == 2)
							m_LogFile->WriteMessage(m_LangMsg.SignatureArchive, TERROR_FORMAT);
						if (hArcData)  { CloseArchive(hArcData);   hArcData  = 0; }
						if (DllModule) { ::FreeLibrary(DllModule); DllModule = 0; }
						return TERROR_FORMAT;
					}
					else
					{
						if (m_DetailLF == 2)
							m_LogFile->WriteMessage(m_LangMsg.SignatureArchive, TERROR_DLL);
						if (hArcData)  { CloseArchive(hArcData);   hArcData  = 0; }
						if (DllModule) { ::FreeLibrary(DllModule); DllModule = 0; }
						return TERROR_DLL;
					}
				}
				else
				{
					//kommentariy arhiva
					if (m_OpenArchiveData.CmtSize) m_MainComment = TRUE;
					else if (m_pTextComment) { delete[] m_pTextComment; m_pTextComment = 0; }
					if (m_DetailLF == 2)
						m_LogFile->WriteMessage(m_LangMsg.SignatureArchive, TMESSAGE_OK);
				}

				//perebor faylov/papok
				int Result = 0;
				while (TRUE)
				{
					//esli rabochiy potok
					if (m_BeginThread)
					{
						//procent vipolneniya raboti
						LONGLONG percent = 0;
						if (m_ArchiveSize) percent = 100 * GetArcPointer(hArcData) / m_ArchiveSize;
						if (m_Percent != percent)	//chtob ne poslat' lishnie soobsheniya (oni tormozyat rabotu)
						{
							m_Percent = percent;
							::PostMessage(m_hWnd, WM_THREAD_PERCENT, 0, (LPARAM)m_Percent);
						}
						//net lis sobitiya zavershit' rabotu potoka
						if(::WaitForSingleObject(m_hEventEnd, 0) == WAIT_OBJECT_0)
						{
							if (hArcData)  { CloseArchive(hArcData);   hArcData  = 0; }
							if (DllModule) { ::FreeLibrary(DllModule); DllModule = 0; }
							return TTERMINATE_THREAD;
						}
					}

					//vibrat' fayl/papku
					if (ReadHeaderEx)
					{
						memset(&m_HeaderDataEx, 0, sizeof(tHeaderDataEx));
						m_HeaderDataEx.HostOS = -1;	//kak v TArchive::InitialParametrs() class TArchive
						m_HeaderDataEx.Method = -1;	//kak v TArchive::InitialParametrs() class TArchive
						Result = ReadHeaderEx(hArcData, &m_HeaderDataEx);
					}
					else
					{
						memset(&m_HeaderData, 0, sizeof(tHeaderData));
						m_HeaderData.HostOS = -1;	//kak v TArchive::InitialParametrs() class TArchive
						m_HeaderData.Method = -1;	//kak v TArchive::InitialParametrs() class TArchive
						Result = ReadHeader(hArcData, &m_HeaderData);
					}
					if(!Result)
					{
						m_BlockType = 0;
						if (ReadHeaderEx)
						{
							MSG = AnalyzeFileHeadEx(m_HeaderDataEx, m_BlockType);
						}
						else
						{
							MSG = AnalyzeFileHead(m_HeaderData, m_BlockType);
						}
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
						//propustit' fayl/papku
						Result = ProcessFile(hArcData, PK_SKIP, "", "");
						if (Result)
						{
							if (hArcData)  { CloseArchive(hArcData);   hArcData  = 0; }
							if (DllModule) { ::FreeLibrary(DllModule); DllModule = 0; }
							return TERROR_DLL;
						}
					}
					else
					{
						//konec arhiva (E_END_ARCHIVE) normal'no
						//oshibka CRC arhiva (E_BAD_ARCHIVE) propustim t.k. TC propuskaet
						if ((Result == E_END_ARCHIVE) || (Result == E_BAD_ARCHIVE))
						{
							CountRatioArchiveSize();
							CountRatioPackFileSize();
							if (hArcData)  { CloseArchive(hArcData);   hArcData  = 0; }
							if (DllModule) { ::FreeLibrary(DllModule); DllModule = 0; }
							return TMESSAGE_OK;
						}
						//oshibka
						else
						{
							if (hArcData)  { CloseArchive(hArcData);   hArcData  = 0; }
							if (DllModule) { ::FreeLibrary(DllModule); DllModule = 0; }
							return TERROR_DLL;
						}
					}
				}

				//zakrit' arhiv
				if (hArcData)  { CloseArchive(hArcData);   hArcData  = 0; }
				if (DllModule) { ::FreeLibrary(DllModule); DllModule = 0; }
				return MSG;
			}
			else
			{
				if (DllModule) { ::FreeLibrary(DllModule); DllModule = 0; }
				if (m_DetailLF == 2)
					m_LogFile->WriteMessage(m_LangMsg.SignatureArchive, TERROR_DLL);
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
