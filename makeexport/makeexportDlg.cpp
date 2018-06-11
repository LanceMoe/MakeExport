
// makeexportDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "makeexport.h"
#include "makeexportDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CmakeexportDlg 对话框




CmakeexportDlg::CmakeexportDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CmakeexportDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CmakeexportDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_DLLPATH, m_WndDllPath);
	DDX_Control(pDX, IDC_OUTLIST, m_wndOut);
	DDX_Control(pDX, IDC_CHECK1, m_wndCplusplus);
	DDX_Control(pDX, IDC_NORMAL, m_wndnormal);
	DDX_Control(pDX, IDC_DEF, m_wnddef);
	DDX_Control(pDX, IDC_SRC, m_wndsource);
	DDX_Control(pDX, IDC_CHECK2, m_wndRemovecCall);
	DDX_Control(pDX, IDC_CHECK3, m_wndByOrdin);
}

BEGIN_MESSAGE_MAP(CmakeexportDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_OPENFILE, &CmakeexportDlg::OnBnClickedOpenfile)
	ON_BN_CLICKED(IDOK, &CmakeexportDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CmakeexportDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BUTTON1, &CmakeexportDlg::OnBnClickedButton1)
END_MESSAGE_MAP()


// CmakeexportDlg 消息处理程序

BOOL CmakeexportDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码

	m_wndCplusplus.SetCheck(1);
	m_wndnormal.SetCheck(1);

	m_wndRemovecCall.SetCheck(1);

	m_ObjectList.type = OBJECT_TYPE_namespace;

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CmakeexportDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CmakeexportDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CmakeexportDlg::OnBnClickedOpenfile()
{
	CString strPath;
	CFileDialog dlg(TRUE,_T("选择dll"),NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,_T("dll文件(*.dll) |*.dll; |所有文件(*.*)|*.*|"), this);  
	if(dlg.DoModal() != IDOK)return;

	
	strPath = dlg.GetPathName();
	m_strTitle = dlg.GetFileTitle();

	m_WndDllPath.SetWindowText( strPath );
	
	LoadExport(strPath);
	OnBnClickedOk();
}

void CmakeexportDlg::OnBnClickedOk()
{
	CString strText;

	m_bRemoveCCall=m_wndRemovecCall.GetCheck();

	if( m_wnddef.GetCheck() )
	{
		strText = ShowAsDef();
	}else if( m_wndsource.GetCheck() )
	{
		strText = ShowAsSource();
	}else 
	{
		strText = ShowNormal();
	}

	//strText.Replace( _T("\r\n"),_T("\r\r\n"));
	m_wndOut.SetWindowText(strText);
}

void CmakeexportDlg::OnBnClickedCancel()
{
	OnCancel();
}

BOOL CmakeexportDlg::LoadExport( LPCTSTR lpFilePath )
{

	//HMODULE hModule = LoadLibrary(lpFilePath);
	HMODULE hModule = LoadLibraryEx(lpFilePath,0,DONT_RESOLVE_DLL_REFERENCES);
	if( 0 == hModule )return FALSE;
	
	EXPORTDATA exportdata,*lpData;
	

	IMAGE_DOS_HEADER * dosheader;
	IMAGE_OPTIONAL_HEADER * opthdr;
	PIMAGE_EXPORT_DIRECTORY exports;
	LPVOID lpRet = 0;
	const char* pFuncName = NULL;
	PULONG pAddressOfFunctions,pAddressOfNames;
	PUSHORT pAddressOfNameOrdinals;
	DWORD i;

	dosheader = (IMAGE_DOS_HEADER *)hModule;
	opthdr =(IMAGE_OPTIONAL_HEADER *) ((BYTE*)hModule+dosheader->e_lfanew+24);
	exports = (PIMAGE_EXPORT_DIRECTORY)((BYTE*)hModule+ opthdr->DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress);
 
	pAddressOfFunctions=(ULONG*)((BYTE*)hModule+exports->AddressOfFunctions); //函数列表
	pAddressOfNames=(ULONG*)((BYTE*)hModule+exports->AddressOfNames);		//名字列表
	pAddressOfNameOrdinals=(USHORT*)((BYTE*)hModule+exports->AddressOfNameOrdinals); //名字相对应的函数列表中的地址项index
 


	m_ObjectList.items[0].clear();
	m_ObjectList.items[1].clear();
	m_ObjectList.items[2].clear();
	m_ObjectList.items[3].clear();
	m_ObjectList.name = _T("");
	m_ObjectList.type = OBJECT_TYPE_namespace;
	m_FunctionMap.clear();

	for (i = 0; i < exports->NumberOfFunctions; i++) 
	{
		exportdata.index = i;
		exportdata.addr = pAddressOfFunctions[i];
		m_FunctionMap.Insert( i,exportdata );
 	}

	for (i = 0; i < exports->NumberOfNames; i++) 
	{
		pFuncName = (const char*)( (BYTE*)hModule + pAddressOfNames[i]);

		lpData = m_FunctionMap.Find( pAddressOfNameOrdinals[i] );
		if( lpData )lpData->name = pFuncName;

 	}

	FreeLibrary(hModule);
	return TRUE;
}

