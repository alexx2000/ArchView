/////////////////////////////////////////////////////////////////////////////
//MyTestFile.cpp
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MyTestFile.h"

/////////////////////////////////////////////////////////////////////////////
//FUNCTIONS PRIVATE
/////////////////////////////////////////////////////////////////////////////

//testirovat' na ACE
int MyTestFile::TestAce()
{
	if (m_pSettings->sSettingsArchives.Ace)
	{
		m_pArchData = new TAce(m_dfa, m_pLanguage->sLanguageMessages,
									m_pLanguage->sLanguageResults,
									m_pLanguage->sLanguageArchives.Ace);

		if (m_pArchData->Start(&m_dft) != TMESSAGE_OK)
		{
			delete m_pArchData;
			m_pArchData = 0;
			m_dfa.NewLF = 0;
			return 0;
		}
		else
		{
			*m_pArchiveType = T_ACE;
			m_dfa.NewLF     = 0;
			return 1;
		}
	}
	return 0;
}

//testirovat' na ARC/PAK
int MyTestFile::TestArcPak()
{
	if (m_pSettings->sSettingsArchives.ArcPak)
	{
		m_pArchData = new TArcPak(m_dfa, m_pLanguage->sLanguageMessages,
									m_pLanguage->sLanguageResults,
									m_pLanguage->sLanguageArchives.ArcPak);

		if (m_pArchData->Start(&m_dft) != TMESSAGE_OK)
		{
			delete m_pArchData;
			m_pArchData = 0;
			m_dfa.NewLF = 0;
			return 0;
		}
		else
		{
			*m_pArchiveType = T_ARC_PAK;
			m_dfa.NewLF     = 0;
			return 1;
		}
	}
	return 0;
}

//testirovat' na ARJ
int MyTestFile::TestArj()
{
	if (m_pSettings->sSettingsArchives.Arj)
	{
		m_pArchData = new TArj(m_dfa, m_pLanguage->sLanguageMessages,
									m_pLanguage->sLanguageResults,
									m_pLanguage->sLanguageArchives.Arj);

		if (m_pArchData->Start(&m_dft) != TMESSAGE_OK)
		{
			delete m_pArchData;
			m_pArchData = 0;
			m_dfa.NewLF = 0;
			return 0;
		}
		else
		{
			*m_pArchiveType = T_ARJ;
			m_dfa.NewLF     = 0;
			return 1;
		}
	}
	return 0;
}

//testirovat' na BH
int MyTestFile::TestBh()
{
	if (m_pSettings->sSettingsArchives.Bh)
	{
		m_pArchData = new TBh(m_dfa, m_pLanguage->sLanguageMessages,
									m_pLanguage->sLanguageResults,
									m_pLanguage->sLanguageArchives.Bh);

		if (m_pArchData->Start(&m_dft) != TMESSAGE_OK)
		{
			delete m_pArchData;
			m_pArchData = 0;
			m_dfa.NewLF = 0;
			return 0;
		}
		else
		{
			*m_pArchiveType = T_BH;
			m_dfa.NewLF     = 0;
			return 1;
		}
	}
	return 0;
}

//testirovat' na BZIP2/TBZ2
int MyTestFile::TestBZip2Tbz2()
{
	if (m_pSettings->sSettingsArchives.BZip2Tbz2)
	{
		m_pArchData = new TBZip2Tbz2(m_dfa, m_pLanguage->sLanguageMessages,
									m_pLanguage->sLanguageResults,
									m_pLanguage->sLanguageArchives.BZip2Tbz2);

		if (m_pArchData->Start(&m_dft) != TMESSAGE_OK)
		{
			delete m_pArchData;
			m_pArchData = 0;
			m_dfa.NewLF = 0;
			return 0;
		}
		else
		{
			*m_pArchiveType = T_BZIP2_TBZ2;
			m_dfa.NewLF     = 0;
			return 1;
		}
	}
	return 0;
}

//testirovat' na CAB
int MyTestFile::TestCab()
{
	if (m_pSettings->sSettingsArchives.Cab)
	{
		m_pArchData = new TCab(m_dfa, m_pLanguage->sLanguageMessages,
									m_pLanguage->sLanguageResults,
									m_pLanguage->sLanguageArchives.Cab);

		if (m_pArchData->Start(&m_dft) != TMESSAGE_OK)
		{
			delete m_pArchData;
			m_pArchData = 0;
			m_dfa.NewLF = 0;
			return 0;
		}
		else
		{
			*m_pArchiveType = T_CAB;
			m_dfa.NewLF     = 0;
			return 1;
		}
	}
	return 0;
}

