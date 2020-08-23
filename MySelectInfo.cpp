/////////////////////////////////////////////////////////////////////////////
//MySelectInfo.cpp
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MySelectInfo.h"

/////////////////////////////////////////////////////////////////////////////
//FUNCTIONS PRIVATE
/////////////////////////////////////////////////////////////////////////////

//vozvrashaet strokovoe znachenie razmernosti
//Prefix - tekushaya razmernost'
//	0 - Byte
//	1 - KByte
//	2 - MByte
//	3 - GByte
//	4 - TByte
//lang   - structura yazika programmi
char* MySelectInfo::GetUnitSizeStr(UnitSize Prefix, LanguageOther& lang)
{
	switch (Prefix)
	{
	case T_BYTE:  return lang.Byte;    break;
	case T_KBYTE: return lang.KByte;   break;
	case T_MBYTE: return lang.MByte;   break;
	case T_GBYTE: return lang.GByte;   break;
	case T_TBYTE: return lang.TByte;   break;
	default:      return lang.Unknown; break;
	}
}

//perevod chisa v stroku s razdeleniem po 3 chisla
//buffer - stroka resul'tat
//number - chislo dlya perevoda
int MySelectInfo::NumberSeparatorString(char* buffer, ULONGLONG number)
{
	int step = 0;
	char str1[MAX_LOADSTRING] = {NULL};
	char str2[MAX_LOADSTRING] = {NULL};
	char str3[MAX_LOADSTRING] = {NULL};
	ULONGLONG temp;
	while (number > 1000)
	{
		temp = number % 1000;
		number /= 1000;
		sprintf(str1, "%.3d", temp);
		if (!step)
		{
			sprintf(str3, "%s", str1);
			step++;
		}
		else
			sprintf(str3, "%s\'%s", str1, str2);
		sprintf(str2, "%s", str3);
	}
	sprintf(str1, "%d", number);
	if (!step) sprintf(buffer, "%s", str1);
	else sprintf(buffer, "%s\'%s", str1, str2);
	return 1;
}

//perevod chisa v stroku s razdeleniem po 3 chisla i razmernost'yu
//buffer - stroka resul'tat
//number - chislo dlya perevoda
//Prefix - tekushaya razmernost'
//	0 - Byte
//	1 - KByte
//	2 - MByte
//	3 - GByte
//	4 - TByte
//lang   - structura yazika programmi
int MySelectInfo::NumberSeparatorStringUnitSize(char* buffer, ULONGLONG number, UnitSize Prefix, LanguageOther& lang)
{
	//stroka - nasha edinica izmereniya informacii
	char* UnitSizeStr = GetUnitSizeStr(Prefix, lang);

	//razbivka chisla po 3 cifri
	char str[MAX_LOADSTRING] = {NULL};
	NumberSeparatorString(str, number);
	//chislo s razmernost'yu (primer 1'000'000 Kb)
	sprintf(buffer, "%s %s", str, UnitSizeStr);

	return 1;
}

//perevod chisa v stroku s plavayushim razmerom b, Kb, Mb, Gb, Tb
//buffer - stroka resul'tat
//number - chislo dlya perevoda
//Prefix - tekushaya razmernost'
//	0 - Byte
//	1 - KByte
//	2 - MByte
//	3 - GByte
//	4 - TByte
//lang   - structura yazika programmi
int MySelectInfo::NumberFloatStringUnitSize(char* buffer, ULONGLONG number, UnitSize Prefix, LanguageOther& lang)
{
	double FloatNumber = (double)(LONGLONG)number;
	char*  UnitSizeStr = 0;

	//perebor razmernostey
	while (FloatNumber > 1000)
	{
		FloatNumber /= 1024;
		Prefix = (UnitSize)((int)Prefix + 1);
	}

	//stroka - nasha edinica izmereniya informacii
	UnitSizeStr = GetUnitSizeStr(Prefix, lang);

	//teper' mojno sformirovat' chislo
	sprintf(buffer, "%.1f %s", FloatNumber, UnitSizeStr);

	return 1;
}

//dlya archiva ACE
int MySelectInfo::SelectInfoAce()
{
	//dlya uprosheniya obrasheniya k dannim sobrannim v odnom meste
	SettingsMain&       sSettingsMain       = m_pSettings->sSettingsMain;
	SettingsParameters& sSettingsParameters = m_pSettings->sSettingsParameters;
	LanguageTabInfo&    sLanguageTabInfo    = m_pLanguage->sLanguageTabInfo;
	LanguageOS&         sLanguageOS         = m_pLanguage->sLanguageOS;
	LanguagePack&       sLanguagePack       = m_pLanguage->sLanguagePack;
	LanguageOther&      sLanguageOther      = m_pLanguage->sLanguageOther;
	MListParametrs&     ListParam           = *m_pListParam;

	TAce* obj_ace = (TAce*)m_pArchData;

	ULONGLONG UnpackSizeFiles   = obj_ace->GetUnpackSizeFiles();
	ULONGLONG PackSizeFiles     = obj_ace->GetPackSizeFiles();
	ULONGLONG ArchiveSize       = obj_ace->GetArchiveSize();
	unsigned  NumberFiles       = obj_ace->GetNumberFiles();
	unsigned  NumberFolders     = obj_ace->GetNumberFolders();
	char*     pDateCreated      = obj_ace->GetDateCreated();
	char*     pTimeCreated      = obj_ace->GetTimeCreated();
	unsigned  DictionarySize    = obj_ace->GetDictionarySize();
	short     HostOS            = obj_ace->GetHostOS();
	short     MethodPack        = obj_ace->GetMethodPack();
	double    RatioArchiveSize  = obj_ace->GetRatioArchiveSize();
	double    RatioPackFileSize = obj_ace->GetRatioPackFileSize();
	double    PackVersion       = obj_ace->GetPackVersion();
	double    UnpackVersion     = obj_ace->GetUnpackVersion();
	unsigned  SfxModule         = obj_ace->GetSfxModule();
	unsigned  RecoveryRecord    = obj_ace->GetRecoveryRecord();
	BOOL      Solid             = obj_ace->GetSolid();
	BOOL      MultiArc          = obj_ace->GetMultiArc();
	unsigned  VolumeNumber      = obj_ace->GetVolumeNumber();
	BOOL      MainComment       = obj_ace->GetMainComment();
	BOOL      FileComment       = obj_ace->GetFileComment();
	BOOL      Password          = obj_ace->GetPassword();
	BOOL      ArchiveLock       = obj_ace->GetArchiveLock();
	BOOL      AuthVerification  = obj_ace->GetAuthVerification();
	char*     pAuthVerification = obj_ace->GetStringAuthVerification();
	char*     pTextComment      = obj_ace->GetTextComment();
	double    TestTime          = m_TestTime;
	double    AnalyzeTime       = obj_ace->GetAnalyzeTime();
	double    AllTime           = m_TestTime + AnalyzeTime;

	if (((int)RatioPackFileSize == 0) && (RatioPackFileSize > 0)) *m_pRatio1 = 1;
	else *m_pRatio1 = (int)RatioPackFileSize;
	if (((int)RatioArchiveSize == 0) && (RatioArchiveSize > 0)) *m_pRatio2 = 1;
	else *m_pRatio2 = (int)RatioArchiveSize;

	char BufStr[MAX_LOADSTRING] = {NULL};

	//zagolovok
	char* _ARCH = m_pLanguage->sLanguageArchives.Ace;
	char* _SFX  = m_pLanguage->sLanguageArchives.Sfx;
	if ((Solid) && (SfxModule)) sprintf(BufStr, "%s %s %s %s", sLanguageTabInfo.Solid, _SFX, _ARCH, sLanguageOther.Archive);
	else
		if (Solid) sprintf(BufStr, "%s %s %s", sLanguageTabInfo.Solid, _ARCH, sLanguageOther.Archive);
		else
			if (SfxModule) sprintf(BufStr, "%s %s %s", _SFX, _ARCH, sLanguageOther.Archive);
			else sprintf(BufStr, "%s %s", _ARCH, sLanguageOther.Archive);
	ListParam.AddParametr("", BufStr);

	//neupakovanniy razmer
	if ((sSettingsMain.AllParametrs) || (UnpackSizeFiles))
	{
		if (!sSettingsMain.FloatSize) NumberSeparatorStringUnitSize(BufStr, UnpackSizeFiles, T_BYTE, sLanguageOther);
		else NumberFloatStringUnitSize(BufStr, UnpackSizeFiles, T_BYTE, sLanguageOther);
		if (sSettingsParameters.UnpackSizeFiles)
			ListParam.AddParametr(sLanguageTabInfo.UnpackSizeFiles, BufStr);
	}

	//upakovanniy razmer
	if ((sSettingsMain.AllParametrs) || (PackSizeFiles))
	{
		if (!sSettingsMain.FloatSize) NumberSeparatorStringUnitSize(BufStr, PackSizeFiles, T_BYTE, sLanguageOther);
		else NumberFloatStringUnitSize(BufStr, PackSizeFiles, T_BYTE, sLanguageOther);
		if (sSettingsParameters.PackSizeFiles)
			ListParam.AddParametr(sLanguageTabInfo.PackSizeFiles, BufStr);
	}

	//razmer arhiva
	if ((sSettingsMain.AllParametrs) || (ArchiveSize))
	{
		if (!sSettingsMain.FloatSize) NumberSeparatorStringUnitSize(BufStr, ArchiveSize, T_BYTE, sLanguageOther);
		else NumberFloatStringUnitSize(BufStr, ArchiveSize, T_BYTE, sLanguageOther);
		if (sSettingsParameters.ArchiveSize)
			ListParam.AddParametr(sLanguageTabInfo.ArchiveSize, BufStr);
	}

	//kolichestvo faylov
	if ((sSettingsMain.AllParametrs) || (NumberFiles))
	{
		if (NumberFiles) NumberSeparatorString(BufStr, NumberFiles);
		else sprintf(BufStr, "%s", sLanguageOther.No);
		if (sSettingsParameters.NumberFiles)
			ListParam.AddParametr(sLanguageTabInfo.NumberFiles, BufStr);
	}

	//kolichestvo papok
	if ((sSettingsMain.AllParametrs) || (NumberFolders))
	{
		if (NumberFolders) NumberSeparatorString(BufStr, NumberFolders);
		else sprintf(BufStr, "%s", sLanguageOther.No);
		if (sSettingsParameters.NumberFolders)
			ListParam.AddParametr(sLanguageTabInfo.NumberFolders, BufStr);
	}

	//data sozdaniya
	if (sSettingsParameters.DateCreated)
		ListParam.AddParametr(sLanguageTabInfo.DateCreated, pDateCreated);

	//vremya sozdaniya
	if (sSettingsParameters.TimeCreated)
		ListParam.AddParametr(sLanguageTabInfo.TimeCreated, pTimeCreated);

	//razdelitel'
	ListParam.AddParametr("", "");

	//razmer slovarya
	if ((sSettingsMain.AllParametrs) || (DictionarySize))
	{
		if (!sSettingsMain.FloatSize) NumberSeparatorStringUnitSize(BufStr, DictionarySize, T_KBYTE, sLanguageOther);
		else NumberFloatStringUnitSize(BufStr, DictionarySize, T_KBYTE, sLanguageOther);
		if (sSettingsParameters.DictionarySize)
			ListParam.AddParametr(sLanguageTabInfo.DictionarySize, BufStr);
	}

	//host OS
	if (sSettingsParameters.HostOS)
		switch (HostOS)
		{
		case 0x00: ListParam.AddParametr(sLanguageTabInfo.HostOS, sLanguageOS.DOS);        break;
		case 0x01: ListParam.AddParametr(sLanguageTabInfo.HostOS, sLanguageOS.OS2);        break;
		case 0x02: ListParam.AddParametr(sLanguageTabInfo.HostOS, sLanguageOS.Win32);      break;
		case 0x03: ListParam.AddParametr(sLanguageTabInfo.HostOS, sLanguageOS.Unix);       break;
		case 0x04: ListParam.AddParametr(sLanguageTabInfo.HostOS, sLanguageOS.MacOS);      break;
		case 0x05: ListParam.AddParametr(sLanguageTabInfo.HostOS, sLanguageOS.WinNT);      break;
		case 0x06: ListParam.AddParametr(sLanguageTabInfo.HostOS, sLanguageOS.Primos);     break;
		case 0x07: ListParam.AddParametr(sLanguageTabInfo.HostOS, sLanguageOS.AppleGS);    break;
		case 0x08: ListParam.AddParametr(sLanguageTabInfo.HostOS, sLanguageOS.AtariST);    break;
		case 0x09: ListParam.AddParametr(sLanguageTabInfo.HostOS, sLanguageOS.VaxVMS);     break;
		case 0x0A: ListParam.AddParametr(sLanguageTabInfo.HostOS, sLanguageOS.Amiga);      break;
		case 0x0B: ListParam.AddParametr(sLanguageTabInfo.HostOS, sLanguageOS.Next);       break;
		default:   ListParam.AddParametr(sLanguageTabInfo.HostOS, sLanguageOther.Unknown); break;
		}

	//metod upakovki
	if (sSettingsParameters.MethodPack)
		switch (MethodPack)
		{
		case 0x00: ListParam.AddParametr(sLanguageTabInfo.MethodPack, sLanguagePack.Stored);   break;
		case 0x01: ListParam.AddParametr(sLanguageTabInfo.MethodPack, sLanguagePack.Fastest);  break;
		case 0x02: ListParam.AddParametr(sLanguageTabInfo.MethodPack, sLanguagePack.Fast);     break;
		case 0x03: ListParam.AddParametr(sLanguageTabInfo.MethodPack, sLanguagePack.Normal);   break;
		case 0x04: ListParam.AddParametr(sLanguageTabInfo.MethodPack, sLanguagePack.Good);     break;
		case 0x05: ListParam.AddParametr(sLanguageTabInfo.MethodPack, sLanguagePack.Best);     break;
		default:   ListParam.AddParametr(sLanguageTabInfo.MethodPack, sLanguageOther.Unknown); break;
		}

	//sjatie ot rzmera arhiva
	sprintf(BufStr, "%.1f %s", RatioArchiveSize, sLanguageOther.Percent);
	if (sSettingsParameters.RatioArchiveSize)
		ListParam.AddParametr(sLanguageTabInfo.RatioArchiveSize, BufStr);

	//sjatie ot upakovannogo razmera
	sprintf(BufStr, "%.1f %s", RatioPackFileSize, sLanguageOther.Percent);
	if (sSettingsParameters.RatioPackFileSize)
		ListParam.AddParametr(sLanguageTabInfo.RatioPackFileSize, BufStr);

	//versiya dlya upakovki
	if ((sSettingsMain.AllParametrs) || (PackVersion != UnpackVersion))
	{
		sprintf(BufStr, "%.1f", PackVersion);
		if (sSettingsParameters.PackVersion)
			ListParam.AddParametr(sLanguageTabInfo.PackVersion, BufStr);
	}

	//versiya dlya raspakovki
	sprintf(BufStr, "%.1f", UnpackVersion);
	if (sSettingsParameters.UnpackVersion)
		ListParam.AddParametr(sLanguageTabInfo.UnpackVersion, BufStr);

	//razdelitel'
	ListParam.AddParametr("", "");

	//SFX modul'
	if ((sSettingsMain.AllParametrs) || (SfxModule))
	{
		if (sSettingsParameters.SfxModule)
			if (!SfxModule) ListParam.AddParametr(sLanguageTabInfo.SfxModule, sLanguageOther.No);
			else
			{
				if (!sSettingsMain.FloatSize) NumberSeparatorStringUnitSize(BufStr, SfxModule, T_BYTE, sLanguageOther);
				else NumberFloatStringUnitSize(BufStr, SfxModule, T_BYTE, sLanguageOther);
				ListParam.AddParametr(sLanguageTabInfo.SfxModule, BufStr);
			}
	}

	//zapis' dlya vosstanovleniya
	if ((sSettingsMain.AllParametrs) || (RecoveryRecord))
	{
		if (sSettingsParameters.RecoveryRecord)
			if (!RecoveryRecord) ListParam.AddParametr(sLanguageTabInfo.RecoveryRecord, sLanguageOther.No);
			else
			{
				if (!sSettingsMain.FloatSize) NumberSeparatorStringUnitSize(BufStr, RecoveryRecord, T_BYTE, sLanguageOther);
				else NumberFloatStringUnitSize(BufStr, RecoveryRecord, T_BYTE, sLanguageOther);
				ListParam.AddParametr(sLanguageTabInfo.RecoveryRecord, BufStr);
			}
	}

	//neprerivnost'
	if ((sSettingsMain.AllParametrs) || (Solid))
	{
		if (sSettingsParameters.Solid)
			if (!Solid) ListParam.AddParametr(sLanguageTabInfo.Solid, sLanguageOther.No);
			else ListParam.AddParametr(sLanguageTabInfo.Solid, sLanguageOther.Yes);
	}

	//mnogotomnost'
	if ((sSettingsMain.AllParametrs) || (MultiArc))
	{
		if (sSettingsParameters.MultiArc)
			if (!MultiArc) ListParam.AddParametr(sLanguageTabInfo.MultiArc, sLanguageOther.No);
			else
			{
				ListParam.AddParametr(sLanguageTabInfo.MultiArc, sLanguageOther.Yes);
				//nomer toma
				if (VolumeNumber)
				{
					sprintf(BufStr, "%s %d", sLanguageOther.Number, VolumeNumber);
					ListParam.AddParametr(sLanguageTabInfo.NumberVolume, BufStr);
				}
			}
	}

	//kommentariy arhiva
	if ((sSettingsMain.AllParametrs) || (MainComment))
	{
		if (sSettingsParameters.MainComment)
			if (!MainComment) ListParam.AddParametr(sLanguageTabInfo.MainComment, sLanguageOther.No);
			else ListParam.AddParametr(sLanguageTabInfo.MainComment, sLanguageOther.Yes);
	}
	//tekst kommentariya
	if (pTextComment) *m_ppTextComment = pTextComment;

	//kommentarii faylov
	if ((sSettingsMain.AllParametrs) || (FileComment))
	{
		if (sSettingsParameters.FileComment)
			if (!FileComment) ListParam.AddParametr(sLanguageTabInfo.FileComment, sLanguageOther.No);
			else ListParam.AddParametr(sLanguageTabInfo.FileComment, sLanguageOther.Yes);
	}

	//parol'
	if ((sSettingsMain.AllParametrs) || (Password))
	{
		if (sSettingsParameters.Password)
			if (!Password) ListParam.AddParametr(sLanguageTabInfo.Password, sLanguageOther.No);
			else ListParam.AddParametr(sLanguageTabInfo.Password, sLanguageOther.Yes);
	}

	//arhiv zablokirovam
	if ((sSettingsMain.AllParametrs) || (ArchiveLock))
	{
		if (sSettingsParameters.ArchiveLock)
			if (!ArchiveLock) ListParam.AddParametr(sLanguageTabInfo.ArchiveLock, sLanguageOther.No);
			else ListParam.AddParametr(sLanguageTabInfo.ArchiveLock, sLanguageOther.Yes);
	}

	//elektronnaya podpis'
	if ((sSettingsMain.AllParametrs) || (AuthVerification))
	{
		if (sSettingsParameters.AuthVerification)
			if (!AuthVerification) ListParam.AddParametr(sLanguageTabInfo.AuthVerification, sLanguageOther.No);
			else
			{
				ListParam.AddParametr(sLanguageTabInfo.AuthVerification, sLanguageOther.Yes);
				if (pAuthVerification) ListParam.AddParametr("", pAuthVerification, 1);
			}
	}

	//razdelitel'
	ListParam.AddParametr("", "");

	sprintf(BufStr, "%.2f %s", TestTime, sLanguageOther.Second);
	if (sSettingsParameters.TestTime)
		ListParam.AddParametr(sLanguageTabInfo.TestTime, BufStr);

	sprintf(BufStr, "%.2f %s", AnalyzeTime, sLanguageOther.Second);
	if (sSettingsParameters.AnalyzeTime)
		ListParam.AddParametr(sLanguageTabInfo.AnalyzeTime, BufStr);

	sprintf(BufStr, "%.2f %s", AllTime, sLanguageOther.Second);
	if (sSettingsParameters.AllTime)
		ListParam.AddParametr(sLanguageTabInfo.AllTime, BufStr);

	return 1;
}

//dlya archiva ARC/PAK
int MySelectInfo::SelectInfoArcPak()
{
	//dlya uprosheniya obrasheniya k dannim sobrannim v odnom meste
	SettingsMain&       sSettingsMain       = m_pSettings->sSettingsMain;
	SettingsParameters& sSettingsParameters = m_pSettings->sSettingsParameters;
	LanguageTabInfo&    sLanguageTabInfo    = m_pLanguage->sLanguageTabInfo;
	LanguageOS&         sLanguageOS         = m_pLanguage->sLanguageOS;
	LanguagePack&       sLanguagePack       = m_pLanguage->sLanguagePack;
	LanguageOther&      sLanguageOther      = m_pLanguage->sLanguageOther;
	MListParametrs&     ListParam           = *m_pListParam;

	TArcPak* obj_arcpak = (TArcPak*)m_pArchData;

	ULONGLONG UnpackSizeFiles   = obj_arcpak->GetUnpackSizeFiles();
	ULONGLONG PackSizeFiles     = obj_arcpak->GetPackSizeFiles();
	ULONGLONG ArchiveSize       = obj_arcpak->GetArchiveSize();
	unsigned  NumberFiles       = obj_arcpak->GetNumberFiles();
	unsigned  NumberFolders     = obj_arcpak->GetNumberFolders();
	short     MethodPack        = obj_arcpak->GetMethodPack();
	double    RatioArchiveSize  = obj_arcpak->GetRatioArchiveSize();
	double    RatioPackFileSize = obj_arcpak->GetRatioPackFileSize();
	BOOL      MainComment       = obj_arcpak->GetMainComment();
	BOOL      ArchiveType       = obj_arcpak->GetArchiveType();
	char*     pTextComment      = obj_arcpak->GetTextComment();
	double    TestTime          = m_TestTime;
	double    AnalyzeTime       = obj_arcpak->GetAnalyzeTime();
	double    AllTime           = m_TestTime + AnalyzeTime;

	if (((int)RatioPackFileSize == 0) && (RatioPackFileSize > 0)) *m_pRatio1 = 1;
	else *m_pRatio1 = (int)RatioPackFileSize;
	if (((int)RatioArchiveSize == 0) && (RatioArchiveSize > 0)) *m_pRatio2 = 1;
	else *m_pRatio2 = (int)RatioArchiveSize;

	char BufStr[MAX_LOADSTRING] = {NULL};

	//zagolovok
	char* _ARCH1 = m_pLanguage->sLanguageArchives.Arc;
	char* _ARCH2 = m_pLanguage->sLanguageArchives.Pak;
	if (!ArchiveType) sprintf(BufStr, "%s %s", _ARCH1, sLanguageOther.Archive);
	else sprintf(BufStr, "%s %s", _ARCH2, sLanguageOther.Archive);
	ListParam.AddParametr("", BufStr);

	//neupakovanniy razmer
	if ((sSettingsMain.AllParametrs) || (UnpackSizeFiles))
	{
		if (!sSettingsMain.FloatSize) NumberSeparatorStringUnitSize(BufStr, UnpackSizeFiles, T_BYTE, sLanguageOther);
		else NumberFloatStringUnitSize(BufStr, UnpackSizeFiles, T_BYTE, sLanguageOther);
		if (sSettingsParameters.UnpackSizeFiles)
			ListParam.AddParametr(sLanguageTabInfo.UnpackSizeFiles, BufStr);
	}

	//upakovanniy razmer
	if ((sSettingsMain.AllParametrs) || (PackSizeFiles))
	{
		if (!sSettingsMain.FloatSize) NumberSeparatorStringUnitSize(BufStr, PackSizeFiles, T_BYTE, sLanguageOther);
		else NumberFloatStringUnitSize(BufStr, PackSizeFiles, T_BYTE, sLanguageOther);
		if (sSettingsParameters.PackSizeFiles)
			ListParam.AddParametr(sLanguageTabInfo.PackSizeFiles, BufStr);
	}

	//razmer arhiva
	if ((sSettingsMain.AllParametrs) || (ArchiveSize))
	{
		if (!sSettingsMain.FloatSize) NumberSeparatorStringUnitSize(BufStr, ArchiveSize, T_BYTE, sLanguageOther);
		else NumberFloatStringUnitSize(BufStr, ArchiveSize, T_BYTE, sLanguageOther);
		if (sSettingsParameters.ArchiveSize)
			ListParam.AddParametr(sLanguageTabInfo.ArchiveSize, BufStr);
	}

	//kolichestvo faylov
	if ((sSettingsMain.AllParametrs) || (NumberFiles))
	{
		if (NumberFiles) NumberSeparatorString(BufStr, NumberFiles);
		else sprintf(BufStr, "%s", sLanguageOther.No);
		if (sSettingsParameters.NumberFiles)
			ListParam.AddParametr(sLanguageTabInfo.NumberFiles, BufStr);
	}

	//kolichestvo papok tol'ko ARC
	if (!ArchiveType)
	{
		if ((sSettingsMain.AllParametrs) || (NumberFolders))
		{
			if (NumberFolders) NumberSeparatorString(BufStr, NumberFolders);
			else sprintf(BufStr, "%s", sLanguageOther.No);
			if (sSettingsParameters.NumberFolders)
				ListParam.AddParametr(sLanguageTabInfo.NumberFolders, BufStr);
		}
	}

	//razdelitel'
	ListParam.AddParametr("", "");

	//metod upakovki
	if (sSettingsParameters.MethodPack)
		switch (MethodPack)
		{
		case 0x01: ListParam.AddParametr(sLanguageTabInfo.MethodPack, sLanguagePack.Unpacked);  break;
		case 0x02: ListParam.AddParametr(sLanguageTabInfo.MethodPack, sLanguagePack.Unpacked);  break;
		case 0x03: ListParam.AddParametr(sLanguageTabInfo.MethodPack, sLanguagePack.Packed);    break;
		case 0x04: ListParam.AddParametr(sLanguageTabInfo.MethodPack, sLanguagePack.Squeezed);  break;
		case 0x05: ListParam.AddParametr(sLanguageTabInfo.MethodPack, sLanguagePack.Crunched);  break;
		case 0x06: ListParam.AddParametr(sLanguageTabInfo.MethodPack, sLanguagePack.Crunched);  break;
		case 0x07: ListParam.AddParametr(sLanguageTabInfo.MethodPack, sLanguagePack.Crunched);  break;
		case 0x08: ListParam.AddParametr(sLanguageTabInfo.MethodPack, sLanguagePack.Crunched);  break;
		case 0x09: ListParam.AddParametr(sLanguageTabInfo.MethodPack, sLanguagePack.Squashed);  break;
		case 0x0A: ListParam.AddParametr(sLanguageTabInfo.MethodPack, sLanguagePack.Crushed);   break;
		case 0x0B: ListParam.AddParametr(sLanguageTabInfo.MethodPack, sLanguagePack.Distilled); break;
		default:   ListParam.AddParametr(sLanguageTabInfo.MethodPack, sLanguageOther.Unknown);  break;
		}

	//sjatie ot rzmera arhiva
	sprintf(BufStr, "%.1f %s", RatioArchiveSize, sLanguageOther.Percent);
	if (sSettingsParameters.RatioArchiveSize)
		ListParam.AddParametr(sLanguageTabInfo.RatioArchiveSize, BufStr);

	//sjatie ot upakovannogo razmera
	sprintf(BufStr, "%.1f %s", RatioPackFileSize, sLanguageOther.Percent);
	if (sSettingsParameters.RatioPackFileSize)
		ListParam.AddParametr(sLanguageTabInfo.RatioPackFileSize, BufStr);

	//razdelitel'
	ListParam.AddParametr("", "");

	//kommentariy arhiva
	if ((sSettingsMain.AllParametrs) || (MainComment))
	{
		if (sSettingsParameters.MainComment)
			if (!MainComment) ListParam.AddParametr(sLanguageTabInfo.MainComment, sLanguageOther.No);
			else ListParam.AddParametr(sLanguageTabInfo.MainComment, sLanguageOther.Yes);
	}
	//tekst kommentariya
	if (pTextComment) *m_ppTextComment = pTextComment;

	//razdelitel'
	ListParam.AddParametr("", "");

	sprintf(BufStr, "%.2f %s", TestTime, sLanguageOther.Second);
	if (sSettingsParameters.TestTime)
		ListParam.AddParametr(sLanguageTabInfo.TestTime, BufStr);

	sprintf(BufStr, "%.2f %s", AnalyzeTime, sLanguageOther.Second);
	if (sSettingsParameters.AnalyzeTime)
		ListParam.AddParametr(sLanguageTabInfo.AnalyzeTime, BufStr);

	sprintf(BufStr, "%.2f %s", AllTime, sLanguageOther.Second);
	if (sSettingsParameters.AllTime)
		ListParam.AddParametr(sLanguageTabInfo.AllTime, BufStr);

	return 1;
}