CString CmakeexportDlg::ShowNormal()
{
 	CString strValue;
	CString strText;
	char		outname[MAX_PATH];
	CStringA strFunName;
	CString	 str;

	BOOL bCPlusplus = m_wndCplusplus.GetCheck();

	for ( XLIB::CXMap<int,EXPORTDATA>::iterator item =m_FunctionMap.begin();item!=m_FunctionMap.end();item ++ )
	{
		if( bCPlusplus )
		{
			strFunName = item->second.name;
			ZeroMemory( outname,MAX_PATH );
			if(strFunName.GetLength())
			{
				UnDecorateSymbolName( strFunName,outname,MAX_PATH,UNDNAME_COMPLETE);
				str = outname;
			}else
			{
				str = _T("");
			}
			
		}else
		{
			str = item->second.name;
		}

		strValue.Format( _T("%d\t%s\t\t%08x\r\n"),item->second.index+1,(LPCTSTR)str,item->second.addr);
		strText += strValue;
	}



	return strText;
}

CString CmakeexportDlg::ShowAsDef()
{
 	CString strValue;
	CString strText;
	CString strName;
	int		index;

	BOOL	bOrdin = m_wndByOrdin.GetCheck();

	strText.Format( _T("LIBRARY %s\r\n"),(LPCTSTR)m_strTitle);
	strText += _T("EXPORTS\r\n");

	for ( XLIB::CXMap<int,EXPORTDATA>::iterator item =m_FunctionMap.begin();item!=m_FunctionMap.end();item ++ )
	{
		strName = item->second.name;
		index = item->second.index+1;
		if( strName.GetLength() )
		{
			if( bOrdin )
			{
				strValue.Format( _T("%s\t\t@%d\r\n"),(LPCTSTR)strName,index );
			}else
			{
				strValue.Format( _T("%s\r\n"),(LPCTSTR)strName );
			}
		}else
		{
			strValue.Format( _T("nonename_%d\t\t@%d NONAME PRIVATE\r\n"),index,index );

		}
		strText += strValue;
	}

	return strText;
}


