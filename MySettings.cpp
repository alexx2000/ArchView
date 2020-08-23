/////////////////////////////////////////////////////////////////////////////
//MySettings.cpp
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MySettings.h"
#include <stdio.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

MySettings::MySettings()
{
	//prioritet arhivov
	for(int i=0; i<T_NUMBER_ARCHIVES-1; i++)
		MasPriorityArchives[i] = T_UNKNOWN;
	//sekcii fayla nastroek ArchView.ini
	strcpy(SectionsINI.SectMain,          "Main"         );
	strcpy(SectionsINI.SectWindow,        "Window"       );
	strcpy(SectionsINI.SectParameters,    "Parameters"   );
	strcpy(SectionsINI.SectLogfile ,      "Logfile"      );
	strcpy(SectionsINI.SectArchives,      "Archives"     );
	strcpy(SectionsINI.SectExtensions,    "Extensions"   );
	strcpy(SectionsINI.SectLibraries,     "Libraries"    );
	strcpy(SectionsINI.SectWCXPluginsTC,  "WCXPluginsTC" );	//ArchView.ini (sekciya wcx-plugins)
	strcpy(SectionsINI.SectPackerPlugins, "PackerPlugins");	//wincmd.ini (sekciya wcx-plugins)
	strcpy(SectionsINI.SectWCXExtensions, "WCXExtensions");
}

/////////////////////////////////////////////////////////////////////////////
//FUNCTIONS PRIVATE
/////////////////////////////////////////////////////////////////////////////

int MySettings::CheckPriorityArchives()
{
	//videlim iz stroki vse prioriteti v nash massiv prioritetov
	//prioritet idet po nomeru kajdomu arhivu sootvetstvuet svoy nomer
	//nomera prisvavivayutsya nachinaya s 1 v alfavitnom poryadke arhivov
	//ACE = 1 | ARJ = 2 | CAB = 3
	//"2 1 3" t.e. snachala ARJ, potom ACE a potom CAB
	int number = sscanf(sSettingsArchives.PriorityArchives,
		//1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18
		"%d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d",
		&MasPriorityArchives[0],
		&MasPriorityArchives[1],
		&MasPriorityArchives[2],
		&MasPriorityArchives[3],
		&MasPriorityArchives[4],
		&MasPriorityArchives[5],
		&MasPriorityArchives[6],
		&MasPriorityArchives[7],
		&MasPriorityArchives[8],
		&MasPriorityArchives[9],
		&MasPriorityArchives[10],
		&MasPriorityArchives[11],
		&MasPriorityArchives[12],
		&MasPriorityArchives[13],
		&MasPriorityArchives[14],
		&MasPriorityArchives[15],
		&MasPriorityArchives[16],
		&MasPriorityArchives[17]);

	//nadoli ispravit' massiv prioritetov
	BOOL CorrectMasPriorityArchives = FALSE;

	//prioritetov doljno bit' stol'ko je skol'ko izvestno arhivov
	if (number < T_NUMBER_ARCHIVES-1)
		CorrectMasPriorityArchives = TRUE;

	int i   = 0;
	int j   = 0;

	//uberem vse neizvestnie prioriteti
	for (i=0; i<T_NUMBER_ARCHIVES-1; i++)
	{
		if (MasPriorityArchives[i] > T_NUMBER_ARCHIVES-1)
		{
			MasPriorityArchives[i] = 0;
			CorrectMasPriorityArchives = TRUE;
		}
		if (MasPriorityArchives[i] == 0)
			CorrectMasPriorityArchives = TRUE;
	}

	//uberem vse povtoryayushiesya
	for (i=0; i<T_NUMBER_ARCHIVES-1; i++)
	{
		for (j=i+1; j<T_NUMBER_ARCHIVES-1; j++)
		{
			if (MasPriorityArchives[i] == MasPriorityArchives[j])
			{
				MasPriorityArchives[j] = 0;
				CorrectMasPriorityArchives = TRUE;
			}
		}
	}

	//esli nado pravit'
	if (CorrectMasPriorityArchives)
	{
		//vmesto 0-ih prioritetov prostavim te chto eshe ne stoyat
		for (i=1; i<T_NUMBER_ARCHIVES; i++)
		{
			//proveryaem est' li takoy prioritet
			BOOL YesThisPrioritet = FALSE;
			for (j=0; j<T_NUMBER_ARCHIVES-1; j++)
			{
				if (MasPriorityArchives[j] == i)
					YesThisPrioritet = TRUE;
			}
			//esli net to vstavlyaem ego
			if (!YesThisPrioritet)
			{
				for (j=0; j<T_NUMBER_ARCHIVES-1; j++)
				{
					if (MasPriorityArchives[j] == 0)
					{
						MasPriorityArchives[j] = i;
						break;
					}
				}
			}
		}

		//izmenennuyu stroku prioritetov zapishem v fayl nastroek
		char BufStr[MAX_LOADSTRING] = {NULL};
		sprintf(sSettingsArchives.PriorityArchives,
			"%d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d",
			MasPriorityArchives[0],
			MasPriorityArchives[1],
			MasPriorityArchives[2],
			MasPriorityArchives[3],
			MasPriorityArchives[4],
			MasPriorityArchives[5],
			MasPriorityArchives[6],
			MasPriorityArchives[7],
			MasPriorityArchives[8],
			MasPriorityArchives[9],
			MasPriorityArchives[10],
			MasPriorityArchives[11],
			MasPriorityArchives[12],
			MasPriorityArchives[13],
			MasPriorityArchives[14],
			MasPriorityArchives[15],
			MasPriorityArchives[16],
			MasPriorityArchives[17]);
		fINI.WriteKey(SectionsINI.SectArchives, "PriorityArchives", sSettingsArchives.PriorityArchives);
	}

	return 1;
}