//dlya archiva ARJ
int MySelectInfo::SelectInfoArj()
{
	//dlya uprosheniya obrasheniya k dannim sobrannim v odnom meste
	SettingsMain&       sSettingsMain       = m_pSettings->sSettingsMain;
	SettingsParameters& sSettingsParameters = m_pSettings->sSettingsParameters;
	LanguageTabInfo&    sLanguageTabInfo    = m_pLanguage->sLanguageTabInfo;
	LanguageOS&         sLanguageOS         = m_pLanguage->sLanguageOS;
	LanguagePack&       sLanguagePack       = m_pLanguage->sLanguagePack;
	LanguageOther&      sLanguageOther      = m_pLanguage->sLanguageOther;
	MListParametrs&     ListParam           = *m_pListParam;

	TArj* obj_arj = (TArj*)m_pArchData;

	ULONGLONG UnpackSizeFiles   = obj_arj->GetUnpackSizeFiles();
	ULONGLONG PackSizeFiles     = obj_arj->GetPackSizeFiles();
	ULONGLONG ArchiveSize       = obj_arj->GetArchiveSize();
	unsigned  NumberFiles       = obj_arj->GetNumberFiles();
	unsigned  NumberFolders     = obj_arj->GetNumberFolders();
	unsigned  NumberChapters    = obj_arj->GetNumberChapters();
	char*     pOriginalName     = obj_arj->GetOriginalName();
	char*     pDateCreated      = obj_arj->GetDateCreated();
	char*     pTimeCreated      = obj_arj->GetTimeCreated();
	char*     pDateModified     = obj_arj->GetDateModified();
	char*     pTimeModified     = obj_arj->GetTimeModified();
	short     HostOS            = obj_arj->GetHostOS();
	short     MethodPack        = obj_arj->GetMethodPack();
	double    RatioArchiveSize  = obj_arj->GetRatioArchiveSize();
	double    RatioPackFileSize = obj_arj->GetRatioPackFileSize();
	double    PackVersion       = obj_arj->GetPackVersion();
	double    UnpackVersion     = obj_arj->GetUnpackVersion();
	unsigned  SfxModule         = obj_arj->GetSfxModule();
	unsigned  RecoveryRecord    = obj_arj->GetRecoveryRecord();
	BOOL      MultiArc          = obj_arj->GetMultiArc();
	unsigned  VolumeNumber      = obj_arj->GetVolumeNumber();
	BOOL      MainComment       = obj_arj->GetMainComment();
	BOOL      FileComment       = obj_arj->GetFileComment();
	BOOL      ChapterComment    = obj_arj->GetChapterComment();
	BOOL      Password          = obj_arj->GetPassword();
	BOOL      ArchiveLock       = obj_arj->GetArchiveLock();
	BOOL      AuthVerification  = obj_arj->GetAuthVerification();
	char*     pTextComment      = obj_arj->GetTextComment();
	double    TestTime          = m_TestTime;
	double    AnalyzeTime       = obj_arj->GetAnalyzeTime();
	double    AllTime           = m_TestTime + AnalyzeTime;

	if (((int)RatioPackFileSize == 0) && (RatioPackFileSize > 0)) *m_pRatio1 = 1;
	else *m_pRatio1 = (int)RatioPackFileSize;
	if (((int)RatioArchiveSize == 0) && (RatioArchiveSize > 0)) *m_pRatio2 = 1;
	else *m_pRatio2 = (int)RatioArchiveSize;

	char BufStr[MAX_LOADSTRING] = {NULL};

	//zagolovok
	char* _ARCH = m_pLanguage->sLanguageArchives.Arj;
	char* _SFX  = m_pLanguage->sLanguageArchives.Sfx;
	if (SfxModule) sprintf(BufStr, "%s %s %s", _SFX, _ARCH, sLanguageOther.Archive);
	else sprintf(BufStr, "%s %s", _ARCH, sLanguageOther.Archive);
	ListParam.AddParametr("", BufStr);

	//neupakovanniy razmer
	if ((sSettingsMain.AllParametrs) || (UnpackSizeFiles))
	{
		if (!sSettingsMain.FloatSize) NumberSeparatorStringUnitSize(BufStr, UnpackSizeFiles, T_BYTE, sLanguageOther);
		else NumberFloatStringUnitSize(BufStr, UnpackSizeFiles, T_BYTE, sLanguageOther);
		if (sSettingsParameters.UnpackSizeFiles)
			ListParam.AddParametr(sLanguageTabInfo.UnpackSizeFiles, BufStr);
	}

	//upakovanniy razmer
	if ((sSettingsMain.AllParametrs) || (PackSizeFiles))
	{
		if (!sSettingsMain.FloatSize) NumberSeparatorStringUnitSize(BufStr, PackSizeFiles, T_BYTE, sLanguageOther);
		else NumberFloatStringUnitSize(BufStr, PackSizeFiles, T_BYTE, sLanguageOther);
		if (sSettingsParameters.PackSizeFiles)
			ListParam.AddParametr(sLanguageTabInfo.PackSizeFiles, BufStr);
	}

	//razmer arhiva
	if ((sSettingsMain.AllParametrs) || (ArchiveSize))
	{
		if (!sSettingsMain.FloatSize) NumberSeparatorStringUnitSize(BufStr, ArchiveSize, T_BYTE, sLanguageOther);
		else NumberFloatStringUnitSize(BufStr, ArchiveSize, T_BYTE, sLanguageOther);
		if (sSettingsParameters.ArchiveSize)
			ListParam.AddParametr(sLanguageTabInfo.ArchiveSize, BufStr);
	}

	//kolichestvo faylov
	if ((sSettingsMain.AllParametrs) || (NumberFiles))
	{
		if (NumberFiles) NumberSeparatorString(BufStr, NumberFiles);
		else sprintf(BufStr, "%s", sLanguageOther.No);
		if (sSettingsParameters.NumberFiles)
			ListParam.AddParametr(sLanguageTabInfo.NumberFiles, BufStr);
	}

	//kolichestvo papok
	if ((sSettingsMain.AllParametrs) || (NumberFolders))
	{
		if (NumberFolders) NumberSeparatorString(BufStr, NumberFolders);
		else sprintf(BufStr, "%s", sLanguageOther.No);
		if (sSettingsParameters.NumberFolders)
			ListParam.AddParametr(sLanguageTabInfo.NumberFolders, BufStr);
	}

	//kolichestvo glav
	if ((sSettingsMain.AllParametrs) || (NumberChapters))
	{
		if (NumberChapters) NumberSeparatorString(BufStr, NumberChapters);
		else sprintf(BufStr, "%s", sLanguageOther.No);
		if (sSettingsParameters.NumberChapters)
			ListParam.AddParametr(sLanguageTabInfo.NumberChapters, BufStr);
	}

	//original'noe imya
	if ((sSettingsMain.AllParametrs) || (pOriginalName))
	{
		if (sSettingsParameters.OriginalName)
			if (pOriginalName) ListParam.AddParametr(sLanguageTabInfo.OriginalName, pOriginalName);
			else ListParam.AddParametr(sLanguageTabInfo.OriginalName, sLanguageOther.No);
	}

	//data sozdaniya
	if (sSettingsParameters.DateCreated)
		ListParam.AddParametr(sLanguageTabInfo.DateCreated, pDateCreated);

	//vremya sozdaniya
	if (sSettingsParameters.TimeCreated)
		ListParam.AddParametr(sLanguageTabInfo.TimeCreated, pTimeCreated);

	//data modifikacii
	if ((sSettingsMain.AllParametrs) || (!(strcmp(pDateModified, pDateCreated) == 0)))
	{
		if (sSettingsParameters.DateModified)
			ListParam.AddParametr(sLanguageTabInfo.DateModified, pDateModified);
	}

	//vremya modifikacii
	if ((sSettingsMain.AllParametrs) || (!(strcmp(pTimeModified, pTimeCreated) == 0)))
	{
		if (sSettingsParameters.TimeModified)
			ListParam.AddParametr(sLanguageTabInfo.TimeModified, pTimeModified);
	}

	//razdelitel'
	ListParam.AddParametr("", "");

	//host OS
	if (sSettingsParameters.HostOS)
		switch (HostOS)
		{
		case 0x00: ListParam.AddParametr(sLanguageTabInfo.HostOS, sLanguageOS.DOS);        break;
		case 0x01: ListParam.AddParametr(sLanguageTabInfo.HostOS, sLanguageOS.Primos);     break;
		case 0x02: ListParam.AddParametr(sLanguageTabInfo.HostOS, sLanguageOS.Unix);       break;
		case 0x03: ListParam.AddParametr(sLanguageTabInfo.HostOS, sLanguageOS.Amiga);      break;
		case 0x04: ListParam.AddParametr(sLanguageTabInfo.HostOS, sLanguageOS.MacOS);      break;
		case 0x05: ListParam.AddParametr(sLanguageTabInfo.HostOS, sLanguageOS.OS2);        break;
		case 0x06: ListParam.AddParametr(sLanguageTabInfo.HostOS, sLanguageOS.AppleGS);    break;
		case 0x07: ListParam.AddParametr(sLanguageTabInfo.HostOS, sLanguageOS.AtariST);    break;
		case 0x08: ListParam.AddParametr(sLanguageTabInfo.HostOS, sLanguageOS.Next);       break;
		case 0x09: ListParam.AddParametr(sLanguageTabInfo.HostOS, sLanguageOS.VaxVMS);     break;
		case 0x0A: ListParam.AddParametr(sLanguageTabInfo.HostOS, sLanguageOS.Win95);      break;
		case 0x0B: ListParam.AddParametr(sLanguageTabInfo.HostOS, sLanguageOS.Win32);      break;
		default:   ListParam.AddParametr(sLanguageTabInfo.HostOS, sLanguageOther.Unknown); break;
		}

	//metod upakovki
	if (sSettingsParameters.MethodPack)
		switch (MethodPack)
		{
		case 0x00: ListParam.AddParametr(sLanguageTabInfo.MethodPack, sLanguagePack.Stored);   break;
		case 0x01: ListParam.AddParametr(sLanguageTabInfo.MethodPack, sLanguagePack.Good);     break;
		case 0x02: ListParam.AddParametr(sLanguageTabInfo.MethodPack, sLanguagePack.Normal);   break;
		case 0x03: ListParam.AddParametr(sLanguageTabInfo.MethodPack, sLanguagePack.Fast);     break;
		case 0x04: ListParam.AddParametr(sLanguageTabInfo.MethodPack, sLanguagePack.Fastest);  break;
		default:   ListParam.AddParametr(sLanguageTabInfo.MethodPack, sLanguageOther.Unknown); break;
		}

	//sjatie ot rzmera arhiva
	sprintf(BufStr, "%.1f %s", RatioArchiveSize, sLanguageOther.Percent);
	if (sSettingsParameters.RatioArchiveSize)
		ListParam.AddParametr(sLanguageTabInfo.RatioArchiveSize, BufStr);

	//sjatie ot upakovannogo razmera
	sprintf(BufStr, "%.1f %s", RatioPackFileSize, sLanguageOther.Percent);
	if (sSettingsParameters.RatioPackFileSize)
		ListParam.AddParametr(sLanguageTabInfo.RatioPackFileSize, BufStr);

	//versiya dlya upakovki
	if ((sSettingsMain.AllParametrs) || (PackVersion != UnpackVersion))
	{
		sprintf(BufStr, "%.1f", PackVersion);
		if (sSettingsParameters.PackVersion)
			ListParam.AddParametr(sLanguageTabInfo.PackVersion, BufStr);
	}

	//versiya dlya raspakovki
	sprintf(BufStr, "%.1f", UnpackVersion);
	if (sSettingsParameters.UnpackVersion)
		ListParam.AddParametr(sLanguageTabInfo.UnpackVersion, BufStr);

	//razdelitel'
	ListParam.AddParametr("", "");

	//SFX modul'
	if ((sSettingsMain.AllParametrs) || (SfxModule))
	{
		if (sSettingsParameters.SfxModule)
			if (!SfxModule) ListParam.AddParametr(sLanguageTabInfo.SfxModule, sLanguageOther.No);
			else
			{
				if (!sSettingsMain.FloatSize) NumberSeparatorStringUnitSize(BufStr, SfxModule, T_BYTE, sLanguageOther);
				else NumberFloatStringUnitSize(BufStr, SfxModule, T_BYTE, sLanguageOther);
				ListParam.AddParametr(sLanguageTabInfo.SfxModule, BufStr);
			}
	}

	//zapis' dlya vosstanovleniya
	if ((sSettingsMain.AllParametrs) || (RecoveryRecord))
	{
		if (sSettingsParameters.RecoveryRecord)
			if (!RecoveryRecord) ListParam.AddParametr(sLanguageTabInfo.RecoveryRecord, sLanguageOther.No);
			else
			{
				if (!sSettingsMain.FloatSize) NumberSeparatorStringUnitSize(BufStr, RecoveryRecord, T_BYTE, sLanguageOther);
				else NumberFloatStringUnitSize(BufStr, RecoveryRecord, T_BYTE, sLanguageOther);
				ListParam.AddParametr(sLanguageTabInfo.RecoveryRecord, BufStr);
			}
	}

	//mnogotomnost'
	if ((sSettingsMain.AllParametrs) || (MultiArc))
	{
		if (sSettingsParameters.MultiArc)
			if (!MultiArc) ListParam.AddParametr(sLanguageTabInfo.MultiArc, sLanguageOther.No);
			else
			{
				ListParam.AddParametr(sLanguageTabInfo.MultiArc, sLanguageOther.Yes);
				//nomer toma
				if (VolumeNumber)
				{
					sprintf(BufStr, "%s %d", sLanguageOther.Number, VolumeNumber);
					ListParam.AddParametr(sLanguageTabInfo.NumberVolume, BufStr);
				}
			}
	}

	//kommentariy arhiva
	if ((sSettingsMain.AllParametrs) || (MainComment))
	{
		if (sSettingsParameters.MainComment)
			if (!MainComment) ListParam.AddParametr(sLanguageTabInfo.MainComment, sLanguageOther.No);
			else ListParam.AddParametr(sLanguageTabInfo.MainComment, sLanguageOther.Yes);
	}
	//tekst kommentariya
	if (pTextComment) *m_ppTextComment = pTextComment;

	//kommentarii faylov
	if ((sSettingsMain.AllParametrs) || (FileComment))
	{
		if (sSettingsParameters.FileComment)
			if (!FileComment) ListParam.AddParametr(sLanguageTabInfo.FileComment, sLanguageOther.No);
			else ListParam.AddParametr(sLanguageTabInfo.FileComment, sLanguageOther.Yes);
	}

	//kommentarii glav
	if ((sSettingsMain.AllParametrs) || (ChapterComment))
	{
		if (sSettingsParameters.ChapterComment)
			if (!ChapterComment) ListParam.AddParametr(sLanguageTabInfo.ChapterComment, sLanguageOther.No);
			else ListParam.AddParametr(sLanguageTabInfo.ChapterComment, sLanguageOther.Yes);
	}

	//parol'
	if ((sSettingsMain.AllParametrs) || (Password))
	{
		if (sSettingsParameters.Password)
			if (!Password) ListParam.AddParametr(sLanguageTabInfo.Password, sLanguageOther.No);
			else ListParam.AddParametr(sLanguageTabInfo.Password, sLanguageOther.Yes);
	}

	//arhiv zablokirovam
	if ((sSettingsMain.AllParametrs) || (ArchiveLock))
	{
		if (sSettingsParameters.ArchiveLock)
			if (!ArchiveLock) ListParam.AddParametr(sLanguageTabInfo.ArchiveLock, sLanguageOther.No);
			else ListParam.AddParametr(sLanguageTabInfo.ArchiveLock, sLanguageOther.Yes);
	}

	//elektronnaya podpis'
	if ((sSettingsMain.AllParametrs) || (AuthVerification))
	{
		if (sSettingsParameters.AuthVerification)
			if (!AuthVerification) ListParam.AddParametr(sLanguageTabInfo.AuthVerification, sLanguageOther.No);
			else ListParam.AddParametr(sLanguageTabInfo.AuthVerification, sLanguageOther.Yes);
	}

	//razdelitel'
	ListParam.AddParametr("", "");

	sprintf(BufStr, "%.2f %s", TestTime, sLanguageOther.Second);
	if (sSettingsParameters.TestTime)
		ListParam.AddParametr(sLanguageTabInfo.TestTime, BufStr);

	sprintf(BufStr, "%.2f %s", AnalyzeTime, sLanguageOther.Second);
	if (sSettingsParameters.AnalyzeTime)
		ListParam.AddParametr(sLanguageTabInfo.AnalyzeTime, BufStr);

	sprintf(BufStr, "%.2f %s", AllTime, sLanguageOther.Second);
	if (sSettingsParameters.AllTime)
		ListParam.AddParametr(sLanguageTabInfo.AllTime, BufStr);

	return 1;
}

//dlya archiva BH
int MySelectInfo::SelectInfoBh()
{
	//dlya uprosheniya obrasheniya k dannim sobrannim v odnom meste
	SettingsMain&       sSettingsMain       = m_pSettings->sSettingsMain;
	SettingsParameters& sSettingsParameters = m_pSettings->sSettingsParameters;
	LanguageTabInfo&    sLanguageTabInfo    = m_pLanguage->sLanguageTabInfo;
	LanguageOS&         sLanguageOS         = m_pLanguage->sLanguageOS;
	LanguagePack&       sLanguagePack       = m_pLanguage->sLanguagePack;
	LanguageOther&      sLanguageOther      = m_pLanguage->sLanguageOther;
	MListParametrs&     ListParam           = *m_pListParam;

	TBh* obj_bh = (TBh*)m_pArchData;

	ULONGLONG UnpackSizeFiles   = obj_bh->GetUnpackSizeFiles();
	ULONGLONG PackSizeFiles     = obj_bh->GetPackSizeFiles();
	ULONGLONG ArchiveSize       = obj_bh->GetArchiveSize();
	unsigned  NumberFiles       = obj_bh->GetNumberFiles();
	unsigned  NumberFolders     = obj_bh->GetNumberFolders();
	short     MethodPack        = obj_bh->GetMethodPack();
	double    RatioArchiveSize  = obj_bh->GetRatioArchiveSize();
	double    RatioPackFileSize = obj_bh->GetRatioPackFileSize();
	double    PackVersion       = obj_bh->GetPackVersion();
	double    UnpackVersion     = obj_bh->GetUnpackVersion();
	unsigned  SfxModule         = obj_bh->GetSfxModule();
	BOOL      FileComment       = obj_bh->GetFileComment();
	BOOL      Password          = obj_bh->GetPassword();
	double    TestTime          = m_TestTime;
	double    AnalyzeTime       = obj_bh->GetAnalyzeTime();
	double    AllTime           = m_TestTime + AnalyzeTime;

	if (((int)RatioPackFileSize == 0) && (RatioPackFileSize > 0)) *m_pRatio1 = 1;
	else *m_pRatio1 = (int)RatioPackFileSize;
	if (((int)RatioArchiveSize == 0) && (RatioArchiveSize > 0)) *m_pRatio2 = 1;
	else *m_pRatio2 = (int)RatioArchiveSize;

	char BufStr[MAX_LOADSTRING] = {NULL};

	//zagolovok
	char* _ARCH = m_pLanguage->sLanguageArchives.Bh;
	char* _SFX  = m_pLanguage->sLanguageArchives.Sfx;
	if (SfxModule) sprintf(BufStr, "%s %s %s", _SFX, _ARCH, sLanguageOther.Archive);
	else sprintf(BufStr, "%s %s", _ARCH, sLanguageOther.Archive);
	ListParam.AddParametr("", BufStr);

	//neupakovanniy razmer
	if ((sSettingsMain.AllParametrs) || (UnpackSizeFiles))
	{
		if (!sSettingsMain.FloatSize) NumberSeparatorStringUnitSize(BufStr, UnpackSizeFiles, T_BYTE, sLanguageOther);
		else NumberFloatStringUnitSize(BufStr, UnpackSizeFiles, T_BYTE, sLanguageOther);
		if (sSettingsParameters.UnpackSizeFiles)
			ListParam.AddParametr(sLanguageTabInfo.UnpackSizeFiles, BufStr);
	}

	//upakovanniy razmer
	if ((sSettingsMain.AllParametrs) || (PackSizeFiles))
	{
		if (!sSettingsMain.FloatSize) NumberSeparatorStringUnitSize(BufStr, PackSizeFiles, T_BYTE, sLanguageOther);
		else NumberFloatStringUnitSize(BufStr, PackSizeFiles, T_BYTE, sLanguageOther);
		if (sSettingsParameters.PackSizeFiles)
			ListParam.AddParametr(sLanguageTabInfo.PackSizeFiles, BufStr);
	}

	//razmer arhiva
	if ((sSettingsMain.AllParametrs) || (ArchiveSize))
	{
		if (!sSettingsMain.FloatSize) NumberSeparatorStringUnitSize(BufStr, ArchiveSize, T_BYTE, sLanguageOther);
		else NumberFloatStringUnitSize(BufStr, ArchiveSize, T_BYTE, sLanguageOther);
		if (sSettingsParameters.ArchiveSize)
			ListParam.AddParametr(sLanguageTabInfo.ArchiveSize, BufStr);
	}

	//kolichestvo faylov
	if ((sSettingsMain.AllParametrs) || (NumberFiles))
	{
		if (NumberFiles) NumberSeparatorString(BufStr, NumberFiles);
		else sprintf(BufStr, "%s", sLanguageOther.No);
		if (sSettingsParameters.NumberFiles)
			ListParam.AddParametr(sLanguageTabInfo.NumberFiles, BufStr);
	}

	//kolichestvo papok
	if ((sSettingsMain.AllParametrs) || (NumberFolders))
	{
		if (NumberFolders) NumberSeparatorString(BufStr, NumberFolders);
		else sprintf(BufStr, "%s", sLanguageOther.No);
		if (sSettingsParameters.NumberFolders)
			ListParam.AddParametr(sLanguageTabInfo.NumberFolders, BufStr);
	}

	//razdelitel'
	ListParam.AddParametr("", "");

	//metod upakovki
	if (sSettingsParameters.MethodPack)
		switch (MethodPack)
		{
		case 0x00:	ListParam.AddParametr(sLanguageTabInfo.MethodPack, sLanguagePack.Stored);   break;
		case 0x01:	ListParam.AddParametr(sLanguageTabInfo.MethodPack, sLanguagePack.Fast);     break;
		case 0x02:	sprintf(BufStr, "%s (%s)", sLanguagePack.Fuse, sLanguagePack.Normal);
					ListParam.AddParametr(sLanguageTabInfo.MethodPack, BufStr);                 break;
		case 0x03:	ListParam.AddParametr(sLanguageTabInfo.MethodPack, sLanguagePack.Normal);   break;
		case 0x04:	ListParam.AddParametr(sLanguageTabInfo.MethodPack, sLanguagePack.Best);     break;
		default:	ListParam.AddParametr(sLanguageTabInfo.MethodPack, sLanguageOther.Unknown); break;
		}

	//sjatie ot rzmera arhiva
	sprintf(BufStr, "%.1f %s", RatioArchiveSize, sLanguageOther.Percent);
	if (sSettingsParameters.RatioArchiveSize)
		ListParam.AddParametr(sLanguageTabInfo.RatioArchiveSize, BufStr);

	//sjatie ot upakovannogo razmera
	sprintf(BufStr, "%.1f %s", RatioPackFileSize, sLanguageOther.Percent);
	if (sSettingsParameters.RatioPackFileSize)
		ListParam.AddParametr(sLanguageTabInfo.RatioPackFileSize, BufStr);

	//versiya dlya upakovki
	if ((sSettingsMain.AllParametrs) || (PackVersion != UnpackVersion))
	{
		sprintf(BufStr, "%.1f", PackVersion);
		if (sSettingsParameters.PackVersion)
			ListParam.AddParametr(sLanguageTabInfo.PackVersion, BufStr);
	}

	//versiya dlya raspakovki
	sprintf(BufStr, "%.1f", UnpackVersion);
	if (sSettingsParameters.UnpackVersion)
		ListParam.AddParametr(sLanguageTabInfo.UnpackVersion, BufStr);

	//razdelitel'
	ListParam.AddParametr("", "");

	//SFX modul'
	if ((sSettingsMain.AllParametrs) || (SfxModule))
	{
		if (sSettingsParameters.SfxModule)
			if (!SfxModule) ListParam.AddParametr(sLanguageTabInfo.SfxModule, sLanguageOther.No);
			else
			{
				if (!sSettingsMain.FloatSize) NumberSeparatorStringUnitSize(BufStr, SfxModule, T_BYTE, sLanguageOther);
				else NumberFloatStringUnitSize(BufStr, SfxModule, T_BYTE, sLanguageOther);
				ListParam.AddParametr(sLanguageTabInfo.SfxModule, BufStr);
			}
	}

	//kommentarii faylov
	if ((sSettingsMain.AllParametrs) || (FileComment))
	{
		if (sSettingsParameters.FileComment)
			if (!FileComment) ListParam.AddParametr(sLanguageTabInfo.FileComment, sLanguageOther.No);
			else ListParam.AddParametr(sLanguageTabInfo.FileComment, sLanguageOther.Yes);
	}

	//parol'
	if ((sSettingsMain.AllParametrs) || (Password))
	{
		if (sSettingsParameters.Password)
			if (!Password) ListParam.AddParametr(sLanguageTabInfo.Password, sLanguageOther.No);
			else ListParam.AddParametr(sLanguageTabInfo.Password, sLanguageOther.Yes);
	}

	//razdelitel'
	ListParam.AddParametr("", "");

	sprintf(BufStr, "%.2f %s", TestTime, sLanguageOther.Second);
	if (sSettingsParameters.TestTime)
		ListParam.AddParametr(sLanguageTabInfo.TestTime, BufStr);

	sprintf(BufStr, "%.2f %s", AnalyzeTime, sLanguageOther.Second);
	if (sSettingsParameters.AnalyzeTime)
		ListParam.AddParametr(sLanguageTabInfo.AnalyzeTime, BufStr);

	sprintf(BufStr, "%.2f %s", AllTime, sLanguageOther.Second);
	if (sSettingsParameters.AllTime)
		ListParam.AddParametr(sLanguageTabInfo.AllTime, BufStr);

	return 1;
}

