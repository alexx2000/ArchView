/////////////////////////////////////////////////////////////////////////////
//MLogFile.h
/////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <string.h>
#include <time.h>

#ifndef _MLOGFILE_CLASS_H
#define _MLOGFILE_CLASS_H

//dlya vedeniya logfayla
class MLogFile
{
	private:
		char*    m_pTypeArch;		//tip arhiva ot kotorogo idut soobsheniya
		char*    m_pMessage;		//soobshenie
		char*    m_TargetFile;		//fayl dlya kotorogo sozdaetsya logfile
		FILE*    m_hFile;			//logfayl
		unsigned m_MaxLenAuthor;	//maximal'naya dlina avtora soobsheniya
		unsigned m_MaxLenMessage;	//maximal'naya dlina soobsheniya

		//multiyazichost'
		LanguageResults& m_LangRes;	//yazik dlya resultatov logfile

	public:
		MLogFile(LanguageResults& langres, char* targetfile);
		MLogFile(LanguageResults& langres, char* targetfile, char* nameauthor, unsigned maxlenauthor = 5, unsigned maxlenmessage = 50);
		~MLogFile();

		int   OpenLogFile (char*, int flag = 0);//otkrit' logfile
		int   CloseLogFile();					//zakrit' logfayl
		int   WriteMessage(char*, int);			//zapisat' soobshenie v logfile
		int   WriteString(const char*);			//zapisat' stroku

		void  SetTypeArchive  (char*);		//ustanovit' tip arhiva
		void  SetMaxLenAuthor (unsigned);	//ustanovit' tip arhiva
		void  SetMaxLenMessage(unsigned);	//ustanovit' tip arhiva

		char*    GetTypeArchive  () { return m_pTypeArch;     }	//opredelit' tip arhiva
		unsigned GetMaxLenAuthor () { return m_MaxLenAuthor;  }	//opredelit' maximal'nuyu dlinu avtora soobsheniya
		unsigned GetMaxLenMessage() { return m_MaxLenMessage; }	//opredelit' maximal'nuyu dlinu soobsheniya
};

#endif	//_MLOGFILE_CLASS_H

