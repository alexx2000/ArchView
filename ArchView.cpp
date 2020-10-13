/////////////////////////////////////////////////////////////////////////////
//ArchView.cpp : Defines the entry point for the DLL application.
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"
#include "ArchView.h"
#include "ListPlugin.h"
#include "StructType.h"
#include "GlobalParametr.h"
#include "MIniFile.h"
#include "MListStrStr.h"
#include <Richedit.h>
#include <tchar.h>
#include <time.h>

//#define _MY_DEBUG

//HINSTANCE of this DLL
HINSTANCE hInst;
TCHAR* RichEditClass;

BOOL APIENTRY DllMain(HANDLE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		hInst=(HINSTANCE)hModule;
		break;
	case DLL_PROCESS_DETACH:
		break;
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		break;
    }
    return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
//FUNCTIONS OTHER
/////////////////////////////////////////////////////////////////////////////

//dobavlaet k DetectString spisok rasshireniy (REKURSIYA)
//GenDetectString - stroka kotoruyu sostavlyaem
//String          - spisok rasshireniy cherez probel
int AddToGenDetectString(char* GenDetectString, const char* StringExt)
{
	char  t_ext[MAX_LOADSTRING] = {NULL};
	char  b_ext[] = "EXT=\"";
	char  e_ext[] = "\"|";
	char* offset  = 0;
	
	offset = (char *)strchr(StringExt, ' ');
	//videlim ocherednoe rasshirenie (tut rekursiya)
	if (offset)
	{
		int len = offset - StringExt;
		strncpy(t_ext, StringExt, len);
		strcat(GenDetectString, b_ext);
		strcat(GenDetectString, t_ext);
		strcat(GenDetectString, e_ext);
		return AddToGenDetectString(GenDetectString, offset+1);
	}
	//eto poslednee rsshirenie (konec rekursii)
	else
	{
		strcpy(t_ext, StringExt);
		strcat(GenDetectString, b_ext);
		strcat(GenDetectString, t_ext);
		strcat(GenDetectString, e_ext);
		return 1;
	}
}

/////////////////////////////////////////////////////////////////////////////
//FUNCTIONS FOR MAIN WINDOW
/////////////////////////////////////////////////////////////////////////////

//registriruem klass glavnogo okna plagina
ATOM MyRegisterClass()
{
	WNDCLASSEX wcex;

	wcex.cbSize        = sizeof(WNDCLASSEX); 
	wcex.style         = CS_HREDRAW | CS_VREDRAW | CS_PARENTDC;
	wcex.lpfnWndProc   = (WNDPROC)WndProc;
	wcex.cbClsExtra    = 0;
	wcex.cbWndExtra    = sizeof(LONG_PTR);
	wcex.hInstance     = hInst;
	wcex.hIcon         = 0;//::LoadIcon(hInst, (LPCTSTR)IDI_MY1);
	wcex.hCursor       = ::LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName  = "TMenuMainWindow";
	wcex.lpszClassName = "TMainClassWindow";
	wcex.hIconSm       = 0;//::LoadIcon(hInst, (LPCTSTR)IDI_SMALL);

	return ::RegisterClassEx(&wcex);
}

//inicializaciya glavnogo okna
//ParentWin - roditel'skoe okno listera
HWND InitInstance(HWND ParentWin)
{
	HWND  hWnd = 0;
	RECT  rcClient;

	::GetClientRect(ParentWin, &rcClient);

	hWnd = ::CreateWindow("TMainClassWindow", "TLister", WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE,
		rcClient.left, rcClient.top,
		rcClient.right - rcClient.left,
		rcClient.bottom - rcClient.top,
		ParentWin, NULL, hInst, NULL);

	if (!hWnd) return FALSE;
	else return hWnd;
}

//obrabotchik osnovnogo okna
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	//dlya organizacii zakladok
	int             NumTab   = 0;
	int             iPage    = 0;
	TypeOfTabItems  ItemShow = T_NULL;
	TCITEM          tie;
	//dlya pokaza procenta analiza v QuickView
	char            BufStr[MAX_LOADSTRING] = {NULL};
	//dlya raboti s parametrami dannoy kopii plagina
	GlobalParametr* gp     = 0;
	try { gp = (GlobalParametr*)::GetWindowLongPtr(hWnd, 0); }
	catch (...) { gp = 0; }

	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
	case WM_COMMAND:
		// Parse the menu selections:
		return ::DefWindowProc(hWnd, message, wParam, lParam);
		break;

	//zavershenie rabochego potoka
	case WM_THREAD_FINISHED:
		if (gp)
		{
			//polnoe vipolnenie (100%)
			sprintf(BufStr, "100 %s", gp->sLanguage.sLanguageOther.Percent);
			::SendMessage(gp->hWndPercent, WM_SETTEXT, 0, (LPARAM)"100 %");
			if (gp->sSettings.sSettingsWindow.ShowProgress)
				::SendMessage(gp->hWndProgress, PBM_SETPOS, (WPARAM)100, 0);

			//rezul'tat raboti rabochego potoka
			switch ((int)lParam)
			{
			case TTERMINATE_THREAD:
				::SendMessage(gp->hWndMsg, WM_SETTEXT, 0, (LPARAM)gp->sLanguage.sLanguageResults.TerminateThread);
				break;
			case TERROR_OPEN_FILE:
				::SendMessage(gp->hWndMsg, WM_SETTEXT, 0, (LPARAM)gp->sLanguage.sLanguageResults.ErrorOpenFile);
				break;
			case TERROR_READ_FILE:
				::SendMessage(gp->hWndMsg, WM_SETTEXT, 0, (LPARAM)gp->sLanguage.sLanguageResults.ErrorReadFile);
				break;
			case TERROR_SEEK_FILE:
				::SendMessage(gp->hWndMsg, WM_SETTEXT, 0, (LPARAM)gp->sLanguage.sLanguageResults.ErrorSeekFile);
				break;
			case TSUDDEN_END_FILE:
				::SendMessage(gp->hWndMsg, WM_SETTEXT, 0, (LPARAM)gp->sLanguage.sLanguageResults.SuddenEndFile);
				break;
			case TERROR_FORMAT:
				::SendMessage(gp->hWndMsg, WM_SETTEXT, 0, (LPARAM)gp->sLanguage.sLanguageResults.ErrorFormat);
				break;
			case TERROR_NOT_DLL:
				::SendMessage(gp->hWndMsg, WM_SETTEXT, 0, (LPARAM)gp->sLanguage.sLanguageResults.ErrorNotDll);
				break;
			case TERROR_DLL:
				::SendMessage(gp->hWndMsg, WM_SETTEXT, 0, (LPARAM)gp->sLanguage.sLanguageResults.ErrorDll);
				break;
			case TERROR_CREATE_WINDOW:
				::SendMessage(gp->hWndMsg, WM_SETTEXT, 0, (LPARAM)gp->sLanguage.sLanguageResults.ErrorCreateWindow);
				break;
			case TERROR_UNKNOWN:
				::SendMessage(gp->hWndMsg, WM_SETTEXT, 0, (LPARAM)gp->sLanguage.sLanguageResults.ErrorUnknown);
				break;
			case TMESSAGE_OK:
				//vibrat' dannie posle analiza arhiva
				gp->SelectInfo.SelectInfoArchive(&gp->sSettings, &gp->sLanguage, gp->pArchData, &gp->ArchiveType, &gp->tInfo.ListParam, &gp->pTextComment, gp->TestTime, &gp->Ratio1, &gp->Ratio2);

				//ubrat' soobshenie o vipolnenii
				::ShowWindow(gp->hWndMsg, SW_HIDE);

				LanguageTabs&    sLanguageTabs    = gp->sLanguage.sLanguageTabs;
				LanguageResults& sLanguageResults = gp->sLanguage.sLanguageResults;

				//dobavim nujnie zakladki
				NumTab      = 0;
				tie.mask    = TCIF_TEXT | TCIF_IMAGE;
				tie.iImage  = -1;

				//zakladka Info
				tie.pszText = sLanguageTabs.Info;
				if (TabCtrl_InsertItem(gp->hWndTab, NumTab, &tie) == -1)
				{ 
					::SendMessage(gp->hWndMsg, WM_SETTEXT, 0, (LPARAM)sLanguageResults.ErrorCreateWindow);
					::ShowWindow(gp->hWndMsg, SW_SHOW);
				} 
				gp->Tabs[NumTab] = T_INFO;

				//zakladka Context
				if (gp->sSettings.sSettingsWindow.ShowTabContext)
				{
					NumTab++;
					tie.pszText = sLanguageTabs.Context;
					if (TabCtrl_InsertItem(gp->hWndTab, NumTab, &tie) == -1)
					{ 
						::SendMessage(gp->hWndMsg, WM_SETTEXT, 0, (LPARAM)sLanguageResults.ErrorCreateWindow);
						::ShowWindow(gp->hWndMsg, SW_SHOW);
					} 
					gp->Tabs[NumTab] = T_CONTEXT;
					if ((gp->sSettings.sSettingsWindow.SaveLastTab) &&
						(gp->sSettings.sSettingsWindow.LastTabNumber == T_CONTEXT))
					{
						CreateTabContext(gp->hWndTab);
						gp->tContext.CREATED = TRUE;
						gp->CurTabId         = NumTab;
						ItemShow             = T_CONTEXT;
					}
				}

				//zakladka Comment
				if ((gp->pTextComment) && (gp->sSettings.sSettingsWindow.ShowTabComment))
				{
					NumTab++;
					tie.pszText = sLanguageTabs.Comment;
					if (TabCtrl_InsertItem(gp->hWndTab, NumTab, &tie) == -1)
					{ 
						::SendMessage(gp->hWndMsg, WM_SETTEXT, 0, (LPARAM)sLanguageResults.ErrorCreateWindow);
						::ShowWindow(gp->hWndMsg, SW_SHOW);
					} 
					gp->Tabs[NumTab] = T_COMMENT;
					if ((gp->sSettings.sSettingsWindow.SaveLastTab) &&
						(gp->sSettings.sSettingsWindow.LastTabNumber == T_COMMENT))
					{
						CreateTabComment(gp->hWndTab);
						gp->tComment.CREATED = TRUE;
						gp->CurTabId         = NumTab;
						ItemShow             = T_COMMENT;
					}
				}

				//zakladka About
				if (gp->sSettings.sSettingsWindow.ShowTabAbout)
				{
					NumTab++;
					tie.pszText = sLanguageTabs.About;
					if (TabCtrl_InsertItem(gp->hWndTab, NumTab, &tie) == -1)
					{ 
						::SendMessage(gp->hWndMsg, WM_SETTEXT, 0, (LPARAM)sLanguageResults.ErrorCreateWindow);
						::ShowWindow(gp->hWndMsg, SW_SHOW);
					} 
					gp->Tabs[NumTab] = T_ABOUT;
					if ((gp->sSettings.sSettingsWindow.SaveLastTab) &&
						(gp->sSettings.sSettingsWindow.LastTabNumber == T_ABOUT))
					{
						CreateTabAbout(gp->hWndTab);
						gp->tAbout.CREATED = TRUE;
						gp->CurTabId       = NumTab;
						ItemShow           = T_ABOUT;
					}
				}

				//oformit' zakladku Info esli eshe nichego ne sozdano
				//vse ostal'nie zakladki imeyut otlichniy ot 0 indeks
				if (!gp->CurTabId)
				{
					CreateTabInfo(gp->hWndTab);
					gp->tInfo.CREATED = TRUE;
					gp->CurTabId      = 0;
					ItemShow          = T_INFO;
				}

				//otobrazit' nujnuyu zakladku
				switch (ItemShow)
				{
				case T_INFO:
					ShowTabInfo(gp->hWndTab);
					break;
				case T_CONTEXT:
					ShowTabContext(gp->hWndTab);
					break;
				case T_COMMENT:
					ShowTabComment(gp->hWndTab);
					break;
				case T_ABOUT:
					ShowTabAbout(gp->hWndTab);
					break;
				default:
					break;
				}
				//v konce chtob okno uspelo otrisovatsya normal'no
				//inache mogut bit' glyuki s zakladkami ili gp->hWndMsg
				TabCtrl_SetCurSel(gp->hWndTab, gp->CurTabId);
				break;
			}
			//esli eto ne rejim QuickView to ustanovit' fokus
			if (gp->TypeQuickView != 1) ::SetFocus(::GetParent(gp->hWndTab));
		}
		break;

	//procent vipolneniya v rabochem potoke
	case WM_THREAD_PERCENT:
		if (gp)
		{
			//pokazat' procent vipolneniya (lParam%)
			sprintf(BufStr, "%d %s", lParam, gp->sLanguage.sLanguageOther.Percent);
			::SendMessage(gp->hWndPercent, WM_SETTEXT, 0, (LPARAM)BufStr);
			if (gp->sSettings.sSettingsWindow.ShowProgress)
				::SendMessage(gp->hWndProgress, PBM_SETPOS, (WPARAM)lParam, 0);
		}
		break;

	case WM_KEYDOWN:
		if (gp)
		{
			int VirtualKeyKode = (int)wParam;
			switch (VirtualKeyKode)
			{
			case VK_CONTROL:
				gp->KeyControlSet = 1;
				break;
			case VK_SHIFT:
				gp->KeyShiftSet   = 1;
				break;
			case VK_TAB:
				//VK_CONTROL + VK_TAB (next tab)
				iPage = TabCtrl_GetCurSel(gp->hWndTab);
				if (gp->KeyControlSet && !gp->KeyShiftSet)
				{
					iPage++;
					if (iPage >= TabCtrl_GetItemCount(gp->hWndTab))
						iPage = 0;
					TabCtrl_SetCurFocus(gp->hWndTab, iPage);
					break;
				}
				//VK_CONTROL + VK_SHIFT + VK_TAB (previous tab)
				if (gp->KeyControlSet && gp->KeyShiftSet)
				{
					iPage--;
					if (iPage < 0)
						iPage = TabCtrl_GetItemCount(gp->hWndTab) - 1;
					TabCtrl_SetCurFocus(gp->hWndTab, iPage);
					break;
				}
				//prosto tab perehod focusa mejdu elementami okna
				switch (gp->Tabs[iPage])
				{
				case T_INFO:
					if (gp->tInfo.hWndRichEdit)
						if (gp->tInfo.hWndRichEdit == ::GetFocus()) ::SetFocus(hWnd);
						else ::SetFocus(gp->tInfo.hWndRichEdit);
					break;
				case T_COMMENT:
					if (gp->tComment.hWndRichEdit)
						if (gp->tComment.hWndRichEdit == ::GetFocus()) ::SetFocus(hWnd);
						else ::SetFocus(gp->tComment.hWndRichEdit);
					break;
				case T_ABOUT:
					if (gp->tAbout.hWndRichEdit)
						if (gp->tAbout.hWndRichEdit == ::GetFocus()) ::SetFocus(hWnd);
						else ::SetFocus(gp->tAbout.hWndRichEdit);
					break;
				default:
					break;
				}
				break;
			default:
				break;
			}
		}
		break;

	case WM_KEYUP:
		if (gp)
		{
			int VirtualKeyKode = (int)wParam;
			switch (VirtualKeyKode)
			{
			case VK_CONTROL:
				gp->KeyControlSet = 0;
				break;
			case VK_SHIFT:
				gp->KeyShiftSet   = 0;
				break;
			default:
				break;
			}
		}
		break;

	case WM_KILLFOCUS:
		if (gp)
		{
			gp->KeyControlSet = 0;
			gp->KeyShiftSet   = 0;
		}
		return ::DefWindowProc(hWnd, message, wParam, lParam);
		break;

	case WM_NOTIFY:
		if (gp)
		{
			iPage = TabCtrl_GetCurSel(gp->hWndTab);
			//izmenilas' li zakladka
			if (gp->CurTabId != iPage)
			{
				//skrit' staroe okno
				switch (gp->Tabs[gp->CurTabId])
				{
				case T_INFO:	HideTabInfo(gp->hWndTab);
								break;
				case T_CONTEXT:	HideTabContext(gp->hWndTab);
								break;
				case T_COMMENT:	HideTabComment(gp->hWndTab);
								break;
				case T_ABOUT:	HideTabAbout(gp->hWndTab);
								break;
				}
				//pokazat' novoe okno
				switch (gp->Tabs[iPage])
				{
				case T_INFO:	if (!gp->tInfo.CREATED)
								{
									CreateTabInfo(gp->hWndTab);
									gp->tInfo.CREATED = TRUE;
								}
								ShowTabInfo(gp->hWndTab);
								break;
				case T_CONTEXT:	if (!gp->tContext.CREATED)
								{
									CreateTabContext(gp->hWndTab);
									gp->tContext.CREATED = TRUE;
								}
								ShowTabContext(gp->hWndTab);
								break;
				case T_COMMENT:	if (!gp->tComment.CREATED)
								{
									CreateTabComment(gp->hWndTab);
									gp->tComment.CREATED = TRUE;
								}
								ShowTabComment(gp->hWndTab);
								break;
				case T_ABOUT:	if (!gp->tAbout.CREATED)
								{
									CreateTabAbout(gp->hWndTab);
									gp->tAbout.CREATED = TRUE;
								}
								ShowTabAbout(gp->hWndTab);
								break;
				}
				gp->ChangeSelTab = 1;		//bilo izmenenie zakladki
				gp->CurTabId     = iPage;	//nomer tekushey zakladki
				::SetFocus(::GetParent(gp->hWndTab));
			}
			else
				::SetFocus(::GetParent(gp->hWndTab));
		}
		break;

	case WM_SIZE:
		if (gp)
		{
			RECT rcClient;

			//okno s zakladkami tab control
			::SetRect(&rcClient, 0, 0, LOWORD(lParam), HIWORD(lParam));
			::SetWindowPos(gp->hWndTab, HWND_TOP,
				rcClient.left, rcClient.top,
				rcClient.right - rcClient.left,
				rcClient.bottom - rcClient.top,
				SWP_DRAWFRAME);

			//okno dlya vivoda soobsheniy
			TabCtrl_AdjustRect(gp->hWndTab, FALSE, &rcClient);
			::SetWindowPos(gp->hWndMsg, HWND_TOP,
				rcClient.left, rcClient.top,
				rcClient.right - rcClient.left,
				rcClient.bottom - rcClient.top,
				SWP_DRAWFRAME);

			//okno dlya vivoda procenta analiza fayla (number)
			::GetClientRect(gp->hWndMsg, &rcClient);
			::SetWindowPos(gp->hWndPercent, HWND_TOP,
				rcClient.right - gp->WidthText1 - 5,
				rcClient.top,
				gp->WidthText1,
				gp->HeightText,
				SWP_DRAWFRAME);

			//okno dlya vivoda procenta analiza fayla (grafic)
			if (gp->sSettings.sSettingsWindow.ShowProgress)
			{
				::SetWindowPos(gp->hWndProgress, HWND_TOP,
					rcClient.left  + gp->wOffset,
					rcClient.top   + gp->HeightText,
					rcClient.right - 2*gp->wOffset,
					gp->HeightText,
					SWP_DRAWFRAME);
			}
			
			if (gp->tInfo.CREATED)    ResizeTabInfo(gp->hWndTab);
			if (gp->tContext.CREATED) ResizeTabContext(gp->hWndTab);
			if (gp->tComment.CREATED) ResizeTabComment(gp->hWndTab);
			if (gp->tAbout.CREATED)   ResizeTabAbout(gp->hWndTab);
		}
		break;

	case WM_PAINT:
		hdc = ::BeginPaint(hWnd, &ps);
		::EndPaint(hWnd, &ps);
		break;
