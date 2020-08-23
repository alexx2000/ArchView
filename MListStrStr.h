/////////////////////////////////////////////////////////////////////////////
//TListStrStr.h: interface for the MListStrStr class.
//////////////////////////////////////////////////////////////////////

#ifndef _MLIST_STRSTR_H
#define _MLIST_STRSTR_H

//element spiska
typedef struct _NodeStrStr
{
	char*        Name;		//imya parametra
	char*        Value;		//znachenie parametra
	_NodeStrStr* Next;		//ukazatel' na sleduyushiy element, konec 0
} NodeStrStr;

//klass dlya raboti so spiskom (stroka,stroka)
//v nem hranim imya wcx-plagina totalcommander i put' k plaginu
class MListStrStr
{
private:
	//peremennie
	NodeStrStr* m_FirstNode;	//perviy element spiska
	NodeStrStr* m_LastNode;		//posledniy element spiska
	int         m_NumberNodes;	//kolichestvo elementov - uzlov

	//udalenie spiska parametrov
	int DeleteListStrStr();

public:
	MListStrStr(): m_FirstNode(0), m_LastNode(0), m_NumberNodes(0) {;}
	virtual ~MListStrStr();

	int         AddNode            (const char*, const char*);	//dobavit' v spisok uzel
	NodeStrStr* GetHeadListStrStr  () {return m_FirstNode;}		//vernut' ukazatel' na nachalo spiska
	int         GetNumberNodes     () {return m_NumberNodes;}	//vernut' kolichestvo uzlov v spiske
	NodeStrStr* SearchNodeFromName (const char*);				//nayti uzel po imeni
	NodeStrStr* SearchNodeFromValue(const char*);				//nayti uzel po znacheniyu
};

#endif //_MLIST_STRSTR_H