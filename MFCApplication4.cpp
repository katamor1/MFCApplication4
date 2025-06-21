
// MFCApplication4.cpp : アプリケーションのクラス動作を定義します。
//

#include "pch.h"
#include "framework.h"
#include "MFCApplication4.h"
#include "MFCApplication4Dlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMFCApplication4App

BEGIN_MESSAGE_MAP(CMFCApplication4App, CWinApp)
ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()

// CMFCApplication4App の構築

CMFCApplication4App::CMFCApplication4App()
{
	// TODO: この位置に構築用コードを追加してください。
	// ここに InitInstance 中の重要な初期化処理をすべて記述してください。
}

// 唯一の CMFCApplication4App オブジェクト

CMFCApplication4App theApp;

// CMFCApplication4App の初期化

BOOL CMFCApplication4App::InitInstance()
{
	CWinApp::InitInstance();

	// ダイアログにシェル ツリー ビューまたはシェル リスト ビュー コントロールが
	// 含まれている場合にシェル マネージャーを作成します。
	CShellManager *pShellManager = new CShellManager;

	// MFC コントロールでテーマを有効にするために、"Windows ネイティブ" のビジュアル マネージャーをアクティブ化
	CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));

	// 標準初期化
	// これらの機能を使わずに最終的な実行可能ファイルの
	// サイズを縮小したい場合は、以下から不要な初期化
	// ルーチンを削除してください。
	// 設定が格納されているレジストリ キーを変更します。
	// TODO: 会社名または組織名などの適切な文字列に
	// この文字列を変更してください。
	SetRegistryKey(_T("アプリケーション ウィザードで生成されたローカル アプリケーション"));

	CMFCApplication4Dlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: ダイアログが <OK> で消された時のコードを
		//  記述してください。
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: ダイアログが <キャンセル> で消された時のコードを
		//  記述してください。
	}
	else if (nResponse == -1)
	{
		TRACE(traceAppMsg, 0, "警告: ダイアログの作成に失敗しました。アプリケーションは予期せずに終了します。\n");
		TRACE(traceAppMsg, 0, "警告: ダイアログで MFC コントロールを使用している場合、#define _AFX_NO_MFC_CONTROLS_IN_DIALOGS を指定できません。\n");
	}

	// 上で作成されたシェル マネージャーを削除します。
	if (pShellManager != nullptr)
	{
		delete pShellManager;
	}

#if !defined(_AFXDLL) && !defined(_AFX_NO_MFC_CONTROLS_IN_DIALOGS)
	ControlBarCleanUp();
#endif

	// ダイアログは閉じられました。アプリケーションのメッセージ ポンプを開始しないで
	//  アプリケーションを終了するために FALSE を返してください。
	return FALSE;
}

// アプリケーション全体で不要な操作をフィルタリングする
BOOL CMFCApplication4App::PreTranslateMessage(MSG *pMsg)
{
	// マウスクリック関連のメッセージ（クライアント領域・非クライアント領域）を検出
	if ((pMsg->message >= WM_LBUTTONDOWN && pMsg->message <= WM_MBUTTONDBLCLK) ||
		(pMsg->message >= WM_NCLBUTTONDOWN && pMsg->message <= WM_NCMBUTTONDBLCLK))
	{
		// ボタンが押された瞬間のメッセージのみを対象とする
		if (pMsg->message == WM_LBUTTONDOWN || pMsg->message == WM_RBUTTONDOWN || pMsg->message == WM_MBUTTONDOWN ||
			pMsg->message == WM_NCLBUTTONDOWN || pMsg->message == WM_NCRBUTTONDOWN || pMsg->message == WM_NCMBUTTONDOWN)
		{
			// メインウィンドウに通知メッセージを送信
			if (m_pMainWnd && m_pMainWnd->GetSafeHwnd())
			{
				m_pMainWnd->PostMessage(WM_APP_SHOW_OPERATION_STATUS, 0, 0);
			}
		}
	}

	// --- 1. Alt+F4 と Alt+Space の無効化 ---
	// WM_SYSKEYDOWNは、Altキーが押されている状態で他のキーが押された場合に発生します
	if (pMsg->message == WM_SYSKEYDOWN)
	{
		// pMsg->wParam には押されたキーの仮想キーコードが入ります
		if (pMsg->wParam == VK_F4)
		{
			// Alt+F4 が押されたら、メッセージを処理済みとして扱う（ディスパッチしない）
			return TRUE;
		}
		if (pMsg->wParam == VK_SPACE)
		{
			// Alt+Space が押されたら、メッセージを処理済みとして扱う
			return TRUE;
		}
	}

	// --- 2. ダブルクリックの無効化 ---
	// ダブルクリック関連のメッセージをまとめてチェックします
	if (pMsg->message >= WM_LBUTTONDBLCLK && pMsg->message <= WM_MBUTTONDBLCLK)
	{
		// ダブルクリックメッセージを無視します
		return TRUE;
	}

	// --- 3. 右クリックの無効化 ---
	// 右クリック関連のメッセージをまとめてチェックします
	if (pMsg->message == WM_RBUTTONDOWN ||
		pMsg->message == WM_RBUTTONUP ||
		pMsg->message == WM_CONTEXTMENU)
	{
		// 右クリック関連のメッセージを無視します
		return TRUE;
	}

	// --- 4. タッチジェスチャの無効化 ---
	// WM_GESTUREの定義がない古いSDKの場合に備えて定義します
#ifndef WM_GESTURE
#define WM_GESTURE 0x0119
#endif

	if (pMsg->message == WM_GESTURE)
	{
		// ズーム、回転、フリックなどのジェスチャメッセージを無視します
		return TRUE;
	}

	// 上記のいずれにも該当しない場合は、通常のメッセージ処理を続行する
	return CWinApp::PreTranslateMessage(pMsg);
}