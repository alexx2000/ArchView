/////////////////////////////////////////////////////////////////////////////
//MListStrStr.cpp
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MListStrStr.h"

/////////////////////////////////////////////////////////////////////////////
//FUNCTIONS PRIVATE
/////////////////////////////////////////////////////////////////////////////

//udalenie spiska parametrov
int MListStrStr::DeleteListStrStr()
{
	NodeStrStr* SelNode = 0;
	NodeStrStr* TmpNode = 0;

	if (m_FirstNode)
	{
		SelNode = m_FirstNode;
		//cikl po vsem elementam - uzlam
		while (1)
		{
			if (SelNode)
			{
				//imya
				if (SelNode->Name)
				{
					delete[] SelNode->Name;
					SelNode->Name = 0;
				}
				//znachenie
				if (SelNode->Value)
				{
					delete[] SelNode->Value;
					SelNode->Value = 0;
				}
				TmpNode = SelNode;
				SelNode = SelNode->Next;	//sleduyushiy element - uzel
				delete TmpNode;
			}
			else break;
		}
	}
	return 1;
}

/////////////////////////////////////////////////////////////////////////////
//Construction/Destruction
/////////////////////////////////////////////////////////////////////////////

MListStrStr::~MListStrStr()
{
	//udalenie spiska parametrov
	DeleteListStrStr();
	m_FirstNode = 0;
	m_LastNode  = 0;
}

/////////////////////////////////////////////////////////////////////////////
//FUNCTIONS PUBLIC
/////////////////////////////////////////////////////////////////////////////

//dobavit' v spisok uzel
//name   - imya parametra (imya wcx-plagina)
//value  - znachenie parametra (put' k plaginu)
//return
//	1 - uzel dobavlen
//	0 - uzel ne dobavlen
int MListStrStr::AddNode(const char* name, const char* value)
{
	int len = 0;

	//sozdaem uzel
	NodeStrStr* NewNode;
	NewNode = new NodeStrStr;
	//imya
	len = strlen(name);
	if (len)
	{
		NewNode->Name = new char[len+1];
		strcpy(NewNode->Name, name);
	}
	else NewNode->Name = 0;
	//znachenie
	len = strlen(value);
	if (len)
	{
		NewNode->Value = new char[len+1];
		strcpy(NewNode->Value, value);
	}
	else NewNode->Value = 0;
	//ostal'noe
	NewNode->Next = 0;

	//perviy parametr ili net
	if (m_NumberNodes == 0)
	{
		m_FirstNode = NewNode;
		m_LastNode  = NewNode;
	}
	else
	{
		m_LastNode->Next = NewNode;
		m_LastNode       = NewNode;
	}

	//+1 uzel
	m_NumberNodes++;
	return 1;
}

//nayti uzel po imeni
//name   - imya parametra
//return
//	Node - naydenniy uzel
//	0    - uzel ne nayden
NodeStrStr* MListStrStr::SearchNodeFromName(const char* name)
{
	NodeStrStr* Node = 0;

	Node = m_FirstNode;
	//cikl po vsem elementam - uzlam
	while (Node)
	{
		if (strcmp(Node->Name, name) == 0) return Node;
		Node = Node->Next;
	}
	return 0;
}

//nayti uzel po znacheniyu
//name   - imya parametra
//return
//	Node - naydenniy uzel
//	0    - uzel ne nayden
NodeStrStr* MListStrStr::SearchNodeFromValue(const char* value)
{
	NodeStrStr* Node = 0;

	Node = m_FirstNode;
	//cikl po vsem elementam - uzlam
	while (Node)
	{
		if (strcmp(Node->Value, value) == 0) return Node;
		Node = Node->Next;
	}
	return 0;
}
