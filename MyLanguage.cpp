/////////////////////////////////////////////////////////////////////////////
//MySettings.cpp
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MyLanguage.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

MyLanguage::MyLanguage()
{
	//sekcii yazikovogo fayla *.lng
	strcpy(SectionsLNG.SectTabs,     "Tabs"    );
	strcpy(SectionsLNG.SectInfo,     "Info"    );
	strcpy(SectionsLNG.SectAbout,    "About"   );
	strcpy(SectionsLNG.SectOS,       "OS"      );
	strcpy(SectionsLNG.SectPack,     "Pack"    );
	strcpy(SectionsLNG.SectOther,    "Other"   );
	strcpy(SectionsLNG.SectArchives, "Archives");
	strcpy(SectionsLNG.SectMessages, "Messages");
	strcpy(SectionsLNG.SectResults,  "Results" );
}

/////////////////////////////////////////////////////////////////////////////
//FUNCTIONS PUBLIC
/////////////////////////////////////////////////////////////////////////////

//chtenie yazika programmi (file *.lng)
//inifile - imya fayla nastroek
int MyLanguage::
ReadLanguageProgram(char* langfile)
{
	fLNG.SetFile(langfile);

	LanguageSections& SN = SectionsLNG;

	//[Tabs]
	//chtenie klyuchey
	fLNG.ReadKey(SN.SectTabs, "Info",    "Info",    sLanguageTabs.Info,    MAX_LOADSTRING);
	fLNG.ReadKey(SN.SectTabs, "Context", "Context", sLanguageTabs.Context, MAX_LOADSTRING);
	fLNG.ReadKey(SN.SectTabs, "Comment", "Comment", sLanguageTabs.Comment, MAX_LOADSTRING);
	fLNG.ReadKey(SN.SectTabs, "About",   "About",   sLanguageTabs.About,   MAX_LOADSTRING);

	//[Info]
	//chtenie klyuchey
	fLNG.ReadKey(SN.SectInfo, "UnpackSizeFiles",   "Unpack size files",       sLanguageTabInfo.UnpackSizeFiles,   MAX_LOADSTRING);
	fLNG.ReadKey(SN.SectInfo, "PackSizeFiles",     "Pack size files",         sLanguageTabInfo.PackSizeFiles,     MAX_LOADSTRING);
	fLNG.ReadKey(SN.SectInfo, "ArchiveSize",       "Archive size",            sLanguageTabInfo.ArchiveSize,       MAX_LOADSTRING);
	fLNG.ReadKey(SN.SectInfo, "NumberFiles",       "Number files",            sLanguageTabInfo.NumberFiles,       MAX_LOADSTRING);
	fLNG.ReadKey(SN.SectInfo, "NumberFolders",     "Number folders",          sLanguageTabInfo.NumberFolders,     MAX_LOADSTRING);
	fLNG.ReadKey(SN.SectInfo, "NumberChapters",    "Number chapters",         sLanguageTabInfo.NumberChapters,    MAX_LOADSTRING);
	fLNG.ReadKey(SN.SectInfo, "DictionarySize",    "Dictionary size",         sLanguageTabInfo.DictionarySize,    MAX_LOADSTRING);
	fLNG.ReadKey(SN.SectInfo, "HostOS",            "Host OS",                 sLanguageTabInfo.HostOS,            MAX_LOADSTRING);
	fLNG.ReadKey(SN.SectInfo, "ExtractOS",         "Extract OS",              sLanguageTabInfo.ExtractOS,         MAX_LOADSTRING);
	fLNG.ReadKey(SN.SectInfo, "MethodPack",        "Method pack",             sLanguageTabInfo.MethodPack,        MAX_LOADSTRING);
	fLNG.ReadKey(SN.SectInfo, "MemorySize",        "Memory size",             sLanguageTabInfo.MemorySize,        MAX_LOADSTRING);
	fLNG.ReadKey(SN.SectInfo, "RatioArchiveSize",  "Ratio archive size",      sLanguageTabInfo.RatioArchiveSize,  MAX_LOADSTRING);
	fLNG.ReadKey(SN.SectInfo, "RatioPackFileSize", "Ratio pack file size",    sLanguageTabInfo.RatioPackFileSize, MAX_LOADSTRING);
	fLNG.ReadKey(SN.SectInfo, "PackVersion",       "Pack version",            sLanguageTabInfo.PackVersion,       MAX_LOADSTRING);
	fLNG.ReadKey(SN.SectInfo, "UnpackVersion",     "Unpack version",          sLanguageTabInfo.UnpackVersion,     MAX_LOADSTRING);
	fLNG.ReadKey(SN.SectInfo, "SfxModule",         "SFX module",              sLanguageTabInfo.SfxModule,         MAX_LOADSTRING);
	fLNG.ReadKey(SN.SectInfo, "RecoveryRecord",    "Recovery record",         sLanguageTabInfo.RecoveryRecord,    MAX_LOADSTRING);
	fLNG.ReadKey(SN.SectInfo, "Solid",             "Solid",                   sLanguageTabInfo.Solid,             MAX_LOADSTRING);
	fLNG.ReadKey(SN.SectInfo, "MultiArc",          "Multi-volume",            sLanguageTabInfo.MultiArc,          MAX_LOADSTRING);
	fLNG.ReadKey(SN.SectInfo, "NumberVolume",      "Number volume",           sLanguageTabInfo.NumberVolume,      MAX_LOADSTRING);
	fLNG.ReadKey(SN.SectInfo, "MainComment",       "Main comment",            sLanguageTabInfo.MainComment,       MAX_LOADSTRING);
	fLNG.ReadKey(SN.SectInfo, "FileComment",       "File comments",           sLanguageTabInfo.FileComment,       MAX_LOADSTRING);
	fLNG.ReadKey(SN.SectInfo, "ChapterComment",    "Chapter comments",        sLanguageTabInfo.ChapterComment,    MAX_LOADSTRING);
	fLNG.ReadKey(SN.SectInfo, "Password",          "Password",                sLanguageTabInfo.Password,          MAX_LOADSTRING);
	fLNG.ReadKey(SN.SectInfo, "ArchiveLock",       "Archive lock",            sLanguageTabInfo.ArchiveLock,       MAX_LOADSTRING);
	fLNG.ReadKey(SN.SectInfo, "AuthVerification",  "Authentity verification", sLanguageTabInfo.AuthVerification,  MAX_LOADSTRING);
	fLNG.ReadKey(SN.SectInfo, "TestTime",          "Test time",               sLanguageTabInfo.TestTime,          MAX_LOADSTRING);
	fLNG.ReadKey(SN.SectInfo, "AnalyzeTime",       "Analyze time",            sLanguageTabInfo.AnalyzeTime,       MAX_LOADSTRING);
	fLNG.ReadKey(SN.SectInfo, "AllTime",           "All time",                sLanguageTabInfo.AllTime,           MAX_LOADSTRING);
	fLNG.ReadKey(SN.SectInfo, "DateCreated",       "Date created",            sLanguageTabInfo.DateCreated,       MAX_LOADSTRING);
	fLNG.ReadKey(SN.SectInfo, "DateModified",      "Date nodified",           sLanguageTabInfo.DateModified,      MAX_LOADSTRING);
	fLNG.ReadKey(SN.SectInfo, "TimeCreated",       "Time created",            sLanguageTabInfo.TimeCreated,       MAX_LOADSTRING);
	fLNG.ReadKey(SN.SectInfo, "TimeModified",      "Time modified",           sLanguageTabInfo.TimeModified,      MAX_LOADSTRING);
	fLNG.ReadKey(SN.SectInfo, "OriginalName",      "Original name",           sLanguageTabInfo.OriginalName,      MAX_LOADSTRING);
	fLNG.ReadKey(SN.SectInfo, "Magic",             "Magic",                   sLanguageTabInfo.Magic,             MAX_LOADSTRING);
	fLNG.ReadKey(SN.SectInfo, "HeadersSize",       "Headers Size",            sLanguageTabInfo.HeadersSize,       MAX_LOADSTRING);
	fLNG.ReadKey(SN.SectInfo, "NumBlocks",         "Blocks",                  sLanguageTabInfo.NumBlocks,         MAX_LOADSTRING);
	fLNG.ReadKey(SN.SectInfo, "Offset",            "Offset",                  sLanguageTabInfo.Offset,            MAX_LOADSTRING);

	//[About]
	//chtenie klyuchey
	fLNG.ReadKey(SN.SectAbout, "Version",           "version",                                sLanguageTabAbout.Version,           MAX_LOADSTRING);
	fLNG.ReadKey(SN.SectAbout, "TypeProgram",       "lister plugin for",                      sLanguageTabAbout.TypeProgram,       MAX_LOADSTRING);
	fLNG.ReadKey(SN.SectAbout, "AndLater",          "and later",                              sLanguageTabAbout.AndLater,          MAX_LOADSTRING);
	fLNG.ReadKey(SN.SectAbout, "UseProgram",        "free for noncommercial use",             sLanguageTabAbout.UseProgram,        MAX_LOADSTRING);
	fLNG.ReadKey(SN.SectAbout, "FunctionProgram",   "shows the information on archives",      sLanguageTabAbout.FunctionProgram,   MAX_LOADSTRING);
	fLNG.ReadKey(SN.SectAbout, "SupportWCXPlugins", "Support wcx-plugins",	                  sLanguageTabAbout.SupportWCXPlugins, MAX_LOADSTRING);
	fLNG.ReadKey(SN.SectAbout, "SpecialThanksTo",   "Special thanks to",                      sLanguageTabAbout.SpecialThanksTo,   MAX_LOADSTRING);
	fLNG.ReadKey(SN.SectAbout, "AdviceAndHelp",     "advice and help",                        sLanguageTabAbout.AdviceAndHelp,     MAX_LOADSTRING);
	fLNG.ReadKey(SN.SectAbout, "AuthorProgram",     "author program",                         sLanguageTabAbout.AuthorProgram,     MAX_LOADSTRING);
	fLNG.ReadKey(SN.SectAbout, "AuthorSite",        "author site",                            sLanguageTabAbout.AuthorSite,        MAX_LOADSTRING);
	fLNG.ReadKey(SN.SectAbout, "ForTranslation",    "for translation",                        sLanguageTabAbout.ForTranslation,    MAX_LOADSTRING);
	fLNG.ReadKey(SN.SectAbout, "Contact",           "Contact",                                sLanguageTabAbout.Contact,           MAX_LOADSTRING);
	fLNG.ReadKey(SN.SectAbout, "SendComment",       "Send comments and wishes to the author", sLanguageTabAbout.SendComment,       MAX_LOADSTRING);
	fLNG.ReadKey(SN.SectAbout, "AllRightsReserved", "All rights reserved",                    sLanguageTabAbout.AllRightsReserved, MAX_LOADSTRING);
	fLNG.ReadKey(SN.SectAbout, "EMail",             "E-mail",                                 sLanguageTabAbout.EMail,             MAX_LOADSTRING);
	fLNG.ReadKey(SN.SectAbout, "HomePage",          "Home page",                              sLanguageTabAbout.HomePage,          MAX_LOADSTRING);
	fLNG.ReadKey(SN.SectAbout, "Translation",       "Translation",                            sLanguageTabAbout.Translation,       MAX_LOADSTRING);
	fLNG.ReadKey(SN.SectAbout, "TranslationMan",    "",                                       sLanguageTabAbout.TranslationMan,    MAX_LOADSTRING);
	fLNG.ReadKey(SN.SectAbout, "TranslationMail",   "",                                       sLanguageTabAbout.TranslationMail,   MAX_LOADSTRING);
	fLNG.ReadKey(SN.SectAbout, "TranslationSite",   "",                                       sLanguageTabAbout.TranslationSite,   MAX_LOADSTRING);

	//[OS]
	//chtenie klyuchey
	fLNG.ReadKey(SN.SectOS, "AcornRISC", "Acorn RISC", sLanguageOS.AcornRISC, MAX_LOADSTRING);
	fLNG.ReadKey(SN.SectOS, "Amiga",     "Amiga",      sLanguageOS.Amiga,     MAX_LOADSTRING);
	fLNG.ReadKey(SN.SectOS, "AppleGS",   "Apple GS",   sLanguageOS.AppleGS,   MAX_LOADSTRING);
	fLNG.ReadKey(SN.SectOS, "AtariST",   "Atari ST",   sLanguageOS.AtariST,   MAX_LOADSTRING);
	fLNG.ReadKey(SN.SectOS, "BeOS",      "BeOS",       sLanguageOS.BeOS,      MAX_LOADSTRING);
	fLNG.ReadKey(SN.SectOS, "CPM",       "CP/M",       sLanguageOS.CPM,       MAX_LOADSTRING);
	fLNG.ReadKey(SN.SectOS, "DOS",       "MS-DOS",     sLanguageOS.DOS,       MAX_LOADSTRING);
	fLNG.ReadKey(SN.SectOS, "MacOS",     "Mac OS",     sLanguageOS.MacOS,     MAX_LOADSTRING);
	fLNG.ReadKey(SN.SectOS, "MVS",       "MVS",        sLanguageOS.MVS,       MAX_LOADSTRING);
	fLNG.ReadKey(SN.SectOS, "Next",      "Next",       sLanguageOS.Next,      MAX_LOADSTRING);
	fLNG.ReadKey(SN.SectOS, "OS2",       "OS/2",       sLanguageOS.OS2,       MAX_LOADSTRING);
	fLNG.ReadKey(SN.SectOS, "Primos",    "Primos",     sLanguageOS.Primos,    MAX_LOADSTRING);
	fLNG.ReadKey(SN.SectOS, "SMSQDOS",   "SMS/QDOS",   sLanguageOS.SMSQDOS,   MAX_LOADSTRING);
	fLNG.ReadKey(SN.SectOS, "Tandem",    "Tandem",     sLanguageOS.Tandem,    MAX_LOADSTRING);
	fLNG.ReadKey(SN.SectOS, "TOPS20",    "TOPS-20",    sLanguageOS.TOPS20,    MAX_LOADSTRING);
	fLNG.ReadKey(SN.SectOS, "Unix",      "Unix",       sLanguageOS.Unix,      MAX_LOADSTRING);
	fLNG.ReadKey(SN.SectOS, "VaxVMS",    "Vax VMS",    sLanguageOS.VaxVMS,    MAX_LOADSTRING);
	fLNG.ReadKey(SN.SectOS, "VFAT",      "VFAT",       sLanguageOS.VFAT,      MAX_LOADSTRING);
	fLNG.ReadKey(SN.SectOS, "VMCMS",     "VM/CMS",     sLanguageOS.VMCMS,     MAX_LOADSTRING);
	fLNG.ReadKey(SN.SectOS, "Win32",     "Win 32",     sLanguageOS.Win32,     MAX_LOADSTRING);
	fLNG.ReadKey(SN.SectOS, "Win95",     "Win 95",     sLanguageOS.Win95,     MAX_LOADSTRING);
	fLNG.ReadKey(SN.SectOS, "WinNT",     "Win NT",     sLanguageOS.WinNT,     MAX_LOADSTRING);
	fLNG.ReadKey(SN.SectOS, "ZSystem",   "Z-System",   sLanguageOS.ZSystem,   MAX_LOADSTRING);

	//[Pack]
	//chtenie klyuchey
	//all
	fLNG.ReadKey(SN.SectPack, "Stored",     "Stored",     sLanguagePack.Stored,     MAX_LOADSTRING);
	fLNG.ReadKey(SN.SectPack, "Superfast",  "Superfast",  sLanguagePack.Superfast,  MAX_LOADSTRING);
	fLNG.ReadKey(SN.SectPack, "Fastest",    "Fastest",    sLanguagePack.Fastest,    MAX_LOADSTRING);
	fLNG.ReadKey(SN.SectPack, "Fast",       "Fast",       sLanguagePack.Fast,       MAX_LOADSTRING);
	fLNG.ReadKey(SN.SectPack, "Normal",     "Normal",     sLanguagePack.Normal,     MAX_LOADSTRING);
	fLNG.ReadKey(SN.SectPack, "Good",       "Good",       sLanguagePack.Good,       MAX_LOADSTRING);
	fLNG.ReadKey(SN.SectPack, "Verygood",   "Verygood",   sLanguagePack.Verygood,   MAX_LOADSTRING);
	fLNG.ReadKey(SN.SectPack, "Best",       "Best",       sLanguagePack.Best,       MAX_LOADSTRING);
	//zip, gz
	fLNG.ReadKey(SN.SectPack, "Deflated",   "Deflated",   sLanguagePack.Deflated,   MAX_LOADSTRING);
	fLNG.ReadKey(SN.SectPack, "Deflated64", "Deflated64", sLanguagePack.Deflated64, MAX_LOADSTRING);
	fLNG.ReadKey(SN.SectPack, "Imploded",   "Imploded",   sLanguagePack.Imploded,   MAX_LOADSTRING);
	fLNG.ReadKey(SN.SectPack, "Imploding",  "Imploding",  sLanguagePack.Imploding,  MAX_LOADSTRING);
	fLNG.ReadKey(SN.SectPack, "Shrunk",     "Shrunk",     sLanguagePack.Shrunk,     MAX_LOADSTRING);
	fLNG.ReadKey(SN.SectPack, "Reduced",    "Reduced",    sLanguagePack.Reduced,    MAX_LOADSTRING);
	fLNG.ReadKey(SN.SectPack, "Tokenizing", "Tokenizing", sLanguagePack.Tokenizing, MAX_LOADSTRING);
	fLNG.ReadKey(SN.SectPack, "BZip2",      "BZip2",      sLanguagePack.BZip2,      MAX_LOADSTRING);
	//bh
	fLNG.ReadKey(SN.SectPack, "Fuse",       "Fuse",       sLanguagePack.Fuse,       MAX_LOADSTRING);
	//bz2, ppm
	fLNG.ReadKey(SN.SectPack, "Rate",       "Rate",       sLanguagePack.Rate,       MAX_LOADSTRING);
	//arc, pak, dwc, zoo
	fLNG.ReadKey(SN.SectPack, "Crunched",   "Crunched",   sLanguagePack.Crunched,   MAX_LOADSTRING);
	fLNG.ReadKey(SN.SectPack, "Unpacked",   "Unpacked",   sLanguagePack.Unpacked,   MAX_LOADSTRING);
	fLNG.ReadKey(SN.SectPack, "Packed",     "Packed",     sLanguagePack.Packed,     MAX_LOADSTRING);
	fLNG.ReadKey(SN.SectPack, "Squeezed",   "Squeezed",   sLanguagePack.Squeezed,   MAX_LOADSTRING);
	fLNG.ReadKey(SN.SectPack, "Squashed",   "Squashed",   sLanguagePack.Squashed,   MAX_LOADSTRING);
	fLNG.ReadKey(SN.SectPack, "Crushed",    "Crushed",    sLanguagePack.Crushed,    MAX_LOADSTRING);
	fLNG.ReadKey(SN.SectPack, "Distilled",  "Distilled",  sLanguagePack.Distilled,  MAX_LOADSTRING);
	//cab
	fLNG.ReadKey(SN.SectPack, "MSZIP",      "MSZIP",      sLanguagePack.MSZIP,      MAX_LOADSTRING);
	fLNG.ReadKey(SN.SectPack, "LZX",        "LZX",        sLanguagePack.LZX,        MAX_LOADSTRING);
	fLNG.ReadKey(SN.SectPack, "Quantum",    "Quantum",    sLanguagePack.Quantum,    MAX_LOADSTRING);
	//ha
	fLNG.ReadKey(SN.SectPack, "CPY",        "CPY",        sLanguagePack.CPY,        MAX_LOADSTRING);
	fLNG.ReadKey(SN.SectPack, "ASC",        "ASC",        sLanguagePack.ASC,        MAX_LOADSTRING);
	fLNG.ReadKey(SN.SectPack, "HSC",        "HSC",        sLanguagePack.HSC,        MAX_LOADSTRING);
	//lha, lzh
	fLNG.ReadKey(SN.SectPack, "lh0",        "-lh0-",      sLanguagePack.lh0,        MAX_LOADSTRING);
	fLNG.ReadKey(SN.SectPack, "lh1",        "-lh1-",      sLanguagePack.lh1,        MAX_LOADSTRING);
	fLNG.ReadKey(SN.SectPack, "lh2",        "-lh2-",      sLanguagePack.lh2,        MAX_LOADSTRING);
	fLNG.ReadKey(SN.SectPack, "lh3",        "-lh3-",      sLanguagePack.lh3,        MAX_LOADSTRING);
	fLNG.ReadKey(SN.SectPack, "lh4",        "-lh4-",      sLanguagePack.lh4,        MAX_LOADSTRING);
	fLNG.ReadKey(SN.SectPack, "lh5",        "-lh5-",      sLanguagePack.lh5,        MAX_LOADSTRING);
	fLNG.ReadKey(SN.SectPack, "lh6",        "-lh6-",      sLanguagePack.lh6,        MAX_LOADSTRING);
	fLNG.ReadKey(SN.SectPack, "lh7",        "-lh7-",      sLanguagePack.lh7,        MAX_LOADSTRING);
	fLNG.ReadKey(SN.SectPack, "lzs",        "-lzs-",      sLanguagePack.lzs,        MAX_LOADSTRING);
	fLNG.ReadKey(SN.SectPack, "lz4",        "-lz4-",      sLanguagePack.lz4,        MAX_LOADSTRING);
	fLNG.ReadKey(SN.SectPack, "lz5",        "-lz5-",      sLanguagePack.lz5,        MAX_LOADSTRING);

	//[Archives]
	//chtenie klyuchey
	fLNG.ReadKey(SN.SectArchives, "Ace",       "ACE",          sLanguageArchives.Ace,       MAX_LOADSTRING);
	fLNG.ReadKey(SN.SectArchives, "ArcPak",    "ARC/PAK",      sLanguageArchives.ArcPak,    MAX_LOADSTRING);
	fLNG.ReadKey(SN.SectArchives, "Arc",       "ARC",          sLanguageArchives.Arc,       MAX_LOADSTRING);
	fLNG.ReadKey(SN.SectArchives, "Pak",       "PAK",          sLanguageArchives.Pak,       MAX_LOADSTRING);
	fLNG.ReadKey(SN.SectArchives, "Arj",       "ARJ",          sLanguageArchives.Arj,       MAX_LOADSTRING);
	fLNG.ReadKey(SN.SectArchives, "Bh",        "BH",           sLanguageArchives.Bh,        MAX_LOADSTRING);
	fLNG.ReadKey(SN.SectArchives, "BZip2Tbz2", "BZIP2/TBZIP2", sLanguageArchives.BZip2Tbz2, MAX_LOADSTRING);
	fLNG.ReadKey(SN.SectArchives, "BZip2",     "BZIP2",        sLanguageArchives.BZip2,     MAX_LOADSTRING);
	fLNG.ReadKey(SN.SectArchives, "Cab",       "CAB",          sLanguageArchives.Cab,       MAX_LOADSTRING);
	fLNG.ReadKey(SN.SectArchives, "Cpio",      "CPIO",         sLanguageArchives.Cpio,      MAX_LOADSTRING);
	fLNG.ReadKey(SN.SectArchives, "Dwc",       "DWC",          sLanguageArchives.Dwc,       MAX_LOADSTRING);
	fLNG.ReadKey(SN.SectArchives, "GZipTgz",   "GZIP/TGZIP",   sLanguageArchives.GZipTgz,   MAX_LOADSTRING);
	fLNG.ReadKey(SN.SectArchives, "GZip",      "GZIP",         sLanguageArchives.GZip,      MAX_LOADSTRING);
	fLNG.ReadKey(SN.SectArchives, "Ha",        "HA",           sLanguageArchives.Ha,        MAX_LOADSTRING);
	fLNG.ReadKey(SN.SectArchives, "LhaLzh",    "LHA/LZH",      sLanguageArchives.LhaLzh,    MAX_LOADSTRING);
	fLNG.ReadKey(SN.SectArchives, "Lha",       "LHA",          sLanguageArchives.Lha,       MAX_LOADSTRING);
	fLNG.ReadKey(SN.SectArchives, "Lzh",       "LZH",          sLanguageArchives.Lzh,       MAX_LOADSTRING);
	fLNG.ReadKey(SN.SectArchives, "Ppm",       "PPM",          sLanguageArchives.Ppm,       MAX_LOADSTRING);
	fLNG.ReadKey(SN.SectArchives, "Rar",       "RAR",          sLanguageArchives.Rar,       MAX_LOADSTRING);
	fLNG.ReadKey(SN.SectArchives, "Sqz",       "SQZ",          sLanguageArchives.Sqz,       MAX_LOADSTRING);
	fLNG.ReadKey(SN.SectArchives, "Tar",       "TAR",          sLanguageArchives.Tar,       MAX_LOADSTRING);
	fLNG.ReadKey(SN.SectArchives, "Z",         "Z",            sLanguageArchives.Z,         MAX_LOADSTRING);
	fLNG.ReadKey(SN.SectArchives, "ZipJar",    "ZIP/JAR",      sLanguageArchives.ZipJar,    MAX_LOADSTRING);
	fLNG.ReadKey(SN.SectArchives, "Zip",       "ZIP",          sLanguageArchives.Zip,       MAX_LOADSTRING);
	fLNG.ReadKey(SN.SectArchives, "Jar",       "JAR",          sLanguageArchives.Jar,       MAX_LOADSTRING);
	fLNG.ReadKey(SN.SectArchives, "Zoo",       "ZOO",          sLanguageArchives.Zoo,       MAX_LOADSTRING);
	fLNG.ReadKey(SN.SectArchives, "7Zip",      "7ZIP",         sLanguageArchives.SevenZip,  MAX_LOADSTRING);
	fLNG.ReadKey(SN.SectArchives, "Sfx",       "SFX",          sLanguageArchives.Sfx,       MAX_LOADSTRING);

	//[Messages]
	//chtenie klyuchey
	fLNG.ReadKey(SN.SectMessages, "_TestArchive",                   "_TEST ARCHIVE_",                   sLanguageMessages._TestArchive,                   MAX_LOADSTRING);
	fLNG.ReadKey(SN.SectMessages, "_AnalyzeArchive",                "_ANALYZE ARCHIVE_",                sLanguageMessages._AnalyzeArchive,                MAX_LOADSTRING);
	fLNG.ReadKey(SN.SectMessages, "BlockArchiveComment",            "block archive comment",            sLanguageMessages.BlockArchiveComment,            MAX_LOADSTRING);
	fLNG.ReadKey(SN.SectMessages, "BlockArchiveEnd",                "block archive end",                sLanguageMessages.BlockArchiveEnd,                MAX_LOADSTRING);
	fLNG.ReadKey(SN.SectMessages, "BlockArhiveHeader",              "block arhive header",              sLanguageMessages.BlockArhiveHeader,              MAX_LOADSTRING);
	fLNG.ReadKey(SN.SectMessages, "BlockAuthentityVerification",    "block authentity verification",    sLanguageMessages.BlockAuthentityVerification,    MAX_LOADSTRING);
	fLNG.ReadKey(SN.SectMessages, "BlockChapter",                   "block chapter",                    sLanguageMessages.BlockChapter,                   MAX_LOADSTRING);
	fLNG.ReadKey(SN.SectMessages, "BlockControlItems",              "block control items",              sLanguageMessages.BlockControlItems,              MAX_LOADSTRING);
	fLNG.ReadKey(SN.SectMessages, "BlockFile",                      "block file",                       sLanguageMessages.BlockFile,                      MAX_LOADSTRING);
	fLNG.ReadKey(SN.SectMessages, "BlockFolder",                    "block folder",                     sLanguageMessages.BlockFolder,                    MAX_LOADSTRING);
	fLNG.ReadKey(SN.SectMessages, "BlockFolderBegin",               "block folder begin",               sLanguageMessages.BlockFolderBegin,               MAX_LOADSTRING);
	fLNG.ReadKey(SN.SectMessages, "BlockFolderEnd",                 "block folder end",                 sLanguageMessages.BlockFolderEnd,                 MAX_LOADSTRING);
	fLNG.ReadKey(SN.SectMessages, "BlockInformationalItems",        "block informational items",        sLanguageMessages.BlockInformationalItems,        MAX_LOADSTRING);
	fLNG.ReadKey(SN.SectMessages, "BlockJar64DirectoryRecord",      "block jar64 directory record",     sLanguageMessages.BlockJar64DirectoryRecord,      MAX_LOADSTRING);
	fLNG.ReadKey(SN.SectMessages, "BlockLocalFile",                 "block local file",                 sLanguageMessages.BlockLocalFile,                 MAX_LOADSTRING);
	fLNG.ReadKey(SN.SectMessages, "BlockLocalFolder",               "block local folder",               sLanguageMessages.BlockLocalFolder,               MAX_LOADSTRING);
	fLNG.ReadKey(SN.SectMessages, "BlockLongNameFile",              "block long name file",             sLanguageMessages.BlockLongNameFile,              MAX_LOADSTRING);
	fLNG.ReadKey(SN.SectMessages, "BlockPassword",                  "block password",                   sLanguageMessages.BlockPassword,                  MAX_LOADSTRING);
	fLNG.ReadKey(SN.SectMessages, "BlockPostfix",                   "block postfix",                    sLanguageMessages.BlockPostfix,                   MAX_LOADSTRING);
	fLNG.ReadKey(SN.SectMessages, "BlockRecoveryRecord",            "block recovery record",            sLanguageMessages.BlockRecoveryRecord,            MAX_LOADSTRING);
	fLNG.ReadKey(SN.SectMessages, "BlockSecurityEnvelope",          "block security envelope",          sLanguageMessages.BlockSecurityEnvelope,          MAX_LOADSTRING);
	fLNG.ReadKey(SN.SectMessages, "BlockUnknown",                   "block unknown",                    sLanguageMessages.BlockUnknown,                   MAX_LOADSTRING);
	fLNG.ReadKey(SN.SectMessages, "BlockVolumeLabel",               "block volume label",               sLanguageMessages.BlockVolumeLabel,               MAX_LOADSTRING);
	fLNG.ReadKey(SN.SectMessages, "BlockZip64DirectoryLocator",     "block zip64 directory locator",    sLanguageMessages.BlockZip64DirectoryLocator,     MAX_LOADSTRING);
	fLNG.ReadKey(SN.SectMessages, "BlockZip64DirectoryRecord",      "block zip64 directory record",     sLanguageMessages.BlockZip64DirectoryRecord,      MAX_LOADSTRING);
	fLNG.ReadKey(SN.SectMessages, "DLLReadArchiveComment",          "DLL read archive comment",         sLanguageMessages.DLLReadArchiveComment,          MAX_LOADSTRING);
	fLNG.ReadKey(SN.SectMessages, "MarkerArchive",                  "marker archive",                   sLanguageMessages.MarkerArchive,                  MAX_LOADSTRING);
	fLNG.ReadKey(SN.SectMessages, "SFXModule",                      "SFX module",                       sLanguageMessages.SFXModule,                      MAX_LOADSTRING);
	fLNG.ReadKey(SN.SectMessages, "SignatureArchive",               "signature archive",                sLanguageMessages.SignatureArchive,               MAX_LOADSTRING);
	fLNG.ReadKey(SN.SectMessages, "SubBlockArchiveComment",         "subblock archive comment",         sLanguageMessages.SubBlockArchiveComment,         MAX_LOADSTRING);
	fLNG.ReadKey(SN.SectMessages, "SubBlockAuthentityVerification", "subblock authentity verification", sLanguageMessages.SubBlockAuthentityVerification, MAX_LOADSTRING);
	fLNG.ReadKey(SN.SectMessages, "SubBlockFileComment",            "subblock file comment",            sLanguageMessages.SubBlockFileComment,            MAX_LOADSTRING);
	fLNG.ReadKey(SN.SectMessages, "SubBlockRecoveryRecord",         "subblock recovery record",         sLanguageMessages.SubBlockRecoveryRecord,         MAX_LOADSTRING);
	fLNG.ReadKey(SN.SectMessages, "SubBlockUnknown",                "subblock unknown",                 sLanguageMessages.SubBlockUnknown,                MAX_LOADSTRING);
	fLNG.ReadKey(SN.SectMessages, "UsePlugin",                      "use plugin",                       sLanguageMessages.UsePlugin,                      MAX_LOADSTRING);

	//[Other]
	//chtenie klyuchey
	fLNG.ReadKey(SN.SectOther, "Yes",        "Yes",         sLanguageOther.Yes,        MAX_LOADSTRING);
	fLNG.ReadKey(SN.SectOther, "No",         "No",          sLanguageOther.No,         MAX_LOADSTRING);
	fLNG.ReadKey(SN.SectOther, "Byte",       "b",           sLanguageOther.Byte,       MAX_LOADSTRING);
	fLNG.ReadKey(SN.SectOther, "KByte",      "Kb",          sLanguageOther.KByte,      MAX_LOADSTRING);
	fLNG.ReadKey(SN.SectOther, "MByte",      "Mb",          sLanguageOther.MByte,      MAX_LOADSTRING);
	fLNG.ReadKey(SN.SectOther, "GByte",      "Gb",          sLanguageOther.GByte,      MAX_LOADSTRING);
	fLNG.ReadKey(SN.SectOther, "TByte",      "Tb",          sLanguageOther.TByte,      MAX_LOADSTRING);
	fLNG.ReadKey(SN.SectOther, "Second",     "sec",         sLanguageOther.Second,     MAX_LOADSTRING);
	fLNG.ReadKey(SN.SectOther, "Percent",    "%",           sLanguageOther.Percent,    MAX_LOADSTRING);
	fLNG.ReadKey(SN.SectOther, "Number",     "#",           sLanguageOther.Number,     MAX_LOADSTRING);
	fLNG.ReadKey(SN.SectOther, "Unknown",    "N/A",         sLanguageOther.Unknown,    MAX_LOADSTRING);
	fLNG.ReadKey(SN.SectOther, "PleaseWait", "Please wait", sLanguageOther.PleaseWait, MAX_LOADSTRING);
	fLNG.ReadKey(SN.SectOther, "Archive",    "Archive",     sLanguageOther.Archive,    MAX_LOADSTRING);
	fLNG.ReadKey(SN.SectOther, "Plugin",     "Plugin",      sLanguageOther.Plugin,     MAX_LOADSTRING);

	//[Results]
	//chtenie klyuchey
	fLNG.ReadKey(SN.SectResults, "TerminateThread",   "Terminate thread",    sLanguageResults.TerminateThread,   MAX_LOADSTRING);
	fLNG.ReadKey(SN.SectResults, "MessageOK",         "Ok",                  sLanguageResults.MessageOK,         MAX_LOADSTRING);
	fLNG.ReadKey(SN.SectResults, "ErrorOpenFile",     "Error open file",     sLanguageResults.ErrorOpenFile,     MAX_LOADSTRING);
	fLNG.ReadKey(SN.SectResults, "ErrorReadFile",     "Error read file",     sLanguageResults.ErrorReadFile,     MAX_LOADSTRING);
	fLNG.ReadKey(SN.SectResults, "ErrorSeekFile",     "Error seek file",     sLanguageResults.ErrorSeekFile,     MAX_LOADSTRING);
	fLNG.ReadKey(SN.SectResults, "SuddenEndFile",     "Sudden end file",     sLanguageResults.SuddenEndFile,     MAX_LOADSTRING);
	fLNG.ReadKey(SN.SectResults, "ErrorFormat",       "Error format",        sLanguageResults.ErrorFormat,       MAX_LOADSTRING);
	fLNG.ReadKey(SN.SectResults, "ErrorNotDll",       "Error not DLL",       sLanguageResults.ErrorNotDll,       MAX_LOADSTRING);
	fLNG.ReadKey(SN.SectResults, "ErrorDll",          "Error DLL",           sLanguageResults.ErrorDll,          MAX_LOADSTRING);
	fLNG.ReadKey(SN.SectResults, "ErrorCreateWindow", "Error create window", sLanguageResults.ErrorCreateWindow, MAX_LOADSTRING);
	fLNG.ReadKey(SN.SectResults, "ErrorUnknown",      "Unknown error",       sLanguageResults.ErrorUnknown,      MAX_LOADSTRING);

	return 1;
}
