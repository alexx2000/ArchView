#include "stdafx.h"
#include "TSevenZip.h"

DEFINE_GUID(CLSID_CFormat7z,
	0x23170F69, 0x40C1, 0x278A, 0x10, 0x00, 0x00, 0x01, 0x10, 0x07, 0x00, 0x00);

HRESULT VarToBool(tagPROPVARIANT& prop, bool* result)
{
	if (prop.vt == VT_BOOL)
		*result = (prop.boolVal != VARIANT_FALSE);
	else if (prop.vt == VT_EMPTY)
		*result = false;
	else
		return E_FAIL;
	return S_OK;
}

HRESULT VarToUInt64(tagPROPVARIANT& prop, UInt64* result)
{
	switch (prop.vt)
	{
	case VT_EMPTY:
	case VT_NULL:
	{
		return E_FAIL;
	}
	case VT_I1:
	case VT_I2:
	{
		*result = prop.iVal;
		break;
	}
	case VT_INT:
	case VT_I4:
	{
		*result = prop.lVal;
		break;
	}
	case VT_I8:
	{
		*result = prop.hVal.QuadPart;
		break;
	}
	case VT_UI1:
	{
		*result = prop.bVal;
		break;
	}
	case VT_UI2:
	{
		*result = prop.uiVal;
		break;
	}
	case VT_UINT:
	case VT_UI4:
	{
		*result = prop.ulVal;
		break;
	}
	case VT_UI8:
	{
		*result = prop.uhVal.QuadPart;
		break;
	}
	default:
		return E_FAIL;
	}
	return S_OK;
}

HRESULT VarToString(tagPROPVARIANT& prop, LPSTR* result)
{
	switch (prop.vt)
	{
	case VT_EMPTY:
	case VT_NULL:
	{
		return E_FAIL;
	}
	case VT_LPSTR:
	{
		size_t len = strlen(prop.pszVal);
		*result = new char[len + 1];
		strcpy_s(*result, len + 1, prop.pszVal);
		break;
	}
	case VT_LPWSTR:
	{
		size_t len = lstrlenW(prop.pwszVal);
		*result = new char[len + 1];
		len = WideCharToMultiByte(CP_ACP, 0, prop.pwszVal, len, *result, len, nullptr, nullptr);
		(*result)[len] = '\0';
		break;
	}
	case VT_BSTR:
	{
		size_t len = lstrlenW(prop.bstrVal);
		*result = new char[len + 1];
		len = WideCharToMultiByte(CP_ACP, 0, prop.bstrVal, len, *result, len, nullptr, nullptr);
		(*result)[len] = '\0';
		SysFreeString(prop.bstrVal);
		break;
	}
	default:
		return E_FAIL;
	}
	return S_OK;
}

//////////////////////////////////////////////////////////////
// Input file stream class

class CInFileStream : public IInStream, public IStreamGetSize
{
private:
	long	m_refCount;
	HANDLE	m_fileStream;

public:
	CInFileStream()
		: m_refCount(0)
		, m_fileStream(INVALID_HANDLE_VALUE)
	{
	}

	virtual ~CInFileStream()
	{
		if (m_fileStream != INVALID_HANDLE_VALUE)
		{
			CloseHandle(m_fileStream);
		}
	}

