/////////////////////////////////////////////////////////////////////////////
//TBh.h
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TArchive.h"

#ifndef _TBH_CLASS_H
#define _TBH_CLASS_H

class TBh: public TArchive
{
private:
	/*Zagolovok fayla v arhive
	bez FILE_NAME[len(NAME_SIZE)], COMMENT[COMMENT_SIZE]
	v konce (oni ne vsegda prisutstvuyut)*/
	typedef struct __BH_FILE_HEAD
	{
		BYTE ID1;
		BYTE ID2;
		BYTE ID3;
		BYTE ID4;
		BYTE HEAD_SIZE1;
		BYTE HEAD_SIZE2;
		BYTE HOST;
		BYTE VER_CREATED;
		BYTE VER_EXTRACT;
		BYTE FLAGS1;
		BYTE FLAGS2;
		BYTE FTIME1;
		BYTE FTIME2;
		BYTE FTIME3;
		BYTE FTIME4;
		BYTE PACK_SIZE1;
		BYTE PACK_SIZE2;
		BYTE PACK_SIZE3;
		BYTE PACK_SIZE4;
		BYTE ORIG_SIZE1;
		BYTE ORIG_SIZE2;
		BYTE ORIG_SIZE3;
		BYTE ORIG_SIZE4;
		BYTE CRC1;
		BYTE CRC2;
		BYTE CRC3;
		BYTE CRC4;
		BYTE ATTR1;
		BYTE ATTR2;
		BYTE NAME_SIZE1;
		BYTE NAME_SIZE2;
		BYTE COMMENT_SIZE1;
		BYTE COMMENT_SIZE2;
	} BH_FILE_HEAD;

	//strukturi dlya chteniya arhiva
	BH_FILE_HEAD m_FILE_HEAD;	//zagolovok fayla

	//vnutrennie funkcii
	int AnalyzeFileHead(BH_FILE_HEAD&, int&);	//obrabotka zagolovka fayla/papki

public:
	TBh(DataForArchive& dfa, LanguageMessages& langmsg, LanguageResults& langres, char* namearch = "BH"): TArchive(dfa, langmsg, langres, namearch) {;}
	~TBh() {;}

	int TestFile        (char*);	//fayl yavlyaetsya arhivom BH
	int AnalyzeInfoOfArc(char*);	//opredelit' parametri (info) arhiva

	ULONGLONG GetUnpackSizeFiles  () {return m_UnpackSizeFiles;}	//vozvrat real'nogo razmera faylov v BYTEs
	ULONGLONG GetPackSizeFiles    () {return m_PackSizeFiles;}		//vozvrat sjatogo razmera faylov v BYTEs
	ULONGLONG GetArchiveSize      () {return m_ArchiveSize;}		//vozvrat razmera arhiva v BYTEs
	unsigned  GetNumberFiles      () {return m_NumberFiles;}		//vozvrat kolichestvo faylov v arhive
	unsigned  GetNumberFolders    () {return m_NumberFolders;}		//vozvrat kolichestvo papok v arhive
	short     GetMethodPack       () {return m_MethodPack;}			//vozvrat metod sjatiya | 0x00 - net sjatiya (Stored) | 0x01 - bistroe sjatie (Fast) | 0x02 - normal'noe sjatie (Fuse (Normal)) | 0x03 - normal'noe sjatie (Normal) | 0x04 - maximal'noe sjatie (Best)
	double    GetRatioArchiveSize () {return m_RatioArchiveSize;}	//sjatie ot razmera arhiva (%)
	double    GetRatioPackFileSize() {return m_RatioPackFileSize;}	//sjatie ot upak razmera faylov (%)
	double    GetPackVersion      () {return m_PackVersion;}		//vozvarat versiay zapakovki (X.X)
	double    GetUnpackVersion    () {return m_UnpackVersion;}		//vozvarat versiay dlya raspakovki (X.X)
	unsigned  GetSfxModule        () {return m_SfxModule;}			//razmer modulya SFX (*.exe) esli 0 to net
	BOOL      GetFileComment      () {return m_FileComment;}		//TRUE - fayli imeyut kommentarii FALSE - net
	BOOL      GetPassword         () {return m_Password;}			//TRUE - arhiv zashishen parolem FALSE - net
};

#endif	//_TBH_CLASS_H