//	case WM_DESTROY:
//		::PostQuitMessage(0);
//		break;
	default:
		return ::DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

//sozdanie okna s zakladkami
//hwndParent - roditel'skoe okno dlya tab control
HWND WINAPI DoCreateTabControl(HWND hwndParent)
{
	GlobalParametr* gp = (GlobalParametr*)::GetWindowLongPtr(hwndParent, 0);

	RECT rcClient;
	HWND hWndTab;
 
	::GetClientRect(hwndParent, &rcClient);

	hWndTab = ::CreateWindow(
		WC_TABCONTROL, "",
		WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | TCS_HOTTRACK | WS_TABSTOP,
		0, 0, rcClient.right, rcClient.bottom,
		hwndParent, NULL, hInst, NULL);

	if (!hWndTab) return NULL;
	else
	{
		::SendMessage(hWndTab, WM_SETFONT, (WPARAM)gp->hFontB, MAKELPARAM(FALSE, 0));
		return hWndTab;
	}
}

//obrabotchik osnovnogo okna
LRESULT CALLBACK RichEditWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	//peremennie
	int  iPage   = 0;
	HWND ListWin = 0;
	//dlya raboti s parametrami dannoy kopii plagina
	GlobalParametr* gp     = 0;
	try { ListWin = ::GetParent(::GetParent(::GetParent(hWnd))); gp = (GlobalParametr*)::GetWindowLongPtr(ListWin, 0); }
	catch (...) { gp = 0; }
	//opredelit' chto eto za okno i ego original'nuiy obrabotchik sobitiy
	WNDPROC OriginalWndProc = 0;
	if (gp)
	{
		if (hWnd == gp->tInfo.hWndRichEdit)
			OriginalWndProc = (WNDPROC)gp->tInfo.REWndProc;
		if (hWnd == gp->tComment.hWndRichEdit)
			OriginalWndProc = (WNDPROC)gp->tComment.REWndProc;
		if (hWnd == gp->tAbout.hWndRichEdit)
			OriginalWndProc = (WNDPROC)gp->tAbout.REWndProc;
		if (!OriginalWndProc)
			OriginalWndProc = ::DefWindowProc;
	}
	else OriginalWndProc = ::DefWindowProc;

	switch (message)
	{
	case WM_KEYDOWN:
		if (gp)
		{
			int VirtualKeyKode = (int)wParam;
			switch (VirtualKeyKode)
			{
			case 'W':
			case VK_ESCAPE:
			case VK_CONTROL:
			case VK_SHIFT:
			case VK_TAB:
				::PostMessage(ListWin, message, wParam, lParam);
				break;
			default:
				break;
			}
		}
		break;

	case WM_KEYUP:
		if (gp)
		{
			int VirtualKeyKode = (int)wParam;
			switch (VirtualKeyKode)
			{
			case VK_CONTROL:
			case VK_SHIFT:
				::PostMessage(ListWin, message, wParam, lParam);
				break;
			default:
				break;
			}
		}
		break;

	default:
		break;
	}
	return OriginalWndProc(hWnd, message, wParam, lParam);
}

/////////////////////////////////////////////////////////////////////////////
//FUNCTIONS FOR TAB INFO
/////////////////////////////////////////////////////////////////////////////

//sozdat' okno zakladki Info
//hwndParent - okno tab control
int CreateTabInfo(HWND hwndParent)
{
	GlobalParametr* gp = (GlobalParametr*)::GetWindowLongPtr(::GetParent(hwndParent), 0);

    RECT rcClient;
    ::GetClientRect(hwndParent, &rcClient);
	TabCtrl_AdjustRect(hwndParent, FALSE, &rcClient);

	gp->tInfo.hWndInfo = ::CreateWindow("STATIC", "",
		WS_CHILD,
		rcClient.left, rcClient.top,
		rcClient.right - rcClient.left,
		rcClient.bottom - rcClient.top,
		hwndParent, NULL, hInst, NULL);

    ::GetClientRect(gp->tInfo.hWndInfo, &rcClient);

	int Offset          = gp->wOffset;			//otstup
	int SeparatorHeight = gp->SeparatorHeight;	//visota razdelitelya
	int HeightText      = gp->HeightText;		//visota okon teksta
	int WidthText1      = gp->WidthText1;		//shirina okna imeni parametra
	int WidthText2      = gp->WidthText2;		//shirina okna znacheniya parametra
	//viravnivanie po centru okna
	int LeftTmp =	(rcClient.right - rcClient.left)/2 - 
					(WidthText1     + WidthText2   )/2;
	int LeftPoint1      = rcClient.left + Offset;
	if (LeftTmp > LeftPoint1) LeftPoint1 = LeftTmp;
	int LeftPoint2      = LeftPoint1 + WidthText1;
	int TopPoint        = rcClient.top + Offset;

	//poluchit' perviy parametr iz spiska parametrov arhiva
	Parametr* CurParam = gp->tInfo.ListParam.GetHeadListParametrs();

	//cikl po vsemu spisku
	while (1)
	{
		if (CurParam)
		{
			//ili parametr
			if (CurParam->Name)
			{
				CurParam->hWndName = ::CreateWindow("STATIC", "",
					WS_CHILD | WS_VISIBLE | SS_LEFTNOWORDWRAP,
					LeftPoint1, TopPoint,
					WidthText1, CurParam->NumStr * HeightText,
					gp->tInfo.hWndInfo, NULL, hInst, NULL);
				::SendMessage(CurParam->hWndName, WM_SETFONT, (WPARAM)gp->hFontN, MAKELPARAM(FALSE, 0));
				::SendMessage(CurParam->hWndName, WM_SETTEXT, 0, (LPARAM)CurParam->Name);

				CurParam->hWndValue = ::CreateWindow("STATIC", "",
					WS_CHILD | WS_VISIBLE | SS_RIGHT,
					LeftPoint2, TopPoint,
					WidthText2, CurParam->NumStr * HeightText,
					gp->tInfo.hWndInfo, NULL, hInst, NULL);
				::SendMessage(CurParam->hWndValue, WM_SETFONT, (WPARAM)gp->hFontN, MAKELPARAM(FALSE, 0));
				::SendMessage(CurParam->hWndValue, WM_SETTEXT, 0, (LPARAM)CurParam->Value);

				TopPoint += CurParam->NumStr * HeightText;		//novaya koordinata okna
				CurParam = CurParam->Next;	//sleduyushiy element - parametr
			}
			//ili razdelitel' ili elektronnaya podpis'
			else
			{
				//elektronnaya podpis'
				if (CurParam->Value)
				{
					CurParam->hWndValue = ::CreateWindow("STATIC", "",
						WS_CHILD | WS_VISIBLE | SS_RIGHT,
						LeftPoint1 + Offset/2,
						TopPoint + SeparatorHeight,
						WidthText1 + WidthText2 - Offset/2 - Offset/2,
						CurParam->NumStr * HeightText,
						gp->tInfo.hWndInfo, NULL, hInst, NULL);
				::SendMessage(CurParam->hWndValue, WM_SETFONT, (WPARAM)gp->hFontB, MAKELPARAM(FALSE, 0));
				::SendMessage(CurParam->hWndValue, WM_SETTEXT, 0, (LPARAM)CurParam->Value);

					TopPoint += CurParam->NumStr * HeightText
							+ SeparatorHeight;		//novaya koordinata okna
					CurParam = CurParam->Next;		//sleduyushiy element - parametr
				}
				//razdelitel'
				else
				{
					//esli posle razdelitelya nichego net to on ne nujen
					if (CurParam->Next)
					{
						CurParam->hWndName = ::CreateWindow("STATIC", "",
							WS_CHILD | WS_VISIBLE | SS_SUNKEN,
							LeftPoint1 + Offset,
							TopPoint   + SeparatorHeight,
							WidthText1 + WidthText2 - Offset - Offset,
							SeparatorHeight,
							gp->tInfo.hWndInfo, NULL, hInst, NULL);

						TopPoint += 4 * SeparatorHeight;//novaya koordinata okna
						CurParam = CurParam->Next;		//sleduyushiy element - parametr
					}
					else CurParam = CurParam->Next;		//sleduyushiy element - parametr
				}
			}
		}
		else break;
	}

	//poloski sjatiya
	int LeftPointL      = Offset;
	int LeftPointR      = LeftPoint2 + WidthText2 + Offset;
	int WidthRatioL     = 0;
	int WidthRatioR     = 0;
	if (LeftPoint1 > 3*Offset) WidthRatioL = LeftPoint1 - 2*Offset;
	if ((rcClient.right - LeftPoint2 - WidthText2) > 3*Offset) WidthRatioR = (rcClient.right - LeftPoint2 - WidthText2) - 2*Offset;
	//viravnivanie i ogranicenie polosok sjatiya
	int WidthRatioTmp   = 5*Offset;
	if (WidthRatioL > WidthRatioTmp)
	{
		LeftPointL = LeftPointL + (WidthRatioL - WidthRatioTmp)/2;
		WidthRatioL = WidthRatioTmp;
	}
	if (WidthRatioR > WidthRatioTmp)
	{
		LeftPointR = LeftPointR + (WidthRatioR - WidthRatioTmp)/2;
		WidthRatioR = WidthRatioTmp;
	}
	//visota zavisit ot nalichiya kommentariya na zakladke Info
	int HeightRatio     = 0;
	if ((gp->pTextComment) && (gp->sSettings.sSettingsWindow.ShowCommentOnTabInfo))
		HeightRatio = TopPoint - rcClient.top - Offset;
	else
		HeightRatio = rcClient.bottom - rcClient.top - 2*Offset;

	char BufStr[MAX_LOADSTRING] = {NULL};
	if ((gp->sSettings.sSettingsWindow.ShowLeftRatio)  && (gp->Ratio1))
	{
		gp->tInfo.hWndTextRL = ::CreateWindow("STATIC", "",
			WS_CHILD | WS_VISIBLE | SS_CENTER,
			LeftPointL - Offset, Offset,
			WidthRatioL + 2*Offset, HeightText,
			gp->tInfo.hWndInfo, NULL, hInst, NULL);
		::SendMessage(gp->tInfo.hWndTextRL, WM_SETFONT, (WPARAM)gp->hFontB, MAKELPARAM(FALSE, 0));
		sprintf(BufStr, "%d", gp->Ratio1);
		::SendMessage(gp->tInfo.hWndTextRL, WM_SETTEXT, 0, (LPARAM)BufStr);
		//esli poloska ne prorisovana nadpis' nado spryatat'
		if (WidthRatioL == 0) ::ShowWindow(gp->tInfo.hWndTextRL, SW_HIDE);
		else ::ShowWindow(gp->tInfo.hWndTextRL, SW_SHOW);
		gp->tInfo.hWndRatioL = CreateWindowEx(0, PROGRESS_CLASS, "",
				WS_CHILD | WS_VISIBLE | PBS_SMOOTH | PBS_VERTICAL,
				LeftPointL, Offset+HeightText, WidthRatioL, HeightRatio-HeightText,
				gp->tInfo.hWndInfo, NULL, hInst, NULL);
		::SendMessage(gp->tInfo.hWndRatioL, PBM_SETPOS, (WPARAM)gp->Ratio1, 0);
	}
	if ((gp->sSettings.sSettingsWindow.ShowRightRatio) && (gp->Ratio2))
	{
		gp->tInfo.hWndTextRR = ::CreateWindow("STATIC", "",
			WS_CHILD | WS_VISIBLE | SS_CENTER,
			LeftPointR - Offset, Offset,
			WidthRatioR + 2*Offset, HeightText,
			gp->tInfo.hWndInfo, NULL, hInst, NULL);
		::SendMessage(gp->tInfo.hWndTextRR, WM_SETFONT, (WPARAM)gp->hFontB, MAKELPARAM(FALSE, 0));
		sprintf(BufStr, "%d", gp->Ratio2);
		::SendMessage(gp->tInfo.hWndTextRR, WM_SETTEXT, 0, (LPARAM)BufStr);
		//esli poloska ne prorisovana nadpis' nado spryatat'
		if (WidthRatioR == 0) ::ShowWindow(gp->tInfo.hWndTextRR, SW_HIDE);
		else ::ShowWindow(gp->tInfo.hWndTextRR, SW_SHOW);
		gp->tInfo.hWndRatioR = CreateWindowEx(0, PROGRESS_CLASS, "",
				WS_CHILD | WS_VISIBLE | PBS_SMOOTH | PBS_VERTICAL,
				LeftPointR, Offset+HeightText, WidthRatioR, HeightRatio-HeightText,
				gp->tInfo.hWndInfo, NULL, hInst, NULL);
		::SendMessage(gp->tInfo.hWndRatioR, PBM_SETPOS, (WPARAM)gp->Ratio2, 0);
	}

	//esli nado to eshe i kommentariy
	if ((gp->pTextComment) && (gp->sSettings.sSettingsWindow.ShowCommentOnTabInfo))
	{
		gp->CmtInfoLeft   = rcClient.left;
		gp->CmtInfoTop    = TopPoint;
		gp->CmtInfoWidth  = rcClient.right - rcClient.left;
		gp->CmtInfoHeight = rcClient.bottom - TopPoint;

		DWORD WindowStyle = 0;
		if (gp->ModeWordWrap) WindowStyle = WS_CHILD | WS_VISIBLE | WS_BORDER | WS_HSCROLL | WS_VSCROLL | ES_READONLY | ES_MULTILINE | WS_TABSTOP;
		else WindowStyle = WS_CHILD | WS_VISIBLE | WS_BORDER | WS_HSCROLL | WS_VSCROLL | ES_READONLY | ES_MULTILINE | ES_AUTOHSCROLL | ES_AUTOVSCROLL | WS_TABSTOP;
		gp->tInfo.hWndRichEdit = ::CreateWindow(RichEditClass, "", 	WindowStyle,
			gp->CmtInfoLeft, gp->CmtInfoTop,
			gp->CmtInfoWidth, gp->CmtInfoHeight,
			gp->tInfo.hWndInfo, NULL, hInst, NULL);
		::SendMessage(gp->tInfo.hWndRichEdit, WM_SETFONT, (WPARAM)gp->hFontN, MAKELPARAM(FALSE, 0));
		::SendMessage(gp->tInfo.hWndRichEdit, WM_SETTEXT, 0, (LPARAM)gp->pTextComment);

		//podmenim obrabotchik sobitiy
		if (gp->tInfo.hWndRichEdit)
		{
			gp->tInfo.REWndProc = ::GetWindowLongPtr(gp->tInfo.hWndRichEdit, GWLP_WNDPROC);
			::SetWindowLongPtr(gp->tInfo.hWndRichEdit, GWLP_WNDPROC, (LONG_PTR)RichEditWndProc);
		}
	}

	return 1;
}

