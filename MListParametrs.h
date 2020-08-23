/////////////////////////////////////////////////////////////////////////////
//MListParametrs.h: interface for the MListParametrs class.
//////////////////////////////////////////////////////////////////////

#ifndef _MLIST_PARAMETRS_H
#define _MLIST_PARAMETRS_H

//element spiska
typedef struct _Parametr
{
	char*      Name;		//imya parametra
	char*      Value;		//znachenie parametra
	unsigned   NumStr;		//kolichesvo strok v znachenii (Name ili Value)
	HWND       hWndName;	//okno dlya vivoda imeni
	HWND       hWndValue;	//okno dlya vivoda parametra
	_Parametr* Next;		//ukazatel' na sleduyushiy element, konec 0
} Parametr;

//klass dlya raboti so spiskom parametrov
//v nem hranim parametri arhiva i okna dlya ih vivoda
class MListParametrs  
{
private:
	//peremennie
	Parametr* m_FirstParametr;	//perviy element spiska
	Parametr* m_LastParametr;	//posledniy element spiska
	int       m_NumberParametrs;//kolichestvo elementov - parametrov

	//udalenie spiska parametrov
	int DeleteListParametrs();

public:
	MListParametrs(): m_FirstParametr(0), m_LastParametr(0), m_NumberParametrs(0) {;}
	virtual ~MListParametrs();

	int       AddParametr            (const char*, const char*, unsigned numstr = 1);//dobavit' v spisok parametr
	int       DeleteHwndListParametrs();							//udalit' vse okna dlya parametrov
	Parametr* GetHeadListParametrs   () {return m_FirstParametr;}	//vernut' ukazatel' na nachalo spiska
	int       GetNumberParametrs     () {return m_NumberParametrs;}	//vernut' kolichestvo parametrov v spiske
};

#endif //_MLIST_PARAMETRS_H
