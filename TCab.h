/////////////////////////////////////////////////////////////////////////////
//TCab.h
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TArchive.h"

#ifndef _TCAB_CLASS_H
#define _TCAB_CLASS_H

class TCab: public TArchive
{
private:
	//zagolovok arhiva
	typedef struct __CAB_ARC_HEAD
	{	
		DWORD SIGNATURE;
		DWORD HEAD_CHECKSUM;
		DWORD FILE_SIZE;
		DWORD FOLDERS_CHECKSUM;
		DWORD OFFSET;
		DWORD FILES_CHECKSUM;
		BYTE VERSION_PACK;
		BYTE VERSION_UNPACK;
		WORD NUMBER_FOLDERS;
		WORD NUMBER_FILES;
		WORD FLAGS;
		WORD SET_ID;
		WORD VOLUME_NUM;
	} CAB_ARC_HEAD;

	//konec zagolovka arhiva
	typedef struct __CAB_ARC_ADD
	{	
		BYTE METHOD;
		BYTE METHOD_ADD;
	} CAB_ARC_ADD;

	/*Zagolovok fayla v arhive
	bez FILE_NAME*/
	typedef struct __CAB_FILE_HEAD
	{
		DWORD UNP_SIZE;
		DWORD OFFSET;
		WORD FILE_CONTROL_ID;
		WORD FDATE;
		WORD FTIME;
		WORD ATTR;
	} CAB_FILE_HEAD;

	//strukturi dlya chteniya arhiva
	CAB_ARC_HEAD  m_ARC_HEAD;	//zagolovok arhiva
	CAB_ARC_ADD   m_ARC_ADD;		//konec zagolovka arhiva
	CAB_FILE_HEAD m_FILE_HEAD;	//zagolovok fayla

	char m_buffer[MAX_LEN_STR];
	WORD m_NumberEntries;		//vmeste faylov i papok

	//vnutrennie funkcii
	int AnalyzeArcHead (CAB_ARC_HEAD&);			//proverka zagolovka arhiva
	int AnalyzeFileHead(CAB_FILE_HEAD&, int&);	//obrabotka zagolovka fayla/papki

public:
	TCab(DataForArchive& dfa, LanguageMessages& langmsg, LanguageResults& langres, char* namearch = "CAB"): TArchive(dfa, langmsg, langres, namearch) {;}
	~TCab() {;}

	int TestFile        (char*);	//fayl yavlyaetsya arhivom CAB
	int AnalyzeInfoOfArc(char*);	//opredelit' parametri (info) arhiva

	ULONGLONG GetUnpackSizeFiles () {return m_UnpackSizeFiles;}	//vozvrat real'nogo razmera faylov v BYTEs
	ULONGLONG GetArchiveSize     () {return m_ArchiveSize;}		//vozvrat razmera arhiva v BYTEs
	unsigned  GetNumberFiles     () {return m_NumberFiles;}		//vozvrat kolichestvo faylov v arhive
	unsigned  GetNumberFolders   () {return m_NumberFolders;}	//vozvrat kolichestvo papok v arhive
	short     GetMethodPack      () {return m_MethodPack;}		//vozvrat metod sjatiya | 0x00 - bez szhatiya (Stored) | 0x01 - (MSZIP (Normal)) | 0x03 - (LZX) | 0x71 - (Quantum) | 0x72 - (NONAME)
	short     GetMethodPackAdd   () {return m_MethodPackAdd;}	//vozvrat metod sjatiya	| 0x0F - (:15 (Superast)) | 0x10 - (:16 (Fastest)) | 0x11 - (:17 (Fast)) | 0x12 - (:18 (Normal)) | 0x13 - (:19 (Good)) | 0x14 - (:20 (Verygood)) | 0x15 - (:21 (Best))
	double    GetRatioArchiveSize() {return m_RatioArchiveSize;}//sjatie ot razmera arhiva (%)
	double    GetPackVersion     () {return m_PackVersion;}		//vozvarat versiay zapakovki (X.X)
	double    GetUnpackVersion   () {return m_UnpackVersion;}	//vozvarat versiay dlya raspakovki (X.X)
	unsigned  GetSfxModule       () {return m_SfxModule;}		//razmer modulya SFX (*.exe) esli 0 to net
	BOOL      GetMultiArc        () {return m_MultiArc;}		//TRUE - arhiv mnogotomniy FALSE - net
	unsigned  GetVolumeNumber    () {return m_VolumeNumber;}	//nomer toma mnogotomnogo arhiva (0 - ?)
	BOOL      GetPassword        () {return m_Password;}		//TRUE - arhiv zashishen parolem FALSE - net
};

#endif	//_TCAB_CLASS_H