//dlya archiva BZIP2/TBZ2
int MySelectInfo::SelectInfoBZip2Tbz2()
{
	//dlya uprosheniya obrasheniya k dannim sobrannim v odnom meste
	SettingsMain&       sSettingsMain       = m_pSettings->sSettingsMain;
	SettingsParameters& sSettingsParameters = m_pSettings->sSettingsParameters;
	LanguageTabInfo&    sLanguageTabInfo    = m_pLanguage->sLanguageTabInfo;
	LanguageOS&         sLanguageOS         = m_pLanguage->sLanguageOS;
	LanguagePack&       sLanguagePack       = m_pLanguage->sLanguagePack;
	LanguageOther&      sLanguageOther      = m_pLanguage->sLanguageOther;
	MListParametrs&     ListParam           = *m_pListParam;

	TBZip2Tbz2* obj_bzip2tbz2 = (TBZip2Tbz2*)m_pArchData;

	ULONGLONG PackSizeFiles     = obj_bzip2tbz2->GetPackSizeFiles();
	ULONGLONG ArchiveSize       = obj_bzip2tbz2->GetArchiveSize();
	unsigned  NumberFiles       = obj_bzip2tbz2->GetNumberFiles();
	short     MethodPack        = obj_bzip2tbz2->GetMethodPack();
	double    TestTime          = m_TestTime;
	double    AnalyzeTime       = obj_bzip2tbz2->GetAnalyzeTime();
	double    AllTime           = m_TestTime + AnalyzeTime;

	char BufStr[MAX_LOADSTRING] = {NULL};

	//zagolovok
	char* _ARCH = m_pLanguage->sLanguageArchives.BZip2;
	sprintf(BufStr, "%s %s", _ARCH, sLanguageOther.Archive);
	ListParam.AddParametr("", BufStr);

	//upakovanniy razmer
	if ((sSettingsMain.AllParametrs) || (PackSizeFiles))
	{
		if (!sSettingsMain.FloatSize) NumberSeparatorStringUnitSize(BufStr, PackSizeFiles, T_BYTE, sLanguageOther);
		else NumberFloatStringUnitSize(BufStr, PackSizeFiles, T_BYTE, sLanguageOther);
		if (sSettingsParameters.PackSizeFiles)
			ListParam.AddParametr(sLanguageTabInfo.PackSizeFiles, BufStr);
	}

	//razmer arhiva
	if ((sSettingsMain.AllParametrs) || (ArchiveSize))
	{
		if (!sSettingsMain.FloatSize) NumberSeparatorStringUnitSize(BufStr, ArchiveSize, T_BYTE, sLanguageOther);
		else NumberFloatStringUnitSize(BufStr, ArchiveSize, T_BYTE, sLanguageOther);
		if (sSettingsParameters.ArchiveSize)
			ListParam.AddParametr(sLanguageTabInfo.ArchiveSize, BufStr);
	}

	//kolichestvo faylov
	if ((sSettingsMain.AllParametrs) || (NumberFiles))
	{
		if (NumberFiles) NumberSeparatorString(BufStr, NumberFiles);
		else sprintf(BufStr, "%s", sLanguageOther.No);
		if (sSettingsParameters.NumberFiles)
			ListParam.AddParametr(sLanguageTabInfo.NumberFiles, BufStr);
	}

	//razdelitel'
	ListParam.AddParametr("", "");

	//metod upakovki
	if (sSettingsParameters.MethodPack)
		switch (MethodPack)
		{
		case 0x31:	sprintf(BufStr, "%s%s1 (%s)", sLanguagePack.Rate, sLanguageOther.Number, sLanguagePack.Superfast);
					ListParam.AddParametr(sLanguageTabInfo.MethodPack, BufStr);                 break;
		case 0x32:	sprintf(BufStr, "%s%s2 (%s)", sLanguagePack.Rate, sLanguageOther.Number, sLanguagePack.Fastest);
					ListParam.AddParametr(sLanguageTabInfo.MethodPack, BufStr);                 break;
		case 0x33:	sprintf(BufStr, "%s%s3 (%s)", sLanguagePack.Rate, sLanguageOther.Number, sLanguagePack.Fast);
					ListParam.AddParametr(sLanguageTabInfo.MethodPack, BufStr);                 break;
		case 0x34:	sprintf(BufStr, "%s%s4 (%s)", sLanguagePack.Rate, sLanguageOther.Number, sLanguagePack.Fast);
					ListParam.AddParametr(sLanguageTabInfo.MethodPack, BufStr);                 break;
		case 0x35:	sprintf(BufStr, "%s%s5 (%s)", sLanguagePack.Rate, sLanguageOther.Number, sLanguagePack.Normal);
					ListParam.AddParametr(sLanguageTabInfo.MethodPack, BufStr);                 break;
		case 0x36:	sprintf(BufStr, "%s%s6 (%s)", sLanguagePack.Rate, sLanguageOther.Number, sLanguagePack.Good);
					ListParam.AddParametr(sLanguageTabInfo.MethodPack, BufStr);                 break;
		case 0x37:	sprintf(BufStr, "%s%s7 (%s)", sLanguagePack.Rate, sLanguageOther.Number, sLanguagePack.Good);
					ListParam.AddParametr(sLanguageTabInfo.MethodPack, BufStr);                 break;
		case 0x38:	sprintf(BufStr, "%s%s8 (%s)", sLanguagePack.Rate, sLanguageOther.Number, sLanguagePack.Verygood);
					ListParam.AddParametr(sLanguageTabInfo.MethodPack, BufStr);                 break;
		case 0x39:	sprintf(BufStr, "%s%s9 (%s)", sLanguagePack.Rate, sLanguageOther.Number, sLanguagePack.Best);
					ListParam.AddParametr(sLanguageTabInfo.MethodPack, BufStr);                 break;
		default:	ListParam.AddParametr(sLanguageTabInfo.MethodPack, sLanguageOther.Unknown); break;
		}

	//razdelitel'
	ListParam.AddParametr("", "");

	sprintf(BufStr, "%.2f %s", TestTime, sLanguageOther.Second);
	if (sSettingsParameters.TestTime)
		ListParam.AddParametr(sLanguageTabInfo.TestTime, BufStr);

	sprintf(BufStr, "%.2f %s", AnalyzeTime, sLanguageOther.Second);
	if (sSettingsParameters.AnalyzeTime)
		ListParam.AddParametr(sLanguageTabInfo.AnalyzeTime, BufStr);

	sprintf(BufStr, "%.2f %s", AllTime, sLanguageOther.Second);
	if (sSettingsParameters.AllTime)
		ListParam.AddParametr(sLanguageTabInfo.AllTime, BufStr);

	return 1;
}

//dlya archiva CAB
int MySelectInfo::SelectInfoCab()
{
	//dlya uprosheniya obrasheniya k dannim sobrannim v odnom meste
	SettingsMain&       sSettingsMain       = m_pSettings->sSettingsMain;
	SettingsParameters& sSettingsParameters = m_pSettings->sSettingsParameters;
	LanguageTabInfo&    sLanguageTabInfo    = m_pLanguage->sLanguageTabInfo;
	LanguageOS&         sLanguageOS         = m_pLanguage->sLanguageOS;
	LanguagePack&       sLanguagePack       = m_pLanguage->sLanguagePack;
	LanguageOther&      sLanguageOther      = m_pLanguage->sLanguageOther;
	MListParametrs&     ListParam           = *m_pListParam;

	TCab* obj_cab = (TCab*)m_pArchData;

	ULONGLONG UnpackSizeFiles   = obj_cab->GetUnpackSizeFiles();
	ULONGLONG ArchiveSize       = obj_cab->GetArchiveSize();
	unsigned  NumberFiles       = obj_cab->GetNumberFiles();
	unsigned  NumberFolders     = obj_cab->GetNumberFolders();
	short     MethodPack        = obj_cab->GetMethodPack();
	short     MethodPackAdd     = obj_cab->GetMethodPackAdd();
	double    RatioArchiveSize  = obj_cab->GetRatioArchiveSize();
	double    PackVersion       = obj_cab->GetPackVersion();
	double    UnpackVersion     = obj_cab->GetUnpackVersion();
	unsigned  SfxModule         = obj_cab->GetSfxModule();
	BOOL      MultiArc          = obj_cab->GetMultiArc();
	unsigned  VolumeNumber      = obj_cab->GetVolumeNumber();
	BOOL      Password          = obj_cab->GetPassword();
	double    TestTime          = m_TestTime;
	double    AnalyzeTime       = obj_cab->GetAnalyzeTime();
	double    AllTime           = m_TestTime + AnalyzeTime;

	if (((int)RatioArchiveSize == 0) && (RatioArchiveSize > 0)) *m_pRatio2 = 1;
	else *m_pRatio2 = (int)RatioArchiveSize;

	char BufStr[MAX_LOADSTRING] = {NULL};

	//zagolovok
	char* _ARCH = m_pLanguage->sLanguageArchives.Cab;
	char* _SFX  = m_pLanguage->sLanguageArchives.Sfx;
	if (SfxModule) sprintf(BufStr, "%s %s %s %s", sLanguageTabInfo.Solid, _SFX, _ARCH, sLanguageOther.Archive);
	else sprintf(BufStr, "%s %s %s", sLanguageTabInfo.Solid, _ARCH, sLanguageOther.Archive);
	ListParam.AddParametr("", BufStr);

	//neupakovanniy razmer
	if ((sSettingsMain.AllParametrs) || (UnpackSizeFiles))
	{
		if (!sSettingsMain.FloatSize) NumberSeparatorStringUnitSize(BufStr, UnpackSizeFiles, T_BYTE, sLanguageOther);
		else NumberFloatStringUnitSize(BufStr, UnpackSizeFiles, T_BYTE, sLanguageOther);
		if (sSettingsParameters.UnpackSizeFiles)
			ListParam.AddParametr(sLanguageTabInfo.UnpackSizeFiles, BufStr);
	}

	//razmer arhiva
	if ((sSettingsMain.AllParametrs) || (ArchiveSize))
	{
		if (!sSettingsMain.FloatSize) NumberSeparatorStringUnitSize(BufStr, ArchiveSize, T_BYTE, sLanguageOther);
		else NumberFloatStringUnitSize(BufStr, ArchiveSize, T_BYTE, sLanguageOther);
		if (sSettingsParameters.ArchiveSize)
			ListParam.AddParametr(sLanguageTabInfo.ArchiveSize, BufStr);
	}

	//kolichestvo faylov
	if ((sSettingsMain.AllParametrs) || (NumberFiles))
	{
		if (NumberFiles) NumberSeparatorString(BufStr, NumberFiles);
		else sprintf(BufStr, "%s", sLanguageOther.No);
		if (sSettingsParameters.NumberFiles)
			ListParam.AddParametr(sLanguageTabInfo.NumberFiles, BufStr);
	}

	//kolichestvo papok
	if ((sSettingsMain.AllParametrs) || (NumberFolders))
	{
		if (NumberFolders) NumberSeparatorString(BufStr, NumberFolders);
		else sprintf(BufStr, "%s", sLanguageOther.No);
		if (sSettingsParameters.NumberFolders)
			ListParam.AddParametr(sLanguageTabInfo.NumberFolders, BufStr);
	}

	//razdelitel'
	ListParam.AddParametr("", "");

	//metod upakovki
	if (sSettingsParameters.MethodPack)
		switch (MethodPack)
		{
		case 0x00:	ListParam.AddParametr(sLanguageTabInfo.MethodPack, sLanguagePack.Stored);   break;
		case 0x01:	sprintf(BufStr, "%s (%s)", sLanguagePack.MSZIP, sLanguagePack.Normal);
					ListParam.AddParametr(sLanguageTabInfo.MethodPack, BufStr);                 break;
		case 0x03:
			switch (MethodPackAdd)
			{
			case 0x0F:	sprintf(BufStr, "%s:15 (%s)", sLanguagePack.LZX, sLanguagePack.Superfast);
						ListParam.AddParametr(sLanguageTabInfo.MethodPack, BufStr);             break;
			case 0x10:	sprintf(BufStr, "%s:16 (%s)", sLanguagePack.LZX, sLanguagePack.Fastest);
						ListParam.AddParametr(sLanguageTabInfo.MethodPack, BufStr);             break;
			case 0x11:	sprintf(BufStr, "%s:17 (%s)", sLanguagePack.LZX, sLanguagePack.Fast);
						ListParam.AddParametr(sLanguageTabInfo.MethodPack, BufStr);             break;
			case 0x12:	sprintf(BufStr, "%s:18 (%s)", sLanguagePack.LZX, sLanguagePack.Normal);
						ListParam.AddParametr(sLanguageTabInfo.MethodPack, BufStr);             break;
			case 0x13:	sprintf(BufStr, "%s:19 (%s)", sLanguagePack.LZX, sLanguagePack.Good);
						ListParam.AddParametr(sLanguageTabInfo.MethodPack, BufStr);             break;
			case 0x14:	sprintf(BufStr, "%s:20 (%s)", sLanguagePack.LZX, sLanguagePack.Verygood);
						ListParam.AddParametr(sLanguageTabInfo.MethodPack, BufStr);             break;
			case 0x15:	sprintf(BufStr, "%s:21 (%s)", sLanguagePack.LZX, sLanguagePack.Best);
						ListParam.AddParametr(sLanguageTabInfo.MethodPack, BufStr);             break;
			default:	sprintf(BufStr, "%s: %s", sLanguagePack.LZX, sLanguageOther.Unknown);
						ListParam.AddParametr(sLanguageTabInfo.MethodPack, BufStr);             break;
			}
			break;
		case 0x71:
			switch (MethodPackAdd)
			{
			case 0x0F:	sprintf(BufStr, "%s:15 (%s)", sLanguagePack.Quantum, sLanguagePack.Superfast);
						ListParam.AddParametr(sLanguageTabInfo.MethodPack, BufStr);             break;
			case 0x10:	sprintf(BufStr, "%s:16 (%s)", sLanguagePack.Quantum, sLanguagePack.Fastest);
						ListParam.AddParametr(sLanguageTabInfo.MethodPack, BufStr);             break;
			case 0x11:	sprintf(BufStr, "%s:17 (%s)", sLanguagePack.Quantum, sLanguagePack.Fast);
						ListParam.AddParametr(sLanguageTabInfo.MethodPack, BufStr);             break;
			case 0x12:	sprintf(BufStr, "%s:18 (%s)", sLanguagePack.Quantum, sLanguagePack.Normal);
						ListParam.AddParametr(sLanguageTabInfo.MethodPack, BufStr);             break;
			case 0x13:	sprintf(BufStr, "%s:19 (%s)", sLanguagePack.Quantum, sLanguagePack.Good);
						ListParam.AddParametr(sLanguageTabInfo.MethodPack, BufStr);             break;
			case 0x14:	sprintf(BufStr, "%s:20 (%s)", sLanguagePack.Quantum, sLanguagePack.Verygood);
						ListParam.AddParametr(sLanguageTabInfo.MethodPack, BufStr);             break;
			case 0x15:	sprintf(BufStr, "%s:21 (%s)", sLanguagePack.Quantum, sLanguagePack.Best);
						ListParam.AddParametr(sLanguageTabInfo.MethodPack, BufStr);             break;
			default:	sprintf(BufStr, "%s: %s", sLanguagePack.Quantum, sLanguageOther.Unknown);
						ListParam.AddParametr(sLanguageTabInfo.MethodPack, BufStr);             break;
			}
			break;
		case 0x72:
			switch (MethodPackAdd)
			{
			case 0x0F:	sprintf(BufStr, "%s:15 (%s)", sLanguageOther.Unknown, sLanguagePack.Superfast);
						ListParam.AddParametr(sLanguageTabInfo.MethodPack, BufStr);             break;
			case 0x10:	sprintf(BufStr, "%s:16 (%s)", sLanguageOther.Unknown, sLanguagePack.Fastest);
						ListParam.AddParametr(sLanguageTabInfo.MethodPack, BufStr);             break;
			case 0x11:	sprintf(BufStr, "%s:17 (%s)", sLanguageOther.Unknown, sLanguagePack.Fast);
						ListParam.AddParametr(sLanguageTabInfo.MethodPack, BufStr);             break;
			case 0x12:	sprintf(BufStr, "%s:18 (%s)", sLanguageOther.Unknown, sLanguagePack.Normal);
						ListParam.AddParametr(sLanguageTabInfo.MethodPack, BufStr);             break;
			case 0x13:	sprintf(BufStr, "%s:19 (%s)", sLanguageOther.Unknown, sLanguagePack.Good);
						ListParam.AddParametr(sLanguageTabInfo.MethodPack, BufStr);             break;
			case 0x14:	sprintf(BufStr, "%s:20 (%s)", sLanguageOther.Unknown, sLanguagePack.Verygood);
						ListParam.AddParametr(sLanguageTabInfo.MethodPack, BufStr);             break;
			case 0x15:	sprintf(BufStr, "%s:21 (%s)", sLanguageOther.Unknown, sLanguagePack.Best);
						ListParam.AddParametr(sLanguageTabInfo.MethodPack, BufStr);             break;
			default:	sprintf(BufStr, "%s: %s", sLanguageOther.Unknown, sLanguageOther.Unknown);
						ListParam.AddParametr(sLanguageTabInfo.MethodPack, BufStr);             break;
			}
			break;
		default:	ListParam.AddParametr(sLanguageTabInfo.MethodPack, sLanguageOther.Unknown); break;
		}

	//sjatie ot rzmera arhiva
	sprintf(BufStr, "%.1f %s", RatioArchiveSize, sLanguageOther.Percent);
	if (sSettingsParameters.RatioArchiveSize)
		ListParam.AddParametr(sLanguageTabInfo.RatioArchiveSize, BufStr);

	//versiya dlya upakovki
	if ((sSettingsMain.AllParametrs) || (PackVersion != UnpackVersion))
	{
		sprintf(BufStr, "%.1f", PackVersion);
		if (sSettingsParameters.PackVersion)
			ListParam.AddParametr(sLanguageTabInfo.PackVersion, BufStr);
	}

	//versiya dlya raspakovki
	sprintf(BufStr, "%.1f", UnpackVersion);
	if (sSettingsParameters.UnpackVersion)
		ListParam.AddParametr(sLanguageTabInfo.UnpackVersion, BufStr);

	//razdelitel'
	ListParam.AddParametr("", "");

	//SFX modul'
	if ((sSettingsMain.AllParametrs) || (SfxModule))
	{
		if (sSettingsParameters.SfxModule)
			if (!SfxModule) ListParam.AddParametr(sLanguageTabInfo.SfxModule, sLanguageOther.No);
			else
			{
				if (!sSettingsMain.FloatSize) NumberSeparatorStringUnitSize(BufStr, SfxModule, T_BYTE, sLanguageOther);
				else NumberFloatStringUnitSize(BufStr, SfxModule, T_BYTE, sLanguageOther);
				ListParam.AddParametr(sLanguageTabInfo.SfxModule, BufStr);
			}
	}

	//mnogotomnost'
	if ((sSettingsMain.AllParametrs) || (MultiArc))
	{
		if (sSettingsParameters.MultiArc)
			if (!MultiArc) ListParam.AddParametr(sLanguageTabInfo.MultiArc, sLanguageOther.No);
			else
			{
				ListParam.AddParametr(sLanguageTabInfo.MultiArc, sLanguageOther.Yes);
				//nomer toma
				if (VolumeNumber)
				{
					sprintf(BufStr, "%s %d", sLanguageOther.Number, VolumeNumber);
					ListParam.AddParametr(sLanguageTabInfo.NumberVolume, BufStr);
				}
			}
	}

	//parol'
	if ((sSettingsMain.AllParametrs) || (Password))
	{
		if (sSettingsParameters.Password)
			if (!Password) ListParam.AddParametr(sLanguageTabInfo.Password, sLanguageOther.No);
			else ListParam.AddParametr(sLanguageTabInfo.Password, sLanguageOther.Yes);
	}

	//razdelitel'
	ListParam.AddParametr("", "");

	sprintf(BufStr, "%.2f %s", TestTime, sLanguageOther.Second);
	if (sSettingsParameters.TestTime)
		ListParam.AddParametr(sLanguageTabInfo.TestTime, BufStr);

	sprintf(BufStr, "%.2f %s", AnalyzeTime, sLanguageOther.Second);
	if (sSettingsParameters.AnalyzeTime)
		ListParam.AddParametr(sLanguageTabInfo.AnalyzeTime, BufStr);

	sprintf(BufStr, "%.2f %s", AllTime, sLanguageOther.Second);
	if (sSettingsParameters.AllTime)
		ListParam.AddParametr(sLanguageTabInfo.AllTime, BufStr);

	return 1;
}