VOID CmakeexportDlg::PrintObjects( LPOBJECTITEM item,CString& strText,LPCTSTR prefix,int itemindex )
{
	CString strValue;
	CString strPrefix;

	strPrefix = prefix;

	for( XLIB::CXMap<CString,OBJECTITEM>::iterator itembase=item->items[itemindex].begin();itembase!=item->items[itemindex].end();itembase++)
	{
		switch(itembase->second.type)
		{
		case OBJECT_TYPE_unknown:
			strValue.Format( _T("//noparser DLLIMPORT%s; \r\n"),(LPCTSTR)itembase->second.name );
			strText += strValue;
			continue;
		case OBJECT_TYPE_variant:
		case OBJECT_TYPE_function:
			strValue.Format( _T("%sDLLIMPORT %s;\r\n"),(LPCTSTR)strPrefix,(LPCTSTR)itembase->second.name );
			strText += strValue;
			continue;
		case OBJECT_TYPE_classfun:

			strValue.Format( _T("%s\t%s;\r\n"),(LPCTSTR)strPrefix,(LPCTSTR)itembase->second.name );
			strText += strValue;
			continue;

		case OBJECT_TYPE_namespace:
			strValue.Format( _T("%snamespace %s\r\n%s{\r\n"),(LPCTSTR)strPrefix,(LPCTSTR)itembase->second.name,(LPCTSTR)strPrefix );
			break;
		case OBJECT_TYPE_class:
			strValue.Format( _T("%sclass DLLIMPORT %s\r\n%s{\r\n"),(LPCTSTR)strPrefix,(LPCTSTR)itembase->second.name,(LPCTSTR)strPrefix );
			break;
		}

		strText += strValue;


		if( itembase->second.items[0].size() )
		{
			strPrefix = _T("\t");
			strPrefix += prefix;
			PrintObjects(&itembase->second,strText,strPrefix,0 );
		}
		if( itembase->second.items[1].size() )
		{
			strPrefix = prefix;
			strValue.Format( _T("%spublic:\r\n"),(LPCTSTR)strPrefix );
			strText += strValue;

			PrintObjects(&itembase->second,strText,strPrefix,1 );
		}
		if( itembase->second.items[2].size() )
		{
			strPrefix = prefix;
			strValue.Format( _T("%sprotected:\r\n"),(LPCTSTR)strPrefix );
			strText += strValue;

			PrintObjects(&itembase->second,strText,strPrefix,2 );
		}
		if( itembase->second.items[3].size() )
		{
			strPrefix = prefix;
			strValue.Format( _T("%sprivate:\r\n"),(LPCTSTR)strPrefix );
			strText += strValue;

			PrintObjects(&itembase->second,strText,strPrefix,3 );
		}

		strPrefix = prefix;
		strValue.Format( _T("%s};\r\n\r\n"),(LPCTSTR)strPrefix );
		strText += strValue;
	}
}

CString CmakeexportDlg::ShowAsSource()
{
	CString strName;
	char		outname[MAX_PATH];
	CStringA strFunName;
 
	CString strText;

	for ( XLIB::CXMap<int,EXPORTDATA>::iterator item =m_FunctionMap.begin();item!=m_FunctionMap.end();item ++ )
	{
		strFunName = item->second.name;

		ZeroMemory( outname,MAX_PATH );
		if( strFunName.GetLength() )
		{
			UnDecorateSymbolName( strFunName,outname,MAX_PATH,UNDNAME_COMPLETE);
			strName = outname;
		}else continue;

		if( strName.Find( _T("public:") ) == 0 || strName.Find( _T("protected:") ) == 0 || strName.Find( _T("private:") ) == 0)
		{
			ParserClass(strName);
		}else if( strName.Find( _T("(") ) == -1 )//变量
		{
			ParserVariant(strName);
		}else
		{
			ParserFunction(strName);
		}
	}

	strText.Format( _T("//头文件,由Make Export Header自动生成\r\n")
					_T("#pragma once\r\n\r\n")
					_T("#define DLLIMPORT __declspec(dllimport)\r\n\r\n")
					_T("#pragma comment(lib,\"%s.lib\")\r\n\r\n"), (LPCTSTR)m_strTitle );
	PrintObjects(&m_ObjectList,strText);

	return strText;
}

/*
namespace
{
 class 
   function
 variant
 function
}


*/

CString ReplaceOnce( CString str,LPCTSTR lpFind,LPCTSTR lpreplace )
{
	CString strRet;
	int pos;

	pos = str.Find( lpFind );
	if( -1 == pos )return str;

	strRet = str.Left( pos );
	strRet += lpreplace;

	strRet += str.Right( str.GetLength() - lstrlen(lpFind) - pos );

	return strRet;
}