/////////////////////////////////////////////////////////////////////////////
//FUNCTIONS PUBLIC
/////////////////////////////////////////////////////////////////////////////

//chtenie nastoek programmi (file ArchView.ini)
//inifile - imya fayla nastroek
int MySettings::
ReadSettingsProgram(char* inifile)
{
	unsigned MaxSfxModuleLen    = 10485760;	//10 Mb (10'485'760 b)
	unsigned MaxWidthAuthorName = 20;		//20 simbols
	unsigned MaxWidthMessage    = 50;		//50 simbols
	fINI.SetFile(inifile);

	SettingsSections& SN = SectionsINI;

	//[Main]
	//chtenie klyuchey
	sSettingsMain.ArchivesSFX     = fINI.ReadKey(SN.SectMain, "ArchivesSFX",     1);
	sSettingsMain.SizeSFX         = fINI.ReadKey(SN.SectMain, "SizeSFX",         512000);  //500 Kb (512'000 b)
	sSettingsMain.FloatSize       = fINI.ReadKey(SN.SectMain, "FloatSize",       0);
	sSettingsMain.AllParametrs    = fINI.ReadKey(SN.SectMain, "AllParametrs",    0);
	sSettingsMain.InvertRatio     = fINI.ReadKey(SN.SectMain, "InvertRatio",     0);
	sSettingsMain.SetDetectString = fINI.ReadKey(SN.SectMain, "SetDetectString", 0);
	fINI.ReadKey(SN.SectMain, "Language", "", sSettingsMain.Language, MAX_LOADSTRING);
	sSettingsMain.UseWCXPluginsTC = fINI.ReadKey(SN.SectMain, "UseWCXPluginsTC", 1);
	sSettingsMain.OptimUsePlugins = fINI.ReadKey(SN.SectMain, "OptimUsePlugins", 1);
	sSettingsMain.UseWincmdIniWCX = fINI.ReadKey(SN.SectMain, "UseWincmdIniWCX", 0);
	fINI.ReadKey(SN.SectMain, "PathWincmdIni", "", sSettingsMain.PathWincmdIni, MAX_LOADSTRING);

	//proverka/ispravlenie dannih
	if ((sSettingsMain.ArchivesSFX)            &&
		(sSettingsMain.ArchivesSFX       != 1))
		fINI.WriteKey(SN.SectMain, "ArchivesSFX",     1);
	if ((sSettingsMain.FloatSize)              &&
		(sSettingsMain.FloatSize         != 1))
		fINI.WriteKey(SN.SectMain, "FloatSize",       1);
	if ((sSettingsMain.AllParametrs)           &&
		(sSettingsMain.AllParametrs      != 1))
		fINI.WriteKey(SN.SectMain, "AllParametrs",    1);
	if ((sSettingsMain.InvertRatio)            &&
		(sSettingsMain.InvertRatio       != 1))
		fINI.WriteKey(SN.SectMain, "InvertRatio",     1);
	if ((sSettingsMain.SetDetectString)        &&
		(sSettingsMain.SetDetectString   != 1))
		fINI.WriteKey(SN.SectMain, "SetDetectString", 1);
	if ((sSettingsMain.UseWCXPluginsTC)        &&
		(sSettingsMain.UseWCXPluginsTC   != 1))
		fINI.WriteKey(SN.SectMain, "UseWCXPluginsTC", 1);
	if ((sSettingsMain.OptimUsePlugins)        &&
		(sSettingsMain.OptimUsePlugins   != 1))
		fINI.WriteKey(SN.SectMain, "OptimUsePlugins", 1);
	if ((sSettingsMain.UseWincmdIniWCX)        &&
		(sSettingsMain.UseWincmdIniWCX   != 1))
		fINI.WriteKey(SN.SectMain, "UseWincmdIniWCX", 1);
	if (sSettingsMain.SizeSFX > MaxSfxModuleLen)	//SFX ne bolee 10 Mb
	{
		sSettingsMain.SizeSFX = MaxSfxModuleLen;
		fINI.WriteKey(SN.SectMain, "SizeSFX", MaxSfxModuleLen);
	}

	//[Window]
	//chtenie klyuchey
	sSettingsWindow.SaveLastTab          = fINI.ReadKey(SN.SectWindow, "SaveLastTab",          1);
	sSettingsWindow.LastTabNumber        = fINI.ReadKey(SN.SectWindow, "LastTabNumber",        0);
	sSettingsWindow.SaveWindowSize       = fINI.ReadKey(SN.SectWindow, "SaveWindowSize",       1);
	sSettingsWindow.WidthWindow          = fINI.ReadKey(SN.SectWindow, "WidthWindow",          0);
	sSettingsWindow.HeightWindow         = fINI.ReadKey(SN.SectWindow, "HeightWindow",         0);
	sSettingsWindow.ShowCommentOnTabInfo = fINI.ReadKey(SN.SectWindow, "ShowCommentOnTabInfo", 0);
	sSettingsWindow.ShowTabContext       = fINI.ReadKey(SN.SectWindow, "ShowTabContext",       0);
	sSettingsWindow.ShowTabComment       = fINI.ReadKey(SN.SectWindow, "ShowTabComment",       1);
	sSettingsWindow.ShowTabAbout         = fINI.ReadKey(SN.SectWindow, "ShowTabAbout",         1);
	sSettingsWindow.ShowProgress         = fINI.ReadKey(SN.SectWindow, "ShowProgress",         1);
	sSettingsWindow.ShowLeftRatio        = fINI.ReadKey(SN.SectWindow, "ShowLeftRatio",        1);
	sSettingsWindow.ShowRightRatio       = fINI.ReadKey(SN.SectWindow, "ShowRightRatio",       1);
	sSettingsWindow.FontSize             = fINI.ReadKey(SN.SectWindow, "FontSize",            12);

	//proverka/ispravlenie dannih
	if ((sSettingsWindow.SaveLastTab)               &&
		(sSettingsWindow.SaveLastTab          != 1))
		fINI.WriteKey(SN.SectWindow, "SaveLastTab",          1);
	if ((sSettingsWindow.SaveWindowSize)            &&
		(sSettingsWindow.SaveWindowSize       != 1))
		fINI.WriteKey(SN.SectWindow, "SaveWindowSize",       1);
	if ((sSettingsWindow.ShowCommentOnTabInfo)      &&
		(sSettingsWindow.ShowCommentOnTabInfo != 1))
		fINI.WriteKey(SN.SectWindow, "ShowCommentOnTabInfo", 1);
	if ((sSettingsWindow.ShowTabContext)            &&
		(sSettingsWindow.ShowTabContext       != 1))
		fINI.WriteKey(SN.SectWindow, "ShowTabContext",       1);
	if ((sSettingsWindow.ShowTabComment)            &&
		(sSettingsWindow.ShowTabComment       != 1))
		fINI.WriteKey(SN.SectWindow, "ShowTabComment",       1);
	if ((sSettingsWindow.ShowTabAbout)              &&
		(sSettingsWindow.ShowTabAbout         != 1))
		fINI.WriteKey(SN.SectWindow, "ShowTabAbout",         1);
	if ((sSettingsWindow.ShowProgress)              &&
		(sSettingsWindow.ShowProgress         != 1))
		fINI.WriteKey(SN.SectWindow, "ShowProgress",         1);
	if ((sSettingsWindow.ShowLeftRatio)             &&
		(sSettingsWindow.ShowLeftRatio        != 1))
		fINI.WriteKey(SN.SectWindow, "ShowLeftRatio",        1);
	if ((sSettingsWindow.ShowRightRatio)            &&
		(sSettingsWindow.ShowRightRatio       != 1))
		fINI.WriteKey(SN.SectWindow, "ShowRightRatio",       1);
	if ((sSettingsWindow.FontSize             < 6)  ||
		(sSettingsWindow.FontSize             > 34))
	{
		sSettingsWindow.FontSize = 12;
		fINI.WriteKey(SN.SectWindow, "FontSize", 12);
	}
	if ((sSettingsWindow.LastTabNumber        < 0)  ||
		(sSettingsWindow.LastTabNumber        >  4))
	{
		sSettingsWindow.LastTabNumber = 0;
		fINI.WriteKey(SN.SectWindow, "LastTabNumber", 0);
	}
	if ((!sSettingsWindow.SaveLastTab) &&	//esli ne zapominat' poslednyuyu zakladku
		(sSettingsWindow.LastTabNumber))
	{
		sSettingsWindow.LastTabNumber = 0;
		fINI.WriteKey(SN.SectWindow, "LastTabNumber", 0);
	}
	HWND hWndDesktop = ::GetDesktopWindow();
	RECT rcClient;
	::GetWindowRect(hWndDesktop, &rcClient);
	unsigned MaxWidth  = rcClient.right  - rcClient.left;
	unsigned MaxHeight = rcClient.bottom - rcClient.top;
	if (sSettingsWindow.WidthWindow > MaxWidth)
	{
		sSettingsWindow.WidthWindow = MaxWidth;
		fINI.WriteKey(SN.SectWindow, "WidthWindow", MaxWidth);
	}
	if (sSettingsWindow.HeightWindow > MaxHeight)
	{
		sSettingsWindow.HeightWindow = MaxHeight;
		fINI.WriteKey(SN.SectWindow, "HeightWindow", MaxHeight);
	}

	//[Parameters]
	//chtenie klyuchey
	sSettingsParameters.UnpackSizeFiles   = fINI.ReadKey(SN.SectParameters, "UnpackSizeFiles",   1);
	sSettingsParameters.PackSizeFiles     = fINI.ReadKey(SN.SectParameters, "PackSizeFiles",     1);
	sSettingsParameters.ArchiveSize       = fINI.ReadKey(SN.SectParameters, "ArchiveSize",       1);
	sSettingsParameters.NumberFiles       = fINI.ReadKey(SN.SectParameters, "NumberFiles",       1);
	sSettingsParameters.NumberFolders     = fINI.ReadKey(SN.SectParameters, "NumberFolders",     1);
	sSettingsParameters.NumberChapters    = fINI.ReadKey(SN.SectParameters, "NumberChapters",    1);
	sSettingsParameters.DictionarySize    = fINI.ReadKey(SN.SectParameters, "DictionarySize",    1);
	sSettingsParameters.HostOS            = fINI.ReadKey(SN.SectParameters, "HostOS",            1);
	sSettingsParameters.ExtractOS         = fINI.ReadKey(SN.SectParameters, "ExtractOS",         1);
	sSettingsParameters.MethodPack        = fINI.ReadKey(SN.SectParameters, "MethodPack",        1);
	sSettingsParameters.MemorySize        = fINI.ReadKey(SN.SectParameters, "MemorySize",        1);
	sSettingsParameters.RatioArchiveSize  = fINI.ReadKey(SN.SectParameters, "RatioArchiveSize",  1);
	sSettingsParameters.RatioPackFileSize = fINI.ReadKey(SN.SectParameters, "RatioPackFileSize", 1);
	sSettingsParameters.PackVersion       = fINI.ReadKey(SN.SectParameters, "PackVersion",       1);
	sSettingsParameters.UnpackVersion     = fINI.ReadKey(SN.SectParameters, "UnpackVersion",     1);
	sSettingsParameters.SfxModule         = fINI.ReadKey(SN.SectParameters, "SfxModule",         1);
	sSettingsParameters.RecoveryRecord    = fINI.ReadKey(SN.SectParameters, "RecoveryRecord",    1);
	sSettingsParameters.Solid             = fINI.ReadKey(SN.SectParameters, "Solid",             1);
	sSettingsParameters.MultiArc          = fINI.ReadKey(SN.SectParameters, "MultiArc",          1);
	sSettingsParameters.MainComment       = fINI.ReadKey(SN.SectParameters, "MainComment",       1);
	sSettingsParameters.FileComment       = fINI.ReadKey(SN.SectParameters, "FileComment",       1);
	sSettingsParameters.ChapterComment    = fINI.ReadKey(SN.SectParameters, "ChapterComment",    1);
	sSettingsParameters.Password          = fINI.ReadKey(SN.SectParameters, "Password",          1);
	sSettingsParameters.ArchiveLock       = fINI.ReadKey(SN.SectParameters, "ArchiveLock",       1);
	sSettingsParameters.AuthVerification  = fINI.ReadKey(SN.SectParameters, "AuthVerification",  1);
	sSettingsParameters.TestTime          = fINI.ReadKey(SN.SectParameters, "TestTime",          1);
	sSettingsParameters.AnalyzeTime       = fINI.ReadKey(SN.SectParameters, "AnalyzeTime",       1);
	sSettingsParameters.AllTime           = fINI.ReadKey(SN.SectParameters, "AllTime",           1);
	sSettingsParameters.DateCreated       = fINI.ReadKey(SN.SectParameters, "DateCreated",       1);
	sSettingsParameters.DateModified      = fINI.ReadKey(SN.SectParameters, "DateModified",      1);
	sSettingsParameters.TimeCreated       = fINI.ReadKey(SN.SectParameters, "TimeCreated",       1);
	sSettingsParameters.TimeModified      = fINI.ReadKey(SN.SectParameters, "TimeModified",      1);
	sSettingsParameters.OriginalName      = fINI.ReadKey(SN.SectParameters, "OriginalName",      1);
	sSettingsParameters.Magic             = fINI.ReadKey(SN.SectParameters, "Magic",             1);

	//proverka/ispravlenie dannih
	if ((sSettingsParameters.UnpackSizeFiles)        &&
		(sSettingsParameters.UnpackSizeFiles   != 1))
		fINI.WriteKey(SN.SectParameters, "UnpackSizeFiles",   1);
	if ((sSettingsParameters.PackSizeFiles)          &&
		(sSettingsParameters.PackSizeFiles     != 1))
		fINI.WriteKey(SN.SectParameters, "PackSizeFiles",     1);
	if ((sSettingsParameters.ArchiveSize)            &&
		(sSettingsParameters.ArchiveSize       != 1))
		fINI.WriteKey(SN.SectParameters, "ArchiveSize",       1);
	if ((sSettingsParameters.NumberFiles)            &&
		(sSettingsParameters.NumberFiles       != 1))
		fINI.WriteKey(SN.SectParameters, "NumberFiles",       1);
	if ((sSettingsParameters.NumberFolders)          &&
		(sSettingsParameters.NumberFolders     != 1))
		fINI.WriteKey(SN.SectParameters, "NumberFolders",     1);
	if ((sSettingsParameters.NumberChapters)         &&
		(sSettingsParameters.NumberChapters    != 1))
		fINI.WriteKey(SN.SectParameters, "NumberChapters",    1);
	if ((sSettingsParameters.DictionarySize)         &&
		(sSettingsParameters.DictionarySize    != 1))
		fINI.WriteKey(SN.SectParameters, "DictionarySize",    1);
	if ((sSettingsParameters.HostOS)                 &&
		(sSettingsParameters.HostOS            != 1))
		fINI.WriteKey(SN.SectParameters, "HostOS",            1);
	if ((sSettingsParameters.ExtractOS)              &&
		(sSettingsParameters.ExtractOS         != 1))
		fINI.WriteKey(SN.SectParameters, "ExtractOS",         1);
	if ((sSettingsParameters.MethodPack)             &&
		(sSettingsParameters.MethodPack        != 1))
		fINI.WriteKey(SN.SectParameters, "MethodPack",        1);
	if ((sSettingsParameters.MemorySize)             &&
		(sSettingsParameters.MemorySize        != 1))
		fINI.WriteKey(SN.SectParameters, "MemorySize",        1);
	if ((sSettingsParameters.RatioArchiveSize)       &&
		(sSettingsParameters.RatioArchiveSize  != 1))
		fINI.WriteKey(SN.SectParameters, "RatioArchiveSize",  1);
	if ((sSettingsParameters.RatioPackFileSize)      &&
		(sSettingsParameters.RatioPackFileSize != 1))
		fINI.WriteKey(SN.SectParameters, "RatioPackFileSize", 1);
	if ((sSettingsParameters.PackVersion)            &&
		(sSettingsParameters.PackVersion       != 1))
		fINI.WriteKey(SN.SectParameters, "PackVersion",       1);
	if ((sSettingsParameters.UnpackVersion)          &&
		(sSettingsParameters.UnpackVersion     != 1))
		fINI.WriteKey(SN.SectParameters, "UnpackVersion",     1);
	if ((sSettingsParameters.SfxModule)              &&
		(sSettingsParameters.SfxModule         != 1))
		fINI.WriteKey(SN.SectParameters, "SfxModule",         1);
	if ((sSettingsParameters.RecoveryRecord)         &&
		(sSettingsParameters.RecoveryRecord    != 1))
		fINI.WriteKey(SN.SectParameters, "RecoveryRecord",    1);
	if ((sSettingsParameters.Solid)                  &&
		(sSettingsParameters.Solid             != 1))
		fINI.WriteKey(SN.SectParameters, "Solid",             1);
	if ((sSettingsParameters.MultiArc)               &&
		(sSettingsParameters.MultiArc          != 1))
		fINI.WriteKey(SN.SectParameters, "MultiArc",          1);
	if ((sSettingsParameters.MainComment)            &&
		(sSettingsParameters.MainComment       != 1))
		fINI.WriteKey(SN.SectParameters, "MainComment",       1);
	if ((sSettingsParameters.FileComment)            &&
		(sSettingsParameters.FileComment       != 1))
		fINI.WriteKey(SN.SectParameters, "FileComment",       1);
	if ((sSettingsParameters.ChapterComment)         &&
		(sSettingsParameters.ChapterComment    != 1))
		fINI.WriteKey(SN.SectParameters, "ChapterComment",    1);
	if ((sSettingsParameters.Password)               &&
		(sSettingsParameters.Password          != 1))
		fINI.WriteKey(SN.SectParameters, "Password",          1);
	if ((sSettingsParameters.ArchiveLock)            &&
		(sSettingsParameters.ArchiveLock       != 1))
		fINI.WriteKey(SN.SectParameters, "ArchiveLock",       1);
	if ((sSettingsParameters.AuthVerification)       &&
		(sSettingsParameters.AuthVerification  != 1))
		fINI.WriteKey(SN.SectParameters, "AuthVerification",  1);
	if ((sSettingsParameters.TestTime)               &&
		(sSettingsParameters.TestTime          != 1))
		fINI.WriteKey(SN.SectParameters, "TestTime",          1);
	if ((sSettingsParameters.AnalyzeTime)            &&
		(sSettingsParameters.AnalyzeTime       != 1))
		fINI.WriteKey(SN.SectParameters, "AnalyzeTime",       1);
	if ((sSettingsParameters.AllTime)                &&
		(sSettingsParameters.AllTime           != 1))
		fINI.WriteKey(SN.SectParameters, "AllTime",           1);
	if ((sSettingsParameters.DateCreated)            &&
		(sSettingsParameters.DateCreated       != 1))
		fINI.WriteKey(SN.SectParameters, "DateCreated",       1);
	if ((sSettingsParameters.DateModified)          &&
		(sSettingsParameters.DateModified      != 1))
		fINI.WriteKey(SN.SectParameters, "DateModified",      1);
	if ((sSettingsParameters.TimeCreated)           &&
		(sSettingsParameters.TimeCreated       != 1))
		fINI.WriteKey(SN.SectParameters, "TimeCreated",       1);
	if ((sSettingsParameters.TimeModified)          &&
		(sSettingsParameters.TimeModified      != 1))
		fINI.WriteKey(SN.SectParameters, "TimeModified",      1);
	if ((sSettingsParameters.OriginalName)          &&
		(sSettingsParameters.OriginalName      != 1))
		fINI.WriteKey(SN.SectParameters, "OriginalName",      1);
	if ((sSettingsParameters.Magic)                 &&
		(sSettingsParameters.Magic             != 1))
		fINI.WriteKey(SN.SectParameters, "Magic",             1);

	//[Logfile]
	//chtenie klyuchey
	sSettingsLogfile.CreateLogFile   = fINI.ReadKey(SN.SectLogfile, "CreateLogFile",   1 );
	sSettingsLogfile.DetailLogFile   = fINI.ReadKey(SN.SectLogfile, "DetailLogFile",   1 );
	sSettingsLogfile.WidthAuthorName = fINI.ReadKey(SN.SectLogfile, "WidthAuthorName", 12);
	sSettingsLogfile.WidthMessage    = fINI.ReadKey(SN.SectLogfile, "WidthMessage",    40);

	//proverka/ispravlenie dannih
	if ((sSettingsLogfile.CreateLogFile)      &&
		(sSettingsLogfile.CreateLogFile != 1))
		fINI.WriteKey(SN.SectLogfile, "CreateLogFile", 1);
	if ((sSettingsLogfile.DetailLogFile)      &&
		(sSettingsLogfile.DetailLogFile != 1)                 )
		fINI.WriteKey(SN.SectLogfile, "DetailLogFile", 1);
	if (sSettingsLogfile.WidthAuthorName > MaxWidthAuthorName)	//ne bolee 20 simvolov
	{
		sSettingsLogfile.WidthAuthorName = MaxWidthAuthorName;
		fINI.WriteKey(SN.SectLogfile, "WidthAuthorName", MaxWidthAuthorName);
	}
	if (sSettingsLogfile.WidthMessage > MaxWidthMessage)	//ne bolee 50 simvolov
	{
		sSettingsLogfile.WidthMessage = MaxWidthMessage;
		fINI.WriteKey(SN.SectLogfile, "WidthMessage", MaxWidthMessage);
	}

	//[Archives]
	//chtenie klyuchey
	fINI.ReadKey(SN.SectArchives, "PriorityArchives", "13 17 1 6 3 11 9 15 2 4 5 7 8 10 12 14 16 18", sSettingsArchives.PriorityArchives, MAX_LOADSTRING);
	sSettingsArchives.Ace       = fINI.ReadKey(SN.SectArchives, "Ace",       1);
	sSettingsArchives.ArcPak    = fINI.ReadKey(SN.SectArchives, "ArcPak",    1);
	sSettingsArchives.Arj       = fINI.ReadKey(SN.SectArchives, "Arj",       1);
	sSettingsArchives.Bh        = fINI.ReadKey(SN.SectArchives, "Bh",        1);
	sSettingsArchives.BZip2Tbz2 = fINI.ReadKey(SN.SectArchives, "BZip2Tbz2", 1);
	sSettingsArchives.Cab       = fINI.ReadKey(SN.SectArchives, "Cab",       1);
	sSettingsArchives.Cpio      = fINI.ReadKey(SN.SectArchives, "Cpio",      1);
	sSettingsArchives.Dwc       = fINI.ReadKey(SN.SectArchives, "Dwc",       1);
	sSettingsArchives.GZipTgz   = fINI.ReadKey(SN.SectArchives, "GZipTgz",   1);
	sSettingsArchives.Ha        = fINI.ReadKey(SN.SectArchives, "Ha",        1);
	sSettingsArchives.LhaLzh    = fINI.ReadKey(SN.SectArchives, "LhaLzh",    1);
	sSettingsArchives.Ppm       = fINI.ReadKey(SN.SectArchives, "Ppm",       1);
	sSettingsArchives.Rar       = fINI.ReadKey(SN.SectArchives, "Rar",       1);
	sSettingsArchives.Sqz       = fINI.ReadKey(SN.SectArchives, "Sqz",       1);
	sSettingsArchives.Tar       = fINI.ReadKey(SN.SectArchives, "Tar",       1);
	sSettingsArchives.Z         = fINI.ReadKey(SN.SectArchives, "Z",         1);
	sSettingsArchives.ZipJar    = fINI.ReadKey(SN.SectArchives, "ZipJar",    1);
	sSettingsArchives.Zoo       = fINI.ReadKey(SN.SectArchives, "Zoo",       1);

	//proverka/ispravlenie dannih
	CheckPriorityArchives();	//proverka prioriteta arhivov
	if ((sSettingsArchives.Ace)       &&
		(sSettingsArchives.Ace       != 1))
		fINI.WriteKey(SN.SectArchives, "Ace",       1);
	if ((sSettingsArchives.ArcPak)    &&
		(sSettingsArchives.ArcPak    != 1))
		fINI.WriteKey(SN.SectArchives, "ArcPak",    1);
	if ((sSettingsArchives.Arj)       &&
		(sSettingsArchives.Arj       != 1))
		fINI.WriteKey(SN.SectArchives, "Arj",       1);
	if ((sSettingsArchives.Bh)        &&
		(sSettingsArchives.Bh        != 1))
		fINI.WriteKey(SN.SectArchives, "Bh",        1);
	if ((sSettingsArchives.BZip2Tbz2) &&
		(sSettingsArchives.BZip2Tbz2 != 1))
		fINI.WriteKey(SN.SectArchives, "BZip2Tbz2", 1);
	if ((sSettingsArchives.Cab)       &&
		(sSettingsArchives.Cab       != 1))
		fINI.WriteKey(SN.SectArchives, "Cab",       1);
	if ((sSettingsArchives.Cpio)      &&
		(sSettingsArchives.Cpio      != 1))
		fINI.WriteKey(SN.SectArchives, "Cpio",      1);
	if ((sSettingsArchives.Dwc)       &&
		(sSettingsArchives.Dwc       != 1))
		fINI.WriteKey(SN.SectArchives, "Dwc",       1);
	if ((sSettingsArchives.GZipTgz)   &&
		(sSettingsArchives.GZipTgz   != 1))
		fINI.WriteKey(SN.SectArchives, "GZipTgz",   1);
	if ((sSettingsArchives.Ha)        &&
		(sSettingsArchives.Ha        != 1))
		fINI.WriteKey(SN.SectArchives, "Ha",        1);
	if ((sSettingsArchives.LhaLzh)    &&
		(sSettingsArchives.LhaLzh    != 1))
		fINI.WriteKey(SN.SectArchives, "LhaLzh",    1);
	if ((sSettingsArchives.Ppm)       &&
		(sSettingsArchives.Ppm       != 1))
		fINI.WriteKey(SN.SectArchives, "Ppm",       1);
	if ((sSettingsArchives.Rar)       &&
		(sSettingsArchives.Rar       != 1))
		fINI.WriteKey(SN.SectArchives, "Rar",       1);
	if ((sSettingsArchives.Sqz)       &&
		(sSettingsArchives.Sqz       != 1))
		fINI.WriteKey(SN.SectArchives, "Sqz",       1);
	if ((sSettingsArchives.Tar)       &&
		(sSettingsArchives.Tar       != 1))
		fINI.WriteKey(SN.SectArchives, "Tar",       1);
	if ((sSettingsArchives.Z)         &&
		(sSettingsArchives.Z         != 1))
		fINI.WriteKey(SN.SectArchives, "Z",         1);
	if ((sSettingsArchives.ZipJar)    &&
		(sSettingsArchives.ZipJar    != 1))
		fINI.WriteKey(SN.SectArchives, "ZipJar",    1);
	if ((sSettingsArchives.Zoo)       &&
		(sSettingsArchives.Zoo       != 1))
		fINI.WriteKey(SN.SectArchives, "Zoo",       1);

	//[Extebsions]
	//chtenie klyuchey
	sSettingsExtensions.OnlyExtensions = fINI.ReadKey(SN.SectExtensions, "OnlyExtensions", 0);
	fINI.ReadKey(SN.SectExtensions, "ExtAce",       "ace",         sSettingsExtensions.ExtAce,       MAX_LOADSTRING);
	fINI.ReadKey(SN.SectExtensions, "ExtArcPak",    "arc pak",     sSettingsExtensions.ExtArcPak,    MAX_LOADSTRING);
	fINI.ReadKey(SN.SectExtensions, "ExtArj",       "arj",         sSettingsExtensions.ExtArj,       MAX_LOADSTRING);
	fINI.ReadKey(SN.SectExtensions, "ExtBh",        "bh",          sSettingsExtensions.ExtBh,        MAX_LOADSTRING);
	fINI.ReadKey(SN.SectExtensions, "ExtBZip2Tbz2", "bz2 tbz2",    sSettingsExtensions.ExtBZip2Tbz2, MAX_LOADSTRING);
	fINI.ReadKey(SN.SectExtensions, "ExtCab",       "cab",         sSettingsExtensions.ExtCab,       MAX_LOADSTRING);
	fINI.ReadKey(SN.SectExtensions, "ExtCpio",      "cpio",        sSettingsExtensions.ExtCpio,      MAX_LOADSTRING);
	fINI.ReadKey(SN.SectExtensions, "ExtDwc",       "dwc",         sSettingsExtensions.ExtDwc,       MAX_LOADSTRING);
	fINI.ReadKey(SN.SectExtensions, "ExtGZipTgz",   "gz tgz",      sSettingsExtensions.ExtGZipTgz,   MAX_LOADSTRING);
	fINI.ReadKey(SN.SectExtensions, "ExtHa",        "ha",          sSettingsExtensions.ExtHa,        MAX_LOADSTRING);
	fINI.ReadKey(SN.SectExtensions, "ExtLhaLzh",    "lha lzh lzs", sSettingsExtensions.ExtLhaLzh,    MAX_LOADSTRING);
	fINI.ReadKey(SN.SectExtensions, "ExtPpm",       "ppm",         sSettingsExtensions.ExtPpm,       MAX_LOADSTRING);
	fINI.ReadKey(SN.SectExtensions, "ExtRar",       "rar",         sSettingsExtensions.ExtRar,       MAX_LOADSTRING);
	fINI.ReadKey(SN.SectExtensions, "ExtSqz",       "sqz",         sSettingsExtensions.ExtSqz,       MAX_LOADSTRING);
	fINI.ReadKey(SN.SectExtensions, "ExtTar",       "tar",         sSettingsExtensions.ExtTar,       MAX_LOADSTRING);
	fINI.ReadKey(SN.SectExtensions, "ExtZ",         "z",           sSettingsExtensions.ExtZ,         MAX_LOADSTRING);
	fINI.ReadKey(SN.SectExtensions, "ExtZipJar",    "zip jar",     sSettingsExtensions.ExtZipJar,    MAX_LOADSTRING);
	fINI.ReadKey(SN.SectExtensions, "ExtZoo",       "zoo",         sSettingsExtensions.ExtZoo,       MAX_LOADSTRING);

	//proverka/ispravlenie dannih
	if ((sSettingsExtensions.OnlyExtensions)      &&
		(sSettingsExtensions.OnlyExtensions != 1))
		fINI.WriteKey(SN.SectExtensions, "OnlyExtensions", 1);

	//[Libraries]
	//chtenie klyuchey
	fINI.ReadKey(SN.SectLibraries, "Ace", "%COMMANDER_PATH%\\unacev2.dll", sSettingsLibraries.Ace, MAX_LOADSTRING);
	fINI.ReadKey(SN.SectLibraries, "Rar", "%COMMANDER_PATH%\\unrar.dll",   sSettingsLibraries.Rar, MAX_LOADSTRING);

	//[WCXPluginsTC] ArchView.ini or [PackerPlugins] wincmd.ini
	//chtenie klyuchey (esli ispol'zuem wcx-plugins)
	if (sSettingsMain.UseWCXPluginsTC)
	{
		//iz sekcii [PackerPlugins] wincmd.ini
		if (sSettingsMain.UseWincmdIniWCX)
		{
			char pwincmdINI[MAX_PATH] = {NULL};

			//chtob put' bil real'nim bez vremennih okrujeniya
			if(!::ExpandEnvironmentStrings(sSettingsMain.PathWincmdIni, pwincmdINI, MAX_PATH))
				strncpy(pwincmdINI, sSettingsMain.PathWincmdIni, MAX_PATH);
			fwincmdINI.SetFile(pwincmdINI);

			fwincmdINI.ReadSectionSpecial(SN.SectPackerPlugins, sSettingsWCXPluginsTC.WCXPluginsTC, sSettingsMain.OptimUsePlugins);
		}
		//iz sekcii [WCXPluginsTC] ArchView.ini
		else
			fINI.ReadSection(SN.SectWCXPluginsTC, sSettingsWCXPluginsTC.WCXPluginsTC, sSettingsMain.OptimUsePlugins);
	}

	//[WCXExtensions]
	//chtenie klyuchey (esli ispol'zuem wcx-plugins)
	sSettingsWCXExtensions.OnlyExtensions = fINI.ReadKey(SN.SectWCXExtensions, "OnlyExtensions", 0);
	if (sSettingsMain.UseWCXPluginsTC)
	{
		char ValueKey[MAX_LOADSTRING] = {NULL};
		NodeStrStr* Node = sSettingsWCXPluginsTC.WCXPluginsTC.GetHeadListStrStr();
		while (Node)
		{
			fINI.ReadKey(SN.SectWCXExtensions, Node->Name, "", ValueKey, MAX_LOADSTRING);
			sSettingsWCXExtensions.WCXExtensions.AddNode(Node->Name, ValueKey);
			Node = Node->Next;
		}
	}

	//proverka/ispravlenie dannih
	if ((sSettingsWCXExtensions.OnlyExtensions)      &&
		(sSettingsWCXExtensions.OnlyExtensions != 1))
		fINI.WriteKey(SN.SectWCXExtensions, "OnlyExtensions", 1);

	return 1;
}