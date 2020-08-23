/////////////////////////////////////////////////////////////////////////////
//MListParametrs.cpp
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ListPlugin.h"
#include "MListParametrs.h"

/////////////////////////////////////////////////////////////////////////////
//FUNCTIONS PRIVATE
/////////////////////////////////////////////////////////////////////////////

//udalenie spiska parametrov
int MListParametrs::DeleteListParametrs()
{
	//udalenie okon dlya parametrov esli oni est'
	DeleteHwndListParametrs();

	Parametr* SelParam = 0;
	Parametr* TmpParam = 0;

	if (m_FirstParametr)
	{
		SelParam = m_FirstParametr;
		//cikl po vsem elementam - parametram
		while (1)
		{
			if (SelParam)
			{
				//imya
				if (SelParam->Name)
				{
					delete[] SelParam->Name;
					SelParam->Name = 0;
				}
				//znachenie
				if (SelParam->Value)
				{
					delete[] SelParam->Value;
					SelParam->Value = 0;
				}
				TmpParam = SelParam;
				SelParam = SelParam->Next;	//sleduyushiy element - parametr
				delete TmpParam;
			}
			else break;
		}
	}
	return 1;
}

/////////////////////////////////////////////////////////////////////////////
//Construction/Destruction
/////////////////////////////////////////////////////////////////////////////

MListParametrs::~MListParametrs()
{
	//udalenie spiska parametrov
	DeleteListParametrs();
	m_FirstParametr = 0;
	m_LastParametr  = 0;
}

/////////////////////////////////////////////////////////////////////////////
//FUNCTIONS PUBLIC
/////////////////////////////////////////////////////////////////////////////

//dobavit' v spisok parametr
//name   - imya parametra
//value  - znachenie parametra
//numstr - kolichestvo strok v znachenii (name ili value)
//return
//	1 - parametr dobavlen
//	0 - parametr ne dobavlen
int MListParametrs::AddParametr(const char* name, const char* value, unsigned numstr)
{
	int len = 0;

	//ne vklyuchat' 2 podryad pustih parametra
	//pustoy parametr - etor razdelitel' parametrov po gruppam
	//(name = "", value = "", numstr=0)
	//tak je proverka a ne perviy li eto parametr
	//(chtob ne voznikla oshibka)
	if (m_NumberParametrs)
		if ((!strlen(name))          &&
			(!m_LastParametr->Name)  &&
			(!strlen(value))         &&
			(!m_LastParametr->Value)) return 0;

	//sozdaem parametr
	Parametr* NewParam;
	NewParam = new Parametr;
	//imya
	len = strlen(name);
	if (len)
	{
		NewParam->Name = new char[len+1];
		strcpy(NewParam->Name, name);
	}
	else NewParam->Name = 0;
	//znachenie
	len = strlen(value);
	if (len)
	{
		NewParam->Value = new char[len+1];
		strcpy(NewParam->Value, value);
	}
	else NewParam->Value = 0;
	//ostal'noe
	NewParam->NumStr    = numstr;
	NewParam->hWndName  = 0;
	NewParam->hWndValue = 0;
	NewParam->Next      = 0;

	//perviy parametr ili net
	if (m_NumberParametrs == 0)
	{
		m_FirstParametr = NewParam;
		m_LastParametr  = NewParam;
	}
	else
	{
		m_LastParametr->Next = NewParam;
		m_LastParametr       = NewParam;
	}

	//+1 parametr
	m_NumberParametrs++;
	return 1;
}

//udalit' vse okna dlya parametrov
int MListParametrs::DeleteHwndListParametrs()
{
	Parametr* SelParam = 0;

	if (m_FirstParametr)
	{
		SelParam = m_FirstParametr;
		//cikl po vsem elementam - parametram
		while (1)
		{
			if (SelParam)
			{
				//dlya imeni
				if (SelParam->hWndName)
				{
					::DestroyWindow(SelParam->hWndName);
					SelParam->hWndName = 0;
				}
				//dlya znacheniya
				if (SelParam->hWndValue)
				{
					::DestroyWindow(SelParam->hWndValue);
					SelParam->hWndValue = 0;
				}
				SelParam = SelParam->Next;	//sleduyushiy element parametr
			}
			else break;
		}
	}
	return 1;
}
