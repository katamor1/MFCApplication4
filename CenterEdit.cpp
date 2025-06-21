/**
 * @file CenterEdit.cpp
 * @brief 多機能カスタムエディットコントロールのクラス実装
 */
#include "pch.h"
#include "CenterEdit.h"
#include "SoftwareKeyboardDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CCenterEdit

// CCenterEditクラスが動的生成可能であることをフレームワークに伝えます。
IMPLEMENT_DYNAMIC(CCenterEdit, CEdit)

/**
 * @brief CCenterEditクラスのコンストラクタ
 * @details フォーカス時と非フォーカス時の背景ブラシ、文字色を初期化します。
 */
CCenterEdit::CCenterEdit()
{
    // ブラシと色の初期化
    m_brushFocus.CreateSolidBrush(RGB(255, 255, 0));   // 黄色
    m_brushNoFocus.CreateSolidBrush(RGB(0, 255, 255)); // シアン色
    m_colorTextFocus = RGB(255, 0, 0);                 // 赤色
    m_colorTextNoFocus = RGB(0, 0, 0);                 // 黒色
}

/**
 * @brief CCenterEditクラスのデストラクタ
 * @details CBrushオブジェクトはメンバ変数であるため、自動的に破棄されます。
 */
CCenterEdit::~CCenterEdit()
{
}

// BEGIN_MESSAGE_MAPブロック
// Windowsメッセージと、それを処理するクラスのメンバ関数（ハンドラ）を関連付けます。
BEGIN_MESSAGE_MAP(CCenterEdit, CEdit)
    ON_WM_CTLCOLOR_REFLECT()
    ON_WM_SETFOCUS()
    ON_WM_KILLFOCUS()
    ON_WM_SIZE()
    ON_CONTROL_REFLECT(EN_CHANGE, &CCenterEdit::OnEnChange)
    ON_MESSAGE(WM_APP_POST_INIT, &CCenterEdit::OnPostInit)
END_MESSAGE_MAP()

// CCenterEdit メッセージ ハンドラー

/**
 * @brief ダイアログリソースからコントロールがサブクラス化される際に呼び出されます。
 * @details コントロールの初期スタイルを設定します。
 */
void CCenterEdit::PreSubclassWindow()
{
    CEdit::PreSubclassWindow();
    // テキストを右揃えにし、複数行を扱えるようにスタイルを変更します。
    ModifyStyle(0, ES_RIGHT | ES_MULTILINE, 0);
    // 入力文字数を4文字に制限します。
    SetLimitText(4);
}

/**
 * @brief キーボードメッセージをウィンドウにディスパッチされる前に横取りします。
 * @details Enterキーが押された場合に、親ウィンドウにフォーカスを移すことで、
 * エディットコントロール内での改行を防ぎ、編集の終了とみなします。
 * @param[in] pMsg メッセージ情報へのポインタ
 * @return メッセージを処理した場合はTRUE、デフォルト処理に任せる場合はFALSE。
 */
BOOL CCenterEdit::PreTranslateMessage(MSG* pMsg)
{
    if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN)
    {
        // 親ウィンドウにフォーカスを移します。
        GetParent()->SetFocus();
        return TRUE; // Enterキーのメッセージはここで処理完了とする
    }
    return CEdit::PreTranslateMessage(pMsg);
}

/**
 * @brief フォーカスを受け取った際のイベントハンドラ (WM_SETFOCUS)
 * @details ソフトウェアキーボードをモーダルで起動します。
 * @param[in] pOldWnd フォーカスを失ったウィンドウへのポインタ
 */
void CCenterEdit::OnSetFocus(CWnd* pOldWnd)
{
    CEdit::OnSetFocus(pOldWnd);
    // 編集前のテキストをバックアップ
    GetWindowText(m_strOriginalText);

    // ソフトウェアキーボードダイアログをモーダルで表示
    CSoftwareKeyboardDlg dlg(this);
    INT_PTR nResponse = dlg.DoModal();

    // キャンセルされた場合はテキストを元に戻す
    if (nResponse == IDCANCEL)
    {
        SetWindowText(m_strOriginalText);
    }
    // キーボード終了後、フォーカスを親に戻す
    if (GetParent())
    {
        GetParent()->SetFocus();
    }
    // フォーカスが外れたので、外観を更新するために再描画を要求
    Invalidate();
}

/**
 * @brief フォーカスを失った際のイベントハンドラ (WM_KILLFOCUS)
 * @details コントロールを再描画して、非フォーカス時の外観（背景色、文字色）に更新します。
 * @param[in] pNewWnd 新しくフォーカスを受け取るウィンドウへのポインタ
 */