//testirovat' na CPIO
int MyTestFile::TestCpio()
{
	if (m_pSettings->sSettingsArchives.Cpio)
	{
		m_pArchData = new TCpio(m_dfa, m_pLanguage->sLanguageMessages,
									m_pLanguage->sLanguageResults,
									m_pLanguage->sLanguageArchives.Cpio);

		if (m_pArchData->Start(&m_dft) != TMESSAGE_OK)
		{
			delete m_pArchData;
			m_pArchData = 0;
			m_dfa.NewLF = 0;
			return 0;
		}
		else
		{
			*m_pArchiveType = T_CPIO;
			m_dfa.NewLF     = 0;
			return 1;
		}
	}
	return 0;
}

//testirovat' na DWC
int MyTestFile::TestDwc()
{
	if (m_pSettings->sSettingsArchives.Dwc)
	{
		m_pArchData = new TDwc(m_dfa, m_pLanguage->sLanguageMessages,
									m_pLanguage->sLanguageResults,
									m_pLanguage->sLanguageArchives.Dwc);

		if (m_pArchData->Start(&m_dft) != TMESSAGE_OK)
		{
			delete m_pArchData;
			m_pArchData = 0;
			m_dfa.NewLF = 0;
			return 0;
		}
		else
		{
			*m_pArchiveType = T_DWC;
			m_dfa.NewLF     = 0;
			return 1;
		}
	}
	return 0;
}

//testirovat' na GZIP/TGZ
int MyTestFile::TestGZipTgz()
{
	if (m_pSettings->sSettingsArchives.GZipTgz)
	{
		m_pArchData = new TGZipTgz(m_dfa, m_pLanguage->sLanguageMessages,
									m_pLanguage->sLanguageResults,
									m_pLanguage->sLanguageArchives.GZipTgz);

		if (m_pArchData->Start(&m_dft) != TMESSAGE_OK)
		{
			delete m_pArchData;
			m_pArchData = 0;
			m_dfa.NewLF = 0;
			return 0;
		}
		else
		{
			*m_pArchiveType = T_GZIP_TGZ;
			m_dfa.NewLF     = 0;
			return 1;
		}
	}
	return 0;
}

//testirovat' na HA
int MyTestFile::TestHa()
{
	if (m_pSettings->sSettingsArchives.Ha)
	{
		m_pArchData = new THa(m_dfa, m_pLanguage->sLanguageMessages,
									m_pLanguage->sLanguageResults,
									m_pLanguage->sLanguageArchives.Ha);

		if (m_pArchData->Start(&m_dft) != TMESSAGE_OK)
		{
			delete m_pArchData;
			m_pArchData = 0;
			m_dfa.NewLF = 0;
			return 0;
		}
		else
		{
			*m_pArchiveType = T_HA;
			m_dfa.NewLF     = 0;
			return 1;
		}
	}
	return 0;
}

//testirovat' na LHZ/LZH
int MyTestFile::TestLhaLzh()
{
	if (m_pSettings->sSettingsArchives.LhaLzh)
	{
		m_pArchData = new TLhaLzh(m_dfa, m_pLanguage->sLanguageMessages,
									m_pLanguage->sLanguageResults,
									m_pLanguage->sLanguageArchives.LhaLzh);

		if (m_pArchData->Start(&m_dft) != TMESSAGE_OK)
		{
			delete m_pArchData;
			m_pArchData = 0;
			m_dfa.NewLF = 0;
			return 0;
		}
		else
		{
			*m_pArchiveType = T_LHA_LZH;
			m_dfa.NewLF     = 0;
			return 1;
		}
	}
	return 0;
}

//testirovat' na PPM
int MyTestFile::TestPpm()
{
	if (m_pSettings->sSettingsArchives.Ppm)
	{
		m_pArchData = new TPpm(m_dfa, m_pLanguage->sLanguageMessages,
									m_pLanguage->sLanguageResults,
									m_pLanguage->sLanguageArchives.Ppm);

		if (m_pArchData->Start(&m_dft) != TMESSAGE_OK)
		{
			delete m_pArchData;
			m_pArchData = 0;
			m_dfa.NewLF = 0;
			return 0;
		}
		else
		{
			*m_pArchiveType = T_PPM;
			m_dfa.NewLF     = 0;
			return 1;
		}
	}
	return 0;
}

