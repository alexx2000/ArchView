/////////////////////////////////////////////////////////////////////////////
//MLogFile.cpp
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MLogFile.h"

/////////////////////////////////////////////////////////////////////////////
//Construction/Destruction
/////////////////////////////////////////////////////////////////////////////

//langres    - yazik resultatov dlya logfile
//targetfile - fayl dlya kotorogo sozdaetsya logfile
MLogFile::MLogFile(LanguageResults& langres, char* targetfile):
	m_LangRes(langres), m_TargetFile(targetfile),
	m_hFile(0), m_pTypeArch(0), m_pMessage(0),
	m_MaxLenAuthor(12), m_MaxLenMessage(40)
{
	SetTypeArchive("MSG");
}

//langres       - yazik resultatov dlya logfile
//targetfile    - fayl dlya kotorogo sozdaetsya logfile
//nameautho     - avtor soobsheniya v logfile
//maxlenauthor  - maximal'naya dlina avtora soobsheniya
//maxlenmessage - maximal'naya dlina soobsheniya
MLogFile::MLogFile(LanguageResults& langres, char* targetfile, char* nameauthor, unsigned maxlenauthor, unsigned maxlenmessage):
	m_LangRes(langres), m_TargetFile(targetfile),
	m_hFile(0), m_pTypeArch(0), m_pMessage(0),
	m_MaxLenAuthor(maxlenauthor), m_MaxLenMessage(maxlenmessage)
{
	SetTypeArchive(nameauthor);
}

MLogFile::~MLogFile()
{
	try
	{
		if (m_pTypeArch) { delete[] m_pTypeArch; m_pTypeArch = 0; }
		if (m_pMessage)  { delete[] m_pMessage;  m_pMessage  = 0; }
		if (m_hFile)     { CloseLogFile();       m_hFile     = 0; }
		m_TargetFile = 0;
	}
	catch (...) {;}
}

/////////////////////////////////////////////////////////////////////////////
//FUNCTIONS PUBLIC
/////////////////////////////////////////////////////////////////////////////

//ustanovit' tip arhiva ot imeni kotorogo
//budut zapisivatsya soobsheniya
//str - ukazatel' na imya arhiva ("RAR", "ZIP", ...)
void MLogFile::
SetTypeArchive(char* str)
{
	char* razd = ": ";	//razdelitel'
	if (m_pTypeArch) { delete[] m_pTypeArch; m_pTypeArch = 0; }

	//proverka dlini imeni
	unsigned lenr = strlen(razd);
	unsigned size = strlen(str);
	if (size > m_MaxLenAuthor) size = m_MaxLenAuthor;

	//kopirovanie imeni
	m_pTypeArch = new char[m_MaxLenAuthor+lenr+1];
	strncpy(m_pTypeArch, str, size);
	m_pTypeArch[size] = '\0';
	strcat (m_pTypeArch, razd);

	//zapolnenie probelami do maximal'noy dlini
	for (unsigned i=strlen(m_pTypeArch); i<m_MaxLenAuthor+lenr; i++)
		m_pTypeArch[i] = ' ';
	m_pTypeArch[m_MaxLenAuthor+lenr] = '\0';
}

//ustanovit' maximal'nuyu dlinu avtora soobsheniya
//maxlenautor - maximal'naya dlina avtora soobsheniya
void MLogFile::
SetMaxLenAuthor(unsigned maxlenauthor)
{
	m_MaxLenAuthor = maxlenauthor;
}

//ustanovit' maximal'nuyu dlinu soobsheniya
//maxlenmessage - maximal'naya dlina soobsheniya
void MLogFile::
SetMaxLenMessage(unsigned maxlenmessage)
{
	m_MaxLenMessage = maxlenmessage;
}

