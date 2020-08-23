/////////////////////////////////////////////////////////////////////////////
//TWcxPluginTC.h
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TArchive.h"
#include "WcxHead.h"

#ifndef _TWCX_PLUGIN_TC_CLASS_H
#define _TWCX_PLUGIN_TC_CLASS_H

#define WCX_MAX_LEN_COMMENT 62000	//kak dlya RAR

class TWcxPluginTC: public TArchive
{
private:
	//strukturi dlya chteniya arhiva
	tHeaderData      m_HeaderData;		//zagolovok fayla/papki
	tOpenArchiveData m_OpenArchiveData;	//zagolovok arhiva

	HMODULE LoadPlugin(LPCSTR lpLibFileName);

	//vnutrennie funkcii
	int      AnalyzeFileHead(tHeaderData&, int&);	//obrabotka zagolovka fayla/papki

	LONGLONG GetArcPointer  (HANDLE);				//opredelit' smeshenie v arhive

public:
	TWcxPluginTC(DataForArchive& dfa, LanguageMessages& langmsg, LanguageResults& langres, char* namearch = "WCXPLUGIN");
	~TWcxPluginTC() {;}

	int TestFile        (char*);	//fayl yavlyaetsya arhivom ACE
	int AnalyzeInfoOfArc(char*);	//opredelit' parametri (info) arhiva

	//const char* GetInternalName      () {return m_pInternalName;}		//vozvrat pol'zovatel'skogo imeni ispol'zuemogo plugina
	ULONGLONG   GetUnpackSizeFiles   () {return m_UnpackSizeFiles;}		//vozvrat real'nogo razmera faylov v BYTEs
	ULONGLONG   GetPackSizeFiles     () {return m_PackSizeFiles;}		//vozvrat sjatogo razmera faylov v BYTEs
	ULONGLONG   GetArchiveSize       () {return m_ArchiveSize;}			//vozvrat razmera arhiva v BYTEs
	unsigned    GetNumberFiles       () {return m_NumberFiles;}			//vozvrat kolichestvo faylov v arhive
	unsigned    GetNumberFolders     () {return m_NumberFolders;}		//vozvrat kolichestvo papok v arhive
	short       GetHostOS            () {return m_HostOS;}				//vozvrat ispolzuemaya pri arhivirovanii OS | 0x00 - MS-DOS | 0x01 - OS/2 | 0x02 - Win32 | 0x03 - Unix | 0x04 - Mac OS | 0x05 - WinNT | 0x06 - Primos | 0x07 - Apple GS | 0x08 - Atari ST | 0x09 - Vax VMS | 0x0A - Amiga | 0x0B - Next
	short       GetMethodPack        () {return m_MethodPack;}			//vozvrat metod sjatiya | 0x00 - bez szhatiya (Stored) | 0x01 - skorostnoe (Fastest) | 0x02 - bystroe (Fast) | 0x03 - obychnoe (Normal) | 0x04 - horoshee (Good) | 0x05 - maksimal'noe (Best)
	double      GetRatioArchiveSize  () {return m_RatioArchiveSize;}	//sjatie ot razmera arhiva      (%)
	double      GetRatioPackFileSize () {return m_RatioPackFileSize;}	//sjatie ot upak razmera faylov      (%)
	double      GetUnpackVersion     () {return m_UnpackVersion;}		//vozvarat versiay dlya raspakovki      (X.X)
	unsigned    GetSfxModule         () {return m_SfxModule;}			//1 - SFX (*.exe) esli 0 to net
	BOOL        GetMainComment       () {return m_MainComment;}			//TRUE - arhiv imeet kommentariy FALSE - net
	BOOL        GetFileComment       () {return m_FileComment;}			//TRUE - fayli imeyut kommentarii FALSE - net
	char*       GetTextComment       () {return m_pTextComment;}		//vernut' ukazatel' na kommentariy arhiva
};

#endif	//_TWCX_PLUGIN_TC_CLASS_H