//testirovat' na RAR
int MyTestFile::TestRar()
{
	if (m_pSettings->sSettingsArchives.Rar)
	{
		m_pArchData = new TRar(m_dfa, m_pLanguage->sLanguageMessages,
									m_pLanguage->sLanguageResults,
									m_pLanguage->sLanguageArchives.Rar);

		if (m_pArchData->Start(&m_dft) != TMESSAGE_OK)
		{
			delete m_pArchData;
			m_pArchData = 0;
			m_dfa.NewLF = 0;
			return 0;
		}
		else
		{
			*m_pArchiveType = T_RAR;
			m_dfa.NewLF     = 0;
			return 1;
		}
	}
	return 0;
}

//testirovat' na SQZ
int MyTestFile::TestSqz()
{
	if (m_pSettings->sSettingsArchives.Sqz)
	{
		m_pArchData = new TSqz(m_dfa, m_pLanguage->sLanguageMessages,
									m_pLanguage->sLanguageResults,
									m_pLanguage->sLanguageArchives.Sqz);

		if (m_pArchData->Start(&m_dft) != TMESSAGE_OK)
		{
			delete m_pArchData;
			m_pArchData = 0;
			m_dfa.NewLF = 0;
			return 0;
		}
		else
		{
			*m_pArchiveType = T_SQZ;
			m_dfa.NewLF     = 0;
			return 1;
		}
	}
	return 0;
}

//testirovat' na TAR
int MyTestFile::TestTar()
{
	if (m_pSettings->sSettingsArchives.Tar)
	{
		m_pArchData = new TTar(m_dfa, m_pLanguage->sLanguageMessages,
									m_pLanguage->sLanguageResults,
									m_pLanguage->sLanguageArchives.Tar);

		if (m_pArchData->Start(&m_dft) != TMESSAGE_OK)
		{
			delete m_pArchData;
			m_pArchData = 0;
			m_dfa.NewLF = 0;
			return 0;
		}
		else
		{
			*m_pArchiveType = T_TAR;
			m_dfa.NewLF     = 0;
			return 1;
		}
	}
	return 0;
}

//testirovat' na Z
int MyTestFile::TestZ()
{
	if (m_pSettings->sSettingsArchives.Z)
	{
		m_pArchData = new TZ(m_dfa, m_pLanguage->sLanguageMessages,
									m_pLanguage->sLanguageResults,
									m_pLanguage->sLanguageArchives.Z);

		if (m_pArchData->Start(&m_dft) != TMESSAGE_OK)
		{
			delete m_pArchData;
			m_pArchData = 0;
			m_dfa.NewLF = 0;
			return 0;
		}
		else
		{
			*m_pArchiveType = T_Z;
			m_dfa.NewLF     = 0;
			return 1;
		}
	}
	return 0;
}

//testirovat' na ZIP/JAR
int MyTestFile::TestZipJar()
{
	if (m_pSettings->sSettingsArchives.ZipJar)
	{
		m_pArchData = new TZipJar(m_dfa, m_pLanguage->sLanguageMessages,
									m_pLanguage->sLanguageResults,
									m_pLanguage->sLanguageArchives.ZipJar);

		if (m_pArchData->Start(&m_dft) != TMESSAGE_OK)
		{
			delete m_pArchData;
			m_pArchData = 0;
			m_dfa.NewLF = 0;
			return 0;
		}
		else
		{
			*m_pArchiveType = T_ZIP_JAR;
			m_dfa.NewLF     = 0;
			return 1;
		}
	}
	return 0;
}

//testirovat' na ZOO
int MyTestFile::TestZoo()
{
	if (m_pSettings->sSettingsArchives.Zoo)
	{
		m_pArchData = new TZoo(m_dfa, m_pLanguage->sLanguageMessages,
									m_pLanguage->sLanguageResults,
									m_pLanguage->sLanguageArchives.Zoo);

		if (m_pArchData->Start(&m_dft) != TMESSAGE_OK)
		{
			delete m_pArchData;
			m_pArchData = 0;
			m_dfa.NewLF = 0;
			return 0;
		}
		else
		{
			*m_pArchiveType = T_ZOO;
			m_dfa.NewLF     = 0;
			return 1;
		}
	}
	return 0;
}

