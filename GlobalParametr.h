/////////////////////////////////////////////////////////////////////////////
//GlobalParametr.h
//////////////////////////////////////////////////////////////////////

#ifndef T_GLOBAL_PARAMETR_H
#define T_GLOBAL_PARAMETR_H

#include "StructType.h"
#include "MListParametrs.h"
#include "MIniFile.h"
#include "MySettings.h"
#include "MyLanguage.h"
#include "MyTestFile.h"
#include "MySelectInfo.h"
#include "TArchive.h"

//parametri dlya dannogo ekzemplyara plagina
//dlya kajdogo zapushennogo ekzemplyara svoi
//prosto videlit' sobrat' vse parametri vmeste
class GlobalParametr
{
public:
	//dannie dlya zakladki Info
	typedef struct _TabInfo
	{
		BOOL           CREATED;		//TRUE - zakladka sozdana, FALSE - net
		HWND           hWndInfo;	//okno dlya informacii ob arhive
		HWND           hWndTextRL;	//okno otobrajeniya sjatiya 1 (text)
		HWND           hWndTextRR;	//okno otobrajeniya sjatiya 2 (text)
		HWND           hWndRatioL;	//okno otobrajeniya sjatiya 1 (grafic)
		HWND           hWndRatioR;	//okno otobrajeniya sjatiya 2 (grafic)
		HWND           hWndRichEdit;//okno dlya vivoda kommentariya esli nado
		LONG_PTR       REWndProc;	//obrabotchik okonnih soobsheniya dlya hWndRichEdit
		MListParametrs ListParam;	//spisok parametrov arhiva
	} TabInfo;

	//dannie dlya zakladki Context
	typedef struct _TabContext
	{
		BOOL CREATED;		//TRUE - zakladka sozdana, FALSE - net
		HWND hWndContext;	//okno dlya soderjimogo arhiva
		HWND hWndHeader;	//v nem vivod
	} TabContext;

	//dannie dlya zakladki Comment
	typedef struct _TabComment
	{
		BOOL     CREATED;		//TRUE - zakladka sozdana, FALSE - net
		HWND     hWndComment;	//okno dlya kommentariya arhiva
		HWND     hWndRichEdit;	//v nem vivod
		LONG_PTR REWndProc;		//obrabotchik okonnih soobsheniya dlya hWndRichEdit
	} TabComment;

	//dannie dlya zakladki About
	typedef struct _TabAbout
	{
		BOOL     CREATED;		//TRUE - zakladka sozdana, FALSE - net
		HWND     hWndAbout;		//okno dlya informacii o programme
		HWND     hWndRichEdit;	//v nem vivod
		LONG_PTR REWndProc;		//obrabotchik okonnih soobsheniya dlya hWndRichEdit
	} TabAbout;

	//OB'EKTI
	HFONT  hFontB;		//shrift jirniy
	HFONT  hFontN;		//shrift normal'niy
	HFONT  hFontS;		//shrift krupniy
	HANDLE hThread;		//rabochiy potok
	HANDLE hEventEnd;	//sobitie dlya iskustvennogo okonchaniya rabochego potoka
	HWND   hWndTab;		//tab control
	HWND   hWndMsg;		//dlya soobsheniy pri vipolnenii
	HWND   hWndPercent;	//dlya vivoda progressa analiza (number)
	HWND   hWndProgress;//dlya vivoda progressa analiza (grafic)

	//DANNIE
	char           FileToLoad[MAX_PATH];//imya fayla dlya testirovaniya/analiza
	int            TypeQuickView;		//tip zapuska plagina 0 - F3, 1 - QuickView
	int            ModeWordWrap;		//perenos strok 0 - otklyuchen; 1 - vklyuchen
	int            CurTabId;			//tekushaya zakladka
	int            ChangeSelTab;		//izmenenie zakladki pol'zovatelem
	int            OriginalWidth;		//ishodnaya shirina okna listera
	int            OriginalHeight;		//ishodnaya visota okna listera
	TypeOfArchive  ArchiveType;			//tip arhiva
	TypeOfTabItems Tabs[T_MAX_TAB_ITEM];//zakladki
	DataForThread  DataThread;			//dannie dlya potoka
	TArchive*      pArchData;			//klass arhiva(on analiziruet fayl)
	MyTestFile     Testbench;			//funkcii testirovaniya fayla na arhiv
	MySelectInfo   SelectInfo;			//funkcii dlya viborki dannih arhiva
	double         TestTime;			//vremya testirovaniya fayla
	int            Ratio1;				//sjatie 1
	int            Ratio2;				//sjatie 2

	//KOMMENTARIY ARHIVA
	char* pTextComment;	//tekst kommentariya

	//DLYA TEKSTOVIH POLEY
	int wOffset;			//otstup
	int SeparatorHeight;	//visota razdelitelya
	int HeightText;			//visota okon teksta
	int WidthText1;			//shirina okna imeni parametra
	int WidthText2;			//shirina okna znacheniya parametra

	//DLYA OKNA KOMMENTARIYA NA ZAKLADKE INFO (razmeri)
	int CmtInfoLeft;
	int CmtInfoTop;
	int CmtInfoWidth;
	int CmtInfoHeight;

	//VSPOMOGATEL'NIE FAYLI
	char*            pIniFile;		//imya fayla nastroek
	char*            pLanguageFile;	//imya fayla yazika (mnogoyazichnaya podderjka)
	char*            pLogFile;		//imya logfayla

	//NASTROYKI
	MySettings sSettings;	//nastroyki programmi (file ArchView.ini)
	MyLanguage sLanguage;	//yazik programmi (file *.lng)

	//ZAKLADKI
	TabInfo    tInfo;	//dannie dlya zakladki Info
	TabContext tContext;//dannie dlya zakladki Context
	TabComment tComment;//dannie dlya zakladki Comment
	TabAbout   tAbout;	//dannie dlya zakladki About

	//KEYS
	int        KeyControlSet;	//VK_CONTROL najata
	int        KeyShiftSet;		//VK_SHIFT najata

	GlobalParametr () {;}
	~GlobalParametr() {;}

	int InitialGlobalParametr(char*, HINSTANCE);
	int ClearGlobalParametr  ();
};

#endif //T_GLOBAL_PARAMETR_H
