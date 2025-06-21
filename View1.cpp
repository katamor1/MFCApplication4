/**
 * @file View1.cpp
 * @brief CViewを派生したプライマリビューのクラス実装
 * @author C++/MFCコーディング (AI)
 * @date 2025/06/22
 */
#include "pch.h"
#include "MFCApplication4.h"
#include "View1.h"

// View1

/**
 * @brief DYNCREATEマクロ
 * @details CView1クラスが動的生成可能であることをフレームワークに伝えます。
 */
IMPLEMENT_DYNCREATE(CView1, CView)

// BEGIN_MESSAGE_MAPブロック
// Windowsメッセージと、それを処理するクラスのメンバ関数（ハンドラ）を関連付けます。
BEGIN_MESSAGE_MAP(CView1, CView)
    ON_WM_DESTROY()
END_MESSAGE_MAP()

/**
 * @brief ワーカースレッドのエントリポイントとなる静的メンバ関数。
 * @details 5秒待機した後、初回のみ親ウィンドウにCView2の表示を要求するメッセージを送信します。
 * スレッド停止イベントを監視しており、メインスレッドからの要求に応じて安全に終了します。
 * @param[in] pParam スレッドに渡されるパラメータ。この実装ではCView1のインスタンスへのポインタ(this)。
 * @return スレッドの終了コード。正常終了時は0、エラー時は1。
 */
UINT CView1::WorkerThreadProc(LPVOID pParam)
{
    // この関数は複数回呼び出される可能性があるため、static変数で初回実行を管理します。
    static bool firstTime = true;
    CView1* pView = static_cast<CView1*>(pParam);
    if (pView == nullptr) return 1; // パラメータが不正な場合はエラー

    while (true) // スレッドのメインループ
    {
        // --- 「条件A」を監視するループ ---
        // ここでは例として、5秒待機、または停止イベントがシグナル状態になるのを待ちます。
        DWORD dwResult = ::WaitForSingleObject(pView->m_eventStopThread.m_hObject, 5000);

        // スレッドが停止要求を受け取ったかチェック
        if (dwResult == WAIT_OBJECT_0)
        {
            // 停止イベントがシグナル状態になった -> スレッドを正常終了
            TRACE("Worker thread received stop signal. Exiting.\n");
            return 0;
        }

        // WAIT_TIMEOUT の場合 -> 条件Aが成立したとみなす
        if (firstTime)
        {
            TRACE("Condition A detected in worker thread. Notifying main frame.\n");

            // 親ウィンドウにView2の表示を要求するメッセージをポストします。
            // ワーカースレッドからUIオブジェクトを直接操作するのは危険なため、
            // PostMessageを使用してUIスレッドに非同期で処理を依頼するのが安全な方法です。
            if (pView->GetParent() && ::IsWindow(pView->GetParent()->GetSafeHwnd()))
            {
                ::PostMessage(pView->GetParent()->GetSafeHwnd(), WM_SHOW_VIEW2, 0, 0);
            }

            TRACE("Worker thread finished its job.\n");
            firstTime = false; // 処理は初回のみ行い、フラグを倒す
        }
        
        // ポーリング間隔
        Sleep(1000);
    }

    return 1; // 通常ここには到達しない
}


/**
 * @brief ウィンドウが破棄される際に呼び出されます (WM_DESTROY)。
 * @details ワーカースレッドが起動している場合、安全に停止処理を行います。
 */
void CView1::OnDestroy()
{
    // スレッドが作成されている場合、安全に停止させる
    if (m_pWorkerThread)
    {
        TRACE("View1 is being destroyed. Stopping worker thread...\n");

        // 1. ワーカースレッドに停止を通知するためにイベントをシグナル状態にする
        m_eventStopThread.SetEvent();

        // 2. スレッドが実際に終了するのを待つ (タイムアウトを5秒に設定)
        //    これにより、スレッドがリソースを解放し、完全に終了するのを保証します。
        ::WaitForSingleObject(m_pWorkerThread->m_hThread, 5000);

        TRACE("Worker thread stopped successfully.\n");
    }

    // 基底クラスのOnDestroyを呼び出す
    CView::OnDestroy();
}

