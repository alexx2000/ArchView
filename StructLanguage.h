/////////////////////////////////////////////////////////////////////////////
//StructLanguage.h
/////////////////////////////////////////////////////////////////////////////

#ifndef T_STRUCT_LANGUAGE_H
#define T_STRUCT_LANGUAGE_H

#include "StructType.h"

/////////////////////////////////////////////////////////////////////////////
//LANGUAGE STRUCTURES
/////////////////////////////////////////////////////////////////////////////

//yazik programmi zakladki(file *.lng)
typedef struct _LanguageTabs
{
	//[Tabs]
	char Info   [MAX_LOADSTRING];
	char Context[MAX_LOADSTRING];
	char Comment[MAX_LOADSTRING];
	char About  [MAX_LOADSTRING];
} LanguageTabs;

//yazik programmi zakladka Info(file *.lng)
typedef struct _LanguageTabInfo
{
	//[Info]
	char UnpackSizeFiles  [MAX_LOADSTRING];
	char PackSizeFiles    [MAX_LOADSTRING];
	char ArchiveSize      [MAX_LOADSTRING];
	char NumberFiles      [MAX_LOADSTRING];
	char NumberFolders    [MAX_LOADSTRING];
	char NumberChapters   [MAX_LOADSTRING];
	char DictionarySize   [MAX_LOADSTRING];
	char HostOS           [MAX_LOADSTRING];
	char ExtractOS        [MAX_LOADSTRING];
	char MethodPack       [MAX_LOADSTRING];
	char MemorySize       [MAX_LOADSTRING];
	char RatioArchiveSize [MAX_LOADSTRING];
	char RatioPackFileSize[MAX_LOADSTRING];
	char PackVersion      [MAX_LOADSTRING];
	char UnpackVersion    [MAX_LOADSTRING];
	char SfxModule        [MAX_LOADSTRING];
	char RecoveryRecord   [MAX_LOADSTRING];
	char Solid            [MAX_LOADSTRING];
	char MultiArc         [MAX_LOADSTRING];
	char NumberVolume     [MAX_LOADSTRING];
	char MainComment      [MAX_LOADSTRING];
	char FileComment      [MAX_LOADSTRING];
	char ChapterComment   [MAX_LOADSTRING];
	char Password         [MAX_LOADSTRING];
	char ArchiveLock      [MAX_LOADSTRING];
	char AuthVerification [MAX_LOADSTRING];
	char TestTime         [MAX_LOADSTRING];
	char AnalyzeTime      [MAX_LOADSTRING];
	char AllTime          [MAX_LOADSTRING];
	char DateCreated      [MAX_LOADSTRING];
	char DateModified     [MAX_LOADSTRING];
	char TimeCreated      [MAX_LOADSTRING];
	char TimeModified     [MAX_LOADSTRING];
	char OriginalName     [MAX_LOADSTRING];
	char Magic            [MAX_LOADSTRING];
} LanguageTabInfo;


//yazik programmi zakladka About(file *.lng)
typedef struct _LanguageTabAbout
{
	//[About]
	char Version          [MAX_LOADSTRING];
	char TypeProgram      [MAX_LOADSTRING];
	char AndLater         [MAX_LOADSTRING];
	char UseProgram       [MAX_LOADSTRING];
	char FunctionProgram  [MAX_LOADSTRING];
	char SupportWCXPlugins[MAX_LOADSTRING];
	char SpecialThanksTo  [MAX_LOADSTRING];
	char AdviceAndHelp    [MAX_LOADSTRING];
	char AuthorProgram    [MAX_LOADSTRING];
	char AuthorSite       [MAX_LOADSTRING];
	char ForTranslation   [MAX_LOADSTRING];
	char Contact          [MAX_LOADSTRING];
	char SendComment      [MAX_LOADSTRING];
	char AllRightsReserved[MAX_LOADSTRING];
	char EMail            [MAX_LOADSTRING];
	char HomePage         [MAX_LOADSTRING];
	char Translation      [MAX_LOADSTRING];
	char TranslationMan   [MAX_LOADSTRING];
	char TranslationMail  [MAX_LOADSTRING];
	char TranslationSite  [MAX_LOADSTRING];
} LanguageTabAbout;


