
// MFCApplication4Dlg.cpp : 実装ファイル
//

#include "pch.h"
#include "framework.h"
#include "MFCApplication4.h"
#include "MFCApplication4Dlg.h"
#include "afxdialogex.h"
#include "CenterEdit.h"
#include "CMyDialog.h"
#include "CMyDialog2.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMFCApplication4Dlg ダイアログ



CMFCApplication4Dlg::CMFCApplication4Dlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_MFCAPPLICATION4_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

}

void CMFCApplication4Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CMFCApplication4Dlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	// カスタムメッセージとハンドラ関数をマッピング
	ON_MESSAGE(WM_SHOW_VIEW2, &CMFCApplication4Dlg::OnShowView2)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_BUTTON1, &CMFCApplication4Dlg::OnBnClickedButton1)
	ON_WM_SYSCOMMAND()
	ON_BN_CLICKED(IDC_BUTTON2, &CMFCApplication4Dlg::OnBnClickedButton2)
    ON_WM_TIMER()
    ON_MESSAGE(WM_APP_SHOW_OPERATION_STATUS, &CMFCApplication4Dlg::OnShowOperationStatus)
END_MESSAGE_MAP()

// ワーカースレッドから通知を受けてView2を作成・表示する
LRESULT CMFCApplication4Dlg::OnShowView2(WPARAM wParam, LPARAM lParam)
{
	// すでにView2が表示されていれば何もしない
	if (m_pView2 && ::IsWindow(m_pView2->GetSafeHwnd()))
	{
		m_pView2->ShowWindow(SW_SHOW);
		m_pView2->SetFocus();
		return 0;
	}

	// View1 (プライマリビュー) のウィンドウ矩形を取得
	CRect rectView1;
	if (!m_pView1) return -1; // View1が見つからない場合はエラー

	m_pView1->GetClientRect(&rectView1);
	m_pView1->ClientToScreen(&rectView1); // スクリーン座標に変換
	ScreenToClient(&rectView1);       // MainFrameのクライアント座標に変換

	// View2をView1の右半分に重ねるための座標を計算
	CRect rectView2;
	rectView2.left = rectView1.left + rectView1.Width() / 2;
	rectView2.top = rectView1.top;
	rectView2.right = rectView1.right + rectView1.Width() / 2;
	rectView2.bottom = rectView1.bottom;

	// View2が未作成なら作成する
	if (m_pView2 == nullptr)
	{
		m_pView2 = new CView2();
		CCreateContext context;
		m_pView2->Create(NULL, _T("View2"), WS_CHILD | WS_VISIBLE | WS_BORDER, rectView2, this, AFX_IDW_PANE_FIRST + 1, &context);
	}

	// View2を正しい位置に表示し、最前面に持ってくる
	m_pView2->SetWindowPos(m_pView1, rectView2.left, rectView2.top, rectView2.Width(), rectView2.Height(), SWP_SHOWWINDOW);
	m_pView2->Invalidate();

	// ★★★ View2を表示した直後にクリッピングを適用 ★★★
	UpdateLayoutAndClipping();

	return 0;
}

// ★★★ CMainFrame::OnSizeハンドラの実装を追加 ★★★
void CMFCApplication4Dlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	// メインウィンドウのサイズが変更されたら、
	// レイアウトとクリッピングを再計算する
	if (m_pView2 && ::IsWindow(m_pView2->GetSafeHwnd()))
	{
		// ここでCView1やCView2のサイズや位置を調整するコードがあれば、それを実行した後に...
		UpdateLayoutAndClipping();
	}
}
// ★★★ UpdateLayoutAndClipping関数の実装を追加 ★★★
void CMFCApplication4Dlg::UpdateLayoutAndClipping()
{
	if (!m_pView1 || !::IsWindow(m_pView1->GetSafeHwnd())) return;
	if (!m_pView2 || !::IsWindow(m_pView2->GetSafeHwnd())) return;

	// 1. 各ビューのクライアント領域をスクリーン座標で取得
	CRect rectView1Screen, rectView2Screen;
	m_pView1->GetWindowRect(&rectView1Screen);
	m_pView2->GetWindowRect(&rectView2Screen);

	// 2. CView2の全体リージョンを作成 (CView2のローカル座標系)
	CRect rectView2Client;
	m_pView2->GetClientRect(&rectView2Client);
	CRgn rgnView2;
	rgnView2.CreateRectRgnIndirect(&rectView2Client);

	// 3. CView1の領域をCView2のクライアント座標系に変換
	//    (スクリーン座標からCView2のクライアント座標へ)
	CRect rectView1inView2 = rectView1Screen;
	m_pView2->ScreenToClient(&rectView1inView2);

	// 4. 変換したCView1の領域でリージョンを作成
	CRgn rgnClip;
	rgnClip.CreateRectRgnIndirect(&rectView1inView2);

	// 5. CView2のリージョンからCView1のリージョンを「差し引く」(RGN_DIFF)
	//    これにより、重なっている部分が「穴」になる
	rgnView2.CombineRgn(&rgnView2, &rgnClip, RGN_DIFF);

	// 6. 計算した新しいリージョンをCView2に設定する
	//    Detach()でリージョンの所有権をウィンドウに渡し、自動解放させる
	m_pView2->SetWindowRgn((HRGN)rgnView2.Detach(), TRUE);
}


// CMFCApplication4Dlg メッセージ ハンドラー