//testirovat' na 7ZIP
int MyTestFile::TestSevenZip()
{
	if (m_pSettings->sSettingsArchives.SevenZip)
	{
		m_pArchData = new TSevenZip(m_dfa, m_pLanguage->sLanguageMessages,
									m_pLanguage->sLanguageResults,
									m_pLanguage->sLanguageArchives.SevenZip);

		if (m_pArchData->Start(&m_dft) != TMESSAGE_OK)
		{
			delete m_pArchData;
			m_pArchData = 0;
			m_dfa.NewLF = 0;
			return 0;
		}
		else
		{
			*m_pArchiveType = T_SEVEN_ZIP;
			m_dfa.NewLF = 0;
			return 1;
		}
	}
	return 0;
}

//testirovat' na arhivatorniy (wcx) plugin Total Commander
int MyTestFile::TestWcxPluginTC(char* nameplugin)
{
	m_pArchData = new TWcxPluginTC(m_dfa, m_pLanguage->sLanguageMessages,
								m_pLanguage->sLanguageResults,
								nameplugin);

	if (m_pArchData->Start(&m_dft) != TMESSAGE_OK)
	{
		delete m_pArchData;
		m_pArchData = 0;
		m_dfa.NewLF = 0;
		return 0;
	}
	else
	{
		*m_pArchiveType = T_WCX_PLUGIN_TC;
		m_dfa.NewLF     = 0;
		return 1;
	}
}

/////////////////////////////////////////////////////////////////////////////
//FUNCTIONS PUBLIC
/////////////////////////////////////////////////////////////////////////////