//udalit' okno zakladki Info
//hwndParent - okno tab control
int DeleteTabInfo(HWND hwndParent)
{
	GlobalParametr* gp = (GlobalParametr*)::GetWindowLongPtr(::GetParent(hwndParent), 0);
	gp->tInfo.ListParam.DeleteHwndListParametrs();
	if (gp->tInfo.hWndRichEdit)
	{
		//vernem obrabotchik sobitiy
		if (gp->tInfo.REWndProc)
		{
			::SetWindowLongPtr(gp->tInfo.hWndRichEdit, GWLP_WNDPROC, gp->tInfo.REWndProc);
			gp->tInfo.REWndProc = 0;
		}
		::DestroyWindow(gp->tInfo.hWndRichEdit);
		gp->tInfo.hWndRichEdit = 0;
	}
	if (gp->tInfo.hWndInfo)
	{
		::DestroyWindow(gp->tInfo.hWndInfo);
		gp->tInfo.hWndInfo = 0;
	}
	if (gp->tInfo.hWndTextRL)
	{
		::DestroyWindow(gp->tInfo.hWndTextRL);
		gp->tInfo.hWndTextRL = 0;
	}
	if (gp->tInfo.hWndTextRR)
	{
		::DestroyWindow(gp->tInfo.hWndTextRR);
		gp->tInfo.hWndTextRR = 0;
	}
	if (gp->tInfo.hWndRatioL)
	{
		::DestroyWindow(gp->tInfo.hWndRatioL);
		gp->tInfo.hWndRatioL = 0;
	}
	if (gp->tInfo.hWndRatioR)
	{
		::DestroyWindow(gp->tInfo.hWndRatioR);
		gp->tInfo.hWndRatioR = 0;
	}
	return 1;
}

//pokazat' okno zakladki Info
//hwndParent - okno tab control
int ShowTabInfo(HWND hwndParent)
{
	GlobalParametr* gp = (GlobalParametr*)::GetWindowLongPtr(::GetParent(hwndParent), 0);
	::ShowWindow(gp->tInfo.hWndInfo, SW_SHOW);
	return 1;
}

//spryatat' okno zakladki Info
//hwndParent - okno tab control
int HideTabInfo(HWND hwndParent)
{
	GlobalParametr* gp = (GlobalParametr*)::GetWindowLongPtr(::GetParent(hwndParent), 0);
	::ShowWindow(gp->tInfo.hWndInfo, SW_HIDE);
	return 1;
}

//izmenit' razmer okna zakladki Info
//hwndParent - okno tab control
int ResizeTabInfo(HWND hwndParent)
{
	GlobalParametr* gp = (GlobalParametr*)::GetWindowLongPtr(::GetParent(hwndParent), 0);

    RECT rcClient;
    ::GetClientRect(hwndParent, &rcClient);
	TabCtrl_AdjustRect(hwndParent, FALSE, &rcClient);

	::SetWindowPos(gp->tInfo.hWndInfo, HWND_TOP,
		rcClient.left, rcClient.top,
		rcClient.right - rcClient.left,
		rcClient.bottom - rcClient.top,
		SWP_DRAWFRAME);

    ::GetClientRect(gp->tInfo.hWndInfo, &rcClient);

	int Offset          = gp->wOffset;			//otstup
	int SeparatorHeight = gp->SeparatorHeight;	//visota razdelitelya
	int HeightText      = gp->HeightText;		//visota okon teksta
	int WidthText1      = gp->WidthText1;		//shirina okna imeni parametra
	int WidthText2      = gp->WidthText2;	//shirina okna znacheniya parametra
	//viravnivanie po centru okna
	int LeftTmp =	(rcClient.right - rcClient.left)/2 - 
					(WidthText1     + WidthText2   )/2;
	int LeftPoint1      = rcClient.left + Offset;
	if (LeftTmp > LeftPoint1) LeftPoint1 = LeftTmp;
	int LeftPoint2      = LeftPoint1 + WidthText1;
	int TopPoint        = rcClient.top + Offset;

	//poluchit' perviy parametr iz spiska parametrov arhiva
	Parametr* CurParam = gp->tInfo.ListParam.GetHeadListParametrs();

	//cikl po vsemu spisku
	while (1)
	{
		if (CurParam)
		{
			//ili parametr
			if (CurParam->Name)
			{
				::SetWindowPos(CurParam->hWndName, HWND_TOP,
					LeftPoint1, TopPoint,
					WidthText1, CurParam->NumStr * HeightText,
					SWP_DRAWFRAME);
				::SetWindowPos(CurParam->hWndValue, HWND_TOP,
					LeftPoint2, TopPoint,
					WidthText2, CurParam->NumStr * HeightText,
					SWP_DRAWFRAME);

				TopPoint += CurParam->NumStr * HeightText;		//novaya koordinata okna
				CurParam = CurParam->Next;	//sleduyushiy element - parametr
			}
			//ili razdelitel' ili elektronnaya podpis'
			else
			{
				//elektronnaya podpis'
				if (CurParam->Value)
				{
					::SetWindowPos(CurParam->hWndValue, HWND_TOP,
						LeftPoint1 + Offset/2,
						TopPoint + SeparatorHeight,
						WidthText1 + WidthText2 - Offset/2 - Offset/2,
						CurParam->NumStr * HeightText,
						SWP_DRAWFRAME);

					TopPoint += CurParam->NumStr * HeightText
							+ SeparatorHeight;		//novaya koordinata okna
					CurParam = CurParam->Next;		//sleduyushiy element - parametr
				}
				//razdelitel'
				else
				{
					::SetWindowPos(CurParam->hWndName, HWND_TOP,
						LeftPoint1 + Offset,
						TopPoint   + SeparatorHeight,
						WidthText1 + WidthText2 - Offset - Offset,
						SeparatorHeight,
						SWP_DRAWFRAME);

					TopPoint += 4 * SeparatorHeight;//novaya koordinata okna
					CurParam = CurParam->Next;		//sleduyushiy element - parametr
				}
			}
		}
		else break;
	}

	//poloski sjatiya
	int LeftPointL      = Offset;
	int LeftPointR      = LeftPoint2 + WidthText2 + Offset;
	int WidthRatioL     = 0;
	int WidthRatioR     = 0;
	if (LeftPoint1 > 3*Offset) WidthRatioL = LeftPoint1 - 2*Offset;
	if ((rcClient.right - LeftPoint2 - WidthText2) > 3*Offset) WidthRatioR = (rcClient.right - LeftPoint2 - WidthText2) - 2*Offset;
	//viravnivanie i ogranicenie polosok sjatiya
	int WidthRatioTmp   = 5*Offset;
	if (WidthRatioL > WidthRatioTmp)
	{
		LeftPointL = LeftPointL + (WidthRatioL - WidthRatioTmp)/2;
		WidthRatioL = WidthRatioTmp;
	}
	if (WidthRatioR > WidthRatioTmp)
	{
		LeftPointR = LeftPointR + (WidthRatioR - WidthRatioTmp)/2;
		WidthRatioR = WidthRatioTmp;
	}
	//visota zavisit ot nalichiya kommentariya na zakladke Info
	int HeightRatio     = 0;
	if ((gp->pTextComment) && (gp->sSettings.sSettingsWindow.ShowCommentOnTabInfo))
		HeightRatio = TopPoint - rcClient.top - Offset;
	else
		HeightRatio = rcClient.bottom - rcClient.top - 2*Offset;

	if ((gp->sSettings.sSettingsWindow.ShowLeftRatio)  && (gp->Ratio1))
	{
		::SetWindowPos(gp->tInfo.hWndTextRL, HWND_TOP,
				LeftPointL - Offset, Offset,
				WidthRatioL + 2*Offset, HeightText,
				SWP_DRAWFRAME);
		//esli poloska ne prorisovana nadpis' nado spryatat'
		if (WidthRatioL == 0) ::ShowWindow(gp->tInfo.hWndTextRL, SW_HIDE);
		else ::ShowWindow(gp->tInfo.hWndTextRL, SW_SHOW);
		::SetWindowPos(gp->tInfo.hWndRatioL, HWND_TOP,
				LeftPointL, Offset+HeightText, WidthRatioL, HeightRatio-HeightText,
				SWP_DRAWFRAME);
	}
	if ((gp->sSettings.sSettingsWindow.ShowRightRatio) && (gp->Ratio2))
	{
		::SetWindowPos(gp->tInfo.hWndTextRR, HWND_TOP,
				LeftPointR - Offset, Offset,
				WidthRatioR + 2*Offset, HeightText,
				SWP_DRAWFRAME);
		//esli poloska ne prorisovana nadpis' nado spryatat'
		if (WidthRatioR == 0) ::ShowWindow(gp->tInfo.hWndTextRR, SW_HIDE);
		else ::ShowWindow(gp->tInfo.hWndTextRR, SW_SHOW);
		::SetWindowPos(gp->tInfo.hWndRatioR, HWND_TOP,
				LeftPointR, Offset+HeightText, WidthRatioR, HeightRatio-HeightText,
				SWP_DRAWFRAME);
	}

	//esli nado to eshe i kommentariy
	if ((gp->pTextComment) && (gp->sSettings.sSettingsWindow.ShowCommentOnTabInfo))
	{
		gp->CmtInfoLeft   = rcClient.left;
		gp->CmtInfoTop    = TopPoint;
		gp->CmtInfoWidth  = rcClient.right - rcClient.left;
		gp->CmtInfoHeight = rcClient.bottom - TopPoint;

		::SetWindowPos(gp->tInfo.hWndRichEdit, HWND_TOP,
			gp->CmtInfoLeft, gp->CmtInfoTop,
			gp->CmtInfoWidth, gp->CmtInfoHeight,
			SWP_DRAWFRAME);
	}

	return 1;
}