//dlya archiva CPIO
int MySelectInfo::SelectInfoCpio()
{
	//dlya uprosheniya obrasheniya k dannim sobrannim v odnom meste
	SettingsMain&       sSettingsMain       = m_pSettings->sSettingsMain;
	SettingsParameters& sSettingsParameters = m_pSettings->sSettingsParameters;
	LanguageTabInfo&    sLanguageTabInfo    = m_pLanguage->sLanguageTabInfo;
	LanguageOS&         sLanguageOS         = m_pLanguage->sLanguageOS;
	LanguagePack&       sLanguagePack       = m_pLanguage->sLanguagePack;
	LanguageOther&      sLanguageOther      = m_pLanguage->sLanguageOther;
	MListParametrs&     ListParam           = *m_pListParam;

	TCpio* obj_cpio = (TCpio*)m_pArchData;

	ULONGLONG UnpackSizeFiles   = obj_cpio->GetUnpackSizeFiles();
	ULONGLONG PackSizeFiles     = obj_cpio->GetPackSizeFiles();
	ULONGLONG ArchiveSize       = obj_cpio->GetArchiveSize();
	unsigned  NumberFiles       = obj_cpio->GetNumberFiles();
	unsigned  NumberFolders     = obj_cpio->GetNumberFolders();
	double    RatioArchiveSize  = obj_cpio->GetRatioArchiveSize();
	double    RatioPackFileSize = obj_cpio->GetRatioPackFileSize();
	double    TestTime          = m_TestTime;
	double    AnalyzeTime       = obj_cpio->GetAnalyzeTime();
	double    AllTime           = m_TestTime + AnalyzeTime;

	if (((int)RatioPackFileSize == 0) && (RatioPackFileSize > 0)) *m_pRatio1 = 1;
	else *m_pRatio1 = (int)RatioPackFileSize;
	if (((int)RatioArchiveSize == 0) && (RatioArchiveSize > 0)) *m_pRatio2 = 1;
	else *m_pRatio2 = (int)RatioArchiveSize;

	char BufStr[MAX_LOADSTRING] = {NULL};

	//zagolovok
	char* _ARCH = m_pLanguage->sLanguageArchives.Cpio;
	sprintf(BufStr, "%s %s", _ARCH, sLanguageOther.Archive);
	ListParam.AddParametr("", BufStr);

	//neupakovanniy razmer
	if ((sSettingsMain.AllParametrs) || (UnpackSizeFiles))
	{
		if (!sSettingsMain.FloatSize) NumberSeparatorStringUnitSize(BufStr, UnpackSizeFiles, T_BYTE, sLanguageOther);
		else NumberFloatStringUnitSize(BufStr, UnpackSizeFiles, T_BYTE, sLanguageOther);
		if (sSettingsParameters.UnpackSizeFiles)
			ListParam.AddParametr(sLanguageTabInfo.UnpackSizeFiles, BufStr);
	}

	//upakovanniy razmer
	if ((sSettingsMain.AllParametrs) || (PackSizeFiles))
	{
		if (!sSettingsMain.FloatSize) NumberSeparatorStringUnitSize(BufStr, PackSizeFiles, T_BYTE, sLanguageOther);
		else NumberFloatStringUnitSize(BufStr, PackSizeFiles, T_BYTE, sLanguageOther);
		if (sSettingsParameters.PackSizeFiles)
			ListParam.AddParametr(sLanguageTabInfo.PackSizeFiles, BufStr);
	}

	//razmer arhiva
	if ((sSettingsMain.AllParametrs) || (ArchiveSize))
	{
		if (!sSettingsMain.FloatSize) NumberSeparatorStringUnitSize(BufStr, ArchiveSize, T_BYTE, sLanguageOther);
		else NumberFloatStringUnitSize(BufStr, ArchiveSize, T_BYTE, sLanguageOther);
		if (sSettingsParameters.ArchiveSize)
			ListParam.AddParametr(sLanguageTabInfo.ArchiveSize, BufStr);
	}

	//kolichestvo faylov
	if ((sSettingsMain.AllParametrs) || (NumberFiles))
	{
		if (NumberFiles) NumberSeparatorString(BufStr, NumberFiles);
		else sprintf(BufStr, "%s", sLanguageOther.No);
		if (sSettingsParameters.NumberFiles)
			ListParam.AddParametr(sLanguageTabInfo.NumberFiles, BufStr);
	}

	//kolichestvo papok
	if ((sSettingsMain.AllParametrs) || (NumberFolders))
	{
		if (NumberFolders) NumberSeparatorString(BufStr, NumberFolders);
		else sprintf(BufStr, "%s", sLanguageOther.No);
		if (sSettingsParameters.NumberFolders)
			ListParam.AddParametr(sLanguageTabInfo.NumberFolders, BufStr);
	}

	//razdelitel'
	ListParam.AddParametr("", "");

	//sjatie ot rzmera arhiva
	sprintf(BufStr, "%.1f %s", RatioArchiveSize, sLanguageOther.Percent);
	if (sSettingsParameters.RatioArchiveSize)
		ListParam.AddParametr(sLanguageTabInfo.RatioArchiveSize, BufStr);

	//sjatie ot upakovannogo razmera
	sprintf(BufStr, "%.1f %s", RatioPackFileSize, sLanguageOther.Percent);
	if (sSettingsParameters.RatioPackFileSize)
		ListParam.AddParametr(sLanguageTabInfo.RatioPackFileSize, BufStr);

	//razdelitel'
	ListParam.AddParametr("", "");

	sprintf(BufStr, "%.2f %s", TestTime, sLanguageOther.Second);
	if (sSettingsParameters.TestTime)
		ListParam.AddParametr(sLanguageTabInfo.TestTime, BufStr);

	sprintf(BufStr, "%.2f %s", AnalyzeTime, sLanguageOther.Second);
	if (sSettingsParameters.AnalyzeTime)
		ListParam.AddParametr(sLanguageTabInfo.AnalyzeTime, BufStr);

	sprintf(BufStr, "%.2f %s", AllTime, sLanguageOther.Second);
	if (sSettingsParameters.AllTime)
		ListParam.AddParametr(sLanguageTabInfo.AllTime, BufStr);

	return 1;
}

//dlya archiva DWC
int MySelectInfo::SelectInfoDwc()
{
	//dlya uprosheniya obrasheniya k dannim sobrannim v odnom meste
	SettingsMain&       sSettingsMain       = m_pSettings->sSettingsMain;
	SettingsParameters& sSettingsParameters = m_pSettings->sSettingsParameters;
	LanguageTabInfo&    sLanguageTabInfo    = m_pLanguage->sLanguageTabInfo;
	LanguageOS&         sLanguageOS         = m_pLanguage->sLanguageOS;
	LanguagePack&       sLanguagePack       = m_pLanguage->sLanguagePack;
	LanguageOther&      sLanguageOther      = m_pLanguage->sLanguageOther;
	MListParametrs&     ListParam           = *m_pListParam;

	TDwc* obj_dwc = (TDwc*)m_pArchData;

	ULONGLONG UnpackSizeFiles   = obj_dwc->GetUnpackSizeFiles();
	ULONGLONG PackSizeFiles     = obj_dwc->GetPackSizeFiles();
	ULONGLONG ArchiveSize       = obj_dwc->GetArchiveSize();
	unsigned  NumberFiles       = obj_dwc->GetNumberFiles();
	short     MethodPack        = obj_dwc->GetMethodPack();
	double    RatioArchiveSize  = obj_dwc->GetRatioArchiveSize();
	double    RatioPackFileSize = obj_dwc->GetRatioPackFileSize();
	unsigned  SfxModule         = obj_dwc->GetSfxModule();
	BOOL      FileComment       = obj_dwc->GetFileComment();
	BOOL      Password          = obj_dwc->GetPassword();
	double    TestTime          = m_TestTime;
	double    AnalyzeTime       = obj_dwc->GetAnalyzeTime();
	double    AllTime           = m_TestTime + AnalyzeTime;

	if (((int)RatioPackFileSize == 0) && (RatioPackFileSize > 0)) *m_pRatio1 = 1;
	else *m_pRatio1 = (int)RatioPackFileSize;
	if (((int)RatioArchiveSize == 0) && (RatioArchiveSize > 0)) *m_pRatio2 = 1;
	else *m_pRatio2 = (int)RatioArchiveSize;

	char BufStr[MAX_LOADSTRING] = {NULL};

	//zagolovok
	char* _ARCH = m_pLanguage->sLanguageArchives.Dwc;
	char* _SFX  = m_pLanguage->sLanguageArchives.Sfx;
	if (SfxModule) sprintf(BufStr, "%s %s %s", _SFX, _ARCH, sLanguageOther.Archive);
	else sprintf(BufStr, "%s %s", _ARCH, sLanguageOther.Archive);
	ListParam.AddParametr("", BufStr);

	//neupakovanniy razmer
	if ((sSettingsMain.AllParametrs) || (UnpackSizeFiles))
	{
		if (!sSettingsMain.FloatSize) NumberSeparatorStringUnitSize(BufStr, UnpackSizeFiles, T_BYTE, sLanguageOther);
		else NumberFloatStringUnitSize(BufStr, UnpackSizeFiles, T_BYTE, sLanguageOther);
		if (sSettingsParameters.UnpackSizeFiles)
			ListParam.AddParametr(sLanguageTabInfo.UnpackSizeFiles, BufStr);
	}

	//upakovanniy razmer
	if ((sSettingsMain.AllParametrs) || (PackSizeFiles))
	{
		if (!sSettingsMain.FloatSize) NumberSeparatorStringUnitSize(BufStr, PackSizeFiles, T_BYTE, sLanguageOther);
		else NumberFloatStringUnitSize(BufStr, PackSizeFiles, T_BYTE, sLanguageOther);
		if (sSettingsParameters.PackSizeFiles)
			ListParam.AddParametr(sLanguageTabInfo.PackSizeFiles, BufStr);
	}

	//razmer arhiva
	if ((sSettingsMain.AllParametrs) || (ArchiveSize))
	{
		if (!sSettingsMain.FloatSize) NumberSeparatorStringUnitSize(BufStr, ArchiveSize, T_BYTE, sLanguageOther);
		else NumberFloatStringUnitSize(BufStr, ArchiveSize, T_BYTE, sLanguageOther);
		if (sSettingsParameters.ArchiveSize)
			ListParam.AddParametr(sLanguageTabInfo.ArchiveSize, BufStr);
	}

	//kolichestvo faylov
	if ((sSettingsMain.AllParametrs) || (NumberFiles))
	{
		if (NumberFiles) NumberSeparatorString(BufStr, NumberFiles);
		else sprintf(BufStr, "%s", sLanguageOther.No);
		if (sSettingsParameters.NumberFiles)
			ListParam.AddParametr(sLanguageTabInfo.NumberFiles, BufStr);
	}

	//razdelitel'
	ListParam.AddParametr("", "");

	//metod upakovki
	if (sSettingsParameters.MethodPack)
		switch (MethodPack)
		{
		case 0x01:	sprintf(BufStr, "%s (%s)", sLanguagePack.Crunched, sLanguagePack.Normal);
					ListParam.AddParametr(sLanguageTabInfo.MethodPack, BufStr);                 break;
		case 0x02:	ListParam.AddParametr(sLanguageTabInfo.MethodPack, sLanguagePack.Stored);   break;
		default:	ListParam.AddParametr(sLanguageTabInfo.MethodPack, sLanguageOther.Unknown); break;
		}

	//sjatie ot rzmera arhiva
	sprintf(BufStr, "%.1f %s", RatioArchiveSize, sLanguageOther.Percent);
	if (sSettingsParameters.RatioArchiveSize)
		ListParam.AddParametr(sLanguageTabInfo.RatioArchiveSize, BufStr);

	//sjatie ot upakovannogo razmera
	sprintf(BufStr, "%.1f %s", RatioPackFileSize, sLanguageOther.Percent);
	if (sSettingsParameters.RatioPackFileSize)
		ListParam.AddParametr(sLanguageTabInfo.RatioPackFileSize, BufStr);

	//razdelitel'
	ListParam.AddParametr("", "");

	//SFX modul'
	if ((sSettingsMain.AllParametrs) || (SfxModule))
	{
		if (sSettingsParameters.SfxModule)
			if (!SfxModule) ListParam.AddParametr(sLanguageTabInfo.SfxModule, sLanguageOther.No);
			else
			{
				if (!sSettingsMain.FloatSize) NumberSeparatorStringUnitSize(BufStr, SfxModule, T_BYTE, sLanguageOther);
				else NumberFloatStringUnitSize(BufStr, SfxModule, T_BYTE, sLanguageOther);
				ListParam.AddParametr(sLanguageTabInfo.SfxModule, BufStr);
			}
	}

	//kommentarii faylov
	if ((sSettingsMain.AllParametrs) || (FileComment))
	{
		if (sSettingsParameters.FileComment)
			if (!FileComment) ListParam.AddParametr(sLanguageTabInfo.FileComment, sLanguageOther.No);
			else ListParam.AddParametr(sLanguageTabInfo.FileComment, sLanguageOther.Yes);
	}

	//parol'
	if ((sSettingsMain.AllParametrs) || (Password))
	{
		if (sSettingsParameters.Password)
			if (!Password) ListParam.AddParametr(sLanguageTabInfo.Password, sLanguageOther.No);
			else ListParam.AddParametr(sLanguageTabInfo.Password, sLanguageOther.Yes);
	}

	//razdelitel'
	ListParam.AddParametr("", "");

	sprintf(BufStr, "%.2f %s", TestTime, sLanguageOther.Second);
	if (sSettingsParameters.TestTime)
		ListParam.AddParametr(sLanguageTabInfo.TestTime, BufStr);

	sprintf(BufStr, "%.2f %s", AnalyzeTime, sLanguageOther.Second);
	if (sSettingsParameters.AnalyzeTime)
		ListParam.AddParametr(sLanguageTabInfo.AnalyzeTime, BufStr);

	sprintf(BufStr, "%.2f %s", AllTime, sLanguageOther.Second);
	if (sSettingsParameters.AllTime)
		ListParam.AddParametr(sLanguageTabInfo.AllTime, BufStr);

	return 1;
}

//dlya archiva GZIP/TGZ
int MySelectInfo::SelectInfoGZipTgz()
{
	//dlya uprosheniya obrasheniya k dannim sobrannim v odnom meste
	SettingsMain&       sSettingsMain       = m_pSettings->sSettingsMain;
	SettingsParameters& sSettingsParameters = m_pSettings->sSettingsParameters;
	LanguageTabInfo&    sLanguageTabInfo    = m_pLanguage->sLanguageTabInfo;
	LanguageOS&         sLanguageOS         = m_pLanguage->sLanguageOS;
	LanguagePack&       sLanguagePack       = m_pLanguage->sLanguagePack;
	LanguageOther&      sLanguageOther      = m_pLanguage->sLanguageOther;
	MListParametrs&     ListParam           = *m_pListParam;

	TGZipTgz* obj_gziptgz = (TGZipTgz*)m_pArchData;

	ULONGLONG UnpackSizeFiles   = obj_gziptgz->GetUnpackSizeFiles();
	ULONGLONG PackSizeFiles     = obj_gziptgz->GetPackSizeFiles();
	ULONGLONG ArchiveSize       = obj_gziptgz->GetArchiveSize();
	unsigned  NumberFiles       = obj_gziptgz->GetNumberFiles();
	char*     pOriginalName     = obj_gziptgz->GetOriginalName();
	short     HostOS            = obj_gziptgz->GetHostOS();
	short     MethodPack        = obj_gziptgz->GetMethodPack();
	short     MethodPackAdd     = obj_gziptgz->GetMethodPackAdd();
	double    RatioArchiveSize  = obj_gziptgz->GetRatioArchiveSize();
	double    RatioPackFileSize = obj_gziptgz->GetRatioPackFileSize();
	BOOL      MainComment       = obj_gziptgz->GetMainComment();
	double    TestTime          = m_TestTime;
	double    AnalyzeTime       = obj_gziptgz->GetAnalyzeTime();
	double    AllTime           = m_TestTime + AnalyzeTime;

	if (((int)RatioPackFileSize == 0) && (RatioPackFileSize > 0)) *m_pRatio1 = 1;
	else *m_pRatio1 = (int)RatioPackFileSize;
	if (((int)RatioArchiveSize == 0) && (RatioArchiveSize > 0)) *m_pRatio2 = 1;
	else *m_pRatio2 = (int)RatioArchiveSize;

	char BufStr[MAX_LOADSTRING] = {NULL};

	//zagolovok
	char* _ARCH = m_pLanguage->sLanguageArchives.GZip;
	sprintf(BufStr, "%s %s", _ARCH, sLanguageOther.Archive);
	ListParam.AddParametr("", BufStr);

	//neupakovanniy razmer
	if ((sSettingsMain.AllParametrs) || (UnpackSizeFiles))
	{
		if (!sSettingsMain.FloatSize) NumberSeparatorStringUnitSize(BufStr, UnpackSizeFiles, T_BYTE, sLanguageOther);
		else NumberFloatStringUnitSize(BufStr, UnpackSizeFiles, T_BYTE, sLanguageOther);
		if (sSettingsParameters.UnpackSizeFiles)
			ListParam.AddParametr(sLanguageTabInfo.UnpackSizeFiles, BufStr);
	}

	//upakovanniy razmer
	if ((sSettingsMain.AllParametrs) || (PackSizeFiles))
	{
		if (!sSettingsMain.FloatSize) NumberSeparatorStringUnitSize(BufStr, PackSizeFiles, T_BYTE, sLanguageOther);
		else NumberFloatStringUnitSize(BufStr, PackSizeFiles, T_BYTE, sLanguageOther);
		if (sSettingsParameters.PackSizeFiles)
			ListParam.AddParametr(sLanguageTabInfo.PackSizeFiles, BufStr);
	}

	//razmer arhiva
	if ((sSettingsMain.AllParametrs) || (ArchiveSize))
	{
		if (!sSettingsMain.FloatSize) NumberSeparatorStringUnitSize(BufStr, ArchiveSize, T_BYTE, sLanguageOther);
		else NumberFloatStringUnitSize(BufStr, ArchiveSize, T_BYTE, sLanguageOther);
		if (sSettingsParameters.ArchiveSize)
			ListParam.AddParametr(sLanguageTabInfo.ArchiveSize, BufStr);
	}

	//kolichestvo faylov
	if ((sSettingsMain.AllParametrs) || (NumberFiles))
	{
		if (NumberFiles) NumberSeparatorString(BufStr, NumberFiles);
		else sprintf(BufStr, "%s", sLanguageOther.No);
		if (sSettingsParameters.NumberFiles)
			ListParam.AddParametr(sLanguageTabInfo.NumberFiles, BufStr);
	}

	//original'noe imya
	if ((sSettingsMain.AllParametrs) || (pOriginalName))
	{
		if (sSettingsParameters.OriginalName)
			if (pOriginalName) ListParam.AddParametr(sLanguageTabInfo.OriginalName, pOriginalName);
			else ListParam.AddParametr(sLanguageTabInfo.OriginalName, sLanguageOther.No);
	}

	//razdelitel'
	ListParam.AddParametr("", "");

	//host OS
	if (sSettingsParameters.HostOS)
		switch (HostOS)
		{
		case 0x00: ListParam.AddParametr(sLanguageTabInfo.HostOS, sLanguageOS.DOS);        break;
		case 0x01: ListParam.AddParametr(sLanguageTabInfo.HostOS, sLanguageOS.Amiga);      break;
		case 0x02: ListParam.AddParametr(sLanguageTabInfo.HostOS, sLanguageOS.VaxVMS);     break;
		case 0x03: ListParam.AddParametr(sLanguageTabInfo.HostOS, sLanguageOS.Unix);       break;
		case 0x04: ListParam.AddParametr(sLanguageTabInfo.HostOS, sLanguageOS.VMCMS);      break;
		case 0x05: ListParam.AddParametr(sLanguageTabInfo.HostOS, sLanguageOS.AtariST);    break;
		case 0x06: ListParam.AddParametr(sLanguageTabInfo.HostOS, sLanguageOS.OS2);        break;
		case 0x07: ListParam.AddParametr(sLanguageTabInfo.HostOS, sLanguageOS.MacOS);      break;
		case 0x08: ListParam.AddParametr(sLanguageTabInfo.HostOS, sLanguageOS.ZSystem);    break;
		case 0x09: ListParam.AddParametr(sLanguageTabInfo.HostOS, sLanguageOS.CPM);        break;
		case 0x0A: ListParam.AddParametr(sLanguageTabInfo.HostOS, sLanguageOS.TOPS20);     break;
		case 0x0B: ListParam.AddParametr(sLanguageTabInfo.HostOS, sLanguageOS.WinNT);      break;
		case 0x0C: ListParam.AddParametr(sLanguageTabInfo.HostOS, sLanguageOS.SMSQDOS);    break;
		case 0x0D: ListParam.AddParametr(sLanguageTabInfo.HostOS, sLanguageOS.AcornRISC);  break;
		default:   ListParam.AddParametr(sLanguageTabInfo.HostOS, sLanguageOther.Unknown); break;
		}

	//metod upakovki
	if (sSettingsParameters.MethodPack)
		switch (MethodPack)
		{
		case 0x08:
			switch (MethodPackAdd)
			{
			case 0x00:	sprintf(BufStr, "%s (%s)", sLanguagePack.Deflated, sLanguagePack.Normal);
						ListParam.AddParametr(sLanguageTabInfo.MethodPack, BufStr);            break;
			case 0x02:	sprintf(BufStr, "%s (%s)", sLanguagePack.Deflated, sLanguagePack.Best);
						ListParam.AddParametr(sLanguageTabInfo.MethodPack, BufStr);            break;
			case 0x04:	sprintf(BufStr, "%s (%s)", sLanguagePack.Deflated, sLanguagePack.Fast);
						ListParam.AddParametr(sLanguageTabInfo.MethodPack, BufStr);            break;
			default:	sprintf(BufStr, "%s (%s)", sLanguagePack.Deflated, sLanguageOther.Unknown);	
						ListParam.AddParametr(sLanguageTabInfo.MethodPack, BufStr);            break;
			}
			break;
		default:   ListParam.AddParametr(sLanguageTabInfo.MethodPack, sLanguageOther.Unknown); break;
		}

	//sjatie ot rzmera arhiva
	sprintf(BufStr, "%.1f %s", RatioArchiveSize, sLanguageOther.Percent);
	if (sSettingsParameters.RatioArchiveSize)
		ListParam.AddParametr(sLanguageTabInfo.RatioArchiveSize, BufStr);

	//sjatie ot upakovannogo razmera
	sprintf(BufStr, "%.1f %s", RatioPackFileSize, sLanguageOther.Percent);
	if (sSettingsParameters.RatioPackFileSize)
		ListParam.AddParametr(sLanguageTabInfo.RatioPackFileSize, BufStr);

	//razdelitel'
	ListParam.AddParametr("", "");

	//kommentariy arhiva
	if ((sSettingsMain.AllParametrs) || (MainComment))
	{
		if (sSettingsParameters.MainComment)
			if (!MainComment) ListParam.AddParametr(sLanguageTabInfo.MainComment, sLanguageOther.No);
			else ListParam.AddParametr(sLanguageTabInfo.MainComment, sLanguageOther.Yes);
	}

	//razdelitel'
	ListParam.AddParametr("", "");

	sprintf(BufStr, "%.2f %s", TestTime, sLanguageOther.Second);
	if (sSettingsParameters.TestTime)
		ListParam.AddParametr(sLanguageTabInfo.TestTime, BufStr);

	sprintf(BufStr, "%.2f %s", AnalyzeTime, sLanguageOther.Second);
	if (sSettingsParameters.AnalyzeTime)
		ListParam.AddParametr(sLanguageTabInfo.AnalyzeTime, BufStr);

	sprintf(BufStr, "%.2f %s", AllTime, sLanguageOther.Second);
	if (sSettingsParameters.AllTime)
		ListParam.AddParametr(sLanguageTabInfo.AllTime, BufStr);

	return 1;
}

