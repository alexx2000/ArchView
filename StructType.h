/////////////////////////////////////////////////////////////////////////////
//StructType.h
/////////////////////////////////////////////////////////////////////////////

#ifndef T_STRUCT_TYPE_H
#define T_STRUCT_TYPE_H

/////////////////////////////////////////////////////////////////////////////
//CONSTANTS
/////////////////////////////////////////////////////////////////////////////

//velichina hranimih strokovih dannih
#define MAX_LOADSTRING 256 + 1	//+1 t.k. stroka zakanchivaetsya BYTE = 0x00

//soobsheniya po obrabotke arhivov
#define TTERMINATE_THREAD     1
#define TMESSAGE_OK           0
#define TERROR_OPEN_FILE      -1
#define TERROR_READ_FILE      -2
#define TERROR_SEEK_FILE      -3
#define TSUDDEN_END_FILE      -4
#define TERROR_FORMAT         -5
#define TERROR_NOT_DLL        -6
#define TERROR_DLL            -7
#define TERROR_CREATE_WINDOW  -8
#define TERROR_UNKNOWN        -9

//soobsheniya iz potoka obrabotki arhva k osnovnomu oknu
#define WM_THREAD_FINISHED 10001
#define WM_THREAD_PERCENT  10002

//soobsheniya dlya obrabotki fayla
#define TEST_ARC           10
#define ANALYZE_ARC        11

//typi zakladok
enum TypeOfTabItems
{
	T_INFO,			//parametri arhiva
	T_CONTEXT,		//derevo faylov i papok
	T_COMMENT,		//kommentariy arhiva
	T_SETTINGS,		//nastroyki
	T_ABOUT,		//o programme
	T_MAX_TAB_ITEM,	//maximal'noe kolichestvo zakladok (tak udobnee)
	T_NULL			//pustaya/default
};

//obrabativaemie tipi arhivov
enum TypeOfArchive
{
	T_UNKNOWN,			//default
	T_ACE,				//*.ace
	T_ARC_PAK,			//*.arc; *.pak
	T_ARJ,				//*.arj
	T_BH,				//*.bh (Blak Hole)
	T_BZIP2_TBZ2,		//*.bz2; *.tbz2 as *.bz2 for file *.tar
	T_CAB,				//*.cab
	T_CPIO,				//*.zpio
	T_DWC,				//*.dwc
	T_GZIP_TGZ,			//*.gz; *tgz as *.gz for file *.tar
	T_HA,				//*.ha
	T_LHA_LZH,			//*.lha; *.lzh
	T_PPM,				//*.ppm
	T_RAR,				//*.rar
	T_SQZ,				//*.sqz
	T_TAR,				//*.tar
	T_Z,				//*.z
	T_ZIP_JAR,			//*.zip; *.jar (Java ARhiver)
	T_ZOO,				//*.zoo
	T_NUMBER_ARCHIVES,	//(T_NUMBER_ARCHIVES-1) - izvestnoe kolichestvo arhivov
	T_WCX_PLUGIN_TC		//*.* arhivatorniy (wcx) plagin Total Commander
};

/////////////////////////////////////////////////////////////////////////////
//OTHER STRUCTURES
/////////////////////////////////////////////////////////////////////////////

//dannie peredavaemie v sozdavaemiy potok
typedef struct _DataForThread
{
	HWND   hWnd;		//okno dlya vozvrata soobsheniy ot potoka
	HANDLE hEventEnd;	//sobitie zakritiya potoka
	char*  Path;		//fayl dlya analiza
	int    Flag;		//testirovat' ili analizirovat'
	BOOL   BeginThread;	//zapusk v rabochem potoke ili net
} DataForThread;

//dannie peredavaemie dlya testirovaniya/razbora arhiva
typedef struct _DataForArchive
{
	int               SFX;				//0 - smotret' SFX arhivi; 1 - net
	unsigned          SizeSFX;			//maksimal'niy razmer SFX modulya
	unsigned          InvertRatio;		//kak rasschitivat' sjatie (0: ratio = puck/unpack; 1: ratio = 1 - puck/unpack)
	char*             pTargetFile;		//fayl dlya kotorogo sozdaetsya logfile
	char*             PathLF;			//imya logfayla	("" - net logfile)
	int               NewLF;			//0 - dopisivatl; 1 - perepisivat' logfile
	int               DetailLF;			//1 - nedetalezirovanniy; 2 - detalezirovanniy logfile
	unsigned          WidthAuthorName;	//shirina avtora soobsheniya dlya logfile
	unsigned          WidthMessage;		//shirina soobsheniya dlya logfile
	char*             pUnArchiveDll;	//DLL dlya dannogo tipa arhiva (chitat' kommentariy)
} DataForArchive;

#endif //T_STRUCT_TYPE_H