VOID ReplaceTemplate( CString& str)
{
	str.Replace( _T("class std::basic_string<char,struct std::char_traits<char>,class std::allocator<char> >"),_T("std::string") );
	str.Replace( _T("class std::basic_string<wchar_t,struct std::char_traits<wchar_t>,class std::allocator<wchar_t> >"),_T("std::wstring") );

	str.Replace( _T("class ATL::CStringT<char,class ATL::StrTraitATL<char,class ATL::ChTraitsCRT<char> > >"),_T("CStringA") );
	str.Replace( _T("class ATL::CStringT<wchar_t,class ATL::StrTraitATL<wchar_t,class ATL::ChTraitsCRT<wchar_t> > >"),_T("CStringW") );
	
	str.Replace( _T("class ATL::CStringT<char,class StrTraitMFC<char,class ATL::ChTraitsCRT<char> > >"),_T("CStringA") );
	str.Replace( _T("class ATL::CStringT<wchar_t,class StrTraitMFC<wchar_t,class ATL::ChTraitsCRT<wchar_t> > >"),_T("CStringW") );
}

VOID CmakeexportDlg::ParserClass( LPCTSTR lpName )
{
/*
public: class XLIB::Cttt & __thiscall XLIB::Cttt::operator=(class XLIB::Cttt const &)	
1	public: __thiscall XLIB::Cttt::Cttt(void)		000110be
5	public: void __cdecl CTest::a(void)		00011195
6	protected: void __thiscall CTest::d(void)		0001101e
7	private: void __thiscall CTest::e(void)		0001100a
public: static void __cdecl CTest::f(void)		000110af

12	public: void __thiscall `public: void __thiscall XLIB::XX::XX::ff(void)'::`2'::XX::e(void)		00011136
13	public: void __thiscall `public: void __thiscall `public: void __thiscall XLIB::XX::XX::ff(void)'::`2'::XX::e(void)'::`4'::XX::e(void)		00011104

1	public: class `public: void __thiscall CXX::f(void)'::`2'::CXX & __thiscall `public: void __thiscall CXX::f(void)'::`2'::CXX::operator=(class `public: void __thiscall CXX::f(void)'::`2'::CXX const &)		00011046
2	public: class `public: void __thiscall `public: void __thiscall CXX::f(void)'::`2'::CXX::f2(void)'::`4'::CXX & __thiscall `public: void __thiscall `public: void __thiscall CXX::f(void)'::`2'::CXX::f2(void)'::`4'::CXX::operator=(class `public: void __thiscall		000110f0
3	public: class CXX & __thiscall CXX::operator=(class CXX const &)		000110a0
4	public: void __thiscall `public: void __thiscall CXX::f(void)'::`2'::CXX::f2(void)		00011023
5	public: void __thiscall `public: void __thiscall `public: void __thiscall CXX::f(void)'::`2'::CXX::f2(void)'::`4'::CXX::f3(void)		00011159
6	public: void __thiscall CXX::f(void)		0001100a
*/

	CString strName,strArgu;
	int		pos;
	CString strObject;
	CString strNamespace;
	CString strType;
	LPOBJECTITEM item = 0;
	strName = lpName;
	int		protecttype = 0;

	//public: void __thiscall CEcdsa::`default constructor closure'(void)
	//public: __thiscall CTXBSTR::operator wchar_t *(void)const 
//	if( strName == _T("public: void __thiscall CEcdsa::`default constructor closure'(void)"))__asm int 3
	if( strName.Find( _T("public:") )== 0 )
	{
		protecttype = 1;
		strName.Replace(_T("public:"),_T(""));

	}else if( strName.Find( _T("protected:") )== 0 )
	{
		protecttype = 2;
		strName.Replace(_T("protected:"),_T(""));

	}else if( strName.Find( _T("private:") )== 0 )
	{
		protecttype = 3;
		strName.Replace(_T("private:"),_T(""));

	}
	strName.Trim();

	pos = strName.ReverseFind( _T('(') );
	if( -1== pos )
	{
		AddUnParser(lpName);
		return;
	}

	strArgu = strName.Right( strName.GetLength()-pos );
	strName = strName.Left(pos);

	CString strTemp;

	pos = strName.Find( _T("::operator") );
	if( -1 == pos )
	{
		pos = strName.ReverseFind( _T('`') );
		if( -1 == pos )
		{
			pos = strName.ReverseFind( _T(' ') );
		}else
		{
				strTemp = strName.Left(pos);
			pos = strTemp.ReverseFind( _T(' ') );
		}
		
	}else
	{
		
		strTemp = strName.Left(pos);
		pos = strTemp.ReverseFind( _T(' ') );
	}
	if( -1== pos )
	{
		AddUnParser(lpName);
		return;
	}

	strObject = strName.Right( strName.GetLength() - pos-1 );
	strType = strName.Left(pos);


	pos = strObject.Find( _T("::") );
	while( -1 != pos )
	{
		pos+=2;
		strNamespace = strObject.Left( pos );
		strObject = strObject.Right( strObject.GetLength() - pos );
		//strType.Replace( strNamespace,_T("") );
		//strArgu.Replace( strNamespace,_T("") );
		strType = ReplaceOnce( strType,strNamespace,_T(""));
		strArgu = ReplaceOnce( strArgu,strNamespace,_T(""));
		strNamespace = strNamespace.Left(strNamespace.GetLength()-2);
		item = ProbeForWriteObject( strNamespace,item );
		pos = strObject.Find( _T("::") );

	}

	if( 0 == item )
	{
		AddUnParser(lpName);
		return;
	}

	strType.Replace( _T("__thiscall"),_T(""));

	if( m_bRemoveCCall )strType.Replace( _T("__cdecl"),_T(""));

	strType.Trim();
	strObject.Trim();
	strArgu.Trim();
	if( strType.GetLength() )
	{
		strName.Format( _T("%s %s%s"),(LPCTSTR)strType,(LPCTSTR)strObject,(LPCTSTR)strArgu);
	}else
	{
		strName.Format( _T("%s%s"),(LPCTSTR)strObject,(LPCTSTR)strArgu);
	}

	//class CXX &	operator=(class CXX const &);去掉默认的等号

	CString DefaultFun;

	DefaultFun.Format( _T("class %s &\toperator=(class %s const &)"),(LPCTSTR)item->name,(LPCTSTR)item->name );
	if( DefaultFun == strName)return;//去掉默认的等号

	DefaultFun.Format( _T("%s(class %s const &)"),(LPCTSTR)item->name,(LPCTSTR)item->name );
	if( DefaultFun == strName)
	{
		strName.Format( _T("%s(void)"),(LPCTSTR)item->name );
	}

	//DefaultFun.Format( _T("%s(void)"),(LPCTSTR)item->name );
	//if( DefaultFun == strName)return;//去掉默认的析构数

	if( -1  != strName.Find( _T('`') ) )
	{
		DefaultFun = _T("//");
		DefaultFun += strName;
		strName = DefaultFun;
	}


	ReplaceTemplate(strName);



	item->type = OBJECT_TYPE_class;
	item = ProbeForWriteObject( strName,item,protecttype);

	item->type = OBJECT_TYPE_classfun;
}