//dlya archiva HA
int MySelectInfo::SelectInfoHa()
{
	//dlya uprosheniya obrasheniya k dannim sobrannim v odnom meste
	SettingsMain&       sSettingsMain       = m_pSettings->sSettingsMain;
	SettingsParameters& sSettingsParameters = m_pSettings->sSettingsParameters;
	LanguageTabInfo&    sLanguageTabInfo    = m_pLanguage->sLanguageTabInfo;
	LanguageOS&         sLanguageOS         = m_pLanguage->sLanguageOS;
	LanguagePack&       sLanguagePack       = m_pLanguage->sLanguagePack;
	LanguageOther&      sLanguageOther      = m_pLanguage->sLanguageOther;
	MListParametrs&     ListParam           = *m_pListParam;

	THa* obj_ha = (THa*)m_pArchData;

	ULONGLONG UnpackSizeFiles   = obj_ha->GetUnpackSizeFiles();
	ULONGLONG PackSizeFiles     = obj_ha->GetPackSizeFiles();
	ULONGLONG ArchiveSize       = obj_ha->GetArchiveSize();
	unsigned  NumberFiles       = obj_ha->GetNumberFiles();
	unsigned  NumberFolders     = obj_ha->GetNumberFolders();
	short     HostOS            = obj_ha->GetHostOS();
	short     MethodPack        = obj_ha->GetMethodPack();
	double    RatioArchiveSize  = obj_ha->GetRatioArchiveSize();
	double    RatioPackFileSize = obj_ha->GetRatioPackFileSize();
	double    TestTime          = m_TestTime;
	double    AnalyzeTime       = obj_ha->GetAnalyzeTime();
	double    AllTime           = m_TestTime + AnalyzeTime;

	if (((int)RatioPackFileSize == 0) && (RatioPackFileSize > 0)) *m_pRatio1 = 1;
	else *m_pRatio1 = (int)RatioPackFileSize;
	if (((int)RatioArchiveSize == 0) && (RatioArchiveSize > 0)) *m_pRatio2 = 1;
	else *m_pRatio2 = (int)RatioArchiveSize;

	char BufStr[MAX_LOADSTRING] = {NULL};

	//zagolovok
	char* _ARCH = m_pLanguage->sLanguageArchives.Ha;
	sprintf(BufStr, "%s %s", _ARCH, sLanguageOther.Archive);
	ListParam.AddParametr("", BufStr);

	//neupakovanniy razmer
	if ((sSettingsMain.AllParametrs) || (UnpackSizeFiles))
	{
		if (!sSettingsMain.FloatSize) NumberSeparatorStringUnitSize(BufStr, UnpackSizeFiles, T_BYTE, sLanguageOther);
		else NumberFloatStringUnitSize(BufStr, UnpackSizeFiles, T_BYTE, sLanguageOther);
		if (sSettingsParameters.UnpackSizeFiles)
			ListParam.AddParametr(sLanguageTabInfo.UnpackSizeFiles, BufStr);
	}

	//upakovanniy razmer
	if ((sSettingsMain.AllParametrs) || (PackSizeFiles))
	{
		if (!sSettingsMain.FloatSize) NumberSeparatorStringUnitSize(BufStr, PackSizeFiles, T_BYTE, sLanguageOther);
		else NumberFloatStringUnitSize(BufStr, PackSizeFiles, T_BYTE, sLanguageOther);
		if (sSettingsParameters.PackSizeFiles)
			ListParam.AddParametr(sLanguageTabInfo.PackSizeFiles, BufStr);
	}

	//razmer arhiva
	if ((sSettingsMain.AllParametrs) || (ArchiveSize))
	{
		if (!sSettingsMain.FloatSize) NumberSeparatorStringUnitSize(BufStr, ArchiveSize, T_BYTE, sLanguageOther);
		else NumberFloatStringUnitSize(BufStr, ArchiveSize, T_BYTE, sLanguageOther);
		if (sSettingsParameters.ArchiveSize)
			ListParam.AddParametr(sLanguageTabInfo.ArchiveSize, BufStr);
	}

	//kolichestvo faylov
	if ((sSettingsMain.AllParametrs) || (NumberFiles))
	{
		if (NumberFiles) NumberSeparatorString(BufStr, NumberFiles);
		else sprintf(BufStr, "%s", sLanguageOther.No);
		if (sSettingsParameters.NumberFiles)
			ListParam.AddParametr(sLanguageTabInfo.NumberFiles, BufStr);
	}

	//kolichestvo papok
	if ((sSettingsMain.AllParametrs) || (NumberFolders))
	{
		if (NumberFolders) NumberSeparatorString(BufStr, NumberFolders);
		else sprintf(BufStr, "%s", sLanguageOther.No);
		if (sSettingsParameters.NumberFolders)
			ListParam.AddParametr(sLanguageTabInfo.NumberFolders, BufStr);
	}

	//razdelitel'
	ListParam.AddParametr("", "");

	//host OS
	if (sSettingsParameters.HostOS)
		switch (HostOS)
		{
		case 0x01: ListParam.AddParametr(sLanguageTabInfo.HostOS, sLanguageOS.DOS);     break;
		case 0x02: ListParam.AddParametr(sLanguageTabInfo.HostOS, sLanguageOS.Unix);       break;
		default:   ListParam.AddParametr(sLanguageTabInfo.HostOS, sLanguageOther.Unknown); break;
		}

	//metod upakovki
	if (sSettingsParameters.MethodPack)
		switch (MethodPack)
		{
		case 0x00:	sprintf(BufStr, "%s (%s)", sLanguagePack.CPY, sLanguagePack.Stored);
					ListParam.AddParametr(sLanguageTabInfo.MethodPack, BufStr);                 break;
		case 0x01:	sprintf(BufStr, "%s (%s)", sLanguagePack.ASC, sLanguagePack.Normal);
					ListParam.AddParametr(sLanguageTabInfo.MethodPack, BufStr);                 break;
		case 0x02:	sprintf(BufStr, "%s (%s)", sLanguagePack.HSC, sLanguagePack.Good);
					ListParam.AddParametr(sLanguageTabInfo.MethodPack, BufStr);                 break;
		default:	ListParam.AddParametr(sLanguageTabInfo.MethodPack, sLanguageOther.Unknown); break;
		}

	//sjatie ot rzmera arhiva
	sprintf(BufStr, "%.1f %s", RatioArchiveSize, sLanguageOther.Percent);
	if (sSettingsParameters.RatioArchiveSize)
		ListParam.AddParametr(sLanguageTabInfo.RatioArchiveSize, BufStr);

	//sjatie ot upakovannogo razmera
	sprintf(BufStr, "%.1f %s", RatioPackFileSize, sLanguageOther.Percent);
	if (sSettingsParameters.RatioPackFileSize)
		ListParam.AddParametr(sLanguageTabInfo.RatioPackFileSize, BufStr);

	//razdelitel'
	ListParam.AddParametr("", "");

	sprintf(BufStr, "%.2f %s", TestTime, sLanguageOther.Second);
	if (sSettingsParameters.TestTime)
		ListParam.AddParametr(sLanguageTabInfo.TestTime, BufStr);

	sprintf(BufStr, "%.2f %s", AnalyzeTime, sLanguageOther.Second);
	if (sSettingsParameters.AnalyzeTime)
		ListParam.AddParametr(sLanguageTabInfo.AnalyzeTime, BufStr);

	sprintf(BufStr, "%.2f %s", AllTime, sLanguageOther.Second);
	if (sSettingsParameters.AllTime)
		ListParam.AddParametr(sLanguageTabInfo.AllTime, BufStr);

	return 1;
}

//dlya archiva LHA/LZH
int MySelectInfo::SelectInfoLhaLzh()
{
	//dlya uprosheniya obrasheniya k dannim sobrannim v odnom meste
	SettingsMain&       sSettingsMain       = m_pSettings->sSettingsMain;
	SettingsParameters& sSettingsParameters = m_pSettings->sSettingsParameters;
	LanguageTabInfo&    sLanguageTabInfo    = m_pLanguage->sLanguageTabInfo;
	LanguageOS&         sLanguageOS         = m_pLanguage->sLanguageOS;
	LanguagePack&       sLanguagePack       = m_pLanguage->sLanguagePack;
	LanguageOther&      sLanguageOther      = m_pLanguage->sLanguageOther;
	MListParametrs&     ListParam           = *m_pListParam;

	TLhaLzh* obj_lhalzh = (TLhaLzh*)m_pArchData;

	ULONGLONG UnpackSizeFiles   = obj_lhalzh->GetUnpackSizeFiles();
	ULONGLONG PackSizeFiles     = obj_lhalzh->GetPackSizeFiles();
	ULONGLONG ArchiveSize       = obj_lhalzh->GetArchiveSize();
	unsigned  NumberFiles       = obj_lhalzh->GetNumberFiles();
	unsigned  NumberFolders     = obj_lhalzh->GetNumberFolders();
	unsigned  DictionarySize    = obj_lhalzh->GetDictionarySize();
	short     MethodPack        = obj_lhalzh->GetMethodPack();
	double    RatioArchiveSize  = obj_lhalzh->GetRatioArchiveSize();
	double    RatioPackFileSize = obj_lhalzh->GetRatioPackFileSize();
	unsigned  SfxModule         = obj_lhalzh->GetSfxModule();
	BOOL      ArchiveType       = obj_lhalzh->GetArchiveType();
	double    TestTime          = m_TestTime;
	double    AnalyzeTime       = obj_lhalzh->GetAnalyzeTime();
	double    AllTime           = m_TestTime + AnalyzeTime;

	if (((int)RatioPackFileSize == 0) && (RatioPackFileSize > 0)) *m_pRatio1 = 1;
	else *m_pRatio1 = (int)RatioPackFileSize;
	if (((int)RatioArchiveSize == 0) && (RatioArchiveSize > 0)) *m_pRatio2 = 1;
	else *m_pRatio2 = (int)RatioArchiveSize;

	char BufStr[MAX_LOADSTRING] = {NULL};

	//zagolovok
	char* _ARCH1 = m_pLanguage->sLanguageArchives.Lha;
	char* _ARCH2 = m_pLanguage->sLanguageArchives.Lzh;
	char* _SFX   = "SFX";
	if (!ArchiveType)
		if (SfxModule) sprintf(BufStr, "%s %s %s", _SFX, _ARCH1, sLanguageOther.Archive);
		else sprintf(BufStr, "%s %s", _ARCH1, sLanguageOther.Archive);
	else
		if (SfxModule) sprintf(BufStr, "%s %s %s", _SFX, _ARCH2, sLanguageOther.Archive);
		else sprintf(BufStr, "%s %s", _ARCH2, sLanguageOther.Archive);
	ListParam.AddParametr("", BufStr);

	//neupakovanniy razmer
	if ((sSettingsMain.AllParametrs) || (UnpackSizeFiles))
	{
		if (!sSettingsMain.FloatSize) NumberSeparatorStringUnitSize(BufStr, UnpackSizeFiles, T_BYTE, sLanguageOther);
		else NumberFloatStringUnitSize(BufStr, UnpackSizeFiles, T_BYTE, sLanguageOther);
		if (sSettingsParameters.UnpackSizeFiles)
			ListParam.AddParametr(sLanguageTabInfo.UnpackSizeFiles, BufStr);
	}

	//upakovanniy razmer
	if ((sSettingsMain.AllParametrs) || (PackSizeFiles))
	{
		if (!sSettingsMain.FloatSize) NumberSeparatorStringUnitSize(BufStr, PackSizeFiles, T_BYTE, sLanguageOther);
		else NumberFloatStringUnitSize(BufStr, PackSizeFiles, T_BYTE, sLanguageOther);
		if (sSettingsParameters.PackSizeFiles)
			ListParam.AddParametr(sLanguageTabInfo.PackSizeFiles, BufStr);
	}

	//razmer arhiva
	if ((sSettingsMain.AllParametrs) || (ArchiveSize))
	{
		if (!sSettingsMain.FloatSize) NumberSeparatorStringUnitSize(BufStr, ArchiveSize, T_BYTE, sLanguageOther);
		else NumberFloatStringUnitSize(BufStr, ArchiveSize, T_BYTE, sLanguageOther);
		if (sSettingsParameters.ArchiveSize)
			ListParam.AddParametr(sLanguageTabInfo.ArchiveSize, BufStr);
	}

	//kolichestvo faylov
	if ((sSettingsMain.AllParametrs) || (NumberFiles))
	{
		if (NumberFiles) NumberSeparatorString(BufStr, NumberFiles);
		else sprintf(BufStr, "%s", sLanguageOther.No);
		if (sSettingsParameters.NumberFiles)
			ListParam.AddParametr(sLanguageTabInfo.NumberFiles, BufStr);
	}

	//kolichestvo papok
	if ((sSettingsMain.AllParametrs) || (NumberFolders))
	{
		if (NumberFolders) NumberSeparatorString(BufStr, NumberFolders);
		else sprintf(BufStr, "%s", sLanguageOther.No);
		if (sSettingsParameters.NumberFolders)
			ListParam.AddParametr(sLanguageTabInfo.NumberFolders, BufStr);
	}

	//razdelitel'
	ListParam.AddParametr("", "");

	//razmer slovarya
	if (!sSettingsMain.FloatSize) NumberSeparatorStringUnitSize(BufStr, DictionarySize, T_KBYTE, sLanguageOther);
	else NumberFloatStringUnitSize(BufStr, DictionarySize, T_KBYTE, sLanguageOther);
	if (sSettingsParameters.DictionarySize)
		ListParam.AddParametr(sLanguageTabInfo.DictionarySize, BufStr);

	//metod upakovki
	if (sSettingsParameters.MethodPack)
		switch (MethodPack)
		{
		case 0x00:	sprintf(BufStr, "%s (%s)", sLanguagePack.lh0, sLanguagePack.Stored);
					ListParam.AddParametr(sLanguageTabInfo.MethodPack, BufStr);                 break;
		case 0x01:	sprintf(BufStr, "%s (%s)", sLanguagePack.lh1, sLanguagePack.Superfast);
					ListParam.AddParametr(sLanguageTabInfo.MethodPack, BufStr);                 break;
		case 0x02:	sprintf(BufStr, "%s (%s)", sLanguagePack.lh2, sLanguagePack.Fastest);
					ListParam.AddParametr(sLanguageTabInfo.MethodPack, BufStr);                 break;
		case 0x03:	sprintf(BufStr, "%s (%s)", sLanguagePack.lh3, sLanguagePack.Fast);
					ListParam.AddParametr(sLanguageTabInfo.MethodPack, BufStr);                 break;
		case 0x04:	sprintf(BufStr, "%s (%s)", sLanguagePack.lh4, sLanguagePack.Normal);
					ListParam.AddParametr(sLanguageTabInfo.MethodPack, BufStr);                 break;
		case 0x05:	sprintf(BufStr, "%s (%s)", sLanguagePack.lh5, sLanguagePack.Good);
					ListParam.AddParametr(sLanguageTabInfo.MethodPack, BufStr);                 break;
		case 0x06:	sprintf(BufStr, "%s (%s)", sLanguagePack.lh6, sLanguagePack.Verygood);
					ListParam.AddParametr(sLanguageTabInfo.MethodPack, BufStr);                 break;
		case 0x07:	sprintf(BufStr, "%s (%s)", sLanguagePack.lh7, sLanguagePack.Best);
					ListParam.AddParametr(sLanguageTabInfo.MethodPack, BufStr);                 break;
		case 0x08:	sprintf(BufStr, "%s (%s)", sLanguagePack.lzs, sLanguagePack.Normal);
					ListParam.AddParametr(sLanguageTabInfo.MethodPack, BufStr);                 break;
		case 0x09:	sprintf(BufStr, "%s (%s)", sLanguagePack.lz4, sLanguagePack.Stored);
					ListParam.AddParametr(sLanguageTabInfo.MethodPack, BufStr);                 break;
		case 0x0A:	sprintf(BufStr, "%s (%s)", sLanguagePack.lz5, sLanguagePack.Good);
					ListParam.AddParametr(sLanguageTabInfo.MethodPack, BufStr);                 break;
		default:	ListParam.AddParametr(sLanguageTabInfo.MethodPack, sLanguageOther.Unknown); break;
		}

	//sjatie ot rzmera arhiva
	sprintf(BufStr, "%.1f %s", RatioArchiveSize, sLanguageOther.Percent);
	if (sSettingsParameters.RatioArchiveSize)
		ListParam.AddParametr(sLanguageTabInfo.RatioArchiveSize, BufStr);

	//sjatie ot upakovannogo razmera
	sprintf(BufStr, "%.1f %s", RatioPackFileSize, sLanguageOther.Percent);
	if (sSettingsParameters.RatioPackFileSize)
		ListParam.AddParametr(sLanguageTabInfo.RatioPackFileSize, BufStr);

	//razdelitel'
	ListParam.AddParametr("", "");

	//SFX modul'
	if ((sSettingsMain.AllParametrs) || (SfxModule))
	{
		if (sSettingsParameters.SfxModule)
			if (!SfxModule) ListParam.AddParametr(sLanguageTabInfo.SfxModule, sLanguageOther.No);
			else
			{
				if (!sSettingsMain.FloatSize) NumberSeparatorStringUnitSize(BufStr, SfxModule, T_BYTE, sLanguageOther);
				else NumberFloatStringUnitSize(BufStr, SfxModule, T_BYTE, sLanguageOther);
				ListParam.AddParametr(sLanguageTabInfo.SfxModule, BufStr);
			}
	}

	//razdelitel'
	ListParam.AddParametr("", "");

	sprintf(BufStr, "%.2f %s", TestTime, sLanguageOther.Second);
	if (sSettingsParameters.TestTime)
		ListParam.AddParametr(sLanguageTabInfo.TestTime, BufStr);

	sprintf(BufStr, "%.2f %s", AnalyzeTime, sLanguageOther.Second);
	if (sSettingsParameters.AnalyzeTime)
		ListParam.AddParametr(sLanguageTabInfo.AnalyzeTime, BufStr);

	sprintf(BufStr, "%.2f %s", AllTime, sLanguageOther.Second);
	if (sSettingsParameters.AllTime)
		ListParam.AddParametr(sLanguageTabInfo.AllTime, BufStr);

	return 1;
}

//dlya archiva PPM
int MySelectInfo::SelectInfoPpm()
{
	//dlya uprosheniya obrasheniya k dannim sobrannim v odnom meste
	SettingsMain&       sSettingsMain       = m_pSettings->sSettingsMain;
	SettingsParameters& sSettingsParameters = m_pSettings->sSettingsParameters;
	LanguageTabInfo&    sLanguageTabInfo    = m_pLanguage->sLanguageTabInfo;
	LanguageOS&         sLanguageOS         = m_pLanguage->sLanguageOS;
	LanguagePack&       sLanguagePack       = m_pLanguage->sLanguagePack;
	LanguageOther&      sLanguageOther      = m_pLanguage->sLanguageOther;
	MListParametrs&     ListParam           = *m_pListParam;

	TPpm* obj_ppm = (TPpm*)m_pArchData;

	ULONGLONG UnpackSizeFiles   = obj_ppm->GetUnpackSizeFiles();
	ULONGLONG PackSizeFiles     = obj_ppm->GetPackSizeFiles();
	ULONGLONG ArchiveSize       = obj_ppm->GetArchiveSize();
	unsigned  NumberFiles       = obj_ppm->GetNumberFiles();
	short     MethodPack        = obj_ppm->GetMethodPack();
	unsigned  MemorySize        = obj_ppm->GetMemorySize();
	double    RatioArchiveSize  = obj_ppm->GetRatioArchiveSize();
	double    RatioPackFileSize = obj_ppm->GetRatioPackFileSize();
	double    TestTime          = m_TestTime;
	double    AnalyzeTime       = obj_ppm->GetAnalyzeTime();
	double    AllTime           = m_TestTime + AnalyzeTime;

	if (((int)RatioPackFileSize == 0) && (RatioPackFileSize > 0)) *m_pRatio1 = 1;
	else *m_pRatio1 = (int)RatioPackFileSize;
	if (((int)RatioArchiveSize == 0) && (RatioArchiveSize > 0)) *m_pRatio2 = 1;
	else *m_pRatio2 = (int)RatioArchiveSize;

	char BufStr[MAX_LOADSTRING] = {NULL};

	//zagolovok
	char* _ARCH = m_pLanguage->sLanguageArchives.Ppm;
	sprintf(BufStr, "%s %s", _ARCH, sLanguageOther.Archive);
	ListParam.AddParametr("", BufStr);

	//neupakovanniy razmer
	if ((sSettingsMain.AllParametrs) || (UnpackSizeFiles))
	{
		if (!sSettingsMain.FloatSize) NumberSeparatorStringUnitSize(BufStr, UnpackSizeFiles, T_BYTE, sLanguageOther);
		else NumberFloatStringUnitSize(BufStr, UnpackSizeFiles, T_BYTE, sLanguageOther);
		if (sSettingsParameters.UnpackSizeFiles)
			ListParam.AddParametr(sLanguageTabInfo.UnpackSizeFiles, BufStr);
	}

	//upakovanniy razmer
	if ((sSettingsMain.AllParametrs) || (PackSizeFiles))
	{
		if (!sSettingsMain.FloatSize) NumberSeparatorStringUnitSize(BufStr, PackSizeFiles, T_BYTE, sLanguageOther);
		else NumberFloatStringUnitSize(BufStr, PackSizeFiles, T_BYTE, sLanguageOther);
		if (sSettingsParameters.PackSizeFiles)
			ListParam.AddParametr(sLanguageTabInfo.PackSizeFiles, BufStr);
	}

	//razmer arhiva
	if ((sSettingsMain.AllParametrs) || (ArchiveSize))
	{
		if (!sSettingsMain.FloatSize) NumberSeparatorStringUnitSize(BufStr, ArchiveSize, T_BYTE, sLanguageOther);
		else NumberFloatStringUnitSize(BufStr, ArchiveSize, T_BYTE, sLanguageOther);
		if (sSettingsParameters.ArchiveSize)
			ListParam.AddParametr(sLanguageTabInfo.ArchiveSize, BufStr);
	}

	//kolichestvo faylov
	if ((sSettingsMain.AllParametrs) || (NumberFiles))
	{
		if (NumberFiles) NumberSeparatorString(BufStr, NumberFiles);
		else sprintf(BufStr, "%s", sLanguageOther.No);
		if (sSettingsParameters.NumberFiles)
			ListParam.AddParametr(sLanguageTabInfo.NumberFiles, BufStr);
	}

	//razdelitel'
	ListParam.AddParametr("", "");

	//metod upakovki
	if (sSettingsParameters.MethodPack)
		switch (MethodPack)
		{
		case 0x02:	sprintf(BufStr, "%s%s1 (%s)", sLanguagePack.Rate, sLanguageOther.Number, sLanguagePack.Superfast);
					ListParam.AddParametr(sLanguageTabInfo.MethodPack, BufStr);                 break;
		case 0x03:	sprintf(BufStr, "%s%s2 (%s)", sLanguagePack.Rate, sLanguageOther.Number, sLanguagePack.Superfast);
					ListParam.AddParametr(sLanguageTabInfo.MethodPack, BufStr);                 break;
		case 0x04:	sprintf(BufStr, "%s%s3 (%s)", sLanguagePack.Rate, sLanguageOther.Number, sLanguagePack.Fastest);
					ListParam.AddParametr(sLanguageTabInfo.MethodPack, BufStr);                 break;
		case 0x05:	sprintf(BufStr, "%s%s4 (%s)", sLanguagePack.Rate, sLanguageOther.Number, sLanguagePack.Fastest);
					ListParam.AddParametr(sLanguageTabInfo.MethodPack, BufStr);                 break;
		case 0x06:	sprintf(BufStr, "%s%s5 (%s)", sLanguagePack.Rate, sLanguageOther.Number, sLanguagePack.Fast);
					ListParam.AddParametr(sLanguageTabInfo.MethodPack, BufStr);                 break;
		case 0x07:	sprintf(BufStr, "%s%s6 (%s)", sLanguagePack.Rate, sLanguageOther.Number, sLanguagePack.Fast);
					ListParam.AddParametr(sLanguageTabInfo.MethodPack, BufStr);                 break;
		case 0x08:	sprintf(BufStr, "%s%s7 (%s)", sLanguagePack.Rate, sLanguageOther.Number, sLanguagePack.Normal);
					ListParam.AddParametr(sLanguageTabInfo.MethodPack, BufStr);                 break;
		case 0x09:	sprintf(BufStr, "%s%s8 (%s)", sLanguagePack.Rate, sLanguageOther.Number, sLanguagePack.Normal);
					ListParam.AddParametr(sLanguageTabInfo.MethodPack, BufStr);                 break;
		case 0x0A:	sprintf(BufStr, "%s%s9 (%s)", sLanguagePack.Rate, sLanguageOther.Number, sLanguagePack.Normal);
					ListParam.AddParametr(sLanguageTabInfo.MethodPack, BufStr);                 break;
		case 0x0B:	sprintf(BufStr, "%s%s10 (%s)", sLanguagePack.Rate, sLanguageOther.Number, sLanguagePack.Good);
					ListParam.AddParametr(sLanguageTabInfo.MethodPack, BufStr);                 break;
		case 0x0C:	sprintf(BufStr, "%s%s11 (%s)", sLanguagePack.Rate, sLanguageOther.Number, sLanguagePack.Good);
					ListParam.AddParametr(sLanguageTabInfo.MethodPack, BufStr);                 break;
		case 0x0D:	sprintf(BufStr, "%s%s12 (%s)", sLanguagePack.Rate, sLanguageOther.Number, sLanguagePack.Verygood);
					ListParam.AddParametr(sLanguageTabInfo.MethodPack, BufStr);                 break;
		case 0x0E:	sprintf(BufStr, "%s%s13 (%s)", sLanguagePack.Rate, sLanguageOther.Number, sLanguagePack.Verygood);
					ListParam.AddParametr(sLanguageTabInfo.MethodPack, BufStr);                 break;
		case 0x0F:	sprintf(BufStr, "%s%s14 (%s)", sLanguagePack.Rate, sLanguageOther.Number, sLanguagePack.Best);
					ListParam.AddParametr(sLanguageTabInfo.MethodPack, BufStr);                 break;
		case 0x10:	sprintf(BufStr, "%s%s15 (%s)", sLanguagePack.Rate, sLanguageOther.Number, sLanguagePack.Best);
					ListParam.AddParametr(sLanguageTabInfo.MethodPack, BufStr);                 break;
		default:	ListParam.AddParametr(sLanguageTabInfo.MethodPack, sLanguageOther.Unknown); break;
		}

	//ispol'zuemaya pamyat'
	if ((sSettingsMain.AllParametrs) || (MemorySize))
	{
		if (!sSettingsMain.FloatSize) NumberSeparatorStringUnitSize(BufStr, MemorySize, T_MBYTE, sLanguageOther);
		else NumberFloatStringUnitSize(BufStr, MemorySize, T_MBYTE, sLanguageOther);
		if (sSettingsParameters.MemorySize)
			ListParam.AddParametr(sLanguageTabInfo.MemorySize, BufStr);
	}

	//sjatie ot rzmera arhiva
	sprintf(BufStr, "%.1f %s", RatioArchiveSize, sLanguageOther.Percent);
	if (sSettingsParameters.RatioArchiveSize)
		ListParam.AddParametr(sLanguageTabInfo.RatioArchiveSize, BufStr);

	//sjatie ot upakovannogo razmera
	sprintf(BufStr, "%.1f %s", RatioPackFileSize, sLanguageOther.Percent);
	if (sSettingsParameters.RatioPackFileSize)
		ListParam.AddParametr(sLanguageTabInfo.RatioPackFileSize, BufStr);

	//razdelitel'
	ListParam.AddParametr("", "");

	sprintf(BufStr, "%.2f %s", TestTime, sLanguageOther.Second);
	if (sSettingsParameters.TestTime)
		ListParam.AddParametr(sLanguageTabInfo.TestTime, BufStr);

	sprintf(BufStr, "%.2f %s", AnalyzeTime, sLanguageOther.Second);
	if (sSettingsParameters.AnalyzeTime)
		ListParam.AddParametr(sLanguageTabInfo.AnalyzeTime, BufStr);

	sprintf(BufStr, "%.2f %s", AllTime, sLanguageOther.Second);
	if (sSettingsParameters.AllTime)
		ListParam.AddParametr(sLanguageTabInfo.AllTime, BufStr);

	return 1;
}

