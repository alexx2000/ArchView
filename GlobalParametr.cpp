/////////////////////////////////////////////////////////////////////////////
//GlobalParametr.cpp
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"
#include "ArchView.h"
#include "GlobalParametr.h"

/////////////////////////////////////////////////////////////////////////////
//FUNCTIONS PUBLIC
/////////////////////////////////////////////////////////////////////////////

//inicializirovat' structuru GlobalParametr
//gp         - structura kotoruyu inicializiruem
//FileToLoad - imya fayla peredannogo v plugin na razbor
//return
//1 - ok
//0 - error;
int GlobalParametr::InitialGlobalParametr(char* pFileToLoad, HINSTANCE hInst)
{
	int i = 0;

	//imya fayla
	strcpy(FileToLoad, pFileToLoad);

	//OB'EKTI
	//rabochiy potok
	hThread      = 0;
	//sobitie dlya iskustvennogo okonca rabochego potoka
	hEventEnd    = ::CreateEvent(NULL, FALSE, FALSE, "");
	//okno tab control
	hWndTab      = 0;
	//okno dlya soobsheniy o vipolnenii
	hWndMsg      = 0;
	//dlya vivoda progressa analiza (number)
	hWndPercent  = 0;
	//dlya vivoda progressa analiza (grafic)
	hWndProgress = 0;

	//DANNIE
	//tekushaya zakladka
	CurTabId             = 0;
	//tip zapuska plagina 0 - F3, 1 - QuickView
	TypeQuickView        = 0;
	//perenos strok 0 - otklyuchen; 1 - vklyuchen
	ModeWordWrap         = 0;
	//izmenenie zakladki po'zovatelem
	ChangeSelTab         = 0;
	//ishodnaya shirina okna listera
	OriginalWidth        = 0;
	//ishodnaya visota okna listera
	OriginalHeight       = 0;
	//tip arhiva
	ArchiveType          = T_UNKNOWN;
	//zakladki
	for(i=0; i<T_MAX_TAB_ITEM; i++)
		Tabs[i]          = T_NULL;
	//dannie dlya rabochego potoka
	DataThread.hEventEnd = 0;
	DataThread.hWnd      = 0;
	DataThread.Path      = 0;
	//ob'ekt arhiva (obrabativaet fayl)
	pArchData            = 0;
	//vremya testirovaniya fayla
	TestTime             = 0;
	//sjatie 1
	Ratio1               = 0;
	//sjatie 2
	Ratio2               = 0;

	//KOMMENTARIY ARHIVA
	pTextComment = 0;

	//VSPOMOGATEL'NIE FAYLI
	//proinicializiruem
	pIniFile      = 0;
	pLogFile      = 0;
	pLanguageFile = 0;
	char* Offset  = 0;
	char  PluginPath[MAX_PATH] = {NULL};
	//poluchaem imya nashego plagina i videlyem iz nego put'
	::GetModuleFileName(hInst, PluginPath, MAX_PATH);
	Offset = strrchr(PluginPath, '\\');
	if (Offset)
	{
		int len = 0;
		//inifile
		char* ArchView_ini = "ArchView.ini";
		len = Offset - PluginPath + 1;
		pIniFile = new char[len + strlen(ArchView_ini) + 1];
		strncpy(pIniFile, PluginPath, len);
		pIniFile[len] = '\0';
		strcat(pIniFile, ArchView_ini);

		//program settings
		sSettings.ReadSettingsProgram(pIniFile);

		//language file
		char* LanguageDir = "Language\\";
		char* LanguageExt = ".lng";
		len = Offset - PluginPath + 1;
		pLanguageFile = new char[len + strlen(LanguageDir) + strlen(sSettings.sSettingsMain.Language) + strlen(LanguageExt) + 1];
		strncpy(pLanguageFile, PluginPath, len);
		pLanguageFile[len] = '\0';
		strcat (pLanguageFile, LanguageDir);
		strcat (pLanguageFile, sSettings.sSettingsMain.Language);
		strcat (pLanguageFile, LanguageExt);

		//program language
		sLanguage.ReadLanguageProgram(pLanguageFile);

		//logfile
		char* ArchView_log = "ArchView.log";
		len = Offset - PluginPath + 1;
		pLogFile = new char[len + strlen(ArchView_log) + 1];
		strncpy(pLogFile, PluginPath, len);
		pLogFile[len] = '\0';
		strcat (pLogFile, ArchView_log);
	}
	else return 0;

	//DLYA TEKSTOVIH POLEY
	wOffset         = 10;	//otstup
	SeparatorHeight = 2;	//visota razdelitelya
	HeightText      = sSettings.sSettingsWindow.FontSize + 2;	//visota okon teksta
	WidthText1      = 150;	//shirina okna imeni parametra
	WidthText2      = 150;	//shirina okna znacheniya parametra

	//DLYA OKNA KOMMENTARIYA NA ZAKLADKE INFO (razmeri)
	CmtInfoLeft   = 0;
	CmtInfoTop    = 0;
	CmtInfoWidth  = 0;
	CmtInfoHeight = 0;

	//OB'EKTI
	//shrift jirniy
	hFontB      = ::CreateFont(sSettings.sSettingsWindow.FontSize,
						0, 0, 0, FW_BOLD, FALSE, FALSE,
						FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
						CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
						DEFAULT_PITCH | FF_ROMAN, "MS Sans Serif");
	//shrift normal'niy
	hFontN      = ::CreateFont(sSettings.sSettingsWindow.FontSize,
						0, 0, 0, FW_NORMAL, FALSE, FALSE,
						FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
						CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
						DEFAULT_PITCH | FF_ROMAN, "MS Sans Serif");
	//shrift krupniy
	hFontS      = ::CreateFont(sSettings.sSettingsWindow.FontSize + 10,
						0, 0, 0, FW_BOLD, FALSE, TRUE,
						FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
						CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
						DEFAULT_PITCH | FF_ROMAN, "MS Sans Serif");

	//ZAKLADKI
	//Info
	tInfo.CREATED         = FALSE;
	tInfo.hWndInfo        = 0;
	tInfo.hWndTextRL      = 0;
	tInfo.hWndTextRR      = 0;
	tInfo.hWndRatioL      = 0;
	tInfo.hWndRatioR      = 0;
	tInfo.hWndRichEdit    = 0;
	tInfo.REWndProc       = 0;
	//Context
	tContext.CREATED      = FALSE;
	tContext.hWndContext  = 0;
	tContext.hWndHeader   = 0;
	//Comment
	tComment.CREATED      = FALSE;
	tComment.hWndComment  = 0;
	tComment.hWndRichEdit = 0;
	tComment.REWndProc    = 0;
	//About
	tAbout.CREATED        = FALSE;
	tAbout.hWndAbout      = 0;
	tAbout.hWndRichEdit   = 0;
	tAbout.REWndProc      = 0;

	//KEYS
	KeyControlSet         = 0;
	KeyShiftSet           = 0;

	//proverki
	if ((!hFontB)      ||
		(!hFontN)      ||
		(!hFontS)      ||
		(!hEventEnd))
		return 0;
	else
		return 1;
}

