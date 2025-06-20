// InPlaceEdit.h
#pragma once

class CGridCtrl; // 前方宣言

// CInPlaceEdit クラス
class CInPlaceEdit : public CEdit
{
public:
    CInPlaceEdit(CGridCtrl* pParent, CPoint cell, CString sInitText);
    virtual ~CInPlaceEdit();

protected:
    // 親コントロールのポインタ
    CGridCtrl* m_pParentCtrl;
    // 編集中のセル位置
    CPoint m_cell;
    // 編集開始前のテキスト（Escキーでの復元用）
    CString m_sOriginalText;
    // Escキーが押されたかのフラグ
    BOOL m_bEsc;

    // メッセージハンドラ
    virtual BOOL PreTranslateMessage(MSG* pMsg);
    afx_msg void OnKillFocus(CWnd* pNewWnd);

    DECLARE_MESSAGE_MAP()
};