//yazik programmi Operacionnie sistemi(file *.lng)
typedef struct _LanguageOS
{
	//[OS]
	char AcornRISC[MAX_LOADSTRING];
	char Amiga    [MAX_LOADSTRING];
	char AppleGS  [MAX_LOADSTRING];
	char AtariST  [MAX_LOADSTRING];
	char BeOS     [MAX_LOADSTRING];
	char CPM      [MAX_LOADSTRING];
	char DOS      [MAX_LOADSTRING];
	char MacOS    [MAX_LOADSTRING];
	char MVS      [MAX_LOADSTRING];
	char Next     [MAX_LOADSTRING];
	char OS2      [MAX_LOADSTRING];
	char Primos   [MAX_LOADSTRING];
	char SMSQDOS  [MAX_LOADSTRING];
	char Tandem   [MAX_LOADSTRING];
	char TOPS20   [MAX_LOADSTRING];
	char Unix     [MAX_LOADSTRING];
	char VaxVMS   [MAX_LOADSTRING];
	char VFAT     [MAX_LOADSTRING];
	char VMCMS    [MAX_LOADSTRING];
	char Win32    [MAX_LOADSTRING];
	char Win95    [MAX_LOADSTRING];
	char WinNT    [MAX_LOADSTRING];
	char ZSystem  [MAX_LOADSTRING];
} LanguageOS;

//yazik programmi metodi upakovki(file *.lng)
typedef struct _LanguagePack
{
	//[Pack]
	//all
	char Stored    [MAX_LOADSTRING];
	char Superfast [MAX_LOADSTRING];
	char Fastest   [MAX_LOADSTRING];
	char Fast      [MAX_LOADSTRING];
	char Normal    [MAX_LOADSTRING];
	char Good      [MAX_LOADSTRING];
	char Verygood  [MAX_LOADSTRING];
	char Best      [MAX_LOADSTRING];
	//zip, gz
	char Deflated  [MAX_LOADSTRING];
	char Deflated64[MAX_LOADSTRING];
	char Imploded  [MAX_LOADSTRING];
	char Imploding [MAX_LOADSTRING];
	char Shrunk    [MAX_LOADSTRING];
	char Reduced   [MAX_LOADSTRING];
	char Tokenizing[MAX_LOADSTRING];
	char BZip2     [MAX_LOADSTRING];
	//bh
	char Fuse      [MAX_LOADSTRING];
	//bz2, ppm
	char Rate      [MAX_LOADSTRING];
	//arc, pak, dwc, zoo
	char Crunched  [MAX_LOADSTRING];
	char Unpacked  [MAX_LOADSTRING];
	char Packed    [MAX_LOADSTRING];
	char Squeezed  [MAX_LOADSTRING];
	char Squashed  [MAX_LOADSTRING];
	char Crushed   [MAX_LOADSTRING];
	char Distilled [MAX_LOADSTRING];
	//cab
	char MSZIP     [MAX_LOADSTRING];
	char LZX       [MAX_LOADSTRING];
	char Quantum   [MAX_LOADSTRING];
	//ha
	char CPY       [MAX_LOADSTRING];
	char ASC       [MAX_LOADSTRING];
	char HSC       [MAX_LOADSTRING];
	//lha, lzh
	char lh0       [MAX_LOADSTRING];
	char lh1       [MAX_LOADSTRING];
	char lh2       [MAX_LOADSTRING];
	char lh3       [MAX_LOADSTRING];
	char lh4       [MAX_LOADSTRING];
	char lh5       [MAX_LOADSTRING];
	char lh6       [MAX_LOADSTRING];
	char lh7       [MAX_LOADSTRING];
	char lzs       [MAX_LOADSTRING];
	char lz4       [MAX_LOADSTRING];
	char lz5       [MAX_LOADSTRING];
} LanguagePack;

//yazik programmi drugoe/rasnoe(file *.lng)
typedef struct _LanguageOther
{
	//[Other]
	char Yes       [MAX_LOADSTRING];
	char No        [MAX_LOADSTRING];
	char Byte      [MAX_LOADSTRING];
	char KByte     [MAX_LOADSTRING];
	char MByte     [MAX_LOADSTRING];
	char GByte     [MAX_LOADSTRING];
	char TByte     [MAX_LOADSTRING];
	char Second    [MAX_LOADSTRING];
	char Percent   [MAX_LOADSTRING];
	char Number    [MAX_LOADSTRING];
	char Unknown   [MAX_LOADSTRING];
	char PleaseWait[MAX_LOADSTRING];
	char Archive   [MAX_LOADSTRING];
	char Plugin    [MAX_LOADSTRING];
} LanguageOther;