VOID CmakeexportDlg::ParserVariant( LPCTSTR lpName )
{
	CString strName;
	int		pos;
	CString strObject;
	CString strNamespace;
	CString strType;
	LPOBJECTITEM item = 0;
	strName = lpName;

	pos = strName.ReverseFind( _T(' ') );
	if( -1== pos )
	{
		AddUnParser(lpName);
		return;
	}

	strObject = strName.Right( strName.GetLength() - pos-1 );
	strType = strName.Left(pos);

	pos = strObject.Find( _T("::") );
	while( -1 != pos )
	{
		pos+=2;
		strNamespace = strObject.Left( pos );
		strObject = strObject.Right( strObject.GetLength() - pos );
		strType.Replace( strNamespace,_T("") );
		strNamespace = strNamespace.Left(strNamespace.GetLength()-2);
		item = ProbeForWriteObject( strNamespace,item );
		pos = strObject.Find( _T("::") );
	}


	strName.Format( _T("%s\t%s"),(LPCTSTR)strType,(LPCTSTR)strObject);
	if( _T("const\t`vftable'") == strName )return;//虚函数表

	if( -1  != strName.Find( _T('`') ) )
	{
		CString temp = _T("//");
		temp += strName;
		strName = temp;
	}

	ReplaceTemplate(strName);

	item = ProbeForWriteObject( strName,item );
	item->type = OBJECT_TYPE_variant;
	//int XLIB::nttt
	//class XLIB::Cttt XLIB::aa		0001813c

}