BOOL CMFCApplication4Dlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// このダイアログのアイコンを設定します。アプリケーションのメイン ウィンドウがダイアログでない場合、
	//  Framework は、この設定を自動的に行います。
	SetIcon(m_hIcon, TRUE);			// 大きいアイコンの設定
	SetIcon(m_hIcon, FALSE);		// 小さいアイコンの設定

	m_editCustom1 = new CCenterEdit();
	CRect rect1(50, 50, 200, 200); // ダイアログ内の位置とサイズを指定
	m_editCustom1->Create(WS_CHILD | WS_VISIBLE, rect1, this, 1000);
	m_editCustom1->ShowWindow(TRUE);
	m_editCustom1->SetWindowTextW(_T("1234")); // 初期値を設定
	m_editCustom1->SetFont(GetFont()); // フォントをダイアログのフォントに設定
	m_editCustom1->SetLimitText(4); // 入力文字数を4文字に制限
	m_editCustom1->SetWindowPos(nullptr, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
	// TODO: 初期化をここに追加します。

	m_editCustom2 = new CCenterEdit();
	CRect rect2(250, 50, 400, 200); // ダイアログ内の位置とサイズを指定
	m_editCustom2->Create(WS_CHILD | WS_VISIBLE, rect2, this, 1001);
	m_editCustom2->ShowWindow(TRUE);
	m_editCustom2->SetWindowTextW(_T("5678")); // 初期値を設定
	m_editCustom2->SetFont(GetFont()); // フォントをダイアログのフォントに設定
	m_editCustom2->SetLimitText(4); // 入力文字数を4文字に制限
	m_editCustom2->SetWindowPos(nullptr, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
	// TODO: 初期化をここに追加します。

	CRect rectView1(450, 50, 600, 200); // ダイアログ内の位置とサイズを指定
	m_pView1 = new CView1();
	CCreateContext context;
	m_pView1->Create(NULL, _T("View2"), WS_CHILD | WS_VISIBLE | WS_BORDER, rectView1, this, AFX_IDW_PANE_FIRST + 1, &context);
	m_pView1->ShowWindow(SW_SHOW);

	// View2を正しい位置に表示し、最前面に持ってくる
	m_pView1->SetWindowPos(&wndTop, rectView1.left, rectView1.top, rectView1.Width(), rectView1.Height(), SWP_SHOWWINDOW);
	m_pView1->Invalidate();

	
	m_editCustom3 = new CMyEdit();
	CRect rect3(250, 250, 400, 300); // ダイアログ内の位置とサイズを指定
	m_editCustom3->Create(WS_CHILD | WS_VISIBLE, rect3, this, 1010);
	m_editCustom3->ShowWindow(TRUE);
	m_editCustom3->SetWindowTextW(_T("AAAA")); // 初期値を設定
	m_editCustom3->SetFont(GetFont()); // フォントをダイアログのフォントに設定

    // --- ▼▼▼ この行を追加 ▼▼▼ ---
    // ダイアログの初期タイトルを保存
    GetWindowText(m_strOriginalTitle);
    // --- ▲▲▲ 追加 ▲▲▲ ---
	return TRUE;  // フォーカスをコントロールに設定した場合を除き、TRUE を返します。
}


// アプリケーションクラスから通知メッセージを受け取るハンドラ
LRESULT CMFCApplication4Dlg::OnShowOperationStatus(WPARAM wParam, LPARAM lParam)
{
    // 現在のタイトルを取得
    CString strCurrentTitle;
    GetWindowText(strCurrentTitle);

    // もしタイトルがまだ「操作中」でなければ、現在のタイトルをオリジナルとして保存更新
    if (strCurrentTitle != _T("操作中"))
    {
        m_strOriginalTitle = strCurrentTitle;
    }

    // タイトルを「操作中」に変更
    SetWindowText(_T("操作中"));

    // 3秒タイマーを開始（またはリセット）する
    SetTimer(ID_TITLE_TIMER, 3000, NULL);

    return 0;
}

// タイマーイベントを処理するハンドラ
void CMFCApplication4Dlg::OnTimer(UINT_PTR nIDEvent)
{
    // 今回設定したタイマーかどうかをIDで確認
    if (nIDEvent == ID_TITLE_TIMER)
    {
        // タイマーを停止（一回限りのタイマーのため）
        KillTimer(ID_TITLE_TIMER);
        
        // タイトルを保存しておいた元の文字列に戻す
        SetWindowText(m_strOriginalTitle);
    }

    CDialogEx::OnTimer(nIDEvent);
}
// ダイアログに最小化ボタンを追加する場合、アイコンを描画するための
//  下のコードが必要です。ドキュメント/ビュー モデルを使う MFC アプリケーションの場合、
//  これは、Framework によって自動的に設定されます。

void CMFCApplication4Dlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 描画のデバイス コンテキスト

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// クライアントの四角形領域内の中央
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// アイコンの描画
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// ユーザーが最小化したウィンドウをドラッグしているときに表示するカーソルを取得するために、
//  システムがこの関数を呼び出します。
HCURSOR CMFCApplication4Dlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CMFCApplication4Dlg::OnBnClickedButton1()
{
	CMyDialog dlg;
	dlg.DoModal(); // モーダルダイアログとして表示
}

void CMFCApplication4Dlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	// nIDの下位4ビットはシステムが使用するため、0xFFF0でマスクして比較します
	if ((nID & 0xFFF0) == SC_CLOSE)
	{
		// SC_CLOSE コマンドを検知した場合、何もしないで関数を抜けます。
		// これにより、Alt+F4、タイトルバーの「×」ボタン、タスクバーからの
		// 「閉じる」操作がすべて無効化されます。
		return;
	}

	// SC_CLOSE 以外のシステムコマンド（例: 最小化、最大化など）は、
	// デフォルトの処理に任せます。
	CDialogEx::OnSysCommand(nID, lParam);
}
void CMFCApplication4Dlg::OnBnClickedButton2()
{
	CMyDialog2 dlg;
	dlg.DoModal(); // モーダルダイアログとして表示
}
