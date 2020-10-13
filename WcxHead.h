/////////////////////////////////////////////////////////////////////////////
//WcxHead.h
/////////////////////////////////////////////////////////////////////////////

#ifndef T_WCX_HEAD_H
#define T_WCX_HEAD_H

#define E_END_ARCHIVE     10            /* No more files in archive */
#define E_BAD_ARCHIVE     13            /* CRC error in archive data */
#define E_UNKNOWN_FORMAT  14            /* Archive format unknown */

#define PK_OM_LIST        0

#define PK_SKIP	          0             /* Skip this file */
#define PK_TEST           1             /* Test file integrity */


typedef struct __WCXtHeaderData
{
	char  ArcName [260];
	char  FileName[260];
	int   Flags;
	int   PackSize;
	int   UnpSize;
	int   HostOS;
	int   FileCRC;
	int   FileTime;
	int   UnpVer;
	int   Method;
	int   FileAttr;
	char* CmtBuf;
	int   CmtBufSize;
	int   CmtSize;
	int   CmtState;
} tHeaderData;

typedef struct {
	char ArcName[1024];
	char FileName[1024];
	int Flags;
	unsigned int PackSize;
	unsigned int PackSizeHigh;
	unsigned int UnpSize;
	unsigned int UnpSizeHigh;
	int HostOS;
	int FileCRC;
	int FileTime;
	int UnpVer;
	int Method;
	int FileAttr;
	char* CmtBuf;
	int CmtBufSize;
	int CmtSize;
	int CmtState;
	char Reserved[1024];
} tHeaderDataEx;

typedef struct __WCXtOpenArchiveData
{
	char* ArcName;
	int   OpenMode;
	int   OpenResult;
	char* CmtBuf;
	int   CmtBufSize;
	int   CmtSize;
	int   CmtState;
} tOpenArchiveData;

/////////////////////////////////////////////////////////////////////////////
//FUNCTIONS IMPORT
//////////////////////////////////////////////////////////////////////

typedef HANDLE (__stdcall *WCXOPENARCHIVE)          (tOpenArchiveData*);
typedef int    (__stdcall *WCXREADHEADER)           (HANDLE, tHeaderData*);
typedef int    (__stdcall* WCXREADHEADEREX)         (HANDLE, tHeaderDataEx*);
typedef int    (__stdcall *WCXPROCESSFILE)          (HANDLE, int, char*, char*);
typedef int    (__stdcall *WCXCLOSEARCHIVE)         (HANDLE);
typedef BOOL   (__stdcall *WCXCANYOUHANDLETHISFILE) (char*);


#endif //T_WCX_HEAD_H