/////////////////////////////////////////////////////////////////////////////
//FUNCTIONS FOR TAB CONTEXT
/////////////////////////////////////////////////////////////////////////////

//sozdat' okno zakladki Context
//hwndParent - okno tab control
int CreateTabContext(HWND hwndParent)
{
	GlobalParametr* gp = (GlobalParametr*)::GetWindowLongPtr(::GetParent(hwndParent), 0);

    RECT rcClient;
    ::GetClientRect(hwndParent, &rcClient);
	TabCtrl_AdjustRect(hwndParent, FALSE, &rcClient);

	gp->tContext.hWndContext = ::CreateWindow("STATIC", "",
		WS_CHILD,
		rcClient.left, rcClient.top,
		rcClient.right - rcClient.left,
		rcClient.bottom - rcClient.top,
		hwndParent, NULL, hInst, NULL);

    ::GetClientRect(gp->tContext.hWndContext, &rcClient);

	HDLAYOUT hdl; 
	WINDOWPOS wp; 

	gp->tContext.hWndHeader = ::CreateWindowEx(0, WC_HEADER, (LPCTSTR) NULL, 
		WS_CHILD | WS_BORDER | HDS_BUTTONS | HDS_HORZ, 
		0, 0, 0, 0, gp->tContext.hWndContext, NULL, hInst, NULL);
	::SendMessage(gp->tContext.hWndHeader, WM_SETFONT, (WPARAM)gp->hFontB, MAKELPARAM(FALSE, 0));

	hdl.prc = &rcClient; 
	hdl.pwpos = &wp; 
	::SendMessage(gp->tContext.hWndHeader, HDM_LAYOUT, 0, (LPARAM) &hdl);

	::SetWindowPos(gp->tContext.hWndHeader, wp.hwndInsertAfter, wp.x, wp.y, 
	wp.cx, wp.cy, wp.flags | SWP_SHOWWINDOW);

	//zapolnyaem
	HDITEM hdi;
	char filefild[]  = "File";
	char sizefild[]  = "Size";
	char ratiofild[] = "Ratio";

	hdi.mask = HDI_TEXT | HDI_FORMAT | HDI_WIDTH;
	hdi.cxy = 100;
	hdi.fmt = HDF_LEFT | HDF_STRING;

	hdi.pszText = filefild;
	hdi.cchTextMax = lstrlen(filefild);
	SendMessage(gp->tContext.hWndHeader, HDM_INSERTITEM, (WPARAM) 1, (LPARAM) &hdi);

	hdi.pszText = sizefild;
	hdi.cchTextMax = lstrlen(sizefild);
	SendMessage(gp->tContext.hWndHeader, HDM_INSERTITEM, (WPARAM) 2, (LPARAM) &hdi);

	hdi.pszText = ratiofild;
	hdi.cchTextMax = lstrlen(ratiofild);
	SendMessage(gp->tContext.hWndHeader, HDM_INSERTITEM, (WPARAM) 3, (LPARAM) &hdi);

	return 1;
}

//udalit' okno zakladki Context
//hwndParent - okno tab control
int DeleteTabContext(HWND hwndParent)
{
	GlobalParametr* gp = (GlobalParametr*)::GetWindowLongPtr(::GetParent(hwndParent), 0);
	if (gp->tContext.hWndHeader)
	{
		::DestroyWindow(gp->tContext.hWndHeader);
		gp->tContext.hWndHeader = 0;
	}
	if (gp->tContext.hWndContext)
	{
		::DestroyWindow(gp->tContext.hWndContext);
		gp->tContext.hWndContext = 0;
	}
	return 1;
}

//pokazat' okno zakladki Context
//hwndParent - okno tab control
int ShowTabContext(HWND hwndParent)
{
	GlobalParametr* gp = (GlobalParametr*)::GetWindowLongPtr(::GetParent(hwndParent), 0);
	::ShowWindow(gp->tContext.hWndContext, SW_SHOW);
	return 1;
}

//spryatat' okno zakladki Context
//hwndParent - okno tab control
int HideTabContext(HWND hwndParent)
{
	GlobalParametr* gp = (GlobalParametr*)::GetWindowLongPtr(::GetParent(hwndParent), 0);
	::ShowWindow(gp->tContext.hWndContext, SW_HIDE);
	return 1;
}

//izmenit' razmer okna zakladki Context
//hwndParent - okno tab control
int ResizeTabContext(HWND hwndParent)
{
	GlobalParametr* gp = (GlobalParametr*)::GetWindowLongPtr(::GetParent(hwndParent), 0);

    RECT rcClient;
    ::GetClientRect(hwndParent, &rcClient);
	TabCtrl_AdjustRect(hwndParent, FALSE, &rcClient);

	::SetWindowPos(gp->tContext.hWndContext, HWND_TOP,
		rcClient.left, rcClient.top,
		rcClient.right - rcClient.left,
		rcClient.bottom - rcClient.top,
		SWP_DRAWFRAME);

    ::GetClientRect(gp->tContext.hWndContext, &rcClient);

	return 1;
}

/////////////////////////////////////////////////////////////////////////////
//FUNCTIONS FOR TAB COMMENT
/////////////////////////////////////////////////////////////////////////////

//sozdat' okno zakladki Comment
//hwndParent - okno tab control
int CreateTabComment(HWND hwndParent)
{
	GlobalParametr* gp = (GlobalParametr*)::GetWindowLongPtr(::GetParent(hwndParent), 0);

    RECT rcClient;
    ::GetClientRect(hwndParent, &rcClient);
	TabCtrl_AdjustRect(hwndParent, FALSE, &rcClient);

	gp->tComment.hWndComment = ::CreateWindow("STATIC", "",
		WS_CHILD,
		rcClient.left, rcClient.top,
		rcClient.right - rcClient.left,
		rcClient.bottom - rcClient.top,
		hwndParent, NULL, hInst, NULL);

    ::GetClientRect(gp->tComment.hWndComment, &rcClient);
	DWORD WindowStyle = 0;
	if (gp->ModeWordWrap) WindowStyle = WS_CHILD | WS_VISIBLE | WS_BORDER | WS_HSCROLL | WS_VSCROLL | ES_READONLY | ES_MULTILINE | WS_TABSTOP;
	else WindowStyle = WS_CHILD | WS_VISIBLE | WS_BORDER | WS_HSCROLL | WS_VSCROLL | ES_READONLY | ES_MULTILINE | ES_AUTOHSCROLL | ES_AUTOVSCROLL | WS_TABSTOP;
	gp->tComment.hWndRichEdit = ::CreateWindow(RichEditClass, "", 	WindowStyle,
		rcClient.left, rcClient.top,
		rcClient.right - rcClient.left,
		rcClient.bottom - rcClient.top,
		gp->tComment.hWndComment, NULL, hInst, NULL);
	::SendMessage(gp->tComment.hWndRichEdit, WM_SETFONT, (WPARAM)gp->hFontN, MAKELPARAM(FALSE, 0));

	//podmenim obrabotchik sobitiy
	if (gp->tComment.hWndRichEdit)
	{
		gp->tComment.REWndProc = ::GetWindowLongPtr(gp->tComment.hWndRichEdit, GWLP_WNDPROC);
		::SetWindowLongPtr(gp->tComment.hWndRichEdit, GWLP_WNDPROC, (LONG_PTR)RichEditWndProc);
	}

	::SendMessage(gp->tComment.hWndRichEdit, WM_SETTEXT, 0, (LPARAM)gp->pTextComment);

	return 1;
}

//udalit' okno zakladki Comment
//hwndParent - okno tab control
int DeleteTabComment(HWND hwndParent)
{
	GlobalParametr* gp = (GlobalParametr*)::GetWindowLongPtr(::GetParent(hwndParent), 0);
	if (gp->tComment.hWndRichEdit)
	{
		//vernem obrabotchik sobitiy
		if (gp->tComment.REWndProc)
		{
			::SetWindowLongPtr(gp->tComment.hWndRichEdit, GWLP_WNDPROC, gp->tComment.REWndProc);
			gp->tComment.REWndProc = 0;
		}
		::DestroyWindow(gp->tComment.hWndRichEdit);
		gp->tComment.hWndRichEdit = 0;
	}
	if (gp->tComment.hWndComment)
	{
		::DestroyWindow(gp->tComment.hWndComment);
		gp->tComment.hWndComment = 0;
	}
	return 1;
}

//pokazat' okno zakladki Comment
//hwndParent - okno tab control
int ShowTabComment(HWND hwndParent)
{
	GlobalParametr* gp = (GlobalParametr*)::GetWindowLongPtr(::GetParent(hwndParent), 0);
	::ShowWindow(gp->tComment.hWndComment, SW_SHOW);
	return 1;
}

//spryatat' okno zakladki Comment
//hwndParent - okno tab control
int HideTabComment(HWND hwndParent)
{
	GlobalParametr* gp = (GlobalParametr*)::GetWindowLongPtr(::GetParent(hwndParent), 0);
	::ShowWindow(gp->tComment.hWndComment, SW_HIDE);
	return 1;
}

//izmenit' razmer okna zakladki Comment
//hwndParent - okno tab control
int ResizeTabComment(HWND hwndParent)
{
	GlobalParametr* gp = (GlobalParametr*)::GetWindowLongPtr(::GetParent(hwndParent), 0);

    RECT rcClient;
    ::GetClientRect(hwndParent, &rcClient);
	TabCtrl_AdjustRect(hwndParent, FALSE, &rcClient);

	::SetWindowPos(gp->tComment.hWndComment, HWND_TOP,
		rcClient.left, rcClient.top,
		rcClient.right - rcClient.left,
		rcClient.bottom - rcClient.top,
		SWP_DRAWFRAME);

    ::GetClientRect(gp->tComment.hWndComment, &rcClient);
	::SetWindowPos(gp->tComment.hWndRichEdit, HWND_TOP,
		rcClient.left, rcClient.top,
		rcClient.right - rcClient.left,
		rcClient.bottom - rcClient.top,
		SWP_DRAWFRAME);

	return 1;
}

/////////////////////////////////////////////////////////////////////////////
//FUNCTIONS FOR TAB ABOUT
/////////////////////////////////////////////////////////////////////////////

