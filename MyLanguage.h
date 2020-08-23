/////////////////////////////////////////////////////////////////////////////
//MyLanguage.h
//////////////////////////////////////////////////////////////////////

#ifndef T_MY_LANGUAGE_H
#define T_MY_LANGUAGE_H

#include "MIniFile.h"
#include "StructLanguage.h"

//klass dlya organizacii yazika programmi
//prosto sobrat' ves' yazik vmeste
class MyLanguage
{
public:
	//imena sekciy yazikovogo fayla *.lng
	typedef struct _LanguageSections
	{
		char SectTabs    [MAX_LOADSTRING];
		char SectInfo    [MAX_LOADSTRING];
		char SectAbout   [MAX_LOADSTRING];
		char SectOS      [MAX_LOADSTRING];
		char SectPack    [MAX_LOADSTRING];
		char SectOther   [MAX_LOADSTRING];
		char SectArchives[MAX_LOADSTRING];
		char SectMessages[MAX_LOADSTRING];
		char SectResults [MAX_LOADSTRING];
	} LanguageSections;

	MIniFile         fLNG;				//dlya obrasheniya k yazikovomu faylu (file *.lng)

	LanguageSections SectionsLNG;		//sekcii yazikovogo fayla (file *.lng)

	//[Tabs]
	LanguageTabs     sLanguageTabs;
	//[Info]
	LanguageTabInfo  sLanguageTabInfo;
	//[About]
	LanguageTabAbout sLanguageTabAbout;
	//[OS]
	LanguageOS       sLanguageOS;
	//[Pack]
	LanguagePack     sLanguagePack;
	//[Other]
	LanguageOther    sLanguageOther;
	//[Archives]
	LanguageArchives sLanguageArchives;
	//[Messages]
	LanguageMessages sLanguageMessages;
	//[Results]
	LanguageResults  sLanguageResults;

	MyLanguage();
	~MyLanguage() {;}

	//dlya raboti s faylami nastroek programmi
	int ReadLanguageProgram(char*);
};

#endif //T_MY_SETTIGS_H