//dlya archiva RAR
int MySelectInfo::SelectInfoRar()
{
	//dlya uprosheniya obrasheniya k dannim sobrannim v odnom meste
	SettingsMain&       sSettingsMain       = m_pSettings->sSettingsMain;
	SettingsParameters& sSettingsParameters = m_pSettings->sSettingsParameters;
	LanguageTabInfo&    sLanguageTabInfo    = m_pLanguage->sLanguageTabInfo;
	LanguageOS&         sLanguageOS         = m_pLanguage->sLanguageOS;
	LanguagePack&       sLanguagePack       = m_pLanguage->sLanguagePack;
	LanguageOther&      sLanguageOther      = m_pLanguage->sLanguageOther;
	MListParametrs&     ListParam           = *m_pListParam;

	TRar* obj_rar = (TRar*)m_pArchData;

	ULONGLONG UnpackSizeFiles   = obj_rar->GetUnpackSizeFiles();
	ULONGLONG PackSizeFiles     = obj_rar->GetPackSizeFiles();
	ULONGLONG ArchiveSize       = obj_rar->GetArchiveSize();
	unsigned  NumberFiles       = obj_rar->GetNumberFiles();
	unsigned  NumberFolders     = obj_rar->GetNumberFolders();
	unsigned  DictionarySize    = obj_rar->GetDictionarySize();
	short     HostOS            = obj_rar->GetHostOS();
	short     MethodPack        = obj_rar->GetMethodPack();
	double    RatioArchiveSize  = obj_rar->GetRatioArchiveSize();
	double    RatioPackFileSize = obj_rar->GetRatioPackFileSize();
	double    UnpackVersion     = obj_rar->GetUnpackVersion();
	unsigned  SfxModule         = obj_rar->GetSfxModule();
	unsigned  RecoveryRecord    = obj_rar->GetRecoveryRecord();
	BOOL      Solid             = obj_rar->GetSolid();
	BOOL      MultiArc          = obj_rar->GetMultiArc();
	unsigned  VolumeNumber      = obj_rar->GetVolumeNumber();
	BOOL      MainComment       = obj_rar->GetMainComment();
	BOOL      FileComment       = obj_rar->GetFileComment();
	BOOL      Password          = obj_rar->GetPassword();
	BOOL      ArchiveLock       = obj_rar->GetArchiveLock();
	BOOL      AuthVerification  = obj_rar->GetAuthVerification();
	char*     pAuthVerification = obj_rar->GetStringAuthVerification();
	char*     pTextComment      = obj_rar->GetTextComment();
	double    TestTime          = m_TestTime;
	double    AnalyzeTime       = obj_rar->GetAnalyzeTime();
	double    AllTime           = m_TestTime + AnalyzeTime;

	if (((int)RatioPackFileSize == 0) && (RatioPackFileSize > 0)) *m_pRatio1 = 1;
	else *m_pRatio1 = (int)RatioPackFileSize;
	if (((int)RatioArchiveSize == 0) && (RatioArchiveSize > 0)) *m_pRatio2 = 1;
	else *m_pRatio2 = (int)RatioArchiveSize;

	char BufStr[MAX_LOADSTRING] = {NULL};

	//zagolovok
	char* _ARCH = m_pLanguage->sLanguageArchives.Rar;
	char* _SFX  = m_pLanguage->sLanguageArchives.Sfx;
	if ((Solid) && (SfxModule)) sprintf(BufStr, "%s %s %s %s", sLanguageTabInfo.Solid, _SFX, _ARCH, sLanguageOther.Archive);
	else
		if (Solid) sprintf(BufStr, "%s %s %s", sLanguageTabInfo.Solid, _ARCH, sLanguageOther.Archive);
		else
			if (SfxModule) sprintf(BufStr, "%s %s %s", _SFX, _ARCH, sLanguageOther.Archive);
			else sprintf(BufStr, "%s %s", _ARCH, sLanguageOther.Archive);
	ListParam.AddParametr("", BufStr);

	//neupakovanniy razmer
	if ((sSettingsMain.AllParametrs) || (UnpackSizeFiles))
	{
		if (!sSettingsMain.FloatSize) NumberSeparatorStringUnitSize(BufStr, UnpackSizeFiles, T_BYTE, sLanguageOther);
		else NumberFloatStringUnitSize(BufStr, UnpackSizeFiles, T_BYTE, sLanguageOther);
		if (sSettingsParameters.UnpackSizeFiles)
			ListParam.AddParametr(sLanguageTabInfo.UnpackSizeFiles, BufStr);
	}

	//upakovanniy razmer
	if ((sSettingsMain.AllParametrs) || (PackSizeFiles))
	{
		if (!sSettingsMain.FloatSize) NumberSeparatorStringUnitSize(BufStr, PackSizeFiles, T_BYTE, sLanguageOther);
		else NumberFloatStringUnitSize(BufStr, PackSizeFiles, T_BYTE, sLanguageOther);
		if (sSettingsParameters.PackSizeFiles)
			ListParam.AddParametr(sLanguageTabInfo.PackSizeFiles, BufStr);
	}

	//razmer arhiva
	if ((sSettingsMain.AllParametrs) || (ArchiveSize))
	{
		if (!sSettingsMain.FloatSize) NumberSeparatorStringUnitSize(BufStr, ArchiveSize, T_BYTE, sLanguageOther);
		else NumberFloatStringUnitSize(BufStr, ArchiveSize, T_BYTE, sLanguageOther);
		if (sSettingsParameters.ArchiveSize)
			ListParam.AddParametr(sLanguageTabInfo.ArchiveSize, BufStr);
	}

	//kolichestvo faylov
	if ((sSettingsMain.AllParametrs) || (NumberFiles))
	{
		if (NumberFiles) NumberSeparatorString(BufStr, NumberFiles);
		else sprintf(BufStr, "%s", sLanguageOther.No);
		if (sSettingsParameters.NumberFiles)
			ListParam.AddParametr(sLanguageTabInfo.NumberFiles, BufStr);
	}

	//kolichestvo papok
	if ((sSettingsMain.AllParametrs) || (NumberFolders))
	{
		if (NumberFolders) NumberSeparatorString(BufStr, NumberFolders);
		else sprintf(BufStr, "%s", sLanguageOther.No);
		if (sSettingsParameters.NumberFolders)
			ListParam.AddParametr(sLanguageTabInfo.NumberFolders, BufStr);
	}

	//razdelitel'
	ListParam.AddParametr("", "");

	//razmer slovarya
	if (!sSettingsMain.FloatSize) NumberSeparatorStringUnitSize(BufStr, DictionarySize, T_KBYTE, sLanguageOther);
	else NumberFloatStringUnitSize(BufStr, DictionarySize, T_KBYTE, sLanguageOther);
	if (sSettingsParameters.DictionarySize)
		ListParam.AddParametr(sLanguageTabInfo.DictionarySize, BufStr);

	//host OS
	if (sSettingsParameters.HostOS)
		switch (HostOS)
		{
		case 0x00: ListParam.AddParametr(sLanguageTabInfo.HostOS, sLanguageOS.DOS);        break;
		case 0x01: ListParam.AddParametr(sLanguageTabInfo.HostOS, sLanguageOS.OS2);        break;
		case 0x02: ListParam.AddParametr(sLanguageTabInfo.HostOS, sLanguageOS.Win32);      break;
		case 0x03: ListParam.AddParametr(sLanguageTabInfo.HostOS, sLanguageOS.Unix);       break;
		case 0x04: ListParam.AddParametr(sLanguageTabInfo.HostOS, sLanguageOS.MacOS);      break;
		case 0x05: ListParam.AddParametr(sLanguageTabInfo.HostOS, sLanguageOS.BeOS);       break;
		default:   ListParam.AddParametr(sLanguageTabInfo.HostOS, sLanguageOther.Unknown); break;
		}

	//metod upakovki
	if (sSettingsParameters.MethodPack)
		switch (MethodPack)
		{
		case 0x30: ListParam.AddParametr(sLanguageTabInfo.MethodPack, sLanguagePack.Stored);   break;
		case 0x31: ListParam.AddParametr(sLanguageTabInfo.MethodPack, sLanguagePack.Fastest);  break;
		case 0x32: ListParam.AddParametr(sLanguageTabInfo.MethodPack, sLanguagePack.Fast);     break;
		case 0x33: ListParam.AddParametr(sLanguageTabInfo.MethodPack, sLanguagePack.Normal);   break;
		case 0x34: ListParam.AddParametr(sLanguageTabInfo.MethodPack, sLanguagePack.Good);     break;
		case 0x35: ListParam.AddParametr(sLanguageTabInfo.MethodPack, sLanguagePack.Best);     break;
		default:   ListParam.AddParametr(sLanguageTabInfo.MethodPack, sLanguageOther.Unknown); break;
		}

	//sjatie ot rzmera arhiva
	sprintf(BufStr, "%.1f %s", RatioArchiveSize, sLanguageOther.Percent);
	if (sSettingsParameters.RatioArchiveSize)
		ListParam.AddParametr(sLanguageTabInfo.RatioArchiveSize, BufStr);

	//sjatie ot upakovannogo razmera
	sprintf(BufStr, "%.1f %s", RatioPackFileSize, sLanguageOther.Percent);
	if (sSettingsParameters.RatioPackFileSize)
		ListParam.AddParametr(sLanguageTabInfo.RatioPackFileSize, BufStr);

	//versiya dlya raspakovki
	sprintf(BufStr, "%.1f", UnpackVersion);
	if (sSettingsParameters.UnpackVersion)
		ListParam.AddParametr(sLanguageTabInfo.UnpackVersion, BufStr);

	//razdelitel'
	ListParam.AddParametr("", "");

	//SFX modul'
	if ((sSettingsMain.AllParametrs) || (SfxModule))
	{
		if (sSettingsParameters.SfxModule)
			if (!SfxModule) ListParam.AddParametr(sLanguageTabInfo.SfxModule, sLanguageOther.No);
			else
			{
				if (!sSettingsMain.FloatSize) NumberSeparatorStringUnitSize(BufStr, SfxModule, T_BYTE, sLanguageOther);
				else NumberFloatStringUnitSize(BufStr, SfxModule, T_BYTE, sLanguageOther);
				ListParam.AddParametr(sLanguageTabInfo.SfxModule, BufStr);
			}
	}

	//zapis' dlya vosstanovleniya
	if ((sSettingsMain.AllParametrs) || (RecoveryRecord))
	{
		if (sSettingsParameters.RecoveryRecord)
			if (!RecoveryRecord) ListParam.AddParametr(sLanguageTabInfo.RecoveryRecord, sLanguageOther.No);
			else
			{
				if (!sSettingsMain.FloatSize) NumberSeparatorStringUnitSize(BufStr, RecoveryRecord, T_BYTE, sLanguageOther);
				else NumberFloatStringUnitSize(BufStr, RecoveryRecord, T_BYTE, sLanguageOther);
				ListParam.AddParametr(sLanguageTabInfo.RecoveryRecord, BufStr);
			}
	}

	//neprerivnost'
	if ((sSettingsMain.AllParametrs) || (Solid))
	{
		if (sSettingsParameters.Solid)
			if (!Solid) ListParam.AddParametr(sLanguageTabInfo.Solid, sLanguageOther.No);
			else ListParam.AddParametr(sLanguageTabInfo.Solid, sLanguageOther.Yes);
	}

	//mnogotomnost'
	if ((sSettingsMain.AllParametrs) || (MultiArc))
	{
		if (sSettingsParameters.MultiArc)
			if (!MultiArc) ListParam.AddParametr(sLanguageTabInfo.MultiArc, sLanguageOther.No);
			else
			{
				ListParam.AddParametr(sLanguageTabInfo.MultiArc, sLanguageOther.Yes);
				//nomer toma
				if (VolumeNumber)
				{
					sprintf(BufStr, "%s %d", sLanguageOther.Number, VolumeNumber);
					ListParam.AddParametr(sLanguageTabInfo.NumberVolume, BufStr);
				}
			}
	}

	//kommentariy arhiva
	if ((sSettingsMain.AllParametrs) || (MainComment))
	{
		if (sSettingsParameters.MainComment)
			if (!MainComment) ListParam.AddParametr(sLanguageTabInfo.MainComment, sLanguageOther.No);
			else ListParam.AddParametr(sLanguageTabInfo.MainComment, sLanguageOther.Yes);
	}
	//tekst kommentariya
	if (pTextComment) *m_ppTextComment = pTextComment;

	//kommentarii faylov
	if ((sSettingsMain.AllParametrs) || (FileComment))
	{
		if (sSettingsParameters.FileComment)
			if (!FileComment) ListParam.AddParametr(sLanguageTabInfo.FileComment, sLanguageOther.No);
			else ListParam.AddParametr(sLanguageTabInfo.FileComment, sLanguageOther.Yes);
	}

	//parol'
	if ((sSettingsMain.AllParametrs) || (Password))
	{
		if (sSettingsParameters.Password)
			if (!Password) ListParam.AddParametr(sLanguageTabInfo.Password, sLanguageOther.No);
			else ListParam.AddParametr(sLanguageTabInfo.Password, sLanguageOther.Yes);
	}

	//arhiv zablokirovam
	if ((sSettingsMain.AllParametrs) || (ArchiveLock))
	{
		if (sSettingsParameters.ArchiveLock)
			if (!ArchiveLock) ListParam.AddParametr(sLanguageTabInfo.ArchiveLock, sLanguageOther.No);
			else ListParam.AddParametr(sLanguageTabInfo.ArchiveLock, sLanguageOther.Yes);
	}

	//elektronnaya podpis'
	if ((sSettingsMain.AllParametrs) || (AuthVerification))
	{
		if (sSettingsParameters.AuthVerification)
			if (!AuthVerification) ListParam.AddParametr(sLanguageTabInfo.AuthVerification, sLanguageOther.No);
			else
			{
				ListParam.AddParametr(sLanguageTabInfo.AuthVerification, sLanguageOther.Yes);
				if (pAuthVerification) ListParam.AddParametr("", pAuthVerification, 4);
			}
	}

	//razdelitel'
	ListParam.AddParametr("", "");

	sprintf(BufStr, "%.2f %s", TestTime, sLanguageOther.Second);
	if (sSettingsParameters.TestTime)
		ListParam.AddParametr(sLanguageTabInfo.TestTime, BufStr);

	sprintf(BufStr, "%.2f %s", AnalyzeTime, sLanguageOther.Second);
	if (sSettingsParameters.AnalyzeTime)
		ListParam.AddParametr(sLanguageTabInfo.AnalyzeTime, BufStr);

	sprintf(BufStr, "%.2f %s", AllTime, sLanguageOther.Second);
	if (sSettingsParameters.AllTime)
		ListParam.AddParametr(sLanguageTabInfo.AllTime, BufStr);

	return 1;
}

//dlya archiva SQZ
int MySelectInfo::SelectInfoSqz()
{
	//dlya uprosheniya obrasheniya k dannim sobrannim v odnom meste
	SettingsMain&       sSettingsMain       = m_pSettings->sSettingsMain;
	SettingsParameters& sSettingsParameters = m_pSettings->sSettingsParameters;
	LanguageTabInfo&    sLanguageTabInfo    = m_pLanguage->sLanguageTabInfo;
	LanguageOS&         sLanguageOS         = m_pLanguage->sLanguageOS;
	LanguagePack&       sLanguagePack       = m_pLanguage->sLanguagePack;
	LanguageOther&      sLanguageOther      = m_pLanguage->sLanguageOther;
	MListParametrs&     ListParam           = *m_pListParam;

	TSqz* obj_sqz = (TSqz*)m_pArchData;

	ULONGLONG UnpackSizeFiles   = obj_sqz->GetUnpackSizeFiles();
	ULONGLONG PackSizeFiles     = obj_sqz->GetPackSizeFiles();
	ULONGLONG ArchiveSize       = obj_sqz->GetArchiveSize();
	unsigned  NumberFiles       = obj_sqz->GetNumberFiles();
	unsigned  NumberFolders     = obj_sqz->GetNumberFolders();
	short     HostOS            = obj_sqz->GetHostOS();
	short     MethodPack        = obj_sqz->GetMethodPack();
	double    RatioArchiveSize  = obj_sqz->GetRatioArchiveSize();
	double    RatioPackFileSize = obj_sqz->GetRatioPackFileSize();
	double    UnpackVersion     = obj_sqz->GetUnpackVersion();
	unsigned  SfxModule         = obj_sqz->GetSfxModule();
	BOOL      MultiArc          = obj_sqz->GetMultiArc();
	BOOL      MainComment       = obj_sqz->GetMainComment();
	BOOL      Password          = obj_sqz->GetPassword();
	BOOL      ArchiveLock       = obj_sqz->GetArchiveLock();
	double    TestTime          = m_TestTime;
	double    AnalyzeTime       = obj_sqz->GetAnalyzeTime();
	double    AllTime           = m_TestTime + AnalyzeTime;

	if (((int)RatioPackFileSize == 0) && (RatioPackFileSize > 0)) *m_pRatio1 = 1;
	else *m_pRatio1 = (int)RatioPackFileSize;
	if (((int)RatioArchiveSize == 0) && (RatioArchiveSize > 0)) *m_pRatio2 = 1;
	else *m_pRatio2 = (int)RatioArchiveSize;

	char BufStr[MAX_LOADSTRING] = {NULL};

	//zagolovok
	char* _ARCH = m_pLanguage->sLanguageArchives.Sqz;
	char* _SFX  = m_pLanguage->sLanguageArchives.Sfx;
	if (SfxModule) sprintf(BufStr, "%s %s %s", _SFX, _ARCH, sLanguageOther.Archive);
	else sprintf(BufStr, "%s %s", _ARCH, sLanguageOther.Archive);
	ListParam.AddParametr("", BufStr);

	//neupakovanniy razmer
	if ((sSettingsMain.AllParametrs) || (UnpackSizeFiles))
	{
		if (!sSettingsMain.FloatSize) NumberSeparatorStringUnitSize(BufStr, UnpackSizeFiles, T_BYTE, sLanguageOther);
		else NumberFloatStringUnitSize(BufStr, UnpackSizeFiles, T_BYTE, sLanguageOther);
		if (sSettingsParameters.UnpackSizeFiles)
			ListParam.AddParametr(sLanguageTabInfo.UnpackSizeFiles, BufStr);
	}

	//upakovanniy razmer
	if ((sSettingsMain.AllParametrs) || (PackSizeFiles))
	{
		if (!sSettingsMain.FloatSize) NumberSeparatorStringUnitSize(BufStr, PackSizeFiles, T_BYTE, sLanguageOther);
		else NumberFloatStringUnitSize(BufStr, PackSizeFiles, T_BYTE, sLanguageOther);
		if (sSettingsParameters.PackSizeFiles)
			ListParam.AddParametr(sLanguageTabInfo.PackSizeFiles, BufStr);
	}

	//razmer arhiva
	if ((sSettingsMain.AllParametrs) || (ArchiveSize))
	{
		if (!sSettingsMain.FloatSize) NumberSeparatorStringUnitSize(BufStr, ArchiveSize, T_BYTE, sLanguageOther);
		else NumberFloatStringUnitSize(BufStr, ArchiveSize, T_BYTE, sLanguageOther);
		if (sSettingsParameters.ArchiveSize)
			ListParam.AddParametr(sLanguageTabInfo.ArchiveSize, BufStr);
	}

	//kolichestvo faylov
	if ((sSettingsMain.AllParametrs) || (NumberFiles))
	{
		if (NumberFiles) NumberSeparatorString(BufStr, NumberFiles);
		else sprintf(BufStr, "%s", sLanguageOther.No);
		if (sSettingsParameters.NumberFiles)
			ListParam.AddParametr(sLanguageTabInfo.NumberFiles, BufStr);
	}

	//kolichestvo papok
	if ((sSettingsMain.AllParametrs) || (NumberFolders))
	{
		if (NumberFolders) NumberSeparatorString(BufStr, NumberFolders);
		else sprintf(BufStr, "%s", sLanguageOther.No);
		if (sSettingsParameters.NumberFolders)
			ListParam.AddParametr(sLanguageTabInfo.NumberFolders, BufStr);
	}

	//razdelitel'
	ListParam.AddParametr("", "");

	//host OS
	if (sSettingsParameters.HostOS)
		switch (HostOS)
		{
		case 0x00: ListParam.AddParametr(sLanguageTabInfo.HostOS, sLanguageOS.DOS);        break;
		case 0x01: ListParam.AddParametr(sLanguageTabInfo.HostOS, sLanguageOS.OS2);        break;
		case 0x02: ListParam.AddParametr(sLanguageTabInfo.HostOS, sLanguageOS.VaxVMS);     break;
		case 0x03: ListParam.AddParametr(sLanguageTabInfo.HostOS, sLanguageOS.OS2);        break;
		case 0x04: ListParam.AddParametr(sLanguageTabInfo.HostOS, sLanguageOS.Amiga);      break;
		case 0x05: ListParam.AddParametr(sLanguageTabInfo.HostOS, sLanguageOS.MacOS);      break;
		case 0x06: ListParam.AddParametr(sLanguageTabInfo.HostOS, sLanguageOS.Unix);       break;
		default:   ListParam.AddParametr(sLanguageTabInfo.HostOS, sLanguageOther.Unknown); break;
		}

	//metod upakovki
	if (sSettingsParameters.MethodPack)
		switch (MethodPack)
		{
		case 0x00: ListParam.AddParametr(sLanguageTabInfo.MethodPack, sLanguagePack.Stored);   break;
		case 0x01: ListParam.AddParametr(sLanguageTabInfo.MethodPack, sLanguagePack.Fast);     break;
		case 0x02: ListParam.AddParametr(sLanguageTabInfo.MethodPack, sLanguagePack.Normal);   break;
		case 0x03: ListParam.AddParametr(sLanguageTabInfo.MethodPack, sLanguagePack.Good);     break;
		case 0x04: ListParam.AddParametr(sLanguageTabInfo.MethodPack, sLanguagePack.Best);     break;
		default:   ListParam.AddParametr(sLanguageTabInfo.MethodPack, sLanguageOther.Unknown); break;
		}

	//sjatie ot rzmera arhiva
	sprintf(BufStr, "%.1f %s", RatioArchiveSize, sLanguageOther.Percent);
	if (sSettingsParameters.RatioArchiveSize)
		ListParam.AddParametr(sLanguageTabInfo.RatioArchiveSize, BufStr);

	//sjatie ot upakovannogo razmera
	sprintf(BufStr, "%.1f %s", RatioPackFileSize, sLanguageOther.Percent);
	if (sSettingsParameters.RatioPackFileSize)
		ListParam.AddParametr(sLanguageTabInfo.RatioPackFileSize, BufStr);

	//versiya dlya raspakovki
	sprintf(BufStr, "%.1f", UnpackVersion);
	if (sSettingsParameters.UnpackVersion)
		ListParam.AddParametr(sLanguageTabInfo.UnpackVersion, BufStr);

	//razdelitel'
	ListParam.AddParametr("", "");

	//SFX modul'
	if ((sSettingsMain.AllParametrs) || (SfxModule))
	{
		if (sSettingsParameters.SfxModule)
			if (!SfxModule) ListParam.AddParametr(sLanguageTabInfo.SfxModule, sLanguageOther.No);
			else
			{
				if (!sSettingsMain.FloatSize) NumberSeparatorStringUnitSize(BufStr, SfxModule, T_BYTE, sLanguageOther);
				else NumberFloatStringUnitSize(BufStr, SfxModule, T_BYTE, sLanguageOther);
				ListParam.AddParametr(sLanguageTabInfo.SfxModule, BufStr);
			}
	}

	//mnogotomnost'
	if ((sSettingsMain.AllParametrs) || (MultiArc))
	{
		if (sSettingsParameters.MultiArc)
			if (!MultiArc) ListParam.AddParametr(sLanguageTabInfo.MultiArc, sLanguageOther.No);
			else ListParam.AddParametr(sLanguageTabInfo.MultiArc, sLanguageOther.Yes);
	}

	//kommentariy arhiva
	if ((sSettingsMain.AllParametrs) || (MainComment))
	{
		if (sSettingsParameters.MainComment)
			if (!MainComment) ListParam.AddParametr(sLanguageTabInfo.MainComment, sLanguageOther.No);
			else ListParam.AddParametr(sLanguageTabInfo.MainComment, sLanguageOther.Yes);
	}

	//parol'
	if ((sSettingsMain.AllParametrs) || (Password))
	{
		if (sSettingsParameters.Password)
			if (!Password) ListParam.AddParametr(sLanguageTabInfo.Password, sLanguageOther.No);
			else ListParam.AddParametr(sLanguageTabInfo.Password, sLanguageOther.Yes);
	}

	//arhiv zablokirovam
	if ((sSettingsMain.AllParametrs) || (ArchiveLock))
	{
		if (sSettingsParameters.ArchiveLock)
			if (!ArchiveLock) ListParam.AddParametr(sLanguageTabInfo.ArchiveLock, sLanguageOther.No);
			else ListParam.AddParametr(sLanguageTabInfo.ArchiveLock, sLanguageOther.Yes);
	}

	//razdelitel'
	ListParam.AddParametr("", "");

	sprintf(BufStr, "%.2f %s", TestTime, sLanguageOther.Second);
	if (sSettingsParameters.TestTime)
		ListParam.AddParametr(sLanguageTabInfo.TestTime, BufStr);

	sprintf(BufStr, "%.2f %s", AnalyzeTime, sLanguageOther.Second);
	if (sSettingsParameters.AnalyzeTime)
		ListParam.AddParametr(sLanguageTabInfo.AnalyzeTime, BufStr);

	sprintf(BufStr, "%.2f %s", AllTime, sLanguageOther.Second);
	if (sSettingsParameters.AllTime)
		ListParam.AddParametr(sLanguageTabInfo.AllTime, BufStr);

	return 1;
}

