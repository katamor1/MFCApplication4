/**
 * @file MFCApplication4Dlg.cpp
 * @brief メインダイアログクラスの実装ファイル
 */
#include "pch.h"
#include "framework.h"
#include "MFCApplication4.h"
#include "MFCApplication4Dlg.h"
#include "afxdialogex.h"
#include "CenterEdit.h"
#include "CMyDialog.h"
#include "CMyDialog2.h"
#include "CMyDialog3.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/**
 * @brief CMFCApplication4Dlgクラスのコンストラクタ
 * @param pParent 親ウィンドウへのポインタ
 */
CMFCApplication4Dlg::CMFCApplication4Dlg(CWnd *pParent /*=nullptr*/)
    : CDialogEx(IDD_MFCAPPLICATION4_DIALOG, pParent)
{
    m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

/**
 * @brief DDX/DDV（ダイアログデータエクスチェンジ/バリデーション）のサポート
 * @param pDX データエクスチェンジオブジェクトへのポインタ
 */
void CMFCApplication4Dlg::DoDataExchange(CDataExchange *pDX)
{
    CDialogEx::DoDataExchange(pDX);
}

// BEGIN_MESSAGE_MAPブロック
// Windowsメッセージと、それを処理するクラスのメンバ関数（ハンドラ）を関連付けます。
BEGIN_MESSAGE_MAP(CMFCApplication4Dlg, CDialogEx)
    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()
    ON_MESSAGE(WM_SHOW_VIEW2, &CMFCApplication4Dlg::OnShowView2)
    ON_WM_SIZE()
    ON_BN_CLICKED(IDC_BUTTON1, &CMFCApplication4Dlg::OnBnClickedButton1)
    ON_WM_SYSCOMMAND()
    ON_BN_CLICKED(IDC_BUTTON2, &CMFCApplication4Dlg::OnBnClickedButton2)
    ON_WM_TIMER()
    ON_MESSAGE(WM_APP_SHOW_OPERATION_STATUS, &CMFCApplication4Dlg::OnShowOperationStatus)
    ON_BN_CLICKED(IDC_BUTTON3, &CMFCApplication4Dlg::OnBnClickedButton3)
    ON_MESSAGE(WM_EXECUTE_FUNCTION, &CMFCApplication4Dlg::OnExecuteFunction)
    ON_BN_CLICKED(IDOK, &CMFCApplication4Dlg::OnBnClickedOk)
END_MESSAGE_MAP()


/**
 * @brief カスタムメッセージ WM_SHOW_VIEW2 のハンドラ
 * @details CView1のワーカースレッドからの通知を受け、CView2を作成・表示します。
 * @param wParam 未使用
 * @param lParam 未使用
 * @return 処理結果
 */
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
    if (!m_pView1)
        return -1; // View1が見つからない場合はエラー

    m_pView1->GetWindowRect(&rectView1); // View1のスクリーン座標を取得
    ScreenToClient(&rectView1);          // ダイアログのクライアント座標に変換

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
        m_pView2->Create(NULL, _T("View2"), WS_CHILD | WS_VISIBLE | WS_BORDER, rectView2, this, AFX_IDW_PANE_FIRST + 2, &context);
    }

    // View2を正しい位置に表示し、最前面に持ってくる
    m_pView2->SetWindowPos(&wndTop, rectView2.left, rectView2.top, rectView2.Width(), rectView2.Height(), SWP_SHOWWINDOW);
    m_pView2->Invalidate();
    // CView2の表示領域を適切にクリッピングする
    UpdateLayoutAndClipping();

    return 0;
}

/**
 * @brief ダイアログのサイズ変更イベント(WM_SIZE)を処理します。
 * @param nType サイズ変更の種類
 * @param cx クライアント領域の新しい幅
 * @param cy クライアント領域の新しい高さ
 */
void CMFCApplication4Dlg::OnSize(UINT nType, int cx, int cy)
{
    CDialogEx::OnSize(nType, cx, cy);

    // CView2が表示されている場合、メインウィンドウのサイズが変更されたら、
    // レイアウトとクリッピングを再計算する
    if (m_pView1 && ::IsWindow(m_pView1->GetSafeHwnd()) &&
        m_pView2 && ::IsWindow(m_pView2->GetSafeHwnd()))
    {
        // ここでCView1やCView2のサイズや位置を調整するコードがあれば、それを実行した後に...
        UpdateLayoutAndClipping();
    }
}

/**
 * @brief CView1とCView2のレイアウトとクリッピング領域を更新します。
 * @details CView2のウィンドウリージョン（描画担当領域）から、CView1と重なる部分を
 * 「差し引く」ことで、CView2がCView1を覆い隠さないようにします。
 */