VOID CmakeexportDlg::ParserFunction( LPCTSTR lpName )
{
	CString strName;
	CString strArgu;
	int		pos;
	CString strObject;
	CString strNamespace;
	CString strType;
	LPOBJECTITEM item = 0;
	strName = lpName;

	pos = strName.ReverseFind( _T('(') );
	if( -1== pos )
	{
		AddUnParser(lpName);
		return;
	}

	strArgu = strName.Right( strName.GetLength()-pos );
	strName = strName.Left(pos);

	pos = strName.ReverseFind( _T(' ') );
	if( -1== pos )
	{
		AddUnParser(lpName);
		return;
	}

	strObject = strName.Right( strName.GetLength() - pos-1 );
	strType = strName.Left(pos);


	pos = strObject.Find( _T("::") );
	while( -1 != pos )
	{
		pos+=2;
		strNamespace = strObject.Left( pos );
		strObject = strObject.Right( strObject.GetLength() - pos );
		//strType.Replace( strNamespace,_T("") );
		//strArgu.Replace( strNamespace,_T("") );
		strType = ReplaceOnce(strType,strNamespace,_T("") );
		strArgu = ReplaceOnce(strArgu,strNamespace,_T("") );
		strNamespace = strNamespace.Left(strNamespace.GetLength()-2);
		item = ProbeForWriteObject( strNamespace,item );
		pos = strObject.Find( _T("::") );

	}

	if( m_bRemoveCCall )strType.Replace( _T("__cdecl"),_T(""));

	strName.Format( _T("%s%s%s"),(LPCTSTR)strType,(LPCTSTR)strObject,(LPCTSTR)strArgu);

	if( -1  != strName.Find( _T('`') ) )
	{
		CString temp = _T("//");
		temp += strName;
		strName = temp;
	}

	ReplaceTemplate(strName);

	item = ProbeForWriteObject( strName,item );
	item->type = OBJECT_TYPE_function;
}

VOID CmakeexportDlg::AddUnParser( LPCTSTR lpname )
{
	LPOBJECTITEM item = 0;
	CString strTemp;

	strTemp = _T(" ");
	strTemp += lpname;
	item = ProbeForWriteObject( strTemp,item );
	item->type = OBJECT_TYPE_unknown;
}

LPOBJECTITEM CmakeexportDlg::ProbeForWriteObject( LPCTSTR name,LPOBJECTITEM parent,int itemindex  )
{
	LPOBJECTITEM lpObject;

	if( 0 == name || lstrlen(name) == 0 )return &m_ObjectList;
	if( 0 == parent )parent = &m_ObjectList;

	lpObject = parent->items[itemindex].Find( name );

	if( 0 == lpObject )
	{
		OBJECTITEM item;
		item.name = name;
		item.type = OBJECT_TYPE_namespace;
		parent->items[itemindex].Insert( name,item );
	}

	lpObject = parent->items[itemindex].Find( name );
	return lpObject;
}

void CmakeexportDlg::OnBnClickedButton1()
{
	CStringA strText;
	CString strFile;
	FILE*	pf;
	
	strFile.Format( _T("%s.def"),(LPCTSTR)m_strTitle );

	strText = ShowAsDef();

	pf = _tfopen( strFile,_T("w+b") );

	if( 0 == pf )
	{

		AfxMessageBox( _T("创建文件失败"));
		return;
	}
	fwrite( (const char*)strText,strText.GetLength(),1,pf );

	fclose(pf);

	
	strFile.Format( _T("%s.h"),(LPCTSTR)m_strTitle );

	strText = ShowAsSource();

	pf = _tfopen( strFile,_T("w+b") );

	if( 0 == pf )
	{

		AfxMessageBox( _T("创建文件失败"));
		return;
	}
	fwrite( (const char*)strText,strText.GetLength(),1,pf );

	fclose(pf);
	

	strFile.Format(_T("link /LIB /DEF:%s.def /OUT:%s.lib"),(LPCTSTR)m_strTitle,(LPCTSTR)m_strTitle);
	strText = strFile;

	WinExec(strText,SW_SHOW);
}


