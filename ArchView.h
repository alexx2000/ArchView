/////////////////////////////////////////////////////////////////////////////
//ArchView.h
//////////////////////////////////////////////////////////////////////

#ifndef T_ARCH_VIEW_H
#define T_ARCH_VIEW_H

//vspomogatel'nie funkcii
int AddToGenDetectString(char*, const char*);

//dlya sozdaniya osnovnogo okna plagina
ATOM    MyRegisterClass          ();
HWND    InitInstance             (HWND);
LRESULT CALLBACK WndProc         (HWND, UINT, WPARAM, LPARAM);
HWND    WINAPI DoCreateTabControl(HWND);

//moi obrabotshiki dlya okon
LRESULT CALLBACK RichEditWndProc (HWND, UINT, WPARAM, LPARAM);

//rabota s zakladkoy Info
int CreateTabInfo(HWND);
int DeleteTabInfo(HWND);
int ShowTabInfo  (HWND);
int HideTabInfo  (HWND);
int ResizeTabInfo(HWND);

//rabota s zakladkoy Comment
int CreateTabContext(HWND);
int DeleteTabContext(HWND);
int ShowTabContext  (HWND);
int HideTabContext  (HWND);
int ResizeTabContext(HWND);

//rabota s zakladkoy Comment
int CreateTabComment(HWND);
int DeleteTabComment(HWND);
int ShowTabComment  (HWND);
int HideTabComment  (HWND);
int ResizeTabComment(HWND);

//rabota s zakladkoy About
int CreateTabAbout(HWND);
int DeleteTabAbout(HWND);
int ShowTabAbout  (HWND);
int HideTabAbout  (HWND);
int ResizeTabAbout(HWND);

#endif //T_ARCH_VIEW_H
