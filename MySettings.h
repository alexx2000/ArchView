/////////////////////////////////////////////////////////////////////////////
//MySettings.h
//////////////////////////////////////////////////////////////////////

#ifndef T_MY_SETTIGS_H
#define T_MY_SETTIGS_H

#include "MIniFile.h"
#include "MListStrStr.h"
#include "StructType.h"

//nastroyki programmi (file ArchView.ini)
typedef struct _SettingsMain
{
	unsigned ArchivesSFX;
	unsigned SizeSFX;
	unsigned FloatSize;
	unsigned AllParametrs;
	unsigned InvertRatio;
	unsigned SetDetectString;
	char     Language[MAX_LOADSTRING];
	unsigned UseWCXPluginsTC;
	unsigned OptimUsePlugins;
	unsigned UseWincmdIniWCX;
	char     PathWincmdIni[MAX_LOADSTRING];
} SettingsMain;

//parametri okna plagina (file ArchView.ini)
typedef struct _SettingsWindow
{
	unsigned SaveLastTab;
	unsigned LastTabNumber;
	unsigned SaveWindowSize;
	unsigned WidthWindow;
	unsigned HeightWindow;
	unsigned ShowCommentOnTabInfo;
	unsigned ShowTabContext;
	unsigned ShowTabComment;
	unsigned ShowTabAbout;
	unsigned ShowProgress;
	unsigned ShowLeftRatio;
	unsigned ShowRightRatio;
	unsigned FontSize;
} SettingsWindow;

//vivodimie parametri (file ArchView.ini)
typedef struct _SettingsParameters
{
	unsigned UnpackSizeFiles;
	unsigned PackSizeFiles;
	unsigned ArchiveSize;
	unsigned NumberFiles;
	unsigned NumberFolders;
	unsigned NumberChapters;
	unsigned DictionarySize;
	unsigned HostOS;
	unsigned ExtractOS;
	unsigned MethodPack;
	unsigned MemorySize;
	unsigned RatioArchiveSize;
	unsigned RatioPackFileSize;
	unsigned PackVersion;
	unsigned UnpackVersion;
	unsigned SfxModule;
	unsigned RecoveryRecord;
	unsigned Solid;
	unsigned MultiArc;
	unsigned MainComment;
	unsigned FileComment;
	unsigned ChapterComment;
	unsigned Password;
	unsigned ArchiveLock;
	unsigned AuthVerification;
	unsigned TestTime;
	unsigned AnalyzeTime;
	unsigned AllTime;
	unsigned DateCreated;
	unsigned DateModified;
	unsigned TimeCreated;
	unsigned TimeModified;
	unsigned OriginalName;
	unsigned Magic;
} SettingsParameters;

//parametri logfile (file ArchView.ini)
typedef struct _SettingsLogfile
{
	unsigned CreateLogFile;
	unsigned DetailLogFile;
	unsigned WidthAuthorName;
	unsigned WidthMessage;
} SettingsLogfile;

//obrabativaemie arhivi (file ArchView.ini)
typedef struct _SettingsArchives
{
	char PriorityArchives[MAX_LOADSTRING];
	int  Ace;
	int  ArcPak;
	int  Arj;
	int  Bh;
	int  BZip2Tbz2;
	int  Cab;
	int  Cpio;
	int  Dwc;
	int  GZipTgz;
	int  Ha;
	int  LhaLzh;
	int  Ppm;
	int  Rar;
	int  Sqz;
	int  Tar;
	int  Z;
	int  ZipJar;
	int  Zoo;
} SettingsArchives;

//rasshireniya dlya arhivov (file ArchView.ini)
typedef struct _SettingsExtensions
{
	int  OnlyExtensions;
	char ExtAce      [MAX_LOADSTRING];
	char ExtArcPak   [MAX_LOADSTRING];
	char ExtArj      [MAX_LOADSTRING];
	char ExtBh       [MAX_LOADSTRING];
	char ExtBZip2Tbz2[MAX_LOADSTRING];
	char ExtCab      [MAX_LOADSTRING];
	char ExtCpio     [MAX_LOADSTRING];
	char ExtDwc      [MAX_LOADSTRING];
	char ExtGZipTgz  [MAX_LOADSTRING];
	char ExtHa       [MAX_LOADSTRING];
	char ExtLhaLzh   [MAX_LOADSTRING];
	char ExtPpm      [MAX_LOADSTRING];
	char ExtRar      [MAX_LOADSTRING];
	char ExtSqz      [MAX_LOADSTRING];
	char ExtTar      [MAX_LOADSTRING];
	char ExtZ        [MAX_LOADSTRING];
	char ExtZipJar   [MAX_LOADSTRING];
	char ExtZoo      [MAX_LOADSTRING];
} SettingsExtensions;