//path - imya logfile
//flag - 0 dopisivat' v konec; 1- perepisat' fayl
//1 - open ok
//0 - open error
int MLogFile::
OpenLogFile(char* path, int flag)
{
	if (m_hFile) CloseLogFile();
	if (flag)
		m_hFile = fopen(path, "w");
	else
		m_hFile = fopen(path, "a");

	if (m_hFile)
	{
		//zapishem datu i vremya otkritiya
		//i imya fayla dlya kotorogo sozdaetsya etot logfile
		char* nline = "\n";
		char* space = " ";
		char date[9];
		char time[9];
		char *message;

		_strdate(date);
		_strtime(time);

		int size =	strlen(nline) + strlen(date)	     +
					strlen(space) + strlen(time)         +
					strlen(nline) + strlen(m_TargetFile) +
					strlen(nline);
		message = new char[size];
		strcpy(message, nline);
		strcat(message, date);
		strcat(message, space);
		strcat(message, time);
		strcat(message, nline);
		strcat(message, m_TargetFile);
		strcat(message, nline);

		fwrite(message, size, 1, m_hFile);
		return 1;
	}
	else return 0;
}

//1 - fayl zakrit
//0 - oshibka zakritiya
int MLogFile::
CloseLogFile()
{
	if (m_hFile)
	{
		if (fclose(m_hFile))
		{
			m_hFile = 0;
			return 1;
		}
		else
			return 0;
	}
	return 1;
}

//zapisat' soobshenie v logfile
//msg    - tip soobsheniya
//result - resultat
int MLogFile::
WriteMessage(char* msg, int result)
{
	char*    message = 0;		//soobshenie/sobitie
	char*    pres    = 0;		//resul'tat vipolneniya
	char*    razd    = " - ";	//razdelitel'
	char*    NewLine = "\n";	//perevod stroki
	unsigned size    = 0;

	//virivnyaem msg po maximal'noy dline
	if (m_pMessage) { delete[] m_pMessage; m_pMessage = 0; }
	//proverka dlini imeni
	size = strlen(msg);
	if (size > m_MaxLenMessage) size = m_MaxLenMessage;
	//kopirovanie imeni
	m_pMessage = new char[m_MaxLenMessage+1];
	strncpy(m_pMessage, msg, size);
	m_pMessage[size] = '\0';
	//zapolnenie probelami do maximal'noy dlini
	for (unsigned i=strlen(m_pMessage); i<m_MaxLenMessage; i++)
		m_pMessage[i] = ' ';
	m_pMessage[m_MaxLenMessage] = '\0';

	//opredelenie stroki resul'tata
	switch (result)
	{
	case TTERMINATE_THREAD:		pres = m_LangRes.TerminateThread;
								break;
	case TMESSAGE_OK:			pres = m_LangRes.MessageOK;
								break;
	case TERROR_OPEN_FILE:		pres = m_LangRes.ErrorOpenFile;
								break;
	case TERROR_READ_FILE:		pres = m_LangRes.ErrorReadFile;
								break;
	case TERROR_SEEK_FILE:		pres = m_LangRes.ErrorSeekFile;
								break;
	case TSUDDEN_END_FILE:		pres = m_LangRes.SuddenEndFile;
								break;
	case TERROR_FORMAT:			pres = m_LangRes.ErrorFormat;
								break;
	case TERROR_NOT_DLL:		pres = m_LangRes.ErrorNotDll;
								break;
	case TERROR_DLL:			pres = m_LangRes.ErrorDll;
								break;
	case TERROR_CREATE_WINDOW:	pres = m_LangRes.ErrorCreateWindow;
								break;
	default:					pres = m_LangRes.ErrorUnknown;
								break;
	}

	//opredelenie dlini vivodimigo soobsheniya
	size  = strlen(m_pTypeArch);
	size += strlen(m_pMessage);
	size += strlen(razd);
	size += strlen(pres);
	size += strlen(NewLine);
	message = new char[size+1];

	//sostavlenie soobsheniya
	strcpy(message, m_pTypeArch);
	strcat(message, m_pMessage);
	strcat(message, razd);
	strcat(message, pres);
	strcat(message, NewLine);

	int res = fwrite(message, size, 1, m_hFile);

	//udelim bufer
	if (message) { delete[] message; message = 0; }

	if ((unsigned)res == size)
		return 1;
	else
		return 0;
}

//zapisat' stroku v logfile
//str    - stroka dlya zapisi
int MLogFile::
WriteString(const char* str)
{
	unsigned size = strlen(str);
	int res = fwrite(str, size, 1, m_hFile);

	if ((unsigned)res == size)
		return 1;
	else
		return 0;
}
