/**
 * @file CMyEdit.h
 * @brief ソフトウェアキーボードを起動するカスタムエディットコントロールのクラス宣言
 * @author C++/MFCコーディング (AI)
 * @date 2025/06/22
 * @details このファイルは、CEditを継承し、フォーカス取得時にソフトウェアキーボードを
 * 起動する機能を持つ CMyEdit クラスを宣言します。
 */
#pragma once

/**
 * @class CMyEdit
 * @brief ソフトウェアキーボード起動機能付きエディットコントロール
 * @details このエディットコントロールは、ユーザーがフォーカスを合わせると（クリックするなど）、
 * 自動的に CSoftwareKeyboardDlg をモーダルダイアログとして表示します。
 * これにより、物理キーボードがない環境でもテキスト入力が可能になります。
 * Escキーなどで入力をキャンセルした場合には、編集前のテキストに復元する機能を持ちます。
 */
class CMyEdit : public CEdit
{
    DECLARE_DYNAMIC(CMyEdit)

public:
    /**
     * @brief デフォルトコンストラクタ
     */
    CMyEdit();
    /**
     * @brief デストラクタ
     */
    virtual ~CMyEdit() override;

protected:
    /// @brief ソフトウェアキーボード表示前のテキストを保持するメンバ変数（キャンセル時の復元用）
    CString m_strOriginalText;

    /**
     * @brief フォーカスを受け取った際のイベントハンドラ (WM_SETFOCUS)
     * @details このハンドラ内でソフトウェアキーボードダイアログを起動します。
     * @param[in] pOldWnd フォーカスを失ったウィンドウへのポインタ
     */
    afx_msg void OnSetFocus(CWnd *pOldWnd);
    
    /// @brief メッセージマップを宣言します。
    DECLARE_MESSAGE_MAP()
};