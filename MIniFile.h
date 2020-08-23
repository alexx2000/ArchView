/////////////////////////////////////////////////////////////////////////////
//MIniFile.h: interface for the MIniFile class.
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_INIFILE_H__2A13E091_A467_441B_89C3_9D2D49B86DDE__INCLUDED_)
#define AFX_INIFILE_H__2A13E091_A467_441B_89C3_9D2D49B86DDE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define MAX_PATH_LENGTH 1024

#define MINI_STANDART_ALL_VALUE    0
#define MINI_OPTIMIZE_ONLY_VALUE   1

#include "MListStrStr.h"

//slujit dlya raboti s faylom nastroek
//eto obichniy ini-file
class MIniFile  
{
public:
	MIniFile();
	virtual ~MIniFile();

	unsigned long int ReadKey    (const char*, const char*, const char*, char*, unsigned long int);
	int               ReadKey    (const char*, const char*, int);
	void              SetFile    (const char*);
	BOOL              WriteKey   (const char*, const char*, const char*);
	int               WriteKey   (const char*, const char*, int);
	const char*       GetFilePath();

	int               ReadSection(const char*, MListStrStr&, int = MINI_STANDART_ALL_VALUE);
	int               ReadSectionSpecial(const char*, MListStrStr&, int = MINI_STANDART_ALL_VALUE);

protected:
	char *m_pFilePath;
};

#endif // !defined(AFX_INIFILE_H__2A13E091_A467_441B_89C3_9D2D49B86DDE__INCLUDED_)