//DLL dlya arhivov (file ArchView.ini)
typedef struct _SettingsLibraries
{
	char Ace[MAX_LOADSTRING];
	char Rar[MAX_LOADSTRING];
} SettingsLibraries;

//spisok (imya,put') arhivatornih (wcx) plaginov Total Commander (file ArchView.ini or file wincmd.ini)
typedef struct _SettingsWCXPluginsTC
{
	MListStrStr WCXPluginsTC;
} SettingsWCXPluginsTC;

//rasshireniya dlya arhivatornih (wcx) plaginov Total Commander (file ArchView.ini)
typedef struct _SettingsWCXExtensions
{
	int  OnlyExtensions;
	MListStrStr WCXExtensions;
} SettingsWCXExtensions;

//klass dlya organizacii nastroek programmi
//prosto sobrat' vse nastroyki vmeste
class MySettings
{
private:
	//imena sekciy fayla nastoek ArchView.ini
	typedef struct _SettingsSections
	{
		char SectMain         [MAX_LOADSTRING];
		char SectWindow       [MAX_LOADSTRING];
		char SectParameters   [MAX_LOADSTRING];
		char SectLogfile      [MAX_LOADSTRING];
		char SectArchives     [MAX_LOADSTRING];
		char SectExtensions   [MAX_LOADSTRING];
		char SectLibraries    [MAX_LOADSTRING];
		char SectWCXPluginsTC [MAX_LOADSTRING];	//ArchView.ini (sekciya wcx-plugins)
		char SectPackerPlugins[MAX_LOADSTRING];	//wincmd.ini (sekciya wcx-plugins)
		char SectWCXExtensions[MAX_LOADSTRING];
	} SettingsSections;

	int CheckPriorityArchives();

public:
	MIniFile         fINI;				//dlya obrasheniya k faylu nastroek (file ArchView.ini)
	MIniFile         fwincmdINI;		//dlya obrasheniya k faylu nastroek (file wincmd.ini) (dlya wcx-plugins)

	SettingsSections SectionsINI;		//sekcii fayla nastroek (file ArchView.ini)

	//[Main]
	SettingsMain          sSettingsMain;			//nastroyki programmi (file ArchView.ini)
	//[Window]
	SettingsWindow        sSettingsWindow;			//parametri okna plagina (file ArchView.ini)
	//[Parameters]
	SettingsParameters    sSettingsParameters;		//vivodimie parametri (file ArchView.ini)
	//[Logfile]
	SettingsLogfile       sSettingsLogfile;			//parametri logfile (file ArchView.ini)
	//[Archives]
	SettingsArchives      sSettingsArchives;		//obrabativaemie arhivi (file ArchView.ini)
	//[Extensions]
	SettingsExtensions    sSettingsExtensions;		//rasshireniya dlya arhivov (file ArchView.ini)
	//[Libraries]
	SettingsLibraries     sSettingsLibraries;		//DLL dlya arhivov (file ArchView.ini)
	//[WCXPluginsTC] ArchView.ini or [PackerPlugins] wincmd.ini
	SettingsWCXPluginsTC  sSettingsWCXPluginsTC;	//spisok (imya,put') arhivatornih (wcx) plaginov Total Commander (file ArchView.ini or file wincmd.ini)
	//[WCXExtensions]
	SettingsWCXExtensions sSettingsWCXExtensions;	//rasshireniya dlya arhivatornih (wcx) plaginov Total Commander (file ArchView.ini)

	//prioritet arhivov pri testirovanii
	//prioritet idet po nomeru kajdomu arhivu sootvetstvuet svoy nomer
	//nomera prisvavivayutsya nachinaya s 1 v alfavitnom poryadke arhivov
	//ACE = 1 | ARJ = 2 | CAB = 3
	//"2 1 3" t.e. snachala ARJ, potom ACE a potom CAB
	int  MasPriorityArchives[T_NUMBER_ARCHIVES-1];

	MySettings();
	~MySettings() {;}

	//dlya raboti s faylami nastroek programmi
	int ReadSettingsProgram(char*);
};

#endif //T_MY_SETTIGS_H