void CMFCApplication4Dlg::UpdateLayoutAndClipping()
{
    if (!m_pView1 || !::IsWindow(m_pView1->GetSafeHwnd())) return;
    if (!m_pView2 || !::IsWindow(m_pView2->GetSafeHwnd())) return;

    // 1. 各ビューのウィンドウ矩形をスクリーン座標で取得
    CRect rectView1Screen, rectView2Screen;
    m_pView1->GetWindowRect(&rectView1Screen);
    m_pView2->GetWindowRect(&rectView2Screen);

    // 2. CView2のクライアント領域全体のリージョンを作成 (CView2のローカル座標系)
    CRect rectView2Client;
    m_pView2->GetClientRect(&rectView2Client);
    CRgn rgnView2;
    rgnView2.CreateRectRgnIndirect(&rectView2Client);

    // 3. CView1の領域(スクリーン座標)をCView2のクライアント座標系に変換
    CRect rectView1inView2 = rectView1Screen;
    m_pView2->ScreenToClient(&rectView1inView2);

    // 4. 変換したCView1の領域で、差し引くためのクリッピングリージョンを作成
    CRgn rgnClip;
    rgnClip.CreateRectRgnIndirect(&rectView1inView2);

    // 5. CView2の全体リージョンからCView1のリージョンを「差し引く」(RGN_DIFF)
    //    これにより、重なっている部分が「穴」の開いたリージョンが生成される
    rgnView2.CombineRgn(&rgnView2, &rgnClip, RGN_DIFF);

    // 6. 計算した新しいリージョンをCView2に設定する
    //    Detach()でリージョンの所有権をウィンドウに渡し、ウィンドウ破棄時に自動解放させる
    m_pView2->SetWindowRgn((HRGN)rgnView2.Detach(), TRUE);
}

/**
 * @brief ダイアログの初期化処理(WM_INITDIALOG)をオーバーライドします。
 * @details アイコンの設定や、各種カスタムコントロールの動的生成と配置を行います。
 * @return フォーカスをコントロールに設定しない場合はTRUE
 */
BOOL CMFCApplication4Dlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    // アイコン設定
    SetIcon(m_hIcon, TRUE);
    SetIcon(m_hIcon, FALSE);

    // CCenterEdit コントロールを動的に生成・配置
    m_editCustom1 = new CCenterEdit();
    CRect rect1(50, 50, 200, 200);
    m_editCustom1->Create(WS_CHILD | WS_VISIBLE | ES_CENTER | ES_WANTRETURN, rect1, this, 1000);
    m_editCustom1->SetWindowTextW(_T("1234"));
    m_editCustom1->SetFont(GetFont());

    m_editCustom2 = new CCenterEdit();
    CRect rect2(250, 50, 400, 200);
    m_editCustom2->Create(WS_CHILD | WS_VISIBLE | ES_CENTER | ES_WANTRETURN, rect2, this, 1001);
    m_editCustom2->SetWindowTextW(_T("5678"));
    m_editCustom2->SetFont(GetFont());

    // CView1 を動的に生成・配置
    CRect rectView1(450, 50, 600, 200);
    m_pView1 = new CView1();
    CCreateContext context;
    m_pView1->Create(NULL, _T("View1"), WS_CHILD | WS_VISIBLE | WS_BORDER, rectView1, this, AFX_IDW_PANE_FIRST + 1, &context);
    m_pView1->SetWindowPos(&wndTop, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
    
    // CMyEdit (ソフトウェアキーボード付き)を動的に生成・配置
    m_editCustom3 = new CMyEdit();
    CRect rect3(250, 250, 400, 300);
    m_editCustom3->Create(WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOVSCROLL | ES_MULTILINE, rect3, this, 1010);
    m_editCustom3->SetWindowTextW(_T("Click here to show Keyboard"));
    m_editCustom3->SetFont(GetFont());

    // ダイアログの初期タイトルを保存
    GetWindowText(m_strOriginalTitle);
    return TRUE;
}


/**
 * @brief Enterキーによるダイアログ終了を無効化するためにオーバーライドします。
 */
void CMFCApplication4Dlg::OnOK()
{
    // この関数をオーバーライドし、中身を空にします。
    // 基底クラスの CDialogEx::OnOK() を呼び出さないことで、
    // Enterキーを押してもダイアログが閉じなくなります。
}


/**
 * @brief キーボードメッセージをウィンドウにディスパッチされる前に横取りします。
 * @details EnterキーとEscapeキーによるダイアログ終了を無効化するために使用します。
 * @param pMsg メッセージ情報へのポインタ
 * @return メッセージを処理した場合はTRUE、デフォルト処理に任せる場合はFALSE。
 */
BOOL CMFCApplication4Dlg::PreTranslateMessage(MSG *pMsg)
{
    // Escapeキーが押された場合、ダイアログが閉じるのを防ぎます。
    if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE)
    {
        return TRUE; // メッセージを処理済みとし、伝搬させない
    }

    // Enterキーが押された場合も同様に無効化
    // ただし、フォーカスのあるコントロールが欲しい場合があるので注意
    if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN)
    {
        // ここで特定のコントロールにフォーカスがある場合のみ無効化する、などの分岐も可能
        return TRUE;
    }

    // その他のメッセージは、デフォルトの処理に任せます。
    return CDialogEx::PreTranslateMessage(pMsg);
}

