/**
 * @file CView2.cpp
 * @brief CViewを派生したカスタムビュークラスの実装
 */
#include "pch.h"
#include "MFCApplication4.h"
#include "CView2.h"

// CView2

/**
 * @brief DYNCREATEマクロ
 * @details CView2クラスが動的生成可能であることをフレームワークに伝えます。
 */
IMPLEMENT_DYNCREATE(CView2, CView)

// BEGIN_MESSAGE_MAPブロック
// Windowsメッセージと、それを処理するクラスのメンバ関数（ハンドラ）を関連付けます。
BEGIN_MESSAGE_MAP(CView2, CView)
END_MESSAGE_MAP()


/**
 * @brief ビューの描画処理を行います (WM_PAINT)。
 * @details この実装では、初回描画時に子コントロールであるCCenterEditを動的に生成・配置します。
 * OnDrawは再描画のたびに呼び出されるため、ポインタがnullptrであるかを確認することで、
 * コントロールが一度だけ生成されるようにしています。
 * @param[in] pDC 描画に使用するデバイスコンテキストへのポインタ。
 */
void CView2::OnDraw(CDC *pDC)
{
    // m_editCustom1がまだ生成されていなければ、ここで生成する
    if (m_editCustom1 == nullptr)
    {
        m_editCustom1 = new CCenterEdit();
        CRect rect1(60, 60, 110, 110); // ビュー内の位置とサイズを指定
        m_editCustom1->Create(WS_CHILD | WS_VISIBLE, rect1, this, 1004);
        m_editCustom1->ShowWindow(TRUE);
        m_editCustom1->SetWindowTextW(_T("1234")); // 初期値を設定
        m_editCustom1->SetFont(GetFont());         // 親と同じフォントを設定
        m_editCustom1->SetLimitText(4);            // 入力文字数を4文字に制限
        m_editCustom1->SetWindowPos(nullptr, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
    }
    // m_editCustom2がまだ生成されていなければ、ここで生成する
    if (m_editCustom2 == nullptr)
    {
        m_editCustom2 = new CCenterEdit();
        CRect rect1(10, 10, 50, 50); // ビュー内の位置とサイズを指定
        m_editCustom2->Create(WS_CHILD | WS_VISIBLE, rect1, this, 1006);
        m_editCustom2->ShowWindow(TRUE);
        m_editCustom2->SetWindowTextW(_T("5678")); // 初期値を設定
        m_editCustom2->SetFont(GetFont());         // 親と同じフォントを設定
        m_editCustom2->SetLimitText(4);            // 入力文字数を4文字に制限
        m_editCustom2->SetWindowPos(nullptr, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
    }
}


/**
 * @brief ウィンドウが作成される直前にフレームワークから呼び出されます。
 * @details ウィンドウのスタイル(CREATESTRUCT)を変更するためにオーバーライドされます。
 * @param[in,out] cs ウィンドウの作成パラメータを保持するCREATESTRUCT構造体。
 * @return ウィンドウ作成を続行する場合はTRUE、中止する場合はFALSE。
 */
BOOL CView2::PreCreateWindow(CREATESTRUCT &cs)
{
    // 基底クラスの処理を先に呼び出すのが作法です
    if (!CView::PreCreateWindow(cs))
        return FALSE;

    // WS_CLIPSIBLINGS スタイルをビット演算で削除します。
    // このスタイルがあると、兄弟ウィンドウ(CView1)と重なる領域は描画されません。
    // このスタイルを削除することで、CView1の上にCView2が重なって表示される場合に、
    // CView1の描画がCView2によってクリッピング（切り抜かれ）されるのを防ぎます。
    cs.style &= ~WS_CLIPSIBLINGS;

    return TRUE;
}

// CView2 診断

#ifdef _DEBUG
/**
 * @brief オブジェクトの有効性を診断します (デバッグビルドのみ)。
 */
void CView2::AssertValid() const
{
    CView::AssertValid();
}

#ifndef _WIN32_WCE
/**
 * @brief オブジェクトの状態をダンプします (デバッグビルドのみ)。
 * @param[in,out] dc ダンプ先のデバイスコンテキスト。
 */
void CView2::Dump(CDumpContext &dc) const
{
    CView::Dump(dc);
}
#endif
#endif //_DEBUG

// CView2 メッセージ ハンドラー