//sozdat' okno zakladki About
//hwndParent - okno tab control
int CreateTabAbout(HWND hwndParent)
{
	GlobalParametr* gp = (GlobalParametr*)::GetWindowLongPtr(::GetParent(hwndParent), 0);

    RECT rcClient;
    ::GetClientRect(hwndParent, &rcClient);
	TabCtrl_AdjustRect(hwndParent, FALSE, &rcClient);

	gp->tAbout.hWndAbout = ::CreateWindow("STATIC", "",
		WS_CHILD,
		rcClient.left, rcClient.top,
		rcClient.right - rcClient.left,
		rcClient.bottom - rcClient.top,
		hwndParent, NULL, hInst, NULL);

    ::GetClientRect(gp->tAbout.hWndAbout, &rcClient);
	gp->tAbout.hWndRichEdit = ::CreateWindow(RichEditClass, "",
		WS_CHILD | WS_VISIBLE | WS_BORDER | WS_HSCROLL | WS_VSCROLL | ES_READONLY | ES_MULTILINE | WS_TABSTOP,
		rcClient.left, rcClient.top,
		rcClient.right - rcClient.left,
		rcClient.bottom - rcClient.top,
		gp->tAbout.hWndAbout, NULL, hInst, NULL);
	::SendMessage(gp->tAbout.hWndRichEdit, WM_SETFONT, (WPARAM)gp->hFontN, MAKELPARAM(FALSE, 0));

	//podmenim obrabotchik sobitiy
	if (gp->tAbout.hWndRichEdit)
	{
		gp->tAbout.REWndProc = ::GetWindowLongPtr(gp->tAbout.hWndRichEdit, GWLP_WNDPROC);
		::SetWindowLongPtr(gp->tAbout.hWndRichEdit, GWLP_WNDPROC, (LONG_PTR)RichEditWndProc);
	}

	//sozdadim stroku About
	LanguageTabAbout& sLanguageTabAbout = gp->sLanguage.sLanguageTabAbout;
	//shabloni
	int   len                = 0;
	char* StrMsg             = 0;
	char  NamePlugin       [] = "ArchView ";
	char  VersionPlugin    [] = " 0.9.2.4 (13/10/2020)";
	char  NameProgram      [] = " Total Commander";
	char  NumberProgram    [] = " 5.51 ";
	char  ErgoSite         [] = " http:\\\\wincmd.ru";
	char  Copyright        [] = "Copyright (c) 2004-2006 Hudyakov Aleksey. ";
	char  MyEMail          [] = "Parcan@mail.ru";
	char  Dash             [] = " - ";
	char  Colon            [] = ": ";
	char  Comma            [] = ", ";
	char  Dot              [] = ". ";
	char  NewLine          [] = "\n";
	char  Separator        [] = "----------";
	char  CzechLng         [] = " Czech.lng";
	char  RussianLng       [] = " Russian.lng";
	char  SpanishLng       [] = " Spanish.lng";
	char  UkrainianLng     [] = " Ukrainian.lng";
	char  DeutschLng       [] = " Deutsch.lng";
	char  GermanLng        [] = " German.lng";
	char  LithuanianLng    [] = " Lithuanian.lng";
	char  PolishLng        [] = " Polish.lng";
	char  FrenchLng        [] = " French.lng";
	char  ReadmeRus        [] = "Readme.rus";
	char  RomanBataev      [] = "Roman Bataev";
	char  CaptainFlint     [] = "CaptainFlint";
	char  SCHMaster        [] = "SCHMaster";
	char  Sheridan         [] = "Sheridan";
	char  ShurikSoft       [] = "ShurikSoft";
	char  PavelAgeev       [] = "Pavel Ageev";
	char  LubomirJezek     [] = "Lubomir Jezek";
	char  AlexandrFilipchik[] = "Alexandr Filipchik";
	char  ChristianGhisler [] = "Christian Ghisler";
	char  AndreyPyasetskiy [] = "Andrey Pyasetskiy";
	char  LuisMejia        [] = "Luis Mejia";
	char  VicoKoby         [] = "Vico//Koby";
	char  OscarSalinasMora [] = "Oscar Salinas Mora (OSM666)";
	char  Xxxxx0           [] = "xxxxx0";
	char  IgorButskiy      [] = "Igor Butskiy";
	char  NorbertFiedler   [] = "Norbert Fiedler";
	char  KurtLettmaier    [] = "Kurt Lettmaier";
	char  PeterBrockamp    [] = "Peter Brockamp";
	char  DomasLasauskas   [] = "Domas Lasauskas";
	char  UltimaPrime      [] = "ULTIMA PRIME";
	char  Ouistiti         [] = "Ouistiti";
	char  SupportFormats   [] = "ACE, ARC (PAK), ARJ, BH - Blak Hole, BZIP2 (TAR BZIP2), CAB, CPIO, DWC, GZIP (TAR GZIP), HA, LHA (LZH), PPM, RAR, SQZ, TAR, Z, ZIP (JAR - Java ARhiver), ZOO";

	//rasschitaem dlinu
	len =	strlen(NamePlugin)
		+	strlen(sLanguageTabAbout.Version)
		+	strlen(VersionPlugin)
		+	strlen(Dash)
		+	strlen(sLanguageTabAbout.TypeProgram)
		+	strlen(NameProgram)
		+	strlen(NumberProgram)
		+	strlen(sLanguageTabAbout.AndLater)
		+	strlen(Comma)
		+	strlen(sLanguageTabAbout.UseProgram)
		+	strlen(Dot)
		+	strlen(NewLine)
		+	strlen(NewLine)
		+	strlen(NamePlugin)
		+	strlen(sLanguageTabAbout.FunctionProgram)
		+   strlen(Colon)
		+   strlen(SupportFormats)
		+	strlen(Dot)
		+   strlen(sLanguageTabAbout.SupportWCXPlugins)
		+	strlen(NameProgram)
		+	strlen(Dot)
		+	strlen(NewLine)
		+	strlen(NewLine)
		+	strlen(sLanguageTabAbout.SpecialThanksTo)
		+	strlen(Colon)
		+	strlen(NewLine)
		+	strlen(Separator)
		+	strlen(NewLine)
		+	strlen(RomanBataev)
		+	strlen(Dash)
		+	strlen(sLanguageTabAbout.AdviceAndHelp)
		+	strlen(NewLine)
		+	strlen(CaptainFlint)
		+	strlen(Dash)
		+	strlen(sLanguageTabAbout.AdviceAndHelp)
		+	strlen(NewLine)
		+	strlen(SCHMaster)
		+	strlen(Dash)
		+	strlen(sLanguageTabAbout.AdviceAndHelp)
		+	strlen(NewLine)
		+	strlen(Sheridan)
		+	strlen(Dash)
		+	strlen(sLanguageTabAbout.AdviceAndHelp)
		+	strlen(NewLine)
		+	strlen(ShurikSoft)
		+	strlen(Dash)
		+	strlen(sLanguageTabAbout.AdviceAndHelp)
		+	strlen(NewLine)
		+	strlen(PavelAgeev)
		+	strlen(Dash)
		+	strlen(sLanguageTabAbout.AdviceAndHelp)
		+	strlen(NewLine)
		+	strlen(LubomirJezek)
		+	strlen(Dash)
		+	strlen(sLanguageTabAbout.AdviceAndHelp)
		+	strlen(NewLine)
		+	strlen(AlexandrFilipchik)
		+	strlen(Dash)
		+	strlen(sLanguageTabAbout.AdviceAndHelp)
		+	strlen(RussianLng)
		+	strlen(Comma)
		+	strlen(ReadmeRus)
		+	strlen(NewLine)
		+	strlen(ChristianGhisler)
		+	strlen(Dash)
		+	strlen(sLanguageTabAbout.AuthorProgram)
		+	strlen(NameProgram)
		+	strlen(NewLine)
		+	strlen(AndreyPyasetskiy)
		+	strlen(Dash)
		+	strlen(sLanguageTabAbout.AuthorSite)
		+	strlen(ErgoSite)
		+	strlen(NewLine)
		+	strlen(LuisMejia)
		+	strlen(Dash)
		+	strlen(sLanguageTabAbout.ForTranslation)
		+	strlen(SpanishLng)
		+	strlen(NewLine)
		+	strlen(VicoKoby)
		+	strlen(Dash)
		+	strlen(sLanguageTabAbout.ForTranslation)
		+	strlen(SpanishLng)
		+	strlen(NewLine)
		+	strlen(OscarSalinasMora)
		+	strlen(Dash)
		+	strlen(sLanguageTabAbout.ForTranslation)
		+	strlen(SpanishLng)
		+	strlen(NewLine)
		+	strlen(Xxxxx0)
		+	strlen(Dash)
		+	strlen(sLanguageTabAbout.ForTranslation)
		+	strlen(CzechLng)
		+	strlen(NewLine)
		+	strlen(IgorButskiy)
		+	strlen(Dash)
		+	strlen(sLanguageTabAbout.ForTranslation)
		+	strlen(UkrainianLng)
		+	strlen(NewLine)
		+	strlen(NorbertFiedler)
		+	strlen(Dash)
		+	strlen(sLanguageTabAbout.ForTranslation)
		+	strlen(DeutschLng)
		+	strlen(NewLine)
		+	strlen(KurtLettmaier)
		+	strlen(Dash)
		+	strlen(sLanguageTabAbout.ForTranslation)
		+	strlen(GermanLng)
		+	strlen(NewLine)
		+	strlen(PeterBrockamp)
		+	strlen(Dash)
		+	strlen(sLanguageTabAbout.ForTranslation)
		+	strlen(DeutschLng)
		+	strlen(NewLine)
		+	strlen(DomasLasauskas)
		+	strlen(Dash)
		+	strlen(sLanguageTabAbout.ForTranslation)
		+	strlen(LithuanianLng)
		+	strlen(NewLine)
		+	strlen(UltimaPrime)
		+	strlen(Dash)
		+	strlen(sLanguageTabAbout.ForTranslation)
		+	strlen(PolishLng)
		+	strlen(NewLine)
		+	strlen(Ouistiti)
		+	strlen(Dash)
		+	strlen(sLanguageTabAbout.ForTranslation)
		+	strlen(FrenchLng)
		+	strlen(NewLine)
		+	strlen(NewLine)
		+	strlen(sLanguageTabAbout.Contact)
		+	strlen(NewLine)
		+	strlen(Separator)
		+	strlen(NewLine)
		+	strlen(sLanguageTabAbout.SendComment)
		+	strlen(Dot)
		+	strlen(NewLine)
		+	strlen(sLanguageTabAbout.EMail)
		+	strlen(Colon)
		+	strlen(MyEMail)
		+	strlen(NewLine)
		+	strlen(Copyright)
		+	strlen(sLanguageTabAbout.AllRightsReserved)
		+	strlen(Dot);
	//vozmojnie dobavleniya
	if ((strlen(sLanguageTabAbout.TranslationMan))  ||
		(strlen(sLanguageTabAbout.TranslationMail)) ||
		(strlen(sLanguageTabAbout.TranslationSite)))
	{
		len +=	strlen(NewLine)
			+	strlen(NewLine)
			+	strlen(sLanguageTabAbout.Translation)
			+	strlen(NewLine)
			+	strlen(Separator);
	}
	if (strlen(sLanguageTabAbout.TranslationMan))
	{
		len +=	strlen(NewLine)
			+	strlen(sLanguageTabAbout.TranslationMan);
	}
	if (strlen(sLanguageTabAbout.TranslationMail))
	{
		len +=	strlen(NewLine)
			+	strlen(sLanguageTabAbout.EMail)
			+	strlen(Colon)
			+	strlen(sLanguageTabAbout.TranslationMail);
	}
	if (strlen(sLanguageTabAbout.TranslationSite))
	{
		len +=	strlen(NewLine)
			+	strlen(sLanguageTabAbout.HomePage)
			+	strlen(Colon)
			+	strlen(sLanguageTabAbout.TranslationSite);
	}
	//sozdadim bufer
	StrMsg = new char[len+1];

	//sozdadim stroku
	strcpy(StrMsg, NamePlugin);
	strcat(StrMsg, sLanguageTabAbout.Version);
	strcat(StrMsg, VersionPlugin);
	strcat(StrMsg, Dash);
	strcat(StrMsg, sLanguageTabAbout.TypeProgram);
	strcat(StrMsg, NameProgram);
	strcat(StrMsg, NumberProgram);
	strcat(StrMsg, sLanguageTabAbout.AndLater);
	strcat(StrMsg, Comma);
	strcat(StrMsg, sLanguageTabAbout.UseProgram);
	strcat(StrMsg, Dot);
	strcat(StrMsg, NewLine);
	strcat(StrMsg, NewLine);
	strcat(StrMsg, NamePlugin);
	strcat(StrMsg, sLanguageTabAbout.FunctionProgram);
	strcat(StrMsg, Colon);
	strcat(StrMsg, SupportFormats);
	strcat(StrMsg, Dot);
	strcat(StrMsg, sLanguageTabAbout.SupportWCXPlugins);
	strcat(StrMsg, NameProgram);
	strcat(StrMsg, Dot);
	strcat(StrMsg, NewLine);
	strcat(StrMsg, NewLine);
	strcat(StrMsg, sLanguageTabAbout.SpecialThanksTo);
	strcat(StrMsg, Colon);
	strcat(StrMsg, NewLine);
	strcat(StrMsg, Separator);
	strcat(StrMsg, NewLine);
	strcat(StrMsg, RomanBataev);
	strcat(StrMsg, Dash);
	strcat(StrMsg, sLanguageTabAbout.AdviceAndHelp);
	strcat(StrMsg, NewLine);
	strcat(StrMsg, CaptainFlint);
	strcat(StrMsg, Dash);
	strcat(StrMsg, sLanguageTabAbout.AdviceAndHelp);
	strcat(StrMsg, NewLine);
	strcat(StrMsg, SCHMaster);
	strcat(StrMsg, Dash);
	strcat(StrMsg, sLanguageTabAbout.AdviceAndHelp);
	strcat(StrMsg, NewLine);
	strcat(StrMsg, Sheridan);
	strcat(StrMsg, Dash);
	strcat(StrMsg, sLanguageTabAbout.AdviceAndHelp);
	strcat(StrMsg, NewLine);
	strcat(StrMsg, ShurikSoft);
	strcat(StrMsg, Dash);
	strcat(StrMsg, sLanguageTabAbout.AdviceAndHelp);
	strcat(StrMsg, NewLine);
	strcat(StrMsg, PavelAgeev);
	strcat(StrMsg, Dash);
	strcat(StrMsg, sLanguageTabAbout.AdviceAndHelp);
	strcat(StrMsg, NewLine);
	strcat(StrMsg, LubomirJezek);
	strcat(StrMsg, Dash);
	strcat(StrMsg, sLanguageTabAbout.AdviceAndHelp);
	strcat(StrMsg, NewLine);
	strcat(StrMsg, AlexandrFilipchik);
	strcat(StrMsg, Dash);
	strcat(StrMsg, sLanguageTabAbout.AdviceAndHelp);
	strcat(StrMsg, RussianLng);
	strcat(StrMsg, Comma);
	strcat(StrMsg, ReadmeRus);
	strcat(StrMsg, NewLine);
	strcat(StrMsg, ChristianGhisler);
	strcat(StrMsg, Dash);
	strcat(StrMsg, sLanguageTabAbout.AuthorProgram);
	strcat(StrMsg, NameProgram);
	strcat(StrMsg, NewLine);
	strcat(StrMsg, AndreyPyasetskiy);
	strcat(StrMsg, Dash);
	strcat(StrMsg, sLanguageTabAbout.AuthorSite);
	strcat(StrMsg, ErgoSite);
	strcat(StrMsg, NewLine);
	strcat(StrMsg, LuisMejia);
	strcat(StrMsg, Dash);
	strcat(StrMsg, sLanguageTabAbout.ForTranslation);
	strcat(StrMsg, SpanishLng);
	strcat(StrMsg, NewLine);
	strcat(StrMsg, VicoKoby);
	strcat(StrMsg, Dash);
	strcat(StrMsg, sLanguageTabAbout.ForTranslation);
	strcat(StrMsg, SpanishLng);
	strcat(StrMsg, NewLine);
	strcat(StrMsg, OscarSalinasMora);
	strcat(StrMsg, Dash);
	strcat(StrMsg, sLanguageTabAbout.ForTranslation);
	strcat(StrMsg, SpanishLng);
	strcat(StrMsg, NewLine);
	strcat(StrMsg, Xxxxx0);
	strcat(StrMsg, Dash);
	strcat(StrMsg, sLanguageTabAbout.ForTranslation);
	strcat(StrMsg, CzechLng);
	strcat(StrMsg, NewLine);
	strcat(StrMsg, IgorButskiy);
	strcat(StrMsg, Dash);
	strcat(StrMsg, sLanguageTabAbout.ForTranslation);
	strcat(StrMsg, UkrainianLng);
	strcat(StrMsg, NewLine);
	strcat(StrMsg, NorbertFiedler);
	strcat(StrMsg, Dash);
	strcat(StrMsg, sLanguageTabAbout.ForTranslation);
	strcat(StrMsg, DeutschLng);
	strcat(StrMsg, NewLine);
	strcat(StrMsg, KurtLettmaier);
	strcat(StrMsg, Dash);
	strcat(StrMsg, sLanguageTabAbout.ForTranslation);
	strcat(StrMsg, GermanLng);
	strcat(StrMsg, NewLine);
	strcat(StrMsg, PeterBrockamp);
	strcat(StrMsg, Dash);
	strcat(StrMsg, sLanguageTabAbout.ForTranslation);
	strcat(StrMsg, DeutschLng);
	strcat(StrMsg, NewLine);
	strcat(StrMsg, DomasLasauskas);
	strcat(StrMsg, Dash);
	strcat(StrMsg, sLanguageTabAbout.ForTranslation);
	strcat(StrMsg, LithuanianLng);
	strcat(StrMsg, NewLine);
	strcat(StrMsg, UltimaPrime);
	strcat(StrMsg, Dash);
	strcat(StrMsg, sLanguageTabAbout.ForTranslation);
	strcat(StrMsg, PolishLng);
	strcat(StrMsg, NewLine);
	strcat(StrMsg, Ouistiti);
	strcat(StrMsg, Dash);
	strcat(StrMsg, sLanguageTabAbout.ForTranslation);
	strcat(StrMsg, FrenchLng);
	strcat(StrMsg, NewLine);
	strcat(StrMsg, NewLine);
	strcat(StrMsg, sLanguageTabAbout.Contact);
	strcat(StrMsg, NewLine);
	strcat(StrMsg, Separator);
	strcat(StrMsg, NewLine);
	strcat(StrMsg, sLanguageTabAbout.SendComment);
	strcat(StrMsg, Dot);
	strcat(StrMsg, NewLine);
	strcat(StrMsg, sLanguageTabAbout.EMail);
	strcat(StrMsg, Colon);
	strcat(StrMsg, MyEMail);
	strcat(StrMsg, NewLine);
	strcat(StrMsg, Copyright);
	strcat(StrMsg, sLanguageTabAbout.AllRightsReserved);
	strcat(StrMsg, Dot);
	//vozmojnie dobavleniya
	if ((strlen(sLanguageTabAbout.TranslationMan))  ||
		(strlen(sLanguageTabAbout.TranslationMail)) ||
		(strlen(sLanguageTabAbout.TranslationSite)))
	{
		strcat(StrMsg, NewLine);
		strcat(StrMsg, NewLine);
		strcat(StrMsg, sLanguageTabAbout.Translation);
		strcat(StrMsg, NewLine);
		strcat(StrMsg, Separator);
	}
	if (strlen(sLanguageTabAbout.TranslationMan))
	{
		strcat(StrMsg, NewLine);
		strcat(StrMsg, sLanguageTabAbout.TranslationMan);
	}
	if (strlen(sLanguageTabAbout.TranslationMail))
	{
		strcat(StrMsg, NewLine);
		strcat(StrMsg, sLanguageTabAbout.EMail);
		strcat(StrMsg, Colon);
		strcat(StrMsg, sLanguageTabAbout.TranslationMail);
	}
	if (strlen(sLanguageTabAbout.TranslationSite))
	{
		strcat(StrMsg, NewLine);
		strcat(StrMsg, sLanguageTabAbout.HomePage);
		strcat(StrMsg, Colon);
		strcat(StrMsg, sLanguageTabAbout.TranslationSite);
	}

	//vivedem stroku
	::SendMessage(gp->tAbout.hWndRichEdit, WM_SETTEXT, 0, (LPARAM)StrMsg);

	//udalim buffer
	if (StrMsg) { delete[] StrMsg, StrMsg = 0; }

	return 1;
}

