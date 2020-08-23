/////////////////////////////////////////////////////////////////////////////
//UnRar.h
/////////////////////////////////////////////////////////////////////////////

#ifndef T_UNRAR_H
#define T_UNRAR_H

#define RAR_OM_LIST 0

typedef struct __RAROpenArchiveDataEx
{
	char         *ArcName;
	wchar_t      *ArcNameW;
	unsigned int OpenMode;
	unsigned int OpenResult;
	char         *CmtBuf;
	unsigned int CmtBufSize;
	unsigned int CmtSize;
	unsigned int CmtState;
	unsigned int Flags;
	unsigned int Reserved[32];
} RAROpenArchiveDataEx;

/////////////////////////////////////////////////////////////////////////////
//FUNCTIONS IMPORT
//////////////////////////////////////////////////////////////////////

typedef HANDLE (PASCAL *RAROPENARCHIVEEX) (RAROpenArchiveDataEx*);
typedef int    (PASCAL *RARCLOSEARCHIVE)  (HANDLE);

#endif //T_UNRAR_H
