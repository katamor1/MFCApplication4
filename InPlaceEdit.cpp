/**
 * @file InPlaceEdit.cpp
 * @brief CGridCtrlのセル内編集（インプレイス編集）に使われるCEdit派生クラスの実装
 */
#include "pch.h"
#include "InPlaceEdit.h"
#include "GridCtrl.h" // 親コントロールのヘッダをインクルード

/**
 * @brief CInPlaceEditクラスのコンストラクタ
 * @param pParent このエディットコントロールを所有する親のCGridCtrlへのポインタ
 * @param cell 編集対象のセルの位置（列、行）
 * @param sInitText 編集開始時の初期テキスト
 */
CInPlaceEdit::CInPlaceEdit(CGridCtrl *pParent, CPoint cell, CString sInitText)
    : m_pParentCtrl(pParent), m_cell(cell), m_sOriginalText(sInitText), m_bEsc(FALSE)
{
}

/**
 * @brief CInPlaceEditクラスのデストラクタ
 */
CInPlaceEdit::~CInPlaceEdit()
{
}

// BEGIN_MESSAGE_MAPブロック
// Windowsメッセージと、それを処理するクラスのメンバ関数（ハンドラ）を関連付けます。
BEGIN_MESSAGE_MAP(CInPlaceEdit, CEdit)
    ON_WM_KILLFOCUS()
END_MESSAGE_MAP()


/**
 * @brief キーボードメッセージをウィンドウにディスパッチされる前に横取りします。
 * @details Enterキー（確定）とEscキー（キャンセル）を捕捉して編集を終了させます。
 * @param pMsg メッセージ情報へのポインタ
 * @return メッセージを処理した場合はTRUE、デフォルト処理に任せる場合はFALSE。
 */
BOOL CInPlaceEdit::PreTranslateMessage(MSG *pMsg)
{
    if (pMsg->message == WM_KEYDOWN)
    {
        // EnterキーまたはEscキーが押された場合
        if (pMsg->wParam == VK_RETURN || pMsg->wParam == VK_ESCAPE)
        {
            // Escキーが押された場合は、キャンセルしたことを示すフラグを立てる
            if (pMsg->wParam == VK_ESCAPE)
            {
                m_bEsc = TRUE;
            }
            // フォーカスを親ウィンドウ(CGridCtrl)に戻すことで、OnKillFocusを能動的にトリガーする。
            // これにより、編集終了のロジックをOnKillFocusに集約できる。
            GetParent()->SetFocus();
            return TRUE; // メッセージはここで処理済みとする
        }
    }
    return CEdit::PreTranslateMessage(pMsg);
}

/**
 * @brief フォーカスを失った際のイベントハンドラ (WM_KILLFOCUS)
 * @details 編集を終了し、結果を親グリッドに通知します。
 * この関数は、ユーザーが他のコントロールをクリックした場合だけでなく、
 * PreTranslateMessageでEnter/Escが捕捉された場合にも呼び出されます。
 * @param[in] pNewWnd 新しくフォーカスを受け取るウィンドウへのポインタ
 */
void CInPlaceEdit::OnKillFocus(CWnd *pNewWnd)
{
    CEdit::OnKillFocus(pNewWnd);

    CString str;
    GetWindowText(str);

    // 親グリッドに渡す更新フラグを決定する。
    // m_bEscフラグがTRUE(Escキーが押された)場合は更新しない(FALSE)。
    // それ以外(Enterキー、フォーカス喪失)の場合は更新する(TRUE)。
    BOOL bUpdate = !m_bEsc;

    // 親グリッドのDestroyInPlaceEditを呼び出し、自身の破棄とデータの更新を依頼する。
    if (m_pParentCtrl)
    {
        m_pParentCtrl->DestroyInPlaceEdit(bUpdate);
    }
}