//dlya archiva TAR
int MySelectInfo::SelectInfoTar()
{
	//dlya uprosheniya obrasheniya k dannim sobrannim v odnom meste
	SettingsMain&       sSettingsMain       = m_pSettings->sSettingsMain;
	SettingsParameters& sSettingsParameters = m_pSettings->sSettingsParameters;
	LanguageTabInfo&    sLanguageTabInfo    = m_pLanguage->sLanguageTabInfo;
	LanguageOS&         sLanguageOS         = m_pLanguage->sLanguageOS;
	LanguagePack&       sLanguagePack       = m_pLanguage->sLanguagePack;
	LanguageOther&      sLanguageOther      = m_pLanguage->sLanguageOther;
	MListParametrs&     ListParam           = *m_pListParam;

	TTar* obj_tar = (TTar*)m_pArchData;

	ULONGLONG UnpackSizeFiles   = obj_tar->GetUnpackSizeFiles();
	ULONGLONG PackSizeFiles     = obj_tar->GetPackSizeFiles();
	ULONGLONG ArchiveSize       = obj_tar->GetArchiveSize();
	unsigned  NumberFiles       = obj_tar->GetNumberFiles();
	unsigned  NumberFolders     = obj_tar->GetNumberFolders();
	char*     pMagic            = obj_tar->GetMagic();
	double    RatioArchiveSize  = obj_tar->GetRatioArchiveSize();
	double    RatioPackFileSize = obj_tar->GetRatioPackFileSize();
	double    TestTime          = m_TestTime;
	double    AnalyzeTime       = obj_tar->GetAnalyzeTime();
	double    AllTime           = m_TestTime + AnalyzeTime;

	if (((int)RatioPackFileSize == 0) && (RatioPackFileSize > 0)) *m_pRatio1 = 1;
	else *m_pRatio1 = (int)RatioPackFileSize;
	if (((int)RatioArchiveSize == 0) && (RatioArchiveSize > 0)) *m_pRatio2 = 1;
	else *m_pRatio2 = (int)RatioArchiveSize;

	char BufStr[MAX_LOADSTRING] = {NULL};

	//zagolovok
	char* _ARCH = m_pLanguage->sLanguageArchives.Tar;
	sprintf(BufStr, "%s %s", _ARCH, sLanguageOther.Archive);
	ListParam.AddParametr("", BufStr);

	//neupakovanniy razmer
	if ((sSettingsMain.AllParametrs) || (UnpackSizeFiles))
	{
		if (!sSettingsMain.FloatSize) NumberSeparatorStringUnitSize(BufStr, UnpackSizeFiles, T_BYTE, sLanguageOther);
		else NumberFloatStringUnitSize(BufStr, UnpackSizeFiles, T_BYTE, sLanguageOther);
		if (sSettingsParameters.UnpackSizeFiles)
			ListParam.AddParametr(sLanguageTabInfo.UnpackSizeFiles, BufStr);
	}

	//upakovanniy razmer
	if ((sSettingsMain.AllParametrs) || (PackSizeFiles))
	{
		if (!sSettingsMain.FloatSize) NumberSeparatorStringUnitSize(BufStr, PackSizeFiles, T_BYTE, sLanguageOther);
		else NumberFloatStringUnitSize(BufStr, PackSizeFiles, T_BYTE, sLanguageOther);
		if (sSettingsParameters.PackSizeFiles)
			ListParam.AddParametr(sLanguageTabInfo.PackSizeFiles, BufStr);
	}

	//razmer arhiva
	if ((sSettingsMain.AllParametrs) || (ArchiveSize))
	{
		if (!sSettingsMain.FloatSize) NumberSeparatorStringUnitSize(BufStr, ArchiveSize, T_BYTE, sLanguageOther);
		else NumberFloatStringUnitSize(BufStr, ArchiveSize, T_BYTE, sLanguageOther);
		if (sSettingsParameters.ArchiveSize)
			ListParam.AddParametr(sLanguageTabInfo.ArchiveSize, BufStr);
	}

	//kolichestvo faylov
	if ((sSettingsMain.AllParametrs) || (NumberFiles))
	{
		if (NumberFiles) NumberSeparatorString(BufStr, NumberFiles);
		else sprintf(BufStr, "%s", sLanguageOther.No);
		if (sSettingsParameters.NumberFiles)
			ListParam.AddParametr(sLanguageTabInfo.NumberFiles, BufStr);
	}

	//kolichestvo papok
	if ((sSettingsMain.AllParametrs) || (NumberFolders))
	{
		if (NumberFolders) NumberSeparatorString(BufStr, NumberFolders);
		else sprintf(BufStr, "%s", sLanguageOther.No);
		if (sSettingsParameters.NumberFolders)
			ListParam.AddParametr(sLanguageTabInfo.NumberFolders, BufStr);
	}

	//magic
	if ((sSettingsMain.AllParametrs) || (pMagic))
	{
		if (sSettingsParameters.Magic)
			if (pMagic) ListParam.AddParametr(sLanguageTabInfo.Magic, pMagic);
			else ListParam.AddParametr(sLanguageTabInfo.Magic, sLanguageOther.No);
	}

	//razdelitel'
	ListParam.AddParametr("", "");

	//sjatie ot rzmera arhiva
	sprintf(BufStr, "%.1f %s", RatioArchiveSize, sLanguageOther.Percent);
	if (sSettingsParameters.RatioArchiveSize)
		ListParam.AddParametr(sLanguageTabInfo.RatioArchiveSize, BufStr);

	//sjatie ot upakovannogo razmera
	sprintf(BufStr, "%.1f %s", RatioPackFileSize, sLanguageOther.Percent);
	if (sSettingsParameters.RatioPackFileSize)
		ListParam.AddParametr(sLanguageTabInfo.RatioPackFileSize, BufStr);

	//razdelitel'
	ListParam.AddParametr("", "");

	sprintf(BufStr, "%.2f %s", TestTime, sLanguageOther.Second);
	if (sSettingsParameters.TestTime)
		ListParam.AddParametr(sLanguageTabInfo.TestTime, BufStr);

	sprintf(BufStr, "%.2f %s", AnalyzeTime, sLanguageOther.Second);
	if (sSettingsParameters.AnalyzeTime)
		ListParam.AddParametr(sLanguageTabInfo.AnalyzeTime, BufStr);

	sprintf(BufStr, "%.2f %s", AllTime, sLanguageOther.Second);
	if (sSettingsParameters.AllTime)
		ListParam.AddParametr(sLanguageTabInfo.AllTime, BufStr);

	return 1;
}

//dlya archiva Z
int MySelectInfo::SelectInfoZ()
{
	//dlya uprosheniya obrasheniya k dannim sobrannim v odnom meste
	SettingsMain&       sSettingsMain       = m_pSettings->sSettingsMain;
	SettingsParameters& sSettingsParameters = m_pSettings->sSettingsParameters;
	LanguageTabInfo&    sLanguageTabInfo    = m_pLanguage->sLanguageTabInfo;
	LanguageOS&         sLanguageOS         = m_pLanguage->sLanguageOS;
	LanguagePack&       sLanguagePack       = m_pLanguage->sLanguagePack;
	LanguageOther&      sLanguageOther      = m_pLanguage->sLanguageOther;
	MListParametrs&     ListParam           = *m_pListParam;

	TZ* obj_z = (TZ*)m_pArchData;

	ULONGLONG PackSizeFiles     = obj_z->GetPackSizeFiles();
	ULONGLONG ArchiveSize       = obj_z->GetArchiveSize();
	unsigned  NumberFiles       = obj_z->GetNumberFiles();
	double    TestTime          = m_TestTime;
	double    AnalyzeTime       = obj_z->GetAnalyzeTime();
	double    AllTime           = m_TestTime + AnalyzeTime;

	char BufStr[MAX_LOADSTRING] = {NULL};

	//zagolovok
	char* _ARCH = m_pLanguage->sLanguageArchives.Z;
	sprintf(BufStr, "%s %s", _ARCH, sLanguageOther.Archive);
	ListParam.AddParametr("", BufStr);

	//upakovanniy razmer
	if ((sSettingsMain.AllParametrs) || (PackSizeFiles))
	{
		if (!sSettingsMain.FloatSize) NumberSeparatorStringUnitSize(BufStr, PackSizeFiles, T_BYTE, sLanguageOther);
		else NumberFloatStringUnitSize(BufStr, PackSizeFiles, T_BYTE, sLanguageOther);
		if (sSettingsParameters.PackSizeFiles)
			ListParam.AddParametr(sLanguageTabInfo.PackSizeFiles, BufStr);
	}

	//razmer arhiva
	if ((sSettingsMain.AllParametrs) || (ArchiveSize))
	{
		if (!sSettingsMain.FloatSize) NumberSeparatorStringUnitSize(BufStr, ArchiveSize, T_BYTE, sLanguageOther);
		else NumberFloatStringUnitSize(BufStr, ArchiveSize, T_BYTE, sLanguageOther);
		if (sSettingsParameters.ArchiveSize)
			ListParam.AddParametr(sLanguageTabInfo.ArchiveSize, BufStr);
	}

	//kolichestvo faylov
	if ((sSettingsMain.AllParametrs) || (NumberFiles))
	{
		if (NumberFiles) NumberSeparatorString(BufStr, NumberFiles);
		else sprintf(BufStr, "%s", sLanguageOther.No);
		if (sSettingsParameters.NumberFiles)
			ListParam.AddParametr(sLanguageTabInfo.NumberFiles, BufStr);
	}

	//razdelitel'
	ListParam.AddParametr("", "");

	sprintf(BufStr, "%.2f %s", TestTime, sLanguageOther.Second);
	if (sSettingsParameters.TestTime)
		ListParam.AddParametr(sLanguageTabInfo.TestTime, BufStr);

	sprintf(BufStr, "%.2f %s", AnalyzeTime, sLanguageOther.Second);
	if (sSettingsParameters.AnalyzeTime)
		ListParam.AddParametr(sLanguageTabInfo.AnalyzeTime, BufStr);

	sprintf(BufStr, "%.2f %s", AllTime, sLanguageOther.Second);
	if (sSettingsParameters.AllTime)
		ListParam.AddParametr(sLanguageTabInfo.AllTime, BufStr);

	return 1;
}

//dlya archiva ZIP/JAR
int MySelectInfo::SelectInfoZipJar()
{
	//dlya uprosheniya obrasheniya k dannim sobrannim v odnom meste
	SettingsMain&       sSettingsMain       = m_pSettings->sSettingsMain;
	SettingsParameters& sSettingsParameters = m_pSettings->sSettingsParameters;
	LanguageTabInfo&    sLanguageTabInfo    = m_pLanguage->sLanguageTabInfo;
	LanguageOS&         sLanguageOS         = m_pLanguage->sLanguageOS;
	LanguagePack&       sLanguagePack       = m_pLanguage->sLanguagePack;
	LanguageOther&      sLanguageOther      = m_pLanguage->sLanguageOther;
	MListParametrs&     ListParam           = *m_pListParam;

	TZipJar* obj_zipjar = (TZipJar*)m_pArchData;

	ULONGLONG UnpackSizeFiles   = obj_zipjar->GetUnpackSizeFiles();
	ULONGLONG PackSizeFiles     = obj_zipjar->GetPackSizeFiles();
	ULONGLONG ArchiveSize       = obj_zipjar->GetArchiveSize();
	unsigned  NumberFiles       = obj_zipjar->GetNumberFiles();
	unsigned  NumberFolders     = obj_zipjar->GetNumberFolders();
	short     HostOS            = obj_zipjar->GetHostOS();
	short     ExtractOS         = obj_zipjar->GetExtractOS();
	short     MethodPack        = obj_zipjar->GetMethodPack();
	short     MethodPackAdd     = obj_zipjar->GetMethodPackAdd();
	double    RatioArchiveSize  = obj_zipjar->GetRatioArchiveSize();
	double    RatioPackFileSize = obj_zipjar->GetRatioPackFileSize();
	double    PackVersion       = obj_zipjar->GetPackVersion();
	double    UnpackVersion     = obj_zipjar->GetUnpackVersion();
	unsigned  SfxModule         = obj_zipjar->GetSfxModule();
	BOOL      MultiArc          = obj_zipjar->GetMultiArc();
	unsigned  VolumeNumber      = obj_zipjar->GetVolumeNumber();
	BOOL      MainComment       = obj_zipjar->GetMainComment();
	BOOL      FileComment       = obj_zipjar->GetFileComment();
	BOOL      Password          = obj_zipjar->GetPassword();
	BOOL      ArchiveType       = obj_zipjar->GetArchiveType();
	char*     pTextComment      = obj_zipjar->GetTextComment();
	double    TestTime          = m_TestTime;
	double    AnalyzeTime       = obj_zipjar->GetAnalyzeTime();
	double    AllTime           = m_TestTime + AnalyzeTime;

	if (((int)RatioPackFileSize == 0) && (RatioPackFileSize > 0)) *m_pRatio1 = 1;
	else *m_pRatio1 = (int)RatioPackFileSize;
	if (((int)RatioArchiveSize == 0) && (RatioArchiveSize > 0)) *m_pRatio2 = 1;
	else *m_pRatio2 = (int)RatioArchiveSize;

	char BufStr[MAX_LOADSTRING] = {NULL};

	//zagolovok
	char* _ARCH1 = m_pLanguage->sLanguageArchives.Zip;
	char* _ARCH2 = m_pLanguage->sLanguageArchives.Jar;
	char* _SFX   = "SFX";
	if (!ArchiveType)
		if (SfxModule) sprintf(BufStr, "%s %s %s", _SFX, _ARCH1, sLanguageOther.Archive);
		else sprintf(BufStr, "%s %s", _ARCH1, sLanguageOther.Archive);
	else
		if (SfxModule) sprintf(BufStr, "%s %s %s", _SFX, _ARCH2, sLanguageOther.Archive);
		else sprintf(BufStr, "%s %s", _ARCH2, sLanguageOther.Archive);
	ListParam.AddParametr("", BufStr);

	//neupakovanniy razmer
	if ((sSettingsMain.AllParametrs) || (UnpackSizeFiles))
	{
		if (!sSettingsMain.FloatSize) NumberSeparatorStringUnitSize(BufStr, UnpackSizeFiles, T_BYTE, sLanguageOther);
		else NumberFloatStringUnitSize(BufStr, UnpackSizeFiles, T_BYTE, sLanguageOther);
		if (sSettingsParameters.UnpackSizeFiles)
			ListParam.AddParametr(sLanguageTabInfo.UnpackSizeFiles, BufStr);
	}

	//upakovanniy razmer
	if ((sSettingsMain.AllParametrs) || (PackSizeFiles))
	{
		if (!sSettingsMain.FloatSize) NumberSeparatorStringUnitSize(BufStr, PackSizeFiles, T_BYTE, sLanguageOther);
		else NumberFloatStringUnitSize(BufStr, PackSizeFiles, T_BYTE, sLanguageOther);
		if (sSettingsParameters.PackSizeFiles)
			ListParam.AddParametr(sLanguageTabInfo.PackSizeFiles, BufStr);
	}

	//razmer arhiva
	if ((sSettingsMain.AllParametrs) || (ArchiveSize))
	{
		if (!sSettingsMain.FloatSize) NumberSeparatorStringUnitSize(BufStr, ArchiveSize, T_BYTE, sLanguageOther);
		else NumberFloatStringUnitSize(BufStr, ArchiveSize, T_BYTE, sLanguageOther);
		if (sSettingsParameters.ArchiveSize)
			ListParam.AddParametr(sLanguageTabInfo.ArchiveSize, BufStr);
	}

	//kolichestvo faylov
	if ((sSettingsMain.AllParametrs) || (NumberFiles))
	{
		if (NumberFiles) NumberSeparatorString(BufStr, NumberFiles);
		else sprintf(BufStr, "%s", sLanguageOther.No);
		if (sSettingsParameters.NumberFiles)
			ListParam.AddParametr(sLanguageTabInfo.NumberFiles, BufStr);
	}

	//kolichestvo papok
	if ((sSettingsMain.AllParametrs) || (NumberFolders))
	{
		if (NumberFolders) NumberSeparatorString(BufStr, NumberFolders);
		else sprintf(BufStr, "%s", sLanguageOther.No);
		if (sSettingsParameters.NumberFolders)
			ListParam.AddParametr(sLanguageTabInfo.NumberFolders, BufStr);
	}

	//razdelitel'
	ListParam.AddParametr("", "");

	//host OS
	if (sSettingsParameters.HostOS)
		switch (HostOS)
		{
		case 0x00: ListParam.AddParametr(sLanguageTabInfo.HostOS, sLanguageOS.DOS);        break;
		case 0x01: ListParam.AddParametr(sLanguageTabInfo.HostOS, sLanguageOS.Amiga);      break;
		case 0x02: ListParam.AddParametr(sLanguageTabInfo.HostOS, sLanguageOS.VaxVMS);     break;
		case 0x03: ListParam.AddParametr(sLanguageTabInfo.HostOS, sLanguageOS.Unix);       break;
		case 0x04: ListParam.AddParametr(sLanguageTabInfo.HostOS, sLanguageOS.VMCMS);      break;
		case 0x05: ListParam.AddParametr(sLanguageTabInfo.HostOS, sLanguageOS.AtariST);    break;
		case 0x06: ListParam.AddParametr(sLanguageTabInfo.HostOS, sLanguageOS.OS2);        break;
		case 0x07: ListParam.AddParametr(sLanguageTabInfo.HostOS, sLanguageOS.MacOS);      break;
		case 0x08: ListParam.AddParametr(sLanguageTabInfo.HostOS, sLanguageOS.ZSystem);    break;
		case 0x09: ListParam.AddParametr(sLanguageTabInfo.HostOS, sLanguageOS.CPM);        break;
		case 0x0A: ListParam.AddParametr(sLanguageTabInfo.HostOS, sLanguageOS.TOPS20);     break;
		case 0x0B: ListParam.AddParametr(sLanguageTabInfo.HostOS, sLanguageOS.WinNT);      break;
		case 0x0C: ListParam.AddParametr(sLanguageTabInfo.HostOS, sLanguageOS.SMSQDOS);    break;
		case 0x0D: ListParam.AddParametr(sLanguageTabInfo.HostOS, sLanguageOS.AcornRISC);  break;
		case 0x0E: ListParam.AddParametr(sLanguageTabInfo.HostOS, sLanguageOS.VFAT);       break;
		case 0x0F: ListParam.AddParametr(sLanguageTabInfo.HostOS, sLanguageOS.MVS);        break;
		case 0x10: ListParam.AddParametr(sLanguageTabInfo.HostOS, sLanguageOS.BeOS);       break;
		case 0x11: ListParam.AddParametr(sLanguageTabInfo.HostOS, sLanguageOS.Tandem);     break;
		default:   ListParam.AddParametr(sLanguageTabInfo.HostOS, sLanguageOther.Unknown); break;
		}

	//extract OS
	if ((sSettingsMain.AllParametrs) || (ExtractOS != HostOS))
	{
		if (sSettingsParameters.ExtractOS)
			switch (ExtractOS)
			{
			case 0x00: ListParam.AddParametr(sLanguageTabInfo.ExtractOS, sLanguageOS.DOS);        break;
			case 0x01: ListParam.AddParametr(sLanguageTabInfo.ExtractOS, sLanguageOS.Amiga);      break;
			case 0x02: ListParam.AddParametr(sLanguageTabInfo.ExtractOS, sLanguageOS.VaxVMS);     break;
			case 0x03: ListParam.AddParametr(sLanguageTabInfo.ExtractOS, sLanguageOS.Unix);       break;
			case 0x04: ListParam.AddParametr(sLanguageTabInfo.ExtractOS, sLanguageOS.VMCMS);      break;
			case 0x05: ListParam.AddParametr(sLanguageTabInfo.ExtractOS, sLanguageOS.AtariST);    break;
			case 0x06: ListParam.AddParametr(sLanguageTabInfo.ExtractOS, sLanguageOS.OS2);        break;
			case 0x07: ListParam.AddParametr(sLanguageTabInfo.ExtractOS, sLanguageOS.MacOS);      break;
			case 0x08: ListParam.AddParametr(sLanguageTabInfo.ExtractOS, sLanguageOS.ZSystem);    break;
			case 0x09: ListParam.AddParametr(sLanguageTabInfo.ExtractOS, sLanguageOS.CPM);        break;
			case 0x0A: ListParam.AddParametr(sLanguageTabInfo.ExtractOS, sLanguageOS.TOPS20);     break;
			case 0x0B: ListParam.AddParametr(sLanguageTabInfo.ExtractOS, sLanguageOS.WinNT);      break;
			case 0x0C: ListParam.AddParametr(sLanguageTabInfo.ExtractOS, sLanguageOS.SMSQDOS);    break;
			case 0x0D: ListParam.AddParametr(sLanguageTabInfo.ExtractOS, sLanguageOS.AcornRISC);  break;
			case 0x0E: ListParam.AddParametr(sLanguageTabInfo.ExtractOS, sLanguageOS.VFAT);       break;
			case 0x0F: ListParam.AddParametr(sLanguageTabInfo.ExtractOS, sLanguageOS.MVS);        break;
			case 0x10: ListParam.AddParametr(sLanguageTabInfo.ExtractOS, sLanguageOS.BeOS);       break;
			case 0x11: ListParam.AddParametr(sLanguageTabInfo.ExtractOS, sLanguageOS.Tandem);     break;
			default:   ListParam.AddParametr(sLanguageTabInfo.ExtractOS, sLanguageOther.Unknown); break;
			}
	}

	//metod upakovki
	if (sSettingsParameters.MethodPack)
		switch (MethodPack)
		{
		case 0x00:	ListParam.AddParametr(sLanguageTabInfo.MethodPack, sLanguagePack.Stored);     break;
		case 0x01:	ListParam.AddParametr(sLanguageTabInfo.MethodPack, sLanguagePack.Shrunk);     break;
		case 0x02:	sprintf(BufStr, "%s%s1", sLanguagePack.Reduced, sLanguageOther.Number);
					ListParam.AddParametr(sLanguageTabInfo.MethodPack, BufStr);                   break;
		case 0x03:	sprintf(BufStr, "%s%s2", sLanguagePack.Reduced, sLanguageOther.Number);
					ListParam.AddParametr(sLanguageTabInfo.MethodPack, BufStr);                   break;
		case 0x04:	sprintf(BufStr, "%s%s3", sLanguagePack.Reduced, sLanguageOther.Number);
					ListParam.AddParametr(sLanguageTabInfo.MethodPack, BufStr);                   break;
		case 0x05:	sprintf(BufStr, "%s%s4", sLanguagePack.Reduced, sLanguageOther.Number);
					ListParam.AddParametr(sLanguageTabInfo.MethodPack, BufStr);                   break;
		case 0x06:	ListParam.AddParametr(sLanguageTabInfo.MethodPack, sLanguagePack.Imploded);   break;
		case 0x07:	ListParam.AddParametr(sLanguageTabInfo.MethodPack, sLanguagePack.Tokenizing); break;
		case 0x08:
			switch (MethodPackAdd)
			{
			case 0x00:	sprintf(BufStr, "%s (%s)", sLanguagePack.Deflated, sLanguagePack.Normal);
						ListParam.AddParametr(sLanguageTabInfo.MethodPack, BufStr);               break;
			case 0x02:	sprintf(BufStr, "%s (%s)", sLanguagePack.Deflated, sLanguagePack.Best);
						ListParam.AddParametr(sLanguageTabInfo.MethodPack, BufStr);               break;
			case 0x04:	sprintf(BufStr, "%s (%s)", sLanguagePack.Deflated, sLanguagePack.Fast);
						ListParam.AddParametr(sLanguageTabInfo.MethodPack, BufStr);               break;
			case 0x06:	sprintf(BufStr, "%s (%s)", sLanguagePack.Deflated, sLanguagePack.Superfast);
						ListParam.AddParametr(sLanguageTabInfo.MethodPack, BufStr);               break;
			default:	sprintf(BufStr, "%s (%s)", sLanguagePack.Deflated, sLanguageOther.Unknown);
						ListParam.AddParametr(sLanguageTabInfo.MethodPack, BufStr);               break;
			}
			break;
		case 0x09:
			switch (MethodPackAdd)
			{
			case 0x00:	sprintf(BufStr, "%s (%s)", sLanguagePack.Deflated64, sLanguagePack.Normal);
						ListParam.AddParametr(sLanguageTabInfo.MethodPack, BufStr);               break;
			case 0x02:	sprintf(BufStr, "%s (%s)", sLanguagePack.Deflated64, sLanguagePack.Best);
						ListParam.AddParametr(sLanguageTabInfo.MethodPack, BufStr);               break;
			case 0x04:	sprintf(BufStr, "%s (%s)", sLanguagePack.Deflated64, sLanguagePack.Fast);
						ListParam.AddParametr(sLanguageTabInfo.MethodPack, BufStr);               break;
			case 0x06:	sprintf(BufStr, "%s (%s)", sLanguagePack.Deflated64, sLanguagePack.Superfast);
						ListParam.AddParametr(sLanguageTabInfo.MethodPack, BufStr);               break;
			default:	sprintf(BufStr, "%s (%s)", sLanguagePack.Deflated64, sLanguageOther.Unknown);
						ListParam.AddParametr(sLanguageTabInfo.MethodPack, BufStr);               break;
			}
			break;
		case 0x0A:	ListParam.AddParametr(sLanguageTabInfo.MethodPack, sLanguagePack.Imploding);  break;
		case 0x0C:	ListParam.AddParametr(sLanguageTabInfo.MethodPack, sLanguagePack.BZip2);      break;
		default:   ListParam.AddParametr(sLanguageTabInfo.MethodPack, sLanguageOther.Unknown);    break;
		}

	//sjatie ot rzmera arhiva
	sprintf(BufStr, "%.1f %s", RatioArchiveSize, sLanguageOther.Percent);
	if (sSettingsParameters.RatioArchiveSize)
		ListParam.AddParametr(sLanguageTabInfo.RatioArchiveSize, BufStr);

	//sjatie ot upakovannogo razmera
	sprintf(BufStr, "%.1f %s", RatioPackFileSize, sLanguageOther.Percent);
	if (sSettingsParameters.RatioPackFileSize)
		ListParam.AddParametr(sLanguageTabInfo.RatioPackFileSize, BufStr);

	//versiya dlya upakovki
	if ((sSettingsMain.AllParametrs) || (PackVersion != UnpackVersion))
	{
		sprintf(BufStr, "%.1f", PackVersion);
		if (sSettingsParameters.PackVersion)
			ListParam.AddParametr(sLanguageTabInfo.PackVersion, BufStr);
	}

	//versiya dlya raspakovki
	sprintf(BufStr, "%.1f", UnpackVersion);
	if (sSettingsParameters.UnpackVersion)
		ListParam.AddParametr(sLanguageTabInfo.UnpackVersion, BufStr);

	//razdelitel'
	ListParam.AddParametr("", "");

	//SFX modul'
	if ((sSettingsMain.AllParametrs) || (SfxModule))
	{
		if (sSettingsParameters.SfxModule)
			if (!SfxModule) ListParam.AddParametr(sLanguageTabInfo.SfxModule, sLanguageOther.No);
			else
			{
				if (!sSettingsMain.FloatSize) NumberSeparatorStringUnitSize(BufStr, SfxModule, T_BYTE, sLanguageOther);
				else NumberFloatStringUnitSize(BufStr, SfxModule, T_BYTE, sLanguageOther);
				ListParam.AddParametr(sLanguageTabInfo.SfxModule, BufStr);
			}
	}

	//mnogotomnost'
	if ((sSettingsMain.AllParametrs) || (MultiArc))
	{
		if (sSettingsParameters.MultiArc)
			if (!MultiArc) ListParam.AddParametr(sLanguageTabInfo.MultiArc, sLanguageOther.No);
			else
			{
				ListParam.AddParametr(sLanguageTabInfo.MultiArc, sLanguageOther.Yes);
				//nomer toma
				if (VolumeNumber)
				{
					sprintf(BufStr, "%s %d", sLanguageOther.Number, VolumeNumber);
					ListParam.AddParametr(sLanguageTabInfo.NumberVolume, BufStr);
				}
			}
	}

	//kommentariy arhiva
	if ((sSettingsMain.AllParametrs) || (MainComment))
	{
		if (sSettingsParameters.MainComment)
			if (!MainComment) ListParam.AddParametr(sLanguageTabInfo.MainComment, sLanguageOther.No);
			else ListParam.AddParametr(sLanguageTabInfo.MainComment, sLanguageOther.Yes);
	}
	//tekst kommentariya
	if (pTextComment) *m_ppTextComment = pTextComment;

	//kommentarii faylov
	if ((sSettingsMain.AllParametrs) || (FileComment))
	{
		if (sSettingsParameters.FileComment)
			if (!FileComment) ListParam.AddParametr(sLanguageTabInfo.FileComment, sLanguageOther.No);
			else ListParam.AddParametr(sLanguageTabInfo.FileComment, sLanguageOther.Yes);
	}

	//parol'
	if ((sSettingsMain.AllParametrs) || (Password))
	{
		if (sSettingsParameters.Password)
			if (!Password) ListParam.AddParametr(sLanguageTabInfo.Password, sLanguageOther.No);
			else ListParam.AddParametr(sLanguageTabInfo.Password, sLanguageOther.Yes);
	}

	//razdelitel'
	ListParam.AddParametr("", "");

	sprintf(BufStr, "%.2f %s", TestTime, sLanguageOther.Second);
	if (sSettingsParameters.TestTime)
		ListParam.AddParametr(sLanguageTabInfo.TestTime, BufStr);

	sprintf(BufStr, "%.2f %s", AnalyzeTime, sLanguageOther.Second);
	if (sSettingsParameters.AnalyzeTime)
		ListParam.AddParametr(sLanguageTabInfo.AnalyzeTime, BufStr);

	sprintf(BufStr, "%.2f %s", AllTime, sLanguageOther.Second);
	if (sSettingsParameters.AllTime)
		ListParam.AddParametr(sLanguageTabInfo.AllTime, BufStr);

	return 1;
}

