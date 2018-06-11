
// makeexportDlg.h : 头文件
//

#pragma once
#include "afxwin.h"

typedef enum
{
	OBJECT_TYPE_unknown,//求知
	OBJECT_TYPE_namespace,//命名空间
	OBJECT_TYPE_class,//类
	OBJECT_TYPE_classfun,//类成员函数
	OBJECT_TYPE_function,//函数
	OBJECT_TYPE_variant//变量
}OBJECT_TYPE;


typedef struct   objectitem
{

	OBJECT_TYPE type;
	CString	 name;
	XLIB::CXMap<CString,objectitem>	items[4];//子树,第一个为公开 ，第二个为publicitems,protecteditems,,privateitems
	/*
	XLIB::CXMap<CString,objectitem>	publicitems;//子树
	XLIB::CXMap<CString,objectitem>	protecteditems;//子树
	
	XLIB::CXMap<CString,objectitem>	privateitems;//子树
	*/
}OBJECTITEM,*LPOBJECTITEM;


// CmakeexportDlg 对话框
class CmakeexportDlg : public CDialog
{
// 构造
public:
	CmakeexportDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_MAKEEXPORT_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持

	XLIB::CXMap<int,EXPORTDATA> m_FunctionMap;
	
	BOOL LoadExport( LPCTSTR lpFilePath );
	CString ShowNormal();
	CString ShowAsDef();
	CString ShowAsSource();

	VOID ParserClass( LPCTSTR lpName );
	VOID ParserVariant( LPCTSTR lpName );
	VOID ParserFunction( LPCTSTR lpName );

	CString m_strTitle;

	OBJECTITEM	m_ObjectList;

	BOOL		m_bRemoveCCall; 
	VOID PrintObjects( LPOBJECTITEM item,CString& strText,LPCTSTR prefix = 0,int itemindex = 0 );
	LPOBJECTITEM ProbeForWriteObject( LPCTSTR name,LPOBJECTITEM parent = 0 ,int itemindex = 0);

	VOID		AddUnParser( LPCTSTR lpname );
// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CEdit m_WndDllPath;
	afx_msg void OnBnClickedOpenfile();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	CEdit m_wndOut;
	CButton m_wndCplusplus;
	CButton m_wndnormal;
	CButton m_wnddef;
	CButton m_wndsource;
	CButton m_wndRemovecCall;
	afx_msg void OnBnClickedButton1();
	CButton m_wndByOrdin;
};