//udalit' okno zakladki About
//hwndParent - okno tab control
int DeleteTabAbout(HWND hwndParent)
{
	GlobalParametr* gp = (GlobalParametr*)::GetWindowLongPtr(::GetParent(hwndParent), 0);
	if (gp->tAbout.hWndRichEdit)
	{
		//vernem obrabotchik sobitiy
		if (gp->tAbout.REWndProc)
		{
			::SetWindowLongPtr(gp->tAbout.hWndRichEdit, GWLP_WNDPROC, gp->tAbout.REWndProc);
			gp->tAbout.REWndProc = 0;
		}
		::DestroyWindow(gp->tAbout.hWndRichEdit);
		gp->tAbout.hWndRichEdit = 0;
	}
	if (gp->tAbout.hWndAbout)
	{
		::DestroyWindow(gp->tAbout.hWndAbout);
		gp->tAbout.hWndAbout = 0;
	}
	return 1;
}

//pokazat' okno zakladki About
//hwndParent - okno tab control
int ShowTabAbout(HWND hwndParent)
{
	GlobalParametr* gp = (GlobalParametr*)::GetWindowLongPtr(::GetParent(hwndParent), 0);
	::ShowWindow(gp->tAbout.hWndAbout, SW_SHOW);
	return 1;
}

//spryatat' okno zakladki About
//hwndParent - okno tab control
int HideTabAbout(HWND hwndParent)
{
	GlobalParametr* gp = (GlobalParametr*)::GetWindowLongPtr(::GetParent(hwndParent), 0);
	::ShowWindow(gp->tAbout.hWndAbout, SW_HIDE);
	return 1;
}

//izmenit' razmer okna zakladki About
//hwndParent - okno tab control
int ResizeTabAbout(HWND hwndParent)
{
	GlobalParametr* gp = (GlobalParametr*)::GetWindowLongPtr(::GetParent(hwndParent), 0);

    RECT rcClient;
    ::GetClientRect(hwndParent, &rcClient);
	TabCtrl_AdjustRect(hwndParent, FALSE, &rcClient);

	::SetWindowPos(gp->tAbout.hWndAbout, HWND_TOP,
		rcClient.left, rcClient.top,
		rcClient.right - rcClient.left,
		rcClient.bottom - rcClient.top,
		SWP_DRAWFRAME);

    ::GetClientRect(gp->tAbout.hWndAbout, &rcClient);
	::SetWindowPos(gp->tAbout.hWndRichEdit, HWND_TOP,
		rcClient.left, rcClient.top,
		rcClient.right - rcClient.left,
		rcClient.bottom - rcClient.top,
		SWP_DRAWFRAME);

	return 1;
}


/////////////////////////////////////////////////////////////////////////////
//FUNCTIONS THREAD
/////////////////////////////////////////////////////////////////////////////

//funkciya zapuskaemaya rabochim potokom
UINT ComputeThreadProc(LPVOID pParam)
{
	DataForThread*  dft = (DataForThread*)pParam;
	GlobalParametr* gp  = (GlobalParametr*)::GetWindowLongPtr(dft->hWnd, 0);

	gp->pArchData->Start(pParam);

	return 0;
}

/////////////////////////////////////////////////////////////////////////////
//EXPORTS FUNCTIONS
/////////////////////////////////////////////////////////////////////////////

HWND __stdcall ListLoad(HWND ParentWin,char* FileToLoad,int ShowFlags)
{
	//v kompilyacii DEBUG programma rabotala bez oshibok
	//a v RELEASE bez DEBUG informacii inogda poyavlyalis' oshibki
	//prichini oshibki neizvestni
	//problema reshena konstrukciey try-catch
	try
	{
		HWND   hwnd                   = 0;
		int    len                    = 0;
		char*  StrMsg                 = 0;
		char   BufStr[MAX_LOADSTRING] = {NULL};
		RECT   rcClient;

		//parametri sozdavaemogo ekzemplyara plagina
		GlobalParametr* gp;
		gp = new GlobalParametr;
		if (!gp->InitialGlobalParametr(FileToLoad, hInst))
		{
			gp->ClearGlobalParametr();
			delete gp;
			gp = 0;
			return NULL;
		}

		//ne reagirovat' na sobstvenniy logfile
		//(a to v zavisimosti ot nastroek mojet srazu zateretsya)
		char pTargetFile[MAX_PATH] = {NULL};
		char pMyLogFile [MAX_PATH] = {NULL};
		strcpy(pTargetFile, FileToLoad);
		strcpy(pMyLogFile,  gp->pLogFile);
		strlwr(pTargetFile);
		strlwr(pMyLogFile);
		if (strcmp(pTargetFile, pMyLogFile) == 0)
		{
			gp->ClearGlobalParametr();
			delete gp;
			gp = 0;
			return NULL;
		}

		//testirovanie fayla (izvestniy li nam arhiv)
		clock_t start = clock();
		gp->pArchData = gp->Testbench.TestArchive(&gp->sSettings, &gp->sLanguage, &gp->ArchiveType, gp->FileToLoad, gp->pLogFile);
		if (!gp->pArchData)
		{
			gp->ClearGlobalParametr();
			delete gp;
			gp = 0;
			return NULL;
		}
		clock_t finish = clock();
		gp->TestTime = ((double)(finish - start) / CLOCKS_PER_SEC);

		//opredelim tip zapauska plagina
		//esli ClassName = "TTOTAL_CMD" - to eto QuickView
		if(::GetParent(ParentWin) != 0) gp->TypeQuickView = 1;
		//rejim perenosa strok
		if (ShowFlags & lcp_wraptext) gp->ModeWordWrap = 1;

		//sozdaem osnovnoe okno
		MyRegisterClass();
		hwnd = InitInstance(ParentWin);
		if (!hwnd)
		{
			gp->ClearGlobalParametr();
			delete gp;
			gp = 0;
			return NULL;
		}

		//priceplyaem k nemu nashi parametri etogo ekzemplyara plagina
		::SetWindowLongPtr(hwnd, 0, (LONG_PTR)gp);

		//sozdaem okno s zakladkami
		gp->hWndTab = DoCreateTabControl(hwnd);
		if (!gp->hWndTab)
		{
			if (hwnd) { ::DestroyWindow(hwnd); hwnd = 0; }
			gp->ClearGlobalParametr();
			delete gp;
			gp = 0;
			return NULL;
		}

		//otrisovivaem
		::ShowWindow(hwnd, SW_SHOW);
		::UpdateWindow(hwnd);
		
		//dlya zapuska cherez F3 vozmojno nado ustanovit' razmer
		if ((!gp->TypeQuickView)                         &&
			(gp->sSettings.sSettingsWindow.SaveWindowSize))
		{
			//sohranim razmeri
			::GetWindowRect(ParentWin, &rcClient);
			gp->OriginalWidth  = rcClient.right  - rcClient.left;
			gp->OriginalHeight = rcClient.bottom - rcClient.top;
			//proverim nashi razmeri
			if (!gp->sSettings.sSettingsWindow.WidthWindow)
				gp->sSettings.sSettingsWindow.WidthWindow  = gp->OriginalWidth;
			if (!gp->sSettings.sSettingsWindow.HeightWindow)
				gp->sSettings.sSettingsWindow.HeightWindow = gp->OriginalHeight;
			//ustanovim novie razmeri
			::SetWindowPos(ParentWin, HWND_TOP,
				rcClient.left, rcClient.top,
				gp->sSettings.sSettingsWindow.WidthWindow,
				gp->sSettings.sSettingsWindow.HeightWindow,
				SWP_DRAWFRAME);
		}

		//sozdat' soobshenie o tom kakoy eto arhiv na vremya ego analiza
		::GetClientRect(gp->hWndTab, &rcClient);
		TabCtrl_AdjustRect(gp->hWndTab, FALSE, &rcClient);
		gp->hWndMsg = ::CreateWindow("STATIC", "",
			WS_CHILD | WS_VISIBLE | WS_BORDER | SS_CENTER | SS_CENTERIMAGE,
			rcClient.left, rcClient.top,
			rcClient.right - rcClient.left,
			rcClient.bottom - rcClient.top,
			gp->hWndTab, NULL, hInst, NULL);
		if (!gp->hWndMsg)
		{
			if (gp->hWndTab) { ::DestroyWindow(gp->hWndTab); gp->hWndTab = 0; }
			if (hwnd) { ::DestroyWindow(hwnd); hwnd = 0; }
			gp->ClearGlobalParametr();
			delete gp;
			gp = 0;
			return NULL;
		}
		::SendMessage(gp->hWndMsg, WM_SETFONT, (WPARAM)gp->hFontS, MAKELPARAM(FALSE, 0));
		//sostavim teks soobsheniya
		char*             space             = " ";
		LanguageOther&    sLanguageOther    = gp->sLanguage.sLanguageOther;
		LanguageArchives& sLanguageArchives = gp->sLanguage.sLanguageArchives;
		switch(gp->ArchiveType)
		{
		case T_ACE:           strcpy(BufStr, sLanguageArchives.Ace);               break;
		case T_ARC_PAK:       strcpy(BufStr, sLanguageArchives.ArcPak);            break;
		case T_ARJ:           strcpy(BufStr, sLanguageArchives.Arj);               break;
		case T_BH:            strcpy(BufStr, sLanguageArchives.Bh);                break;
		case T_BZIP2_TBZ2:    strcpy(BufStr, sLanguageArchives.BZip2Tbz2);         break;
		case T_CAB:           strcpy(BufStr, sLanguageArchives.Cab);               break;
		case T_CPIO:          strcpy(BufStr, sLanguageArchives.Cpio);              break;
		case T_DWC:           strcpy(BufStr, sLanguageArchives.Dwc);               break;
		case T_GZIP_TGZ:      strcpy(BufStr, sLanguageArchives.GZipTgz);           break;
		case T_HA:            strcpy(BufStr, sLanguageArchives.Ha);                break;
		case T_LHA_LZH:       strcpy(BufStr, sLanguageArchives.LhaLzh);            break;
		case T_PPM:           strcpy(BufStr, sLanguageArchives.Ppm);               break;
		case T_RAR:           strcpy(BufStr, sLanguageArchives.Rar);               break;
		case T_SQZ:           strcpy(BufStr, sLanguageArchives.Sqz);               break;
		case T_TAR:           strcpy(BufStr, sLanguageArchives.Tar);               break;
		case T_Z:             strcpy(BufStr, sLanguageArchives.Z);                 break;
		case T_ZIP_JAR:       strcpy(BufStr, sLanguageArchives.ZipJar);            break;
		case T_ZOO:           strcpy(BufStr, sLanguageArchives.Zoo);               break;
		case T_WCX_PLUGIN_TC: strcpy(BufStr, gp->pArchData->GetInternalName());    break;
		default:              strcpy(BufStr, sLanguageOther.Unknown);              break;
		}
		len = strlen(BufStr) + strlen(space) + strlen(sLanguageOther.PleaseWait);
		StrMsg = new char[len+1];
		strcpy(StrMsg, BufStr);
		strcat(StrMsg, space);
		strcat(StrMsg, sLanguageOther.PleaseWait);
		::SendMessage(gp->hWndMsg, WM_SETTEXT, 0, (LPARAM)StrMsg);
		if (StrMsg) { delete[] StrMsg; StrMsg = 0; }

		//sozdadim okno dlya vivoda procenta analiza fayla (number)
		::GetClientRect(gp->hWndMsg, &rcClient);
		gp->hWndPercent = ::CreateWindow("STATIC", "",
			WS_CHILD | WS_VISIBLE | SS_RIGHT,
			rcClient.right - gp->WidthText1 - 5,
			rcClient.top,
			gp->WidthText1,
			gp->HeightText,
			gp->hWndMsg, NULL, hInst, NULL);
		if (!gp->hWndPercent)
		{
			if (gp->hWndTab) { ::DestroyWindow(gp->hWndTab); gp->hWndTab = 0; }
			if (hwnd) { ::DestroyWindow(hwnd); hwnd = 0; }
			gp->ClearGlobalParametr();
			delete gp;
			gp = 0;
			return NULL;
		}
		::SendMessage(gp->hWndPercent, WM_SETFONT, (WPARAM)gp->hFontN, MAKELPARAM(FALSE, 0));
		sprintf(BufStr, "0 %s", gp->sLanguage.sLanguageOther.Percent);
		::SendMessage(gp->hWndPercent, WM_SETTEXT, 0, (LPARAM)BufStr);
		//rasschitivaem visotu teksta i izmenim razmer okna
		HDC MyHdc = ::GetDC(gp->hWndPercent);
		char GetTextSizeString[] = "abcdefghijklmnopqrstuvwxyz0[(%";
		SIZE tTextSize = {0, 0};
		::SelectObject(MyHdc, gp->hFontN);
		::GetTextExtentPoint32(MyHdc, GetTextSizeString, strlen(GetTextSizeString), &tTextSize);
		gp->HeightText = tTextSize.cy + 1;
		gp->WidthText1 = tTextSize.cx + 1;
		gp->WidthText2 = tTextSize.cx + 1;
		::ReleaseDC(gp->hWndPercent, MyHdc);
		::SetWindowPos(gp->hWndPercent, HWND_TOP,
			rcClient.right - gp->WidthText1 - 5,
			rcClient.top,
			gp->WidthText1,
			gp->HeightText,
			SWP_DRAWFRAME);

		//sozdadim okno dlya vivoda procenta analiza fayla (grafic)
		if (gp->sSettings.sSettingsWindow.ShowProgress)
		{
			::SendMessage(gp->hWndProgress, PBM_SETPOS, (WPARAM)gp->Ratio1, 0);
			gp->hWndProgress = ::CreateWindowEx(0, PROGRESS_CLASS, "",
				WS_CHILD | WS_VISIBLE,
				rcClient.left  + gp->wOffset,
				rcClient.top   + gp->HeightText,
				rcClient.right - 2*gp->wOffset,
				gp->HeightText,
				gp->hWndMsg, NULL, hInst, NULL);
			if (!gp->hWndProgress)
			{
				if (gp->hWndTab) { ::DestroyWindow(gp->hWndTab); gp->hWndTab = 0; }
				if (hwnd) { ::DestroyWindow(hwnd); hwnd = 0; }
				gp->ClearGlobalParametr();
				delete gp;
				gp = 0;
				return NULL;
			}
		}
		
#ifndef _MY_DEBUG

		//rabochiy potok obrabotki
		DWORD ThID;
		gp->DataThread.hWnd        = hwnd;			//okno dlya otpravki soobsheniy iz potoka
		gp->DataThread.hEventEnd   = gp->hEventEnd;	//sobitie dlya iskustvennogo okonchaniya rabochego potoka
		gp->DataThread.Flag        = ANALYZE_ARC;	//analizirovat'
		gp->DataThread.BeginThread = TRUE;			//v rabochem potoke
		gp->DataThread.Path        = gp->FileToLoad;//fayl dlya analiza
		//zapusk potoka
		gp->hThread = ::CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ComputeThreadProc, &gp->DataThread, THREAD_PRIORITY_NORMAL, &ThID);
		if(!gp->hThread)
		{
			if (gp->hWndTab) { ::DestroyWindow(gp->hWndTab); gp->hWndTab = 0; }
			if (hwnd) { ::DestroyWindow(hwnd); hwnd = 0; }
			gp->ClearGlobalParametr();
			delete gp;
			gp = 0;
			return NULL;
		}

#else

		//dlya otladki v odnom potoke
		gp->DataThread.hWnd        = 0;				//okno dlya otpravki soobsheniy iz potoka
		gp->DataThread.hEventEnd   = 0;				//sobitie dlya iskustvennogo okonchaniya rabochego potoka
		gp->DataThread.Flag        = ANALYZE_ARC;	//analizirovat'
		gp->DataThread.BeginThread = FALSE;			//v rabochem potoke
		gp->DataThread.Path        = gp->FileToLoad;//fayl dlya analiza
		//zapusk analiza
		if (gp->pArchData->Start(&gp->DataThread) != TMESSAGE_OK)
		{
			if (gp->hWndTab) { ::DestroyWindow(gp->hWndTab); gp->hWndTab = 0; }
			if (hwnd) { ::DestroyWindow(hwnd); hwnd = 0; }
			gp->ClearGlobalParametr();
			delete gp;
			gp = 0;
			return NULL;
		}

#endif

		return hwnd;
	}
	catch (...)
	{
		return NULL;
	}
}