//dlya archiva ZOO
int MySelectInfo::SelectInfoZoo()
{
	//dlya uprosheniya obrasheniya k dannim sobrannim v odnom meste
	SettingsMain&       sSettingsMain       = m_pSettings->sSettingsMain;
	SettingsParameters& sSettingsParameters = m_pSettings->sSettingsParameters;
	LanguageTabInfo&    sLanguageTabInfo    = m_pLanguage->sLanguageTabInfo;
	LanguageOS&         sLanguageOS         = m_pLanguage->sLanguageOS;
	LanguagePack&       sLanguagePack       = m_pLanguage->sLanguagePack;
	LanguageOther&      sLanguageOther      = m_pLanguage->sLanguageOther;
	MListParametrs&     ListParam           = *m_pListParam;

	TZoo* obj_zoo = (TZoo*)m_pArchData;

	ULONGLONG UnpackSizeFiles   = obj_zoo->GetUnpackSizeFiles();
	ULONGLONG PackSizeFiles     = obj_zoo->GetPackSizeFiles();
	ULONGLONG ArchiveSize       = obj_zoo->GetArchiveSize();
	unsigned  NumberFiles       = obj_zoo->GetNumberFiles();
	short     MethodPack        = obj_zoo->GetMethodPack();
	double    RatioArchiveSize  = obj_zoo->GetRatioArchiveSize();
	double    RatioPackFileSize = obj_zoo->GetRatioPackFileSize();
	double    PackVersion       = obj_zoo->GetPackVersion();
	double    UnpackVersion     = obj_zoo->GetUnpackVersion();
	BOOL      MainComment       = obj_zoo->GetMainComment();
	BOOL      FileComment       = obj_zoo->GetFileComment();
	char*     pTextComment      = obj_zoo->GetTextComment();
	double    TestTime          = m_TestTime;
	double    AnalyzeTime       = obj_zoo->GetAnalyzeTime();
	double    AllTime           = m_TestTime + AnalyzeTime;

	if (((int)RatioPackFileSize == 0) && (RatioPackFileSize > 0)) *m_pRatio1 = 1;
	else *m_pRatio1 = (int)RatioPackFileSize;
	if (((int)RatioArchiveSize == 0) && (RatioArchiveSize > 0)) *m_pRatio2 = 1;
	else *m_pRatio2 = (int)RatioArchiveSize;

	char BufStr[MAX_LOADSTRING] = {NULL};

	//zagolovok
	char* _ARCH = m_pLanguage->sLanguageArchives.Zoo;
	sprintf(BufStr, "%s %s", _ARCH, sLanguageOther.Archive);
	ListParam.AddParametr("", BufStr);

	//neupakovanniy razmer
	if ((sSettingsMain.AllParametrs) || (UnpackSizeFiles))
	{
		if (!sSettingsMain.FloatSize) NumberSeparatorStringUnitSize(BufStr, UnpackSizeFiles, T_BYTE, sLanguageOther);
		else NumberFloatStringUnitSize(BufStr, UnpackSizeFiles, T_BYTE, sLanguageOther);
		if (sSettingsParameters.UnpackSizeFiles)
			ListParam.AddParametr(sLanguageTabInfo.UnpackSizeFiles, BufStr);
	}

	//upakovanniy razmer
	if ((sSettingsMain.AllParametrs) || (PackSizeFiles))
	{
		if (!sSettingsMain.FloatSize) NumberSeparatorStringUnitSize(BufStr, PackSizeFiles, T_BYTE, sLanguageOther);
		else NumberFloatStringUnitSize(BufStr, PackSizeFiles, T_BYTE, sLanguageOther);
		if (sSettingsParameters.PackSizeFiles)
			ListParam.AddParametr(sLanguageTabInfo.PackSizeFiles, BufStr);
	}

	//razmer arhiva
	if ((sSettingsMain.AllParametrs) || (ArchiveSize))
	{
		if (!sSettingsMain.FloatSize) NumberSeparatorStringUnitSize(BufStr, ArchiveSize, T_BYTE, sLanguageOther);
		else NumberFloatStringUnitSize(BufStr, ArchiveSize, T_BYTE, sLanguageOther);
		if (sSettingsParameters.ArchiveSize)
			ListParam.AddParametr(sLanguageTabInfo.ArchiveSize, BufStr);
	}

	//kolichestvo faylov
	if ((sSettingsMain.AllParametrs) || (NumberFiles))
	{
		if (NumberFiles) NumberSeparatorString(BufStr, NumberFiles);
		else sprintf(BufStr, "%s", sLanguageOther.No);
		if (sSettingsParameters.NumberFiles)
			ListParam.AddParametr(sLanguageTabInfo.NumberFiles, BufStr);
	}

	//razdelitel'
	ListParam.AddParametr("", "");

	//metod upakovki
	if (sSettingsParameters.MethodPack)
		switch (MethodPack)
		{
		case 0x00:	ListParam.AddParametr(sLanguageTabInfo.MethodPack, sLanguagePack.Stored);   break;
		case 0x01:	sprintf(BufStr, "%s (%s)", sLanguagePack.Crunched, sLanguagePack.Normal);
					ListParam.AddParametr(sLanguageTabInfo.MethodPack, BufStr);                 break;
		case 0x02:	ListParam.AddParametr(sLanguageTabInfo.MethodPack, sLanguagePack.Good);     break;
		default:	ListParam.AddParametr(sLanguageTabInfo.MethodPack, sLanguageOther.Unknown); break;
		}

	//sjatie ot rzmera arhiva
	sprintf(BufStr, "%.1f %s", RatioArchiveSize, sLanguageOther.Percent);
	if (sSettingsParameters.RatioArchiveSize)
		ListParam.AddParametr(sLanguageTabInfo.RatioArchiveSize, BufStr);

	//sjatie ot upakovannogo razmera
	sprintf(BufStr, "%.1f %s", RatioPackFileSize, sLanguageOther.Percent);
	if (sSettingsParameters.RatioPackFileSize)
		ListParam.AddParametr(sLanguageTabInfo.RatioPackFileSize, BufStr);

	//versiya dlya upakovki
	if ((sSettingsMain.AllParametrs) || (PackVersion != UnpackVersion))
	{
		sprintf(BufStr, "%.1f", PackVersion);
		if (sSettingsParameters.PackVersion)
			ListParam.AddParametr(sLanguageTabInfo.PackVersion, BufStr);
	}

	//versiya dlya raspakovki
	sprintf(BufStr, "%.1f", UnpackVersion);
	if (sSettingsParameters.UnpackVersion)
		ListParam.AddParametr(sLanguageTabInfo.UnpackVersion, BufStr);

	//razdelitel'
	ListParam.AddParametr("", "");

	//kommentariy arhiva
	if ((sSettingsMain.AllParametrs) || (MainComment))
	{
		if (sSettingsParameters.MainComment)
			if (!MainComment) ListParam.AddParametr(sLanguageTabInfo.MainComment, sLanguageOther.No);
			else ListParam.AddParametr(sLanguageTabInfo.MainComment, sLanguageOther.Yes);
	}
	//tekst kommentariya
	if (pTextComment) *m_ppTextComment = pTextComment;

	//kommentarii faylov
	if ((sSettingsMain.AllParametrs) || (FileComment))
	{
		if (sSettingsParameters.FileComment)
			if (!FileComment) ListParam.AddParametr(sLanguageTabInfo.FileComment, sLanguageOther.No);
			else ListParam.AddParametr(sLanguageTabInfo.FileComment, sLanguageOther.Yes);
	}

	//razdelitel'
	ListParam.AddParametr("", "");

	sprintf(BufStr, "%.2f %s", TestTime, sLanguageOther.Second);
	if (sSettingsParameters.TestTime)
		ListParam.AddParametr(sLanguageTabInfo.TestTime, BufStr);

	sprintf(BufStr, "%.2f %s", AnalyzeTime, sLanguageOther.Second);
	if (sSettingsParameters.AnalyzeTime)
		ListParam.AddParametr(sLanguageTabInfo.AnalyzeTime, BufStr);

	sprintf(BufStr, "%.2f %s", AllTime, sLanguageOther.Second);
	if (sSettingsParameters.AllTime)
		ListParam.AddParametr(sLanguageTabInfo.AllTime, BufStr);

	return 1;
}

//dlya arhivatornih (wcx) plaginov Total Commander
int MySelectInfo::SelectInfoWcxPluginTC()
{
	//dlya uprosheniya obrasheniya k dannim sobrannim v odnom meste
	SettingsMain&       sSettingsMain       = m_pSettings->sSettingsMain;
	SettingsParameters& sSettingsParameters = m_pSettings->sSettingsParameters;
	LanguageTabInfo&    sLanguageTabInfo    = m_pLanguage->sLanguageTabInfo;
	LanguageOS&         sLanguageOS         = m_pLanguage->sLanguageOS;
	LanguagePack&       sLanguagePack       = m_pLanguage->sLanguagePack;
	LanguageOther&      sLanguageOther      = m_pLanguage->sLanguageOther;
	MListParametrs&     ListParam           = *m_pListParam;

	TWcxPluginTC* obj_wcxplugintc = (TWcxPluginTC*)m_pArchData;

	ULONGLONG UnpackSizeFiles   = obj_wcxplugintc->GetUnpackSizeFiles();
	ULONGLONG PackSizeFiles     = obj_wcxplugintc->GetPackSizeFiles();
	ULONGLONG ArchiveSize       = obj_wcxplugintc->GetArchiveSize();
	unsigned  NumberFiles       = obj_wcxplugintc->GetNumberFiles();
	unsigned  NumberFolders     = obj_wcxplugintc->GetNumberFolders();
	short     HostOS            = obj_wcxplugintc->GetHostOS();
	short     MethodPack        = obj_wcxplugintc->GetMethodPack();
	double    RatioArchiveSize  = obj_wcxplugintc->GetRatioArchiveSize();
	double    RatioPackFileSize = obj_wcxplugintc->GetRatioPackFileSize();
	double    UnpackVersion     = obj_wcxplugintc->GetUnpackVersion();
	unsigned  SfxModule         = obj_wcxplugintc->GetSfxModule();
	BOOL      MainComment       = obj_wcxplugintc->GetMainComment();
	BOOL      FileComment       = obj_wcxplugintc->GetFileComment();
	char*     pTextComment      = obj_wcxplugintc->GetTextComment();
	double    TestTime          = m_TestTime;
	double    AnalyzeTime       = obj_wcxplugintc->GetAnalyzeTime();
	double    AllTime           = m_TestTime + AnalyzeTime;

	if (((int)RatioPackFileSize == 0) && (RatioPackFileSize > 0)) *m_pRatio1 = 1;
	else *m_pRatio1 = (int)RatioPackFileSize;
	if (((int)RatioArchiveSize == 0) && (RatioArchiveSize > 0)) *m_pRatio2 = 1;
	else *m_pRatio2 = (int)RatioArchiveSize;

	char BufStr[MAX_LOADSTRING] = {NULL};

	//zagolovok
	const char* _ARCH = obj_wcxplugintc->GetInternalName();
	const char* _SFX  = m_pLanguage->sLanguageArchives.Sfx;
	if (SfxModule) sprintf(BufStr, "%s %s %s", _SFX, _ARCH, sLanguageOther.Plugin);
	else sprintf(BufStr, "%s %s", _ARCH, sLanguageOther.Plugin);
	ListParam.AddParametr("", BufStr);

	//neupakovanniy razmer
	if ((sSettingsMain.AllParametrs) || (UnpackSizeFiles))
	{
		if (!sSettingsMain.FloatSize) NumberSeparatorStringUnitSize(BufStr, UnpackSizeFiles, T_BYTE, sLanguageOther);
		else NumberFloatStringUnitSize(BufStr, UnpackSizeFiles, T_BYTE, sLanguageOther);
		if (sSettingsParameters.UnpackSizeFiles)
			ListParam.AddParametr(sLanguageTabInfo.UnpackSizeFiles, BufStr);
	}

	//upakovanniy razmer
	if ((sSettingsMain.AllParametrs) || (PackSizeFiles))
	{
		if (!sSettingsMain.FloatSize) NumberSeparatorStringUnitSize(BufStr, PackSizeFiles, T_BYTE, sLanguageOther);
		else NumberFloatStringUnitSize(BufStr, PackSizeFiles, T_BYTE, sLanguageOther);
		if (sSettingsParameters.PackSizeFiles)
			ListParam.AddParametr(sLanguageTabInfo.PackSizeFiles, BufStr);
	}

	//razmer arhiva
	if ((sSettingsMain.AllParametrs) || (ArchiveSize))
	{
		if (!sSettingsMain.FloatSize) NumberSeparatorStringUnitSize(BufStr, ArchiveSize, T_BYTE, sLanguageOther);
		else NumberFloatStringUnitSize(BufStr, ArchiveSize, T_BYTE, sLanguageOther);
		if (sSettingsParameters.ArchiveSize)
			ListParam.AddParametr(sLanguageTabInfo.ArchiveSize, BufStr);
	}

	//kolichestvo faylov
	if ((sSettingsMain.AllParametrs) || (NumberFiles))
	{
		if (NumberFiles) NumberSeparatorString(BufStr, NumberFiles);
		else sprintf(BufStr, "%s", sLanguageOther.No);
		if (sSettingsParameters.NumberFiles)
			ListParam.AddParametr(sLanguageTabInfo.NumberFiles, BufStr);
	}

	//kolichestvo papok
	if ((sSettingsMain.AllParametrs) || (NumberFolders))
	{
		if (NumberFolders) NumberSeparatorString(BufStr, NumberFolders);
		else sprintf(BufStr, "%s", sLanguageOther.No);
		if (sSettingsParameters.NumberFolders)
			ListParam.AddParametr(sLanguageTabInfo.NumberFolders, BufStr);
	}

	//razdelitel'
	ListParam.AddParametr("", "");


	//host OS
	if ((sSettingsMain.AllParametrs) || (HostOS > 0))
		if (sSettingsParameters.HostOS)
		{
			sprintf(BufStr, "%d", HostOS);
			ListParam.AddParametr(sLanguageTabInfo.HostOS, BufStr);
		}

	//metod upakovki
	if ((sSettingsMain.AllParametrs) || (MethodPack > 0))
		if (sSettingsParameters.MethodPack)
		{
			sprintf(BufStr, "%d", MethodPack);
			ListParam.AddParametr(sLanguageTabInfo.MethodPack, BufStr);
		}


	//sjatie ot rzmera arhiva
	if ((sSettingsMain.AllParametrs) || (RatioArchiveSize))
	{
		sprintf(BufStr, "%.1f %s", RatioArchiveSize, sLanguageOther.Percent);
		if (sSettingsParameters.RatioArchiveSize)
			ListParam.AddParametr(sLanguageTabInfo.RatioArchiveSize, BufStr);
	}

	//sjatie ot upakovannogo razmera
	if ((sSettingsMain.AllParametrs) || (RatioPackFileSize))
	{
		sprintf(BufStr, "%.1f %s", RatioPackFileSize, sLanguageOther.Percent);
		if (sSettingsParameters.RatioPackFileSize)
			ListParam.AddParametr(sLanguageTabInfo.RatioPackFileSize, BufStr);
	}

	//versiya dlya raspakovki
	if ((sSettingsMain.AllParametrs) || (UnpackVersion))
	{
		sprintf(BufStr, "%.1f", UnpackVersion);
		if (sSettingsParameters.UnpackVersion)
			ListParam.AddParametr(sLanguageTabInfo.UnpackVersion, BufStr);
	}

	//razdelitel'
	ListParam.AddParametr("", "");

	//SFX modul'
	if ((sSettingsMain.AllParametrs) || (SfxModule))
	{
		if (sSettingsParameters.SfxModule)
			if (!SfxModule) ListParam.AddParametr(sLanguageTabInfo.SfxModule, sLanguageOther.No);
			else ListParam.AddParametr(sLanguageTabInfo.SfxModule, sLanguageOther.Yes);
	}

	//kommentariy arhiva
	if ((sSettingsMain.AllParametrs) || (MainComment))
	{
		if (sSettingsParameters.MainComment)
			if (!MainComment) ListParam.AddParametr(sLanguageTabInfo.MainComment, sLanguageOther.No);
			else ListParam.AddParametr(sLanguageTabInfo.MainComment, sLanguageOther.Yes);
	}
	//tekst kommentariya
	if (pTextComment) *m_ppTextComment = pTextComment;

	//kommentarii faylov
	if ((sSettingsMain.AllParametrs) || (FileComment))
	{
		if (sSettingsParameters.FileComment)
			if (!FileComment) ListParam.AddParametr(sLanguageTabInfo.FileComment, sLanguageOther.No);
			else ListParam.AddParametr(sLanguageTabInfo.FileComment, sLanguageOther.Yes);
	}

	//razdelitel'
	ListParam.AddParametr("", "");

	sprintf(BufStr, "%.2f %s", TestTime, sLanguageOther.Second);
	if (sSettingsParameters.TestTime)
		ListParam.AddParametr(sLanguageTabInfo.TestTime, BufStr);

	sprintf(BufStr, "%.2f %s", AnalyzeTime, sLanguageOther.Second);
	if (sSettingsParameters.AnalyzeTime)
		ListParam.AddParametr(sLanguageTabInfo.AnalyzeTime, BufStr);

	sprintf(BufStr, "%.2f %s", AllTime, sLanguageOther.Second);
	if (sSettingsParameters.AllTime)
		ListParam.AddParametr(sLanguageTabInfo.AllTime, BufStr);

	return 1;
}

/////////////////////////////////////////////////////////////////////////////
//FUNCTIONS PUBLIC
/////////////////////////////////////////////////////////////////////////////

//viborka dannih arhiva v zavisimosti ot tipa arhiva
int MySelectInfo::SelectInfoArchive(MySettings* pSettings, MyLanguage* pLanguage, TArchive* pArchData, TypeOfArchive* pArchiveType, MListParametrs* pListParam, char** ppTextComment, double TestTime, int* pRatio2, int* pRatio1)
{
	m_pSettings     = pSettings;
	m_pLanguage     = pLanguage;
	m_pArchData     = pArchData;
	m_pArchiveType  = pArchiveType;
	m_pListParam    = pListParam;
	m_ppTextComment = ppTextComment;
	m_TestTime      = TestTime;
	m_pRatio1       = pRatio1;
	m_pRatio2       = pRatio2;

	switch (*m_pArchiveType)
	{
	case T_ACE:
		SelectInfoAce();         break;
	case T_ARC_PAK:
		SelectInfoArcPak();      break;
	case T_ARJ:
		SelectInfoArj();         break;
	case T_BH:
		SelectInfoBh();          break;
	case T_BZIP2_TBZ2:
		SelectInfoBZip2Tbz2();   break;
	case T_CAB:
		SelectInfoCab();         break;
	case T_CPIO:
		SelectInfoCpio();        break;
	case T_DWC:
		SelectInfoDwc();         break;
	case T_GZIP_TGZ:
		SelectInfoGZipTgz();     break;
	case T_HA:
		SelectInfoHa();          break;
	case T_LHA_LZH:
		SelectInfoLhaLzh();      break;
	case T_PPM:
		SelectInfoPpm();         break;
	case T_RAR:
		SelectInfoRar();         break;
	case T_SQZ:
		SelectInfoSqz();         break;
	case T_TAR:
		SelectInfoTar();         break;
	case T_Z:
		SelectInfoZ();           break;
	case T_ZIP_JAR:
		SelectInfoZipJar();      break;
	case T_ZOO:
		SelectInfoZoo();         break;
	case T_WCX_PLUGIN_TC:
		SelectInfoWcxPluginTC(); break;
	default:
		break;
	}
	return 1;
}