//testirovanie fayla (izvestniy li nam arhiv)
//gp - structura parametrov dannogo ekzemplyara plugina
//return
//	1 - ok
//	0 - no arhive
TArchive* MyTestFile::TestArchive(MySettings* pSettings, MyLanguage* pLanguage, TypeOfArchive* pArchiveType, char* pFileToLoad, char* pLogFile)
{
	m_pSettings    = pSettings;
	m_pLanguage    = pLanguage;
	m_pArchiveType = pArchiveType;

	SettingsMain&          sSettingsMain          = m_pSettings->sSettingsMain;
	SettingsLogfile&       sSettingsLogfile       = m_pSettings->sSettingsLogfile;
	SettingsExtensions&    sSettingsExtensions    = m_pSettings->sSettingsExtensions;
	SettingsWCXPluginsTC&  sSettingsWCXPluginsTC  = m_pSettings->sSettingsWCXPluginsTC;
	SettingsWCXExtensions& sSettingsWCXExtensions = m_pSettings->sSettingsWCXExtensions;

	//dannie dlya sozdaniya ob'ekta ARHIV
	if (sSettingsLogfile.CreateLogFile) m_dfa.PathLF = pLogFile;
	else m_dfa.PathLF = "";
	m_dfa.SFX             = sSettingsMain.ArchivesSFX;			//proveryat' na SFX
	m_dfa.SizeSFX         = sSettingsMain.SizeSFX;				//maximal'naya dlina SFX
	m_dfa.InvertRatio     = sSettingsMain.InvertRatio;			//kak rasschitivat' sjatie (0 - puck/unpack; 1 - unpack/pack)
	m_dfa.pTargetFile     = pFileToLoad;						//fayl dlya kotorogo sozdaetsya logfile
	m_dfa.NewLF           = 1;									//noviy logfile
	m_dfa.DetailLF        = sSettingsLogfile.DetailLogFile+1;	//v fayle nastroek 0 i 1 (v TArchive 1 i 2)
	m_dfa.WidthAuthorName = sSettingsLogfile.WidthAuthorName;	//shirina avtora soobsheniya dlya logfile
	m_dfa.WidthMessage    = sSettingsLogfile.WidthMessage;		//shirina soobsheniya dlya logfile
	m_dfa.pUnArchiveDll   = 0;									//DLL dlya dannogo tipa arhiva (chitat' kommentariy)

	//dannie peredavaemie dlya testirovaniya
	m_dft.hWnd        = 0;				//neispol'zuem
	m_dft.hEventEnd   = 0;				//neispol'zuem
	m_dft.Path        = pFileToLoad;	//fayl dlya analiza
	m_dft.Flag        = TEST_ARC;		//testirovat'
	m_dft.BeginThread = FALSE;			//ne v rabochem potoke

	//videlim rasshirenie fayla
	char  EXT[MAX_LOADSTRING]       = {NULL};
	char  ARCHEXT[MAX_LOADSTRING+2] = {NULL};

	char* offset = strrchr(pFileToLoad, '.');
	if (offset)
	{
		sprintf(EXT, " %s ", offset+1);
		strlwr (EXT);
	}

	//opredlit' arhiv po rasshireniyu
	//k sojaleniyu ne znayu kak sdelet' cherez (switch - case)
	if (strlen(EXT))
	{
		//rasshireniya proveryaem v poryadke prioriteta arhiva
		for (int i=0; i<T_NUMBER_ARCHIVES-1; i++)
		{
			m_dfa.pUnArchiveDll = 0;	//DLL dlya dannogo tipa arhiva (chitat' kommentariy)
			switch((TypeOfArchive)m_pSettings->MasPriorityArchives[i])
			{
			case T_ACE:
				m_dfa.pUnArchiveDll = m_pSettings->sSettingsLibraries.Ace;	//DLL dlya dannogo tipa arhiva (chitat' kommentariy)
				sprintf(ARCHEXT, " %s ", sSettingsExtensions.ExtAce);
				strlwr (ARCHEXT);
				if (strstr(ARCHEXT, EXT))
					if (TestAce()) return m_pArchData;
					else goto _T_LABEL_END_EXT;
				break;
			case T_ARC_PAK:
				sprintf(ARCHEXT, " %s ", sSettingsExtensions.ExtArcPak);
				strlwr (ARCHEXT);
				if (strstr(ARCHEXT, EXT))
					if (TestArcPak()) return m_pArchData;
					else goto _T_LABEL_END_EXT;
				break;
			case T_ARJ:
				sprintf(ARCHEXT, " %s ", sSettingsExtensions.ExtArj);
				strlwr (ARCHEXT);
				if (strstr(ARCHEXT, EXT))
					if (TestArj()) return m_pArchData;
					else goto _T_LABEL_END_EXT;
				break;
			case T_BH:
				sprintf(ARCHEXT, " %s ", sSettingsExtensions.ExtBh);
				strlwr (ARCHEXT);
				if (strstr(ARCHEXT, EXT))
					if (TestBh()) return m_pArchData;
					else goto _T_LABEL_END_EXT;
				break;
			case T_BZIP2_TBZ2:
				sprintf(ARCHEXT, " %s ", sSettingsExtensions.ExtBZip2Tbz2);
				strlwr (ARCHEXT);
				if (strstr(ARCHEXT, EXT))
					if (TestBZip2Tbz2()) return m_pArchData;
					else goto _T_LABEL_END_EXT;
				break;
			case T_CAB:
				sprintf(ARCHEXT, " %s ", sSettingsExtensions.ExtCab);
				strlwr (ARCHEXT);
				if (strstr(ARCHEXT, EXT))
					if (TestCab()) return m_pArchData;
					else goto _T_LABEL_END_EXT;
				break;
			case T_CPIO:
				sprintf(ARCHEXT, " %s ", sSettingsExtensions.ExtCpio);
				strlwr (ARCHEXT);
				if (strstr(ARCHEXT, EXT))
					if (TestCpio()) return m_pArchData;
					else goto _T_LABEL_END_EXT;
				break;
			case T_DWC:
				sprintf(ARCHEXT, " %s ", sSettingsExtensions.ExtDwc);
				strlwr (ARCHEXT);
				if (strstr(ARCHEXT, EXT))
					if (TestDwc()) return m_pArchData;
					else goto _T_LABEL_END_EXT;
				break;
			case T_GZIP_TGZ:
				sprintf(ARCHEXT, " %s ", sSettingsExtensions.ExtGZipTgz);
				strlwr (ARCHEXT);
				if (strstr(ARCHEXT, EXT))
					if (TestGZipTgz()) return m_pArchData;
					else goto _T_LABEL_END_EXT;
				break;
			case T_HA:
				sprintf(ARCHEXT, " %s ", sSettingsExtensions.ExtHa);
				strlwr (ARCHEXT);
				if (strstr(ARCHEXT, EXT))
					if (TestHa()) return m_pArchData;
					else goto _T_LABEL_END_EXT;
				break;
			case T_LHA_LZH:
				sprintf(ARCHEXT, " %s ", sSettingsExtensions.ExtLhaLzh);
				strlwr (ARCHEXT);
				if (strstr(ARCHEXT, EXT))
					if (TestLhaLzh()) return m_pArchData;
					else goto _T_LABEL_END_EXT;
				break;
			case T_PPM:
				sprintf(ARCHEXT, " %s ", sSettingsExtensions.ExtPpm);
				strlwr (ARCHEXT);
				if (strstr(ARCHEXT, EXT))
					if (TestPpm()) return m_pArchData;
					else goto _T_LABEL_END_EXT;
				break;
			case T_RAR:
				m_dfa.pUnArchiveDll = m_pSettings->sSettingsLibraries.Rar;	//DLL dlya dannogo tipa arhiva (chitat' kommentariy)
				sprintf(ARCHEXT, " %s ", sSettingsExtensions.ExtRar);
				strlwr (ARCHEXT);
				if (strstr(ARCHEXT, EXT))
					if (TestRar()) return m_pArchData;
					else goto _T_LABEL_END_EXT;
				break;
			case T_SQZ:
				sprintf(ARCHEXT, " %s ", sSettingsExtensions.ExtSqz);
				strlwr (ARCHEXT);
				if (strstr(ARCHEXT, EXT))
					if (TestSqz()) return m_pArchData;
					else goto _T_LABEL_END_EXT;
				break;
			case T_TAR:
				sprintf(ARCHEXT, " %s ", sSettingsExtensions.ExtTar);
				strlwr (ARCHEXT);
				if (strstr(ARCHEXT, EXT))
					if (TestTar()) return m_pArchData;
					else goto _T_LABEL_END_EXT;
				break;
			case T_Z:
				sprintf(ARCHEXT, " %s ", sSettingsExtensions.ExtZ);
				strlwr (ARCHEXT);
				if (strstr(ARCHEXT, EXT))
					if (TestZ()) return m_pArchData;
					else goto _T_LABEL_END_EXT;
				break;
			case T_ZIP_JAR:
				sprintf(ARCHEXT, " %s ", sSettingsExtensions.ExtZipJar);
				strlwr (ARCHEXT);
				if (strstr(ARCHEXT, EXT))
					if (TestZipJar()) return m_pArchData;
					else goto _T_LABEL_END_EXT;
				break;
			case T_ZOO:
				sprintf(ARCHEXT, " %s ", sSettingsExtensions.ExtZoo);
				strlwr (ARCHEXT);
				if (strstr(ARCHEXT, EXT))
					if (TestZoo()) return m_pArchData;
					else goto _T_LABEL_END_EXT;
				break;
			case T_SEVEN_ZIP:
				sprintf(ARCHEXT, " %s ", sSettingsExtensions.ExtSevenZip);
				strlwr(ARCHEXT);
				if (strstr(ARCHEXT, EXT))
					if (TestSevenZip()) return m_pArchData;
					else goto _T_LABEL_END_EXT;
				break;
			default:
				m_dfa.pUnArchiveDll = 0;	//DLL dlya dannogo tipa arhiva (chitat' kommentariy)
				break;
			}
		}

		//rasshireniya proveryaem v poryadke obyavleniya wcx-plugins (esli wcx-plugins ispol'zuyutsya)
		if (sSettingsMain.UseWCXPluginsTC)
		{
			NodeStrStr* NodeWCXPluginsTC  = sSettingsWCXPluginsTC.WCXPluginsTC.GetHeadListStrStr();
			NodeStrStr* NodeWCXExtensions = 0;
			//perebor plaginov
			while(NodeWCXPluginsTC)
			{
				//poisk zaregestrirovannih rasshireniya
				NodeWCXExtensions = sSettingsWCXExtensions.WCXExtensions.SearchNodeFromName(NodeWCXPluginsTC->Name);
				if (NodeWCXExtensions)
				{
					m_dfa.pUnArchiveDll = NodeWCXPluginsTC->Value;	//DLL dlya dannogo plagina
					sprintf(ARCHEXT, " %s ", NodeWCXExtensions->Value);
					strlwr (ARCHEXT);
					if (strstr(ARCHEXT, EXT))
						if (TestWcxPluginTC(NodeWCXPluginsTC->Name)) return m_pArchData;
						else m_dfa.pUnArchiveDll = 0;	//DLL dlya dannogo plagina
				}
				NodeWCXPluginsTC = NodeWCXPluginsTC->Next;
			}
		}
	}
	//sobstvenno tak ya emulitoval konstrukciyu (switch - case)
	_T_LABEL_END_EXT: m_dfa.pUnArchiveDll = 0;	//DLL dlya dannogo tipa arhiva (chitat' kommentariy)

	//prohod po vsem vozmojnim arhivam po ih prioritetu
	//esli ne otklyuchena takaya proverka
	if (!sSettingsExtensions.OnlyExtensions)
	{
		for (int i=0; i<T_NUMBER_ARCHIVES-1; i++)
		{
			m_dfa.pUnArchiveDll = 0;	//DLL dlya dannogo tipa arhiva (chitat' kommentariy)
			switch((TypeOfArchive)m_pSettings->MasPriorityArchives[i])
			{
			case T_ACE:
				m_dfa.pUnArchiveDll = m_pSettings->sSettingsLibraries.Ace;	//DLL dlya dannogo tipa arhiva (chitat' kommentariy)
				if (TestAce      ()) return m_pArchData;
				else break;
			case T_ARC_PAK:
				if (TestArcPak   ()) return m_pArchData;
				else break;
			case T_ARJ:
				if (TestArj      ()) return m_pArchData;
				else break;
			case T_BH:
				if (TestBh       ()) return m_pArchData;
				else break;
			case T_BZIP2_TBZ2:
				if (TestBZip2Tbz2()) return m_pArchData;
				else break;
			case T_CAB:
				if (TestCab      ()) return m_pArchData;
				else break;
			case T_CPIO:
				if (TestCpio     ()) return m_pArchData;
				else break;
			case T_DWC:
				if (TestDwc      ()) return m_pArchData;
				else break;
			case T_GZIP_TGZ:
				if (TestGZipTgz  ()) return m_pArchData;
				else break;
			case T_HA:
				if (TestHa       ()) return m_pArchData;
				else break;
			case T_LHA_LZH:
				if (TestLhaLzh   ()) return m_pArchData;
				else break;
			case T_PPM:
				if (TestPpm      ()) return m_pArchData;
				else break;
			case T_RAR:
				m_dfa.pUnArchiveDll = m_pSettings->sSettingsLibraries.Rar;	//DLL dlya dannogo tipa arhiva (chitat' kommentariy)
				if (TestRar      ()) return m_pArchData;
				else break;
			case T_SQZ:
				if (TestSqz      ()) return m_pArchData;
				else break;
			case T_TAR:
				if (TestTar      ()) return m_pArchData;
				else break;
			case T_Z:
				if (TestZ        ()) return m_pArchData;
				else break;
			case T_ZIP_JAR:
				if (TestZipJar   ()) return m_pArchData;
				else break;
			case T_ZOO:
				if (TestZoo      ()) return m_pArchData;
				else break;
			case T_SEVEN_ZIP:
				if (TestSevenZip ()) return m_pArchData;
				else break;
			default:
				m_dfa.pUnArchiveDll = 0;	//DLL dlya dannogo tipa arhiva (chitat' kommentariy)
				break;
			}
		}
	}

	//prohod po vsem vozmojnim pluginam v porydke ih obyavleniya
	//esli ne otklyuchena takaya proverka i (esli wcx-plugins ispol'zuyutsya)
	if (sSettingsMain.UseWCXPluginsTC)
	{
		if (!sSettingsWCXExtensions.OnlyExtensions)
		{
			NodeStrStr* NodeWCXPluginsTC  = sSettingsWCXPluginsTC.WCXPluginsTC.GetHeadListStrStr();
			//perebor plaginov
			while(NodeWCXPluginsTC)
			{
				m_dfa.pUnArchiveDll = NodeWCXPluginsTC->Value;	//DLL dlya dannogo plagina
				if (TestWcxPluginTC(NodeWCXPluginsTC->Name)) return m_pArchData;
				else m_dfa.pUnArchiveDll = 0;	//DLL dlya dannogo plagina
				NodeWCXPluginsTC = NodeWCXPluginsTC->Next;
			}
		}
	}

	return m_pArchData;
}