void __stdcall ListCloseWindow(HWND ListWin)
{
	//v kompilyacii DEBUG programma rabotala bez oshibok
	//a v RELEASE bez DEBUG informacii inogda poyavlyalis' oshibki
	//prichini oshibki neizvestni
	//problema reshena konstrukciey try-catch
	try
	{
		GlobalParametr* gp = (GlobalParametr*)::GetWindowLongPtr(ListWin, 0);
		HWND hParentWnd     = 0;
		int  QuickView      = gp->TypeQuickView;
		int  SaveWindowSize = gp->sSettings.sSettingsWindow.SaveWindowSize;
		int  LeftPos        = 0;
		int  TopPos         = 0;
		int  Width          = 0;
		int  Height         = 0;

		//ustanavlivaem sobitie okonchaniya rabochego potoka
		//chtob on zavershilsya esli eshe vipolnyaetsya
		::SetEvent(gp->hEventEnd);

		//esli ustanovlena opciya zapominat' zakladku i zakladka bila izmenna pol'zovatelem
		if ((gp->sSettings.sSettingsWindow.SaveLastTab) && (gp->ChangeSelTab))
		{
			//fayl nastroek ArchView.ini
			MIniFile fINI;
			fINI.SetFile(gp->pIniFile);
			fINI.WriteKey(gp->sSettings.SectionsINI.SectWindow, "LastTabNumber", gp->Tabs[gp->CurTabId]);
		}
 
		//esli ispol'zuetsya opciya zapominat' razmer okna
		//tut sohranim razmer okna
		if ((!QuickView) && (SaveWindowSize))
		{
			//fayl nastroek ArchView.ini
			MIniFile fINI;
			RECT     rcClient;
			fINI.SetFile(gp->pIniFile);
			//sohranim razmeri
			hParentWnd = ::GetParent(ListWin);
			::GetWindowRect(hParentWnd, &rcClient);
			LeftPos    = rcClient.left;
			TopPos     = rcClient.top;
			Width      = gp->OriginalWidth;
			Height     = gp->OriginalHeight;
			fINI.WriteKey(gp->sSettings.SectionsINI.SectWindow, "WidthWindow",  rcClient.right  - rcClient.left);
			fINI.WriteKey(gp->sSettings.SectionsINI.SectWindow, "HeightWindow", rcClient.bottom - rcClient.top);
		}

		//proveryem/jdem okonchaniya rabochego processa
		DWORD ExitCode;
		if (GetExitCodeThread(gp->hThread, &ExitCode))
		{
			while(ExitCode == STILL_ACTIVE)
			{
				//pri neudache prodoljaem zavershenie dal'she
				if(!GetExitCodeThread(gp->hThread, &ExitCode)) break;
			}
		}

		gp->ClearGlobalParametr();
		delete gp;
		gp = 0;
		//otchistim parametr i v okne
		::SetWindowLongPtr(ListWin, 0, (long)0);

		::DestroyWindow(ListWin);
		::UnregisterClass("TMainClassWindow", hInst);

		//esli ispol'zuetsya opciya zapominat' razmer okna
		//tut vosstanovim stariy razmer okna listera
		if ((!QuickView) && (SaveWindowSize))
		{
			//vosstanovim razmeri
			::SetWindowPos(hParentWnd, HWND_TOP,
				LeftPos, TopPos, Width, Height, SWP_DRAWFRAME);
		}
	}
	catch (...) {;}
}

void __stdcall ListGetDetectString(char* DetectString, int maxlen)
{
	//v kompilyacii DEBUG programma rabotala bez oshibok
	//a v RELEASE bez DEBUG informacii inogda poyavlyalis' oshibki
	//prichini oshibki neizvestni
	//problema reshena konstrukciey try-catch
	try
	{
		//poluchim put' k logfile
		char  pIniFile[MAX_PATH]   = {NULL};
		char* Offset               = 0;
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
			strncpy(pIniFile, PluginPath, len);
			pIniFile[len] = '\0';
			strcat(pIniFile, ArchView_ini);

			MIniFile fINI;		//dlya obrasheniya k faylu nastroek (file ArchView.ini)
			MIniFile fwincmdINI;//dlya obrasheniya k faylu nastroek (file wincmd.ini)
			fINI.SetFile(pIniFile);
			char SectMain[] = "Main";
			//posmotrim v nastroykah nadoli sizdavat' DetectString
			if (fINI.ReadKey(SectMain, "SetDetectString", 0))
			{
				char SectArchives     [] = "Archives";
				char SectExtensions   [] = "Extensions";
				char SectWCXPluginsTC [] = "WCXPluginsTC";
				char SectPackerPlugins[] = "PackerPlugins";
				char SectWCXExtensions[] = "WCXExtensions";

				int  UseWCXPluginsTC = fINI.ReadKey(SectMain, "UseWCXPluginsTC", 1);
				int  UseWincmdIniWCX = fINI.ReadKey(SectMain, "UseWincmdIniWCX", 0);
				char PathWincmdIni[MAX_LOADSTRING] = {NULL};
				fINI.ReadKey(SectMain, "PathWincmdIni", "%COMMANDER_PATH%\\wincmd.ini", PathWincmdIni, MAX_LOADSTRING);

				MListStrStr WCXPluginsTC;
				MListStrStr WCXExtensions;

				//chtenie wcx-plugins (esli ispol'zuem wcx-plugins)
				if (UseWCXPluginsTC)
				{
					//iz sekcii [PackerPlugins] wincmd.ini
					if (UseWincmdIniWCX)
					{
						char pwincmdINI[MAX_PATH] = {NULL};

						//chtob put' bil real'nim bez vremennih okrujeniya
						if(!::ExpandEnvironmentStrings(PathWincmdIni, pwincmdINI, MAX_PATH))
							strncpy(pwincmdINI, PathWincmdIni, MAX_PATH);
						fwincmdINI.SetFile(pwincmdINI);

						fwincmdINI.ReadSection(SectPackerPlugins, WCXPluginsTC, MINI_STANDART_ALL_VALUE);
					}
					//iz sekcii [WCXPluginsTC] ArchView.ini
					else
						fINI.ReadSection(SectWCXPluginsTC, WCXPluginsTC, MINI_STANDART_ALL_VALUE);
				}

				//chtenie rasshireniy dlya wcx-plugins (esli ispol'zuem wcx-plugins)
				if (UseWCXPluginsTC)
				{
					char ValueKey[MAX_LOADSTRING] = {NULL};
					NodeStrStr* Node = WCXPluginsTC.GetHeadListStrStr();
					while (Node)
					{
						fINI.ReadKey(SectWCXExtensions, Node->Name, "", ValueKey, MAX_LOADSTRING);
						WCXExtensions.AddNode(Node->Name, ValueKey);
						Node = Node->Next;
					}
				}

				//chitaem nastroyki o tom kakie arhivi razbiraem
				//kakie im zaregestrirovani rasshireniya
				//+rasshireniya dlya wcx-plugins
				//i sostavlyaem sootvetstvuyushuyu DetectString
				char  BufStr[MAX_LOADSTRING] = {NULL};
				char* GenDetectString;
				GenDetectString = new char[MAX_LOADSTRING*(T_NUMBER_ARCHIVES+WCXExtensions.GetNumberNodes())];
				GenDetectString[0] = '\0';

				//ACE
				if (fINI.ReadKey(SectArchives, "Ace",       1))
				{
					fINI.ReadKey(SectExtensions, "ExtAce",       "ace",      BufStr, MAX_LOADSTRING);
					strupr(BufStr);
					AddToGenDetectString(GenDetectString, BufStr);
				}
				//ARC/PAK
				if (fINI.ReadKey(SectArchives, "ArcPak",    1))
				{
					fINI.ReadKey(SectExtensions, "ExtArcPak",    "arc pak",  BufStr, MAX_LOADSTRING);
					strupr(BufStr);
					AddToGenDetectString(GenDetectString, BufStr);
				}
				//ARJ
				if (fINI.ReadKey(SectArchives, "Arj",       1))
				{
					fINI.ReadKey(SectExtensions, "ExtArj",       "arj",      BufStr, MAX_LOADSTRING);
					strupr(BufStr);
					AddToGenDetectString(GenDetectString, BufStr);
				}
				//BH
				if (fINI.ReadKey(SectArchives, "Bh",        1))
				{
					fINI.ReadKey(SectExtensions, "ExtBh",        "bh",       BufStr, MAX_LOADSTRING);
					strupr(BufStr);
					AddToGenDetectString(GenDetectString, BufStr);
				}
				//BZIP2/TBZ2
				if (fINI.ReadKey(SectArchives, "BZip2Tbz2", 1))
				{
					fINI.ReadKey(SectExtensions, "ExtBZip2Tbz2", "bz2 tbz2", BufStr, MAX_LOADSTRING);
					strupr(BufStr);
					AddToGenDetectString(GenDetectString, BufStr);
				}
				//CAB
				if (fINI.ReadKey(SectArchives, "Cab",       1))
				{
					fINI.ReadKey(SectExtensions, "ExtCab",       "cab",      BufStr, MAX_LOADSTRING);
					strupr(BufStr);
					AddToGenDetectString(GenDetectString, BufStr);
				}
				//CPIO
				if (fINI.ReadKey(SectArchives, "Cpio",      1))
				{
					fINI.ReadKey(SectExtensions, "ExtCpio",      "cpio",     BufStr, MAX_LOADSTRING);
					strupr(BufStr);
					AddToGenDetectString(GenDetectString, BufStr);
				}
				//DWC
				if (fINI.ReadKey(SectArchives, "Dwc",       1))
				{
					fINI.ReadKey(SectExtensions, "ExtDwc",       "dwc",      BufStr, MAX_LOADSTRING);
					strupr(BufStr);
					AddToGenDetectString(GenDetectString, BufStr);
				}
				//GZIP/TGZ
				if (fINI.ReadKey(SectArchives, "GZipTgz",   1))
				{
					fINI.ReadKey(SectExtensions, "ExtGZipTgz",   "gz tgz",   BufStr, MAX_LOADSTRING);
					strupr(BufStr);
					AddToGenDetectString(GenDetectString, BufStr);
				}
				//HA
				if (fINI.ReadKey(SectArchives, "Ha",        1))
				{
					fINI.ReadKey(SectExtensions, "ExtHa",        "ha",       BufStr, MAX_LOADSTRING);
					strupr(BufStr);
					AddToGenDetectString(GenDetectString, BufStr);
				}
				//LHA/LZH
				if (fINI.ReadKey(SectArchives, "LhaLzh",    1))
				{
					fINI.ReadKey(SectExtensions, "ExtLhaLzh",    "lha lzh",  BufStr, MAX_LOADSTRING);
					strupr(BufStr);
					AddToGenDetectString(GenDetectString, BufStr);
				}
				//PPM
				if (fINI.ReadKey(SectArchives, "Ppm",       1))
				{
					fINI.ReadKey(SectExtensions, "ExtPpm",       "ppm",      BufStr, MAX_LOADSTRING);
					strupr(BufStr);
					AddToGenDetectString(GenDetectString, BufStr);
				}
				//RAR
				if (fINI.ReadKey(SectArchives, "Rar",       1))
				{
					fINI.ReadKey(SectExtensions, "ExtRar",       "rar",      BufStr, MAX_LOADSTRING);
					strupr(BufStr);
					AddToGenDetectString(GenDetectString, BufStr);
				}
				//SQZ
				if (fINI.ReadKey(SectArchives, "Sqz",       1))
				{
					fINI.ReadKey(SectExtensions, "ExtSqz",       "sqz",      BufStr, MAX_LOADSTRING);
					strupr(BufStr);
					AddToGenDetectString(GenDetectString, BufStr);
				}
				//TAR
				if (fINI.ReadKey(SectArchives, "Tar",       1))
				{
					fINI.ReadKey(SectExtensions, "ExtTar",       "tar",      BufStr, MAX_LOADSTRING);
					strupr(BufStr);
					AddToGenDetectString(GenDetectString, BufStr);
				}
				//Z
				if (fINI.ReadKey(SectArchives, "Z",         1))
				{
					fINI.ReadKey(SectExtensions, "ExtZ",         "z",        BufStr, MAX_LOADSTRING);
					strupr(BufStr);
					AddToGenDetectString(GenDetectString, BufStr);
				}
				//ZIP/JAR
				if (fINI.ReadKey(SectArchives, "ZipJar",    1))
				{
					fINI.ReadKey(SectExtensions, "ExtZipJar",    "zip jar",  BufStr, MAX_LOADSTRING);
					strupr(BufStr);
					AddToGenDetectString(GenDetectString, BufStr);
				}
				//ZOO
				if (fINI.ReadKey(SectArchives, "Zoo",       1))
				{
					fINI.ReadKey(SectExtensions, "ExtZoo",       "zoo",      BufStr, MAX_LOADSTRING);
					strupr(BufStr);
					AddToGenDetectString(GenDetectString, BufStr);
				}
				//rasshireniya wcx-plugins
				NodeStrStr* Node = WCXExtensions.GetHeadListStrStr();
				while (Node)
				{
					if (Node->Value)
					{
						strupr(Node->Value);
						AddToGenDetectString(GenDetectString, Node->Value);
					}
					Node = Node->Next;
				}

				//kopiruem nash sgenerirovanniy DetectString
				len = strlen(GenDetectString);
				if (len)
				{
					//uberem posledniy simvol ILI = "|"
					GenDetectString[len-1] = '\0';
					len--;

					if (len > maxlen)
					{
						strncpy(DetectString, GenDetectString, maxlen);
						DetectString[maxlen] = '\0';
					}
					else
						strcpy(DetectString, GenDetectString);
				}

				//osvobojdaem
				if(GenDetectString) { delete[] GenDetectString; GenDetectString = 0; }
			}
		}
	}
	catch (...) {;}
}

