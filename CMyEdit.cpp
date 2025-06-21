/**
 * @file CMyEdit.cpp
 * @brief ソフトウェアキーボードを起動するカスタムエディットコントロールのクラス実装
 * @author C++/MFCコーディング (AI)
 * @date 2025/06/22
 */
#include "pch.h"
#include "CMyEdit.h"
#include "SoftwareKeyboardDlg.h" // ソフトウェアキーボードダイアログのヘッダをインクルード

// CMyEditクラスが動的生成可能であることをフレームワークに伝えます。
IMPLEMENT_DYNAMIC(CMyEdit, CEdit)

/**
 * @brief CMyEditクラスのコンストラクタ
 */
CMyEdit::CMyEdit()
{
}

/**
 * @brief CMyEditクラスのデストラクタ
 */
CMyEdit::~CMyEdit()
{
}

// BEGIN_MESSAGE_MAPブロック
// Windowsメッセージと、それを処理するクラスのメンバ関数（ハンドラ）を関連付けます。
BEGIN_MESSAGE_MAP(CMyEdit, CEdit)
    ON_WM_SETFOCUS()
END_MESSAGE_MAP()


/**
 * @brief フォーカスを受け取った際のイベントハンドラ (WM_SETFOCUS)
 * @details このコントロールがフォーカスを得たタイミングで、ソフトウェアキーボードを
 * モーダルダイアログとして表示します。
 * @param[in] pOldWnd フォーカスを失ったウィンドウへのポインタ
 */
void CMyEdit::OnSetFocus(CWnd *pOldWnd)
{
    // まず、基底クラスのOnSetFocusを呼び出します。
    CEdit::OnSetFocus(pOldWnd);

    // 1. 編集前の現在のテキストをバックアップとして保存します。
    GetWindowText(m_strOriginalText);

    // 2. ソフトウェアキーボードをモーダルダイアログとして表示します。
    //    コンストラクタに自身(this)を渡し、どのエディットコントロールへの入力かを伝えます。
    CSoftwareKeyboardDlg dlg(this);
    INT_PTR nResponse = dlg.DoModal();

    // 3. キーボードが「キャンセル」で閉じられた場合 (Escキーや閉じるボタン)、
    //    テキストをバックアップしておいた元の内容に戻します。
    if (nResponse == IDCANCEL)
    {
        SetWindowText(m_strOriginalText);
    }

    // 4. キーボードが閉じたら、フォーカスを親ウィンドウに戻します。
    //    これにより、エディットコントロールからフォーカスが外れ、
    //    再度クリックされるまでキーボードが表示されないようにします。
    if (GetParent())
    {
        GetParent()->SetFocus();
    }
}