	STDMETHOD(Open)(LPCSTR lpFileName)
	{
		m_fileStream = CreateFile(lpFileName, GENERIC_READ, FILE_SHARE_READ,
			NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if (m_fileStream != INVALID_HANDLE_VALUE)
		{
			return S_OK;
		}
		return HRESULT_FROM_WIN32(GetLastError());
	}

	STDMETHOD(QueryInterface)(REFIID iid, void** ppvObject)
	{
		if (iid == __uuidof(IUnknown))
		{
			*ppvObject = reinterpret_cast<IUnknown*>(this);
			AddRef();
			return S_OK;
		}
		if (iid == IID_ISequentialInStream)
		{
			*ppvObject = static_cast<ISequentialInStream*>(this);
			AddRef();
			return S_OK;
		}
		if (iid == IID_IInStream)
		{
			*ppvObject = static_cast<IInStream*>(this);
			AddRef();
			return S_OK;
		}
		if (iid == IID_IStreamGetSize)
		{
			*ppvObject = static_cast<IStreamGetSize*>(this);
			AddRef();
			return S_OK;
		}
		return E_NOINTERFACE;
	}

	STDMETHOD_(ULONG, AddRef)()
	{
		return static_cast<ULONG>(InterlockedIncrement(&m_refCount));
	}

	STDMETHOD_(ULONG, Release)()
	{
		ULONG res = static_cast<ULONG>(InterlockedDecrement(&m_refCount));
		if (res == 0)
		{
			delete this;
		}
		return res;
	}

	// ISequentialInStream
	STDMETHOD(Read)(void* data, UInt32 size, UInt32* processedSize)
	{
		if (m_fileStream != INVALID_HANDLE_VALUE)
		{
			if (ReadFile(m_fileStream, data, size, (LPDWORD)processedSize, nullptr))
			{
				return S_OK;
			}
			return HRESULT_FROM_WIN32(GetLastError());
		}
		return S_FALSE;
	}

	// IInStream
	STDMETHOD(Seek)(Int64 offset, UInt32 seekOrigin, UInt64* newPosition)
	{
		if (m_fileStream != INVALID_HANDLE_VALUE)
		{
			LARGE_INTEGER  liDistanceToMove;
			liDistanceToMove.QuadPart = offset;
			if (SetFilePointerEx(m_fileStream, liDistanceToMove, (PLARGE_INTEGER)newPosition, seekOrigin))
			{
				return S_OK;
			}
			return HRESULT_FROM_WIN32(GetLastError());
		}
		return S_FALSE;
	}

	// IStreamGetSize
	STDMETHOD(GetSize)(UInt64* size)
	{
		if (m_fileStream != INVALID_HANDLE_VALUE)
		{
			LARGE_INTEGER lpDistanceToMove;

			if (GetFileSizeEx(m_fileStream, (PLARGE_INTEGER)size))
			{
				return S_OK;
			}
			return HRESULT_FROM_WIN32(GetLastError());
		}
		return S_FALSE;
	}
};

//////////////////////////////////////////////////////////////
// Archive Open callback class

class CArchiveOpenCallback :
	public IArchiveOpenCallback,
	public ICryptoGetTextPassword,
	public CMyUnknownImp
{
private:
	BSTR  Password;
	bool PasswordIsDefined;

public:
	MY_UNKNOWN_IMP1(ICryptoGetTextPassword)

	STDMETHOD(SetTotal)(const UInt64* files, const UInt64* bytes)
	{
		return S_OK;
	}

	STDMETHOD(SetCompleted)(const UInt64* files, const UInt64* bytes)
	{
		return S_OK;
	}

	STDMETHOD(CryptoGetTextPassword)(BSTR* password)
	{
		if (!PasswordIsDefined)
		{
			// You can ask real password here from user
			// Password = GetPassword(OutStream);
			// PasswordIsDefined = true;
			return E_ABORT;
		}
		return StringToBstr(Password, password);
	}

	CArchiveOpenCallback() : Password(nullptr), PasswordIsDefined(false) {}
};

/////////////////////////////////////////////////////////////////////////////
//FUNCTIONS PRIVATE
/////////////////////////////////////////////////////////////////////////////

HRESULT TSevenZip::ReadPropBool(UInt32 index, PROPID propID, bool* result)
{
	tagPROPVARIANT prop;
	memset(&prop, 0, sizeof(tagPROPVARIANT));
	RINOK(archive->GetProperty(index, propID, &prop));
	return VarToBool(prop, result);
}

HRESULT TSevenZip::ReadPropUInt64(UInt32 index, PROPID propID, UInt64* result)
{
	tagPROPVARIANT prop;
	memset(&prop, 0, sizeof(tagPROPVARIANT));
	RINOK(archive->GetProperty(index, propID, &prop));
	return VarToUInt64(prop, result);
}

HRESULT TSevenZip::ReadPropString(UInt32 index, PROPID propID, LPSTR* result)
{
	tagPROPVARIANT prop;
	memset(&prop, 0, sizeof(tagPROPVARIANT));
	RINOK(archive->GetProperty(index, propID, &prop));
	return VarToString(prop, result);
}

HRESULT TSevenZip::ReadArcPropBool(PROPID propID, bool* result)
{
	tagPROPVARIANT prop;
	memset(&prop, 0, sizeof(tagPROPVARIANT));
	RINOK(archive->GetArchiveProperty(propID, &prop));
	return VarToBool(prop, result);
}

HRESULT TSevenZip::ReadArcPropUInt64(PROPID propID, UInt64* result)
{
	tagPROPVARIANT prop;
	memset(&prop, 0, sizeof(tagPROPVARIANT));
	RINOK(archive->GetArchiveProperty(propID, &prop));
	return VarToUInt64(prop, result);
}

HRESULT TSevenZip::ReadArcPropString(PROPID propID, LPSTR* result)
{
	tagPROPVARIANT prop;
	memset(&prop, 0, sizeof(tagPROPVARIANT));
	RINOK(archive->GetArchiveProperty(propID, &prop));
	return VarToString(prop, result);
}

HRESULT TSevenZip::LoadPlugin()
{
#ifdef _M_AMD64
	const LPCSTR lpLibFileName = "TC7Z64.DLL";
#else
	const LPCSTR lpLibFileName = "TC7Z.DLL";
#endif
	char lpLibFileName64[MAX_PATH] = { 0 };
	char lpCommanderPath[MAX_PATH] = { 0 };
	char lpLibFilePath64[MAX_PATH] = "%COMMANDER_PATH%";

	if (!::ExpandEnvironmentStrings(lpLibFilePath64, lpCommanderPath, MAX_PATH))
	{
		return HRESULT_FROM_WIN32(GetLastError());
	}
	snprintf(lpLibFileName64, sizeof(lpLibFileName64), "%s\\%s", lpCommanderPath, lpLibFileName);

	if (GetFileAttributes(lpLibFileName64) != INVALID_FILE_ATTRIBUTES)
	{
		m_hLibrary = LoadLibrary(lpLibFileName64);
	}
	else
	{
		snprintf(lpLibFileName64, sizeof(lpLibFileName64), "%s\\7z.dll", lpCommanderPath);
		m_hLibrary = LoadLibrary(lpLibFileName64);
	}
	if (m_hLibrary == 0)
	{
		return HRESULT_FROM_WIN32(GetLastError());
	}
	Func_CreateObject createObjectFunc = (Func_CreateObject)GetProcAddress(m_hLibrary, "CreateObject");
	if (!createObjectFunc)
	{
		return HRESULT_FROM_WIN32(GetLastError());
	}
	RINOK(createObjectFunc(&CLSID_CFormat7z, &IID_IInArchive, (void**)&archive));
}

/////////////////////////////////////////////////////////////////////////////
//FUNCTIONS PUBLIC
/////////////////////////////////////////////////////////////////////////////

TSevenZip::TSevenZip(DataForArchive& dfa, LanguageMessages& langmsg, LanguageResults& langres, char* namearch) : 
	TArchive(dfa, langmsg, langres, namearch), m_hLibrary(0)
{
	LoadPlugin();
}

TSevenZip::~TSevenZip()
{
	archive.Release();
	if (m_hLibrary) FreeLibrary(m_hLibrary);
}

int TSevenZip::TestFile(char* path)
{
	InitialParametrs();
	
	if (!archive)
	{
		if (m_DetailLF == 2)
			m_LogFile->WriteMessage(m_LangMsg.SignatureArchive, TERROR_DLL);
		return TERROR_DLL;
	}
	
	CInFileStream* fileSpec = new CInFileStream;
	file = fileSpec;

	if (fileSpec->Open(path) != S_OK)
	{
		return TERROR_OPEN_FILE;
	}

	UInt64 newPosition;
	UInt32 processedSize;
	WORD EXECUTABLE_ID = 0;	
	if (fileSpec->Read(&EXECUTABLE_ID, sizeof(EXECUTABLE_ID), &processedSize) != S_OK)
	{
		return TERROR_READ_FILE;
	}
	if ((fileSpec->Seek(0, STREAM_SEEK_SET, &newPosition) != S_OK) || (newPosition))
	{
		return TERROR_SEEK_FILE;
	}
	if (EXECUTABLE_ID == 'ZM')
	{
		m_SfxModule = 1;
		if (!m_CheckSFX) return TERROR_FORMAT;
	}
	fileSpec->GetSize((UInt64 *)&m_ArchiveSize);
	{
		CArchiveOpenCallback* openCallbackSpec = new CArchiveOpenCallback;
		CMyComPtr<IArchiveOpenCallback> openCallback(openCallbackSpec);

		const UInt64 scanSize = 1 << 23;
		if (archive->Open(file, &scanSize, openCallback) != S_OK)
		{
			if (m_DetailLF == 2)
				m_LogFile->WriteMessage(m_LangMsg.SignatureArchive, TERROR_FORMAT);
			return TERROR_FORMAT;
		}
	}
	return TMESSAGE_OK;
}

int TSevenZip::AnalyzeInfoOfArc(char* path)
{
	bool value;
	UInt64 size;
	LPSTR str = NULL;
	UInt32 numItems = 0;

	if (ReadArcPropBool(kpidSolid, &value) == S_OK)
	{
		m_Solid = value;
	}
	if (ReadArcPropUInt64(kpidOffset, &size) == S_OK)
	{
		m_Offset = size;
		if (m_SfxModule) m_SfxModule = size;
	}
	if (ReadArcPropUInt64(kpidNumBlocks, &size) == S_OK)
	{
		m_NumBlocks = size;
	}
	if (ReadArcPropUInt64(kpidHeadersSize, &size) == S_OK)
	{
		m_HeadersSize = size;
	}
	if (ReadArcPropString(kpidMethod, &str) == S_OK)
	{
		if (m_pMethodPack) { delete[] m_pMethodPack; }
		m_pMethodPack = str;
	}
	if (archive->GetNumberOfItems(&numItems) != S_OK)
	{
		return TERROR_READ_FILE;
	}
	for (UInt32 i = 0; i < numItems; i++)
	{	
		if (m_BeginThread)
		{
			//net lis sobitiya zavershit' rabotu potoka
			if (::WaitForSingleObject(m_hEventEnd, 0) == WAIT_OBJECT_0)
			{
				return TTERMINATE_THREAD;
			}
		}
		// Get uncompressed size of file
		if (ReadPropUInt64(i, kpidSize, &size) == S_OK)
		{
			m_UnpackSizeFiles += size;
		}
		// Get compressed size of file
		if (ReadPropUInt64(i, kpidPackSize, &size) == S_OK)
		{
			m_PackSizeFiles += size;
		}
		if (ReadPropBool(i, kpidEncrypted, &value) == S_OK)
		{
			if (value) m_Password = value;
		}

		if ((ReadPropBool(i, kpidIsDir, &value) == S_OK) && (value))
		{
			m_BlockType = 2;
			m_NumberFolders++;
		}
		else
		{
			m_BlockType = 1;
			m_NumberFiles++;
		}

		if (m_DetailLF == 2)
		{
			switch (m_BlockType)
			{
			case 1: m_LogFile->WriteMessage(m_LangMsg.BlockFile, MSG);
				break;
			case 2: m_LogFile->WriteMessage(m_LangMsg.BlockFolder, MSG);
				break;
			}
		}
	}
	CountRatioArchiveSize();
	CountRatioPackFileSize();

	archive->Close();

	return TMESSAGE_OK;
}
