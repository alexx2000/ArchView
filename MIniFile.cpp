/////////////////////////////////////////////////////////////////////////////
//MIniFile.cpp: implementation of the MIniFile class.
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MIniFile.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

MIniFile::MIniFile()
{
	m_pFilePath = NULL;
}

MIniFile::~MIniFile()
{
	try
	{
		if(m_pFilePath) { delete[] m_pFilePath; m_pFilePath = 0; }
	}
	catch (...) {;}
}

BOOL MIniFile::WriteKey(const char* pSection, const char* pKey, const char* pData)
{
	if(!m_pFilePath)
		return false;
	return (BOOL)WritePrivateProfileString(pSection, pKey, pData, m_pFilePath); 
}

int MIniFile::WriteKey(const char* pSection, const char* pKey, int nData)
{
	if(!m_pFilePath)
		return false;
	char pData[12] = {NULL};
	wsprintf(pData,"%i",nData);
	return (BOOL)WritePrivateProfileString(pSection, pKey, pData, m_pFilePath); 
}

void MIniFile::SetFile(const char* pFileName)
{
	if(m_pFilePath) { delete[] m_pFilePath; m_pFilePath = 0; }
	int nAllocAmount = strlen(pFileName) + 1;
	m_pFilePath      = new char[nAllocAmount];
	strcpy(m_pFilePath, pFileName);
}

unsigned long int MIniFile::ReadKey(const char* pSection, const char* pKey, const char* pDefault,char* pOutputBuffer, unsigned long nOutputBufferLength)
{
	if(!m_pFilePath) return false;
	return GetPrivateProfileString(pSection, pKey, pDefault, pOutputBuffer, nOutputBufferLength, m_pFilePath);
}

int MIniFile::ReadKey(const char* pSection, const char* pKey, int nDefault)
{
	if(!m_pFilePath) return false;
	return (long int)GetPrivateProfileInt(pSection, pKey, nDefault, m_pFilePath);
}

const char *MIniFile::GetFilePath()
{
	return m_pFilePath;
}

int MIniFile::ReadSection(const char* pSection, MListStrStr& ListStrStr, int Optimize)
{
	if(!m_pFilePath) return false;
	const int BufSize          = 65536;
	char      AllKeys[BufSize] = {NULL};
	int       CharsRead        = GetPrivateProfileSection(pSection, AllKeys, BufSize, m_pFilePath);
	if (CharsRead)
	{
		char* ptmp     = AllKeys;
		char* peq_sign = 0;
		while (*ptmp)
		{
			if (*ptmp == ';')	//propuskaem kommentarii
			{
				ptmp += strlen(ptmp) + 1;
				continue;
			}
			peq_sign = strstr(ptmp, "=");
			if (peq_sign)
			{
				*(peq_sign++) = '\0';
				if (Optimize != MINI_STANDART_ALL_VALUE)	//MINI_OPTIMIZE_ONLY_VALUE
					{ if (!ListStrStr.SearchNodeFromValue(peq_sign)) ListStrStr.AddNode(ptmp, peq_sign); }
				else
					ListStrStr.AddNode(ptmp, peq_sign);
				ptmp = peq_sign + strlen(peq_sign) + 1;
			}
			else				//propuskaem stroki bez znaka '='
				ptmp += strlen(ptmp) + 1;
		}
	}
	return CharsRead;
}

int MIniFile::ReadSectionSpecial(const char* pSection, MListStrStr& ListStrStr, int Optimize)
{
	if(!m_pFilePath) return false;
	const int BufSize          = 65536;
	char      AllKeys[BufSize] = {NULL};
	int       CharsRead        = GetPrivateProfileSection(pSection, AllKeys, BufSize, m_pFilePath);
	if (CharsRead)
	{
		char* ptmp     = AllKeys;
		char* peq_sign1 = 0;
		char* peq_sign2 = 0;
		while (*ptmp)
		{
			if (*ptmp == ';')	//propuskaem kommentarii
			{
				ptmp += strlen(ptmp) + 1;
				continue;
			}
			peq_sign1 = strstr(ptmp, "=");
			if (peq_sign1)
			{
				*(peq_sign1++) = '\0';
				peq_sign2 = strstr(peq_sign1, ",");
				if (peq_sign2)
				{
					peq_sign2++;
					if (Optimize != MINI_STANDART_ALL_VALUE)	//MINI_OPTIMIZE_ONLY_VALUE
						{ if (!ListStrStr.SearchNodeFromValue(peq_sign2)) ListStrStr.AddNode(ptmp, peq_sign2); }
					else
						ListStrStr.AddNode(ptmp, peq_sign2);
					ptmp = peq_sign2 + strlen(peq_sign2) + 1;
				}
				else
				{
					if (Optimize == MINI_OPTIMIZE_ONLY_VALUE)
						{ if (!ListStrStr.SearchNodeFromValue(peq_sign1)) ListStrStr.AddNode(ptmp, peq_sign1); }
					else
						ListStrStr.AddNode(ptmp, peq_sign1);
					ptmp = peq_sign1 + strlen(peq_sign1) + 1;
				}
			}
			else				//propuskaem stroki bez znaka '='
				ptmp += strlen(ptmp) + 1;
		}
	}
	return CharsRead;
}
