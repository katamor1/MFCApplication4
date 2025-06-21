/**
 * @file InPlaceEdit.h
 * @brief CGridCtrlのセル内編集（インプレイス編集）に使われるCEdit派生クラスの宣言
 * @details このファイルは、CGridCtrlのセル上で直接テキスト編集を行うための
 * CEditから派生した CInPlaceEdit クラスを宣言します。
 */
#pragma once

// CGridCtrlクラスの前方宣言
// InPlaceEdit.h と GridCtrl.h の間で相互にインクルードし合う
// 循環参照を避けるために、ここではヘッダファイルをインクルードせず、クラス名のみを宣言します。
class CGridCtrl;

/**
 * @class CInPlaceEdit
 * @brief インプレイス編集用のエディットコントロール
 * @details CGridCtrlのセルが編集モードに入った際に動的に生成されるCEdit派生クラスです。
 * Enterキー、Escキー、またはフォーカス喪失を検知して、編集の確定・キャンセルを
 * 親であるCGridCtrlに通知し、自身を破棄します。
 */
class CInPlaceEdit : public CEdit
{
public:
    /**
     * @brief コンストラクタ
     * @param[in] pParent このエディットコントロールを所有する親のCGridCtrlへのポインタ
     * @param[in] cell 編集対象のセルの位置（列、行）
     * @param[in] sInitText 編集開始時の初期テキスト
     */
    CInPlaceEdit(CGridCtrl* pParent, CPoint cell, CString sInitText);

    /**
     * @brief デストラクタ
     */
    virtual ~CInPlaceEdit() override;
    
protected:
    /// @brief 親コントロール(CGridCtrl)へのポインタ
    CGridCtrl* m_pParentCtrl;
    /// @brief 編集中のセルの位置 (x:列, y:行)
    CPoint m_cell;
    /// @brief 編集開始前のテキスト（Escキーでの復元用）
    CString m_sOriginalText;
    /// @brief Escキーが押されたかどうかを記録するフラグ (TRUE: Esc押下, FALSE: それ以外)
    BOOL m_bEsc;

protected:
    /**
     * @brief キーボードメッセージをウィンドウにディスパッチされる前に横取りします。
     * @details EnterキーとEscキーを検知して、編集を終了させるために使用します。
     * @param[in] pMsg メッセージ情報へのポインタ
     * @return メッセージを処理した場合はTRUE、デフォルト処理に任せる場合はFALSE。
     */
    virtual BOOL PreTranslateMessage(MSG* pMsg) override;
    
    /**
     * @brief フォーカスを失った際のイベントハンドラ (WM_KILLFOCUS)
     * @details 編集を終了し、結果を親グリッドに通知します。
     * Enter/Escキーが押された際にも、このイベントが間接的に呼び出されます。
     * @param[in] pNewWnd 新しくフォーカスを受け取るウィンドウへのポインタ
     */
    afx_msg void OnKillFocus(CWnd* pNewWnd);

    /// @brief メッセージマップを宣言します。
    DECLARE_MESSAGE_MAP()
};