/**
 * @brief カスタムメッセージ WM_APP_SHOW_OPERATION_STATUS のハンドラ
 * @details CWinAppのPreTranslateMessageからの通知を受け、タイトルを「操作中」に変更しタイマーを開始します。
 * @param wParam 未使用
 * @param lParam 未使用
 * @return 処理結果
 */
LRESULT CMFCApplication4Dlg::OnShowOperationStatus(WPARAM wParam, LPARAM lParam)
{
    // 現在のタイトルを取得
    CString strCurrentTitle;
    GetWindowText(strCurrentTitle);

    // タイトルがまだ「操作中」でなければ、現在のタイトルを保存
    if (strCurrentTitle != _T("操作中"))
    {
        m_strOriginalTitle = strCurrentTitle;
    }

    // タイトルを「操作中」に変更
    SetWindowText(_T("操作中"));

    // 3秒後に1回だけ発生するタイマーを開始（またはリセット）する
    SetTimer(ID_TITLE_TIMER, 3000, NULL);

    return 0;
}


/**
 * @brief タイマーイベント(WM_TIMER)を処理します。
 * @details ID_TITLE_TIMERからのイベントを受け、ウィンドウタイトルを元に戻します。
 * @param nIDEvent タイマーのID
 */
void CMFCApplication4Dlg::OnTimer(UINT_PTR nIDEvent)
{
    // 目的のタイマーIDか確認
    if (nIDEvent == ID_TITLE_TIMER)
    {
        // タイマーを停止（一回限りのタイマーのため）
        KillTimer(ID_TITLE_TIMER);

        // タイトルを保存しておいた元の文字列に戻す
        SetWindowText(m_strOriginalTitle);
    }

    CDialogEx::OnTimer(nIDEvent);
}

/**
 * @brief ダイアログの描画処理(WM_PAINT)をオーバーライドします。
 * @details ダイアログが最小化されている場合にアイコンを描画します。
 */
void CMFCApplication4Dlg::OnPaint()
{
    if (IsIconic())
    {
        CPaintDC dc(this);

        SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

        int cxIcon = GetSystemMetrics(SM_CXICON);
        int cyIcon = GetSystemMetrics(SM_CYICON);
        CRect rect;
        GetClientRect(&rect);
        int x = (rect.Width() - cxIcon + 1) / 2;
        int y = (rect.Height() - cyIcon + 1) / 2;

        dc.DrawIcon(x, y, m_hIcon);
    }
    else
    {
        CDialogEx::OnPaint();
    }
}


/**
 * @brief 最小化されたウィンドウをドラッグする際のカーソルを取得します。
 * @return アイコンのハンドル
 */
HCURSOR CMFCApplication4Dlg::OnQueryDragIcon()
{
    return static_cast<HCURSOR>(m_hIcon);
}

/**
 * @brief カスタムメッセージ WM_EXECUTE_FUNCTION のハンドラ
 * @details CMyDialog3からの機能実行通知を受け、メッセージボックスで内容を表示します。
 * @param wParam パラメータ 'n'
 * @param lParam パラメータ 'k'
 * @return 処理結果
 */
LRESULT CMFCApplication4Dlg::OnExecuteFunction(WPARAM wParam, LPARAM lParam)
{
    // wParamから「n」の値、lParamから「k」の値を取得
    int n = (int)wParam;
    int k = (int)lParam;

    // メッセージボックスに表示する文字列を "F(n-k)" の形式でフォーマット
    CString msg;
    msg.Format(_T("機能 F(%d-%d) が実行されました。"), n, k);
    AfxMessageBox(msg);

    return 0;
}


/**
 * @brief ボタン1(IDC_BUTTON1)のクリックイベントハンドラ
 * @details CMyDialogをモーダルで表示します。
 */
void CMFCApplication4Dlg::OnBnClickedButton1()
{
    CMyDialog dlg;
    dlg.DoModal();
}

/**
 * @brief システムコマンドメッセージ(WM_SYSCOMMAND)を処理します。
 * @details 特に、ウィンドウを閉じるコマンド(SC_CLOSE)を無効化するために使用します。
 * @param nID コマンドID
 * @param lParam コマンドのパラメータ
 */
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

/**
 * @brief ボタン2(IDC_BUTTON2)のクリックイベントハンドラ
 * @details CMyDialog2をモーダルで表示します。
 */
void CMFCApplication4Dlg::OnBnClickedButton2()
{
    CMyDialog2 dlg;
    dlg.DoModal();
}

/**
 * @brief ボタン3(IDC_BUTTON3)のクリックイベントハンドラ
 * @details CMyDialog3をモーダルで表示します。
 */
void CMFCApplication4Dlg::OnBnClickedButton3()
{
    CMyDialog3 dlg;
    dlg.DoModal();
}

/**
 * @brief OKボタン(IDOK)のクリックイベントハンドラ
 * @details ダイアログをIDOKで終了します。
 */
void CMFCApplication4Dlg::OnBnClickedOk()
{
    EndDialog(IDOK);
}