//yazik programmi drugoe/rasnoe(file *.lng)
typedef struct _LanguageArchives
{
	//[Archives]
	char Ace      [MAX_LOADSTRING];
	char ArcPak   [MAX_LOADSTRING];
	char Arc      [MAX_LOADSTRING];
	char Pak      [MAX_LOADSTRING];
	char Arj      [MAX_LOADSTRING];
	char Bh       [MAX_LOADSTRING];
	char BZip2Tbz2[MAX_LOADSTRING];
	char BZip2    [MAX_LOADSTRING];
	char Cab      [MAX_LOADSTRING];
	char Cpio     [MAX_LOADSTRING];
	char Dwc      [MAX_LOADSTRING];
	char GZipTgz  [MAX_LOADSTRING];
	char GZip     [MAX_LOADSTRING];
	char Ha       [MAX_LOADSTRING];
	char LhaLzh   [MAX_LOADSTRING];
	char Lha      [MAX_LOADSTRING];
	char Lzh      [MAX_LOADSTRING];
	char Ppm      [MAX_LOADSTRING];
	char Rar      [MAX_LOADSTRING];
	char Sqz      [MAX_LOADSTRING];
	char Tar      [MAX_LOADSTRING];
	char Z        [MAX_LOADSTRING];
	char ZipJar   [MAX_LOADSTRING];
	char Zip      [MAX_LOADSTRING];
	char Jar      [MAX_LOADSTRING];
	char Zoo      [MAX_LOADSTRING];
	char Sfx      [MAX_LOADSTRING];
} LanguageArchives;

//yazik programmi soobsheniya(file *.lng)
typedef struct _LanguageMessages
{
	//[Messages]
	char _TestArchive                  [MAX_LOADSTRING];
	char _AnalyzeArchive               [MAX_LOADSTRING];
	char BlockArchiveComment           [MAX_LOADSTRING];
	char BlockArchiveEnd               [MAX_LOADSTRING];
	char BlockArhiveHeader             [MAX_LOADSTRING];
	char BlockAuthentityVerification   [MAX_LOADSTRING];
	char BlockChapter                  [MAX_LOADSTRING];
	char BlockControlItems             [MAX_LOADSTRING];
	char BlockFile                     [MAX_LOADSTRING];
	char BlockFolder                   [MAX_LOADSTRING];
	char BlockFolderBegin              [MAX_LOADSTRING];
	char BlockFolderEnd                [MAX_LOADSTRING];
	char BlockInformationalItems       [MAX_LOADSTRING];
	char BlockJar64DirectoryRecord     [MAX_LOADSTRING];
	char BlockLocalFile                [MAX_LOADSTRING];
	char BlockLocalFolder              [MAX_LOADSTRING];
	char BlockLongNameFile             [MAX_LOADSTRING];
	char BlockPassword                 [MAX_LOADSTRING];
	char BlockPostfix                  [MAX_LOADSTRING];
	char BlockRecoveryRecord           [MAX_LOADSTRING];
	char BlockSecurityEnvelope         [MAX_LOADSTRING];
	char BlockUnknown                  [MAX_LOADSTRING];
	char BlockVolumeLabel              [MAX_LOADSTRING];
	char BlockZip64DirectoryRecord     [MAX_LOADSTRING];
	char BlockZip64DirectoryLocator    [MAX_LOADSTRING];
	char DLLReadArchiveComment         [MAX_LOADSTRING];
	char MarkerArchive                 [MAX_LOADSTRING];
	char SFXModule                     [MAX_LOADSTRING];
	char SignatureArchive              [MAX_LOADSTRING];
	char SubBlockArchiveComment        [MAX_LOADSTRING];
	char SubBlockAuthentityVerification[MAX_LOADSTRING];
	char SubBlockFileComment           [MAX_LOADSTRING];
	char SubBlockRecoveryRecord        [MAX_LOADSTRING];
	char SubBlockUnknown               [MAX_LOADSTRING];
	char UsePlugin                     [MAX_LOADSTRING];
} LanguageMessages;

//yazik programmi oshibki (file *.lng)
typedef struct _LanguageResults
{
	//[Errors]
	char TerminateThread  [MAX_LOADSTRING];
	char MessageOK        [MAX_LOADSTRING];
	char ErrorOpenFile    [MAX_LOADSTRING];
	char ErrorReadFile    [MAX_LOADSTRING];
	char ErrorSeekFile    [MAX_LOADSTRING];
	char SuddenEndFile    [MAX_LOADSTRING];
	char ErrorFormat      [MAX_LOADSTRING];
	char ErrorNotDll      [MAX_LOADSTRING];
	char ErrorDll         [MAX_LOADSTRING];
	char ErrorCreateWindow[MAX_LOADSTRING];
	char ErrorUnknown     [MAX_LOADSTRING];
} LanguageResults;

#endif //T_STRUCT_LANGUAGE_H