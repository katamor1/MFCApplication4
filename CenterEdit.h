/**
 * @file CenterEdit.h
 * @brief 多機能カスタムエディットコントロールのクラス宣言
 * @details このファイルは、CEditを継承し、テキストの垂直・水平中央揃え、
 * フォーカス状態に応じた外観変更、ソフトウェアキーボードの起動機能などを
 * 持つ CCenterEdit クラスを宣言します。
 */
#pragma once

/// @brief 初回表示のタイミングでテキスト位置を最終調整するためのカスタムメッセージ
#define WM_APP_POST_INIT (WM_APP + 1)

/**
 * @class CCenterEdit
 * @brief テキストを中央揃えし、フォーカス状態で外観が変わるカスタムエディットコントロール
 * @details CEditを継承し、以下の機能を提供します。
 * - テキストを垂直方向および水平方向（右揃え）に中央揃えで表示します。
 * - フォーカスを持っているときと失ったときで、背景色と文字色が変わります。
 * - フォーカスを受け取ると、ソフトウェアキーボードをモーダルで表示します。
 * - コントロールのサイズが変更されても、テキストの中央揃えを維持します。
 */
class CCenterEdit : public CEdit
{
    DECLARE_DYNAMIC(CCenterEdit)

public:
    /**
     * @brief デフォルトコンストラクタ
     */
    explicit CCenterEdit();

    /**
     * @brief デストラクタ
     */
    virtual ~CCenterEdit() override;

    // オーバーライド
protected:
    /**
     * @brief ダイアログリソースからコントロールがサブクラス化される際に呼び出されます。
     * @details コントロールの初期スタイルを設定します。
     */
    virtual void PreSubclassWindow() override;

    /**
     * @brief キーボードメッセージをウィンドウにディスパッチされる前に横取りします。
     * @details Enterキーのデフォルト動作（改行など）を無効化するために使用します。
     * @param[in] pMsg メッセージ情報へのポインタ
     * @return メッセージを処理した場合はTRUE、デフォルト処理に任せる場合はFALSE。
     */
    virtual BOOL PreTranslateMessage(MSG* pMsg) override;

    // メンバ変数
protected:
    /// @brief フォーカス時の背景ブラシ
    CBrush m_brushFocus;
    /// @brief 非フォーカス時の背景ブラシ
    CBrush m_brushNoFocus;
    /// @brief フォーカス時の文字色
    COLORREF m_colorTextFocus;
    /// @brief 非フォーカス時の文字色
    COLORREF m_colorTextNoFocus;
    /// @brief 初回サイズ決定後に一度だけ初期化処理を行うためのフラグ
    bool m_bIsFirstTimeInit = true;
    /// @brief ソフトウェアキーボード表示前のテキスト（キャンセル時の復元用）
    CString m_strOriginalText;

    // 実装
protected:
    /**
     * @brief テキストの垂直位置を更新するヘルパー関数
     * @details コントロールの現在の高さとフォントサイズから、テキストが垂直方向に
     * 中央揃えになるように内部の描画矩形を調整します。
     */
    void UpdateTextPosition();

    // メッセージ マップ
protected:
    /**
     * @brief コントロールの描画色を設定するためのリフレクトメッセージハンドラ (WM_CTLCOLOR_REFLECT)
     * @details フォーカスの有無に応じて、背景ブラシと文字色を返します。
     * @param[in,out] pDC デバイスコンテキスト
     * @param[in] nCtlColor コントロールの種類 (この場合はCTLCOLOR_EDIT)
     * @return 背景描画に使用するブラシのハンドル
     */
    afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
    
    /**
     * @brief フォーカスを受け取った際のイベントハンドラ (WM_SETFOCUS)
     * @details ソフトウェアキーボードを起動します。
     * @param[in] pOldWnd フォーカスを失ったウィンドウへのポインタ
     */
    afx_msg void OnSetFocus(CWnd* pOldWnd);
    
    /**
     * @brief フォーカスを失った際のイベントハンドラ (WM_KILLFOCUS)
     * @details コントロールを再描画して外観を変更します。
     * @param[in] pNewWnd 新しくフォーカスを受け取るウィンドウへのポインタ
     */
    afx_msg void OnKillFocus(CWnd* pNewWnd);
    
    /**
     * @brief コントロールのサイズが変更された際のイベントハンドラ (WM_SIZE)
     * @param nType サイズ変更の種類
     * @param cx 新しい幅
     * @param cy 新しい高さ
     */
    afx_msg void OnSize(UINT nType, int cx, int cy);
    
    /**
     * @brief テキストが変更された際のイベントハンドラ (EN_CHANGE)
     */
    afx_msg void OnEnChange();
    
    /**
     * @brief カスタムメッセージ WM_APP_POST_INIT のハンドラ
     * @details 初回表示の適切なタイミングでテキスト位置の最終調整を行います。
     * @param wParam 未使用
     * @param lParam 未使用
     * @return 常に0
     */
    afx_msg LRESULT OnPostInit(WPARAM wParam, LPARAM lParam);

    /// @brief メッセージマップを宣言します。
    DECLARE_MESSAGE_MAP()
};