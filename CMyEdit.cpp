// MyEdit.cpp
#include "pch.h" // or stdafx.h
#include "CMyEdit.h"
#include "SoftwareKeyboardDlg.h"

IMPLEMENT_DYNAMIC(CMyEdit, CEdit)

CMyEdit::CMyEdit()
{
}

CMyEdit::~CMyEdit()
{
}

BEGIN_MESSAGE_MAP(CMyEdit, CEdit)
    ON_WM_SETFOCUS()
END_MESSAGE_MAP()

// CMyEdit message handlers

void CMyEdit::OnSetFocus(CWnd* pOldWnd)
{
    CEdit::OnSetFocus(pOldWnd);

    // 1. 現在のテキストを保存
    GetWindowText(m_strOriginalText);

    // 2. ソフトウェアキーボードをモーダルで表示
    //    コンストラクタに自身(this)を渡し、どのエディットへの入力か伝える
    CSoftwareKeyboardDlg dlg(this);
    INT_PTR nResponse = dlg.DoModal();

    // 3. IDCANCELで閉じられた場合、テキストを元に戻す
    if (nResponse == IDCANCEL)
    {
        SetWindowText(m_strOriginalText);
    }
    
    // 4. キーボードが閉じたら、親ウィンドウにフォーカスを戻す
    //    これにより、エディットコントロールからフォーカスが外れる
    if (GetParent())
    {
        GetParent()->SetFocus();
    }
}