/**
 * @brief ビューの描画処理を行います (WM_PAINT)。
 * @details ビューの背景を描画し、初回描画時に子コントロールであるCCenterEditを動的に生成・配置します。
 * @param[in] pDC 描画に使用するデバイスコンテキストへのポインタ。
 */
void CView1::OnDraw(CDC* pDC)
{
    CRect rectClient;
    GetClientRect(&rectClient);
    
    // 背景色で塗りつぶし
    CBrush brush;
    brush.CreateSolidBrush(RGB(0xDD, 0x88, 0x11));
    pDC->FillRect(rectClient, &brush);

    // m_editCustom1がまだ生成されていなければ、ここで生成する
    if (m_editCustom1 == nullptr)
    {
        m_editCustom1 = new CCenterEdit();
        CRect rect1(10, 10, 50, 50); // ビュー内の位置とサイズを指定
        m_editCustom1->Create(WS_CHILD | WS_VISIBLE, rect1, this, 1003);
        m_editCustom1->ShowWindow(TRUE);
        m_editCustom1->SetWindowTextW(_T("1234")); // 初期値を設定
        m_editCustom1->SetFont(GetFont()); // 親と同じフォントを設定
        m_editCustom1->SetLimitText(4); // 入力文字数を4文字に制限
        m_editCustom1->SetWindowPos(nullptr, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
    }
    
    // m_editCustom2がまだ生成されていなければ、ここで生成する
    if (m_editCustom2 == nullptr)
    {
        m_editCustom2 = new CCenterEdit();
        CRect rect1(60, 10, 110, 50); // ビュー内の位置とサイズを指定
        m_editCustom2->Create(WS_CHILD | WS_VISIBLE, rect1, this, 1005);
        m_editCustom2->ShowWindow(TRUE);
        m_editCustom2->SetWindowTextW(_T("5678")); // 初期値を設定
        m_editCustom2->SetFont(GetFont()); // 親と同じフォントを設定
        m_editCustom2->SetLimitText(4); // 入力文字数を4文字に制限
        m_editCustom2->SetWindowPos(nullptr, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
    }
}


/**
 * @brief ウィンドウが作成される直前にフレームワークから呼び出されます。
 * @details このビューの上に他のビューが重なることを想定し、描画スタイルを調整します。
 * @param[in,out] cs ウィンドウの作成パラメータを保持するCREATESTRUCT構造体。
 * @return ウィンドウ作成を続行する場合はTRUE、中止する場合はFALSE。
 */
BOOL CView1::PreCreateWindow(CREATESTRUCT& cs)
{
    // まず基底クラスの関数を呼び出すのが作法です
    if (!CView::PreCreateWindow(cs))
        return FALSE;

    // WS_CLIPSIBLINGS スタイルを削除します。
    // これにより、このビュー(CView1)の上に兄弟ウィンドウ(CView2)が重なった際、
    // CView1の描画領域がCView2によってクリッピング（切り抜かれ）されるのを防ぎます。
    cs.style &= ~WS_CLIPSIBLINGS;

    // WS_EX_TRANSPARENT 拡張スタイルを追加します。
    // これにより、このビューの下にある兄弟ウィンドウが先に描画されるようになり、
    // 透過的な効果が得られます。また、このビューへのマウスメッセージは下のウィンドウに透過します。
    cs.dwExStyle |= WS_EX_TRANSPARENT;
    return TRUE;
}

// View1 診断

#ifdef _DEBUG
/**
 * @brief オブジェクトの有効性を診断します (デバッグビルドのみ)。
 */
void CView1::AssertValid() const
{
    CView::AssertValid();
}

#ifndef _WIN32_WCE
/**
 * @brief オブジェクトの状態をダンプします (デバッグビルドのみ)。
 * @param[in,out] dc ダンプ先のデバイスコンテキスト。
 */
void CView1::Dump(CDumpContext& dc) const
{
    CView::Dump(dc);
}
#endif
#endif //_DEBUG