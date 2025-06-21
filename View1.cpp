// View1.cpp : 実装ファイル
//

#include "pch.h"
#include "MFCApplication4.h"
#include "View1.h"

// View1
IMPLEMENT_DYNCREATE(CView1, CView)

BEGIN_MESSAGE_MAP(CView1, CView)
ON_WM_DESTROY()
END_MESSAGE_MAP()

// ワーカースレッド関数
UINT CView1::WorkerThreadProc(LPVOID pParam)
{
	static bool firstTime = true;
	CView1 *pView = static_cast<CView1 *>(pParam);
	while (pView)
	{
		// --- 「条件A」を監視するループ ---
		// ここでは例として、5秒待機した後に条件成立とみなす
		DWORD dwResult = ::WaitForSingleObject(pView->m_eventStopThread.m_hObject, 5000);

		// スレッドが停止要求を受け取ったか、タイムアウトしたかをチェック
		if (dwResult == WAIT_OBJECT_0)
		{
			// 停止イベントがシグナル状態になった -> スレッド終了
			TRACE("Worker thread received stop signal. Exiting.\n");
			return 0;
		}

		if (firstTime)
		{
			// WAIT_TIMEOUT の場合 -> 条件Aが成立したとみなす
			TRACE("Condition A detected in worker thread. Notifying main frame.\n");

			// メインフレームにView2の表示を要求するメッセージをポスト
			// GetParentFrame() はUIスレッドのオブジェクトなので、ここから直接メンバ関数を呼び出してはならない
			// PostMessageで非同期に通知するのが安全な方法
			::PostMessage(pView->GetParent()->GetSafeHwnd(), WM_SHOW_VIEW2, 0, 0);

			TRACE("Worker thread finished its job. Exiting.\n");
			firstTime = false; // 初回のみ処理を行う
		}
		Sleep(1000);
	}

	return 1; // エラー
}

void CView1::OnDestroy()
{
	// スレッドが作成されている場合、安全に停止させる
	if (m_pWorkerThread)
	{
		TRACE("View1 is being destroyed. Stopping worker thread...\n");

		// 1. ワーカースレッドに停止を通知する
		m_eventStopThread.SetEvent();

		// 2. スレッドが終了するのを待つ (タイムアウトを5秒に設定)
		//    これにより、スレッドがリソースを解放するのを待つことができる
		::WaitForSingleObject(m_pWorkerThread->m_hThread, 5000);

		TRACE("Worker thread stopped successfully.\n");
	}

	CView::OnDestroy();
}

void CView1::OnDraw(CDC *pDC)
{
	CRect rectClient;
	GetClientRect(&rectClient);
	pDC->SetBkColor(RGB(0xDD, 0x88, 0x11));
	CBrush brush;
	brush.CreateSolidBrush(RGB(0xDD, 0x88, 0x11));

	pDC->FillRect(rectClient, &brush);
	// ... (CView1の描画処理)
	if (m_editCustom1 == nullptr)
	{

		m_editCustom1 = new CCenterEdit();
		CRect rect1(10, 10, 50, 50); // ダイアログ内の位置とサイズを指定
		m_editCustom1->Create(WS_CHILD | WS_VISIBLE, rect1, this, 1003);
		m_editCustom1->ShowWindow(TRUE);
		m_editCustom1->SetWindowTextW(_T("1234")); // 初期値を設定
		m_editCustom1->SetFont(GetFont());		   // フォントをダイアログのフォントに設定
		m_editCustom1->SetLimitText(4);			   // 入力文字数を4文字に制限
		m_editCustom1->SetWindowPos(nullptr, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
	}
	if (m_editCustom2 == nullptr)
	{

		m_editCustom2 = new CCenterEdit();
		CRect rect1(60, 10, 110, 50); // ダイアログ内の位置とサイズを指定
		m_editCustom2->Create(WS_CHILD | WS_VISIBLE, rect1, this, 1005);
		m_editCustom2->ShowWindow(TRUE);
		m_editCustom2->SetWindowTextW(_T("5678")); // 初期値を設定
		m_editCustom2->SetFont(GetFont());		   // フォントをダイアログのフォントに設定
		m_editCustom2->SetLimitText(4);			   // 入力文字数を4文字に制限
		m_editCustom2->SetWindowPos(nullptr, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
	}
}

BOOL CView1::PreCreateWindow(CREATESTRUCT &cs)
{
	// まず基底クラスの関数を呼び出すのが作法です
	if (!CView::PreCreateWindow(cs))
		return FALSE;

	// WS_CLIPSIBLINGS スタイルをビット演算で削除します。
	// これにより、CView1は兄弟ウィンドウの領域を無視して描画するようになります。
	cs.style &= ~WS_CLIPSIBLINGS;

	cs.dwExStyle |= WS_EX_TRANSPARENT;
	return TRUE;
}

// View1 診断

#ifdef _DEBUG
void CView1::AssertValid() const
{
	CView::AssertValid();
}

#ifndef _WIN32_WCE
void CView1::Dump(CDumpContext &dc) const
{
	CView::Dump(dc);
}
#endif
#endif //_DEBUG
