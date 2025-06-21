// InPlaceEdit.cpp
#include "pch.h" // または stdafx.h
#include "InPlaceEdit.h"
#include "GridCtrl.h" // 親コントロールのヘッダをインクルード

CInPlaceEdit::CInPlaceEdit(CGridCtrl *pParent, CPoint cell, CString sInitText)
    : m_pParentCtrl(pParent), m_cell(cell), m_sOriginalText(sInitText), m_bEsc(FALSE)
{
}

CInPlaceEdit::~CInPlaceEdit()
{
}

BEGIN_MESSAGE_MAP(CInPlaceEdit, CEdit)
ON_WM_KILLFOCUS()
END_MESSAGE_MAP()

// EnterキーとEscキーを捕捉する
BOOL CInPlaceEdit::PreTranslateMessage(MSG *pMsg)
{
    if (pMsg->message == WM_KEYDOWN)
    {
        if (pMsg->wParam == VK_RETURN || pMsg->wParam == VK_ESCAPE)
        {
            if (pMsg->wParam == VK_ESCAPE)
            {
                m_bEsc = TRUE;
            }
            // フォーカスを親ウィンドウに戻すことでOnKillFocusをトリガーする
            GetParent()->SetFocus();
            return TRUE; // メッセージを処理したのでTRUEを返す
        }
    }
    return CEdit::PreTranslateMessage(pMsg);
}

// フォーカスを失ったときの処理
void CInPlaceEdit::OnKillFocus(CWnd *pNewWnd)
{
    CEdit::OnKillFocus(pNewWnd);

    CString str;
    GetWindowText(str);

    // bUpdateフラグ: Escが押されていたらFALSE(更新しない)、それ以外はTRUE(更新する)
    BOOL bUpdate = !m_bEsc;
    m_pParentCtrl->DestroyInPlaceEdit(bUpdate);
}