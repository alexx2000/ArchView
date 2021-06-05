#pragma once
#include "TArchive.h"

#include <stdio.h>
#include <InitGuid.h>

#include <Windows.h>
#include <cguid.h>
#include <atlcomcli.h>

#include "7zip/IStream.h"
#include "7zip/MyCom.h"
#include "7zip/IArchive.h"
#include "7zip/IPassword.h" 

class TSevenZip :
	public TArchive
{
private:
	CMyComPtr<IInArchive> archive;
	CMyComPtr<IInStream> file;
	HMODULE m_hLibrary;

	HRESULT LoadPlugin();

	HRESULT ReadArcPropBool(PROPID propID, bool* result);
	HRESULT ReadArcPropUInt64(PROPID propID, UInt64* result);
	HRESULT ReadArcPropString(PROPID propID, LPSTR* result);

	HRESULT ReadPropBool(UInt32 index, PROPID propID, bool* result);
	HRESULT ReadPropUInt64(UInt32 index, PROPID propID, UInt64* result);
	HRESULT ReadPropString(UInt32 index, PROPID propID, LPSTR* result);

protected:


public:
	TSevenZip(DataForArchive& dfa, LanguageMessages& langmsg, LanguageResults& langres, char* namearch = "7ZIP");
	~TSevenZip();

	int TestFile(char*);
	int AnalyzeInfoOfArc(char*);

	ULONGLONG GetUnpackSizeFiles() { return m_UnpackSizeFiles; }	 //vozvrat real'nogo razmera faylov v BYTEs
	ULONGLONG GetPackSizeFiles() { return m_PackSizeFiles; }		 //vozvrat sjatogo razmera faylov v BYTEs
	ULONGLONG GetArchiveSize() { return m_ArchiveSize; }		     //vozvrat razmera arhiva v BYTEs
	unsigned  GetNumberFiles() { return m_NumberFiles; }		     //vozvrat kolichestvo faylov v arhive
	unsigned  GetNumberFolders() { return m_NumberFolders; }		 //vozvrat kolichestvo papok v arhive
	double    GetRatioArchiveSize() { return m_RatioArchiveSize; }	 //sjatie ot razmera arhiva (%)
	double    GetRatioPackFileSize() { return m_RatioPackFileSize; } //sjatie ot upak razmera faylov (%)
	unsigned  GetSfxModule() { return m_SfxModule; }			     //razmer modulya SFX (*.exe) esli 0 to net
	BOOL      GetSolid() { return m_Solid; }				         //TRUE - arhiv neprerivniy FALSE - net
	BOOL      GetPassword() { return m_Password; }			         //TRUE - arhiv zashishen parolem FALSE - net
	char*     GetMethodPack() { return m_pMethodPack; }		         //vozvrat metod sjatiya
	LONGLONG  GetNumBlocks() { return m_NumBlocks; }		         //vozvrat kolichestvo blokov
	LONGLONG  GetHeadersSize() { return m_HeadersSize; }	         //vozvrat razmer zagolovkov
	LONGLONG  GetOffset() { return m_Offset; }		                 //vozvrat smeshchenie arhiva
};