//otchistit' structuru GlobalParametr
int GlobalParametr::ClearGlobalParametr()
{
	try
	{
		//imena faylov
		if (pIniFile)      { delete[] pIniFile;      pIniFile      = 0; }
		if (pLanguageFile) { delete[] pLanguageFile; pLanguageFile = 0; }
		if (pLogFile)      { delete[] pLogFile;      pLogFile      = 0; }
		//shrifti
		if (hFontB) { ::DeleteObject(hFontB); hFontB = 0; }
		if (hFontN) { ::DeleteObject(hFontN); hFontN = 0; }
		if (hFontS) { ::DeleteObject(hFontS); hFontS = 0; }
		//dannie
		if (pArchData)       { delete pArchData;         pArchData = 0; }
		if (hEventEnd)       { ::CloseHandle(hEventEnd); hEventEnd = 0; }
		if (hThread)         { ::CloseHandle(hThread);   hThread   = 0; }

		//zakladki
		if (tInfo.CREATED)    DeleteTabInfo(hWndTab);
		if (tContext.CREATED) DeleteTabContext(hWndTab);
		if (tComment.CREATED) DeleteTabComment(hWndTab);
		if (tAbout.CREATED)   DeleteTabAbout(hWndTab);

		//okna
		if (hWndPercent)  { ::DestroyWindow(hWndPercent);  hWndPercent  = 0; }
		if (hWndProgress) { ::DestroyWindow(hWndProgress); hWndProgress = 0; }
		if (hWndMsg)      { ::DestroyWindow(hWndMsg);      hWndMsg      = 0; }
		if (hWndTab)      { ::DestroyWindow(hWndTab);      hWndTab      = 0; }
	}
	catch (...) {;}

	return 1;
}