int __stdcall ListSendCommand(HWND ListWin, int Command, int Parameter)
{
	//dlya raboti s parametrami dannoy kopii plagina
	GlobalParametr* gp     = 0;
	try { gp = (GlobalParametr*)::GetWindowLongPtr(ListWin, 0); }
	catch (...) { gp = 0; }

	RECT  rcClient    = {0};
	DWORD WindowStyle = 0;
	BOOL  RESETFOCUS  = FALSE;

	switch (Command)
	{
	case lc_newparams:
		if ((Parameter & lcp_wraptext) && (!gp->ModeWordWrap))
		{
			gp->ModeWordWrap = 1;

			if (gp->tComment.hWndRichEdit)
			{
				//proverim focussirovku okna
				if (gp->tComment.hWndRichEdit == ::GetFocus()) RESETFOCUS = TRUE;
				else RESETFOCUS = FALSE;

				//vernem obrabotchik sobitiy
				if (gp->tComment.REWndProc)
				{
					::SetWindowLongPtr(gp->tComment.hWndRichEdit, GWLP_WNDPROC, gp->tComment.REWndProc);
					gp->tComment.REWndProc = 0;
				}
				::DestroyWindow(gp->tComment.hWndRichEdit);
				gp->tComment.hWndRichEdit = 0;

				::GetClientRect(gp->tComment.hWndComment, &rcClient);
				WindowStyle = WS_CHILD | WS_VISIBLE | WS_BORDER | WS_HSCROLL | WS_VSCROLL | ES_READONLY | ES_MULTILINE | WS_TABSTOP;
				gp->tComment.hWndRichEdit = ::CreateWindow(RichEditClass, "", 	WindowStyle,
					rcClient.left, rcClient.top,
					rcClient.right - rcClient.left,
					rcClient.bottom - rcClient.top,
					gp->tComment.hWndComment, NULL, hInst, NULL);
				::SendMessage(gp->tComment.hWndRichEdit, WM_SETFONT, (WPARAM)gp->hFontN, MAKELPARAM(FALSE, 0));
				::SendMessage(gp->tComment.hWndRichEdit, WM_SETTEXT, 0, (LPARAM)gp->pTextComment);

				//podmenim obrabotchik sobitiy
				if (gp->tComment.hWndRichEdit)
				{
					gp->tComment.REWndProc = ::GetWindowLongPtr(gp->tComment.hWndRichEdit, GWLP_WNDPROC);
					::SetWindowLongPtr(gp->tComment.hWndRichEdit, GWLP_WNDPROC, (LONG_PTR)RichEditWndProc);
				}

				//esli nado vernem focus
				if (RESETFOCUS) ::SetFocus(gp->tComment.hWndRichEdit);
			}
			if (gp->tInfo.hWndRichEdit)
			{
				//proverim focussirovku okna
				if (gp->tInfo.hWndRichEdit == ::GetFocus()) RESETFOCUS = TRUE;
				else RESETFOCUS = FALSE;

				//vernem obrabotchik sobitiy
				if (gp->tInfo.REWndProc)
				{
					::SetWindowLongPtr(gp->tInfo.hWndRichEdit, GWLP_WNDPROC, gp->tInfo.REWndProc);
					gp->tInfo.REWndProc = 0;
				}
				::DestroyWindow(gp->tInfo.hWndRichEdit);
				gp->tInfo.hWndRichEdit = 0;

				WindowStyle = WS_CHILD | WS_VISIBLE | WS_BORDER | WS_HSCROLL | WS_VSCROLL | ES_READONLY | ES_MULTILINE | WS_TABSTOP;
				gp->tInfo.hWndRichEdit = ::CreateWindow(RichEditClass, "", 	WindowStyle,
					gp->CmtInfoLeft, gp->CmtInfoTop,
					gp->CmtInfoWidth, gp->CmtInfoHeight,
					gp->tInfo.hWndInfo, NULL, hInst, NULL);
				::SendMessage(gp->tInfo.hWndRichEdit, WM_SETFONT, (WPARAM)gp->hFontN, MAKELPARAM(FALSE, 0));
				::SendMessage(gp->tInfo.hWndRichEdit, WM_SETTEXT, 0, (LPARAM)gp->pTextComment);

				//podmenim obrabotchik sobitiy
				if (gp->tInfo.hWndRichEdit)
				{
					gp->tInfo.REWndProc = ::GetWindowLongPtr(gp->tInfo.hWndRichEdit, GWLP_WNDPROC);
					::SetWindowLongPtr(gp->tInfo.hWndRichEdit, GWLP_WNDPROC, (LONG_PTR)RichEditWndProc);
				}

				//esli nado vernem focus
				if (RESETFOCUS) ::SetFocus(gp->tInfo.hWndRichEdit);
			}
			return LISTPLUGIN_OK;
		}
		if (!(Parameter & lcp_wraptext) && (gp->ModeWordWrap))
		{
			gp->ModeWordWrap = 0;

			if (gp->tComment.hWndRichEdit)
			{
				//proverim focussirovku okna
				if (gp->tComment.hWndRichEdit == ::GetFocus()) RESETFOCUS = TRUE;
				else RESETFOCUS = FALSE;

				//vernem obrabotchik sobitiy
				if (gp->tComment.REWndProc)
				{
					::SetWindowLongPtr(gp->tComment.hWndRichEdit, GWLP_WNDPROC, gp->tComment.REWndProc);
					gp->tComment.REWndProc = 0;
				}
				::DestroyWindow(gp->tComment.hWndRichEdit);
				gp->tComment.hWndRichEdit = 0;

				::GetClientRect(gp->tComment.hWndComment, &rcClient);
				WindowStyle = WS_CHILD | WS_VISIBLE | WS_BORDER | WS_HSCROLL | WS_VSCROLL | ES_READONLY | ES_MULTILINE | ES_AUTOHSCROLL | ES_AUTOVSCROLL | WS_TABSTOP;
				gp->tComment.hWndRichEdit = ::CreateWindow(RichEditClass, "", 	WindowStyle,
					rcClient.left, rcClient.top,
					rcClient.right - rcClient.left,
					rcClient.bottom - rcClient.top,
					gp->tComment.hWndComment, NULL, hInst, NULL);
				::SendMessage(gp->tComment.hWndRichEdit, WM_SETFONT, (WPARAM)gp->hFontN, MAKELPARAM(FALSE, 0));
				::SendMessage(gp->tComment.hWndRichEdit, WM_SETTEXT, 0, (LPARAM)gp->pTextComment);

				//podmenim obrabotchik sobitiy
				if (gp->tComment.hWndRichEdit)
				{
					gp->tComment.REWndProc = ::GetWindowLongPtr(gp->tComment.hWndRichEdit, GWLP_WNDPROC);
					::SetWindowLongPtr(gp->tComment.hWndRichEdit, GWLP_WNDPROC, (LONG_PTR)RichEditWndProc);
				}

				//esli nado vernem focus
				if (RESETFOCUS) ::SetFocus(gp->tComment.hWndRichEdit);
			}
			if (gp->tInfo.hWndRichEdit)
			{
				//proverim focussirovku okna
				if (gp->tInfo.hWndRichEdit == ::GetFocus()) RESETFOCUS = TRUE;
				else RESETFOCUS = FALSE;

				//vernem obrabotchik sobitiy
				if (gp->tInfo.REWndProc)
				{
					::SetWindowLongPtr(gp->tInfo.hWndRichEdit, GWLP_WNDPROC, gp->tInfo.REWndProc);
					gp->tInfo.REWndProc = 0;
				}
				::DestroyWindow(gp->tInfo.hWndRichEdit);
				gp->tInfo.hWndRichEdit = 0;

				WindowStyle = WS_CHILD | WS_VISIBLE | WS_BORDER | WS_HSCROLL | WS_VSCROLL | ES_READONLY | ES_MULTILINE | ES_AUTOHSCROLL | ES_AUTOVSCROLL | WS_TABSTOP;
				gp->tInfo.hWndRichEdit = ::CreateWindow(RichEditClass, "", 	WindowStyle,
					gp->CmtInfoLeft, gp->CmtInfoTop,
					gp->CmtInfoWidth, gp->CmtInfoHeight,
					gp->tInfo.hWndInfo, NULL, hInst, NULL);
				::SendMessage(gp->tInfo.hWndRichEdit, WM_SETFONT, (WPARAM)gp->hFontN, MAKELPARAM(FALSE, 0));
				::SendMessage(gp->tInfo.hWndRichEdit, WM_SETTEXT, 0, (LPARAM)gp->pTextComment);

				//podmenim obrabotchik sobitiy
				if (gp->tInfo.hWndRichEdit)
				{
					gp->tInfo.REWndProc = ::GetWindowLongPtr(gp->tInfo.hWndRichEdit, GWLP_WNDPROC);
					::SetWindowLongPtr(gp->tInfo.hWndRichEdit, GWLP_WNDPROC, (LONG_PTR)RichEditWndProc);
				}

				//esli nado vernem focus
				if (RESETFOCUS) ::SetFocus(gp->tInfo.hWndRichEdit);
			}
			return LISTPLUGIN_OK;
		}
		return LISTPLUGIN_ERROR;
		break;
	default:
		return LISTPLUGIN_ERROR;
		break;
	}
}

void __stdcall ListSetDefaultParams(ListDefaultParamStruct*)
{
	HMODULE hLib;

	// Rich Edit Version 2.0/3.0
	if ((hLib = LoadLibrary(_T("riched20.dll"))))
	{
		RichEditClass = RICHEDIT_CLASS;
	}
	// Rich Edit Version 1.0
	else if ((hLib = LoadLibrary(_T("riched32.dll"))))
	{
		RichEditClass = RICHEDIT_CLASS10A;
	}
	else
	{
		RichEditClass = _T("");
	}
}