void CCenterEdit::OnKillFocus(CWnd* pNewWnd)
{
    CEdit::OnKillFocus(pNewWnd);
    // 再描画を要求し、OnCtlColorが呼び出されるようにする
    Invalidate();
}

/**
 * @brief テキストが変更された際のイベントハンドラ (EN_CHANGE)
 * @details テキストが変更されるたびに、垂直中央揃えを再計算します。
 */
void CCenterEdit::OnEnChange()
{
    UpdateTextPosition();
}


/**
 * @brief テキストの垂直位置を更新するヘルパー関数
 * @details コントロールの現在の高さとフォントサイズから、テキストが垂直方向に
 * 中央揃えになるように内部の描画矩形を調整します。
 */
void CCenterEdit::UpdateTextPosition()
{
    CRect rectClient;
    GetClientRect(&rectClient);

    if (rectClient.IsRectEmpty())
    {
        return;
    }

    // デバイスコンテキストを取得してフォントの高さを測定
    CDC* pDC = GetDC();
    if (pDC)
    {
        CFont* pFont = GetFont();
        CFont* pOldFont = pDC->SelectObject(pFont);
        TEXTMETRIC tm;
        pDC->GetTextMetrics(&tm);
        pDC->SelectObject(pOldFont);
        ReleaseDC(pDC);

        // 上マージンを計算して、テキストが垂直方向の中央に来るようにする
        int topMargin = (rectClient.Height() - tm.tmHeight) / 2;
        if (topMargin < 0)
        {
            topMargin = 0;
        }

        // 新しい描画矩形を設定
        CRect rectFormat = rectClient;
        rectFormat.top = topMargin;
        
        SetRect(&rectFormat);
    }
}

/**
 * @brief コントロールのサイズが変更された際のイベントハンドラ (WM_SIZE)
 * @details サイズが変更されるたびにテキストの中央揃えを再計算します。
 * また、初回サイズ決定時に最終的な初期化処理をポストします。
 * @param nType サイズ変更の種類
 * @param cx 新しい幅
 * @param cy 新しい高さ
 */
void CCenterEdit::OnSize(UINT nType, int cx, int cy)
{
    CEdit::OnSize(nType, cx, cy);
    // 初めてウィンドウサイズが確定したタイミングを捉える
    if (m_bIsFirstTimeInit && cx > 0 && cy > 0)
    {
        m_bIsFirstTimeInit = false;
        // PostMessageを使い、現在のメッセージ処理が終わった後に初期化を実行させる
        PostMessage(WM_APP_POST_INIT, 0, 0);
    }
    // サイズ変更のたびにテキスト位置を更新
    UpdateTextPosition();
    Invalidate();
}

/**
 * @brief カスタムメッセージ WM_APP_POST_INIT のハンドラ
 * @details OnSizeからポストされたメッセージを受け、初回表示の適切なタイミングで
 * テキスト位置の最終調整を行います。
 * @param wParam 未使用
 * @param lParam 未使用
 * @return 常に0
 */
LRESULT CCenterEdit::OnPostInit(WPARAM wParam, LPARAM lParam)
{
    UpdateTextPosition();
    Invalidate();
    return 0;
}

/**
 * @brief コントロールの描画色を設定するためのリフレクトメッセージハンドラ (WM_CTLCOLOR_REFLECT)
 * @details フォーカスの有無に応じて、背景ブラシと文字色を返します。
 * @param[in,out] pDC デバイスコンテキスト
 * @param[in] nCtlColor コントロールの種類 (この場合はCTLCOLOR_EDIT)
 * @return 背景描画に使用するブラシのハンドル
 */
HBRUSH CCenterEdit::CtlColor(CDC* pDC, UINT nCtlColor)
{
    // 現在フォーカスを持っているウィンドウが自身であるかを確認
    CWnd* pFocusWnd = GetFocus();
    bool bHasFocus = (pFocusWnd && pFocusWnd->GetSafeHwnd() == GetSafeHwnd());

    // フォーカスがある場合
    if (bHasFocus)
    {
        // フォーカス用の背景色と文字色を設定
        LOGBRUSH LogBrush;
        m_brushFocus.GetLogBrush(&LogBrush);
        pDC->SetBkColor(LogBrush.lbColor);
        pDC->SetTextColor(m_colorTextFocus);
        return (HBRUSH)m_brushFocus.GetSafeHandle();
    }
    // フォーカスがない場合
    else
    {
        // 非フォーカス用の背景色と文字色を設定
        LOGBRUSH LogBrush;
        m_brushNoFocus.GetLogBrush(&LogBrush);
        pDC->SetBkColor(LogBrush.lbColor);
        pDC->SetTextColor(m_colorTextNoFocus);
        return (HBRUSH)m_brushNoFocus.GetSafeHandle();
    }
}