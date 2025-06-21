/**
 * @file CMyDialog3.cpp
 * @brief CGridCtrlと機能ボタンを連携させるダイアログのクラス実装
 */
#include "pch.h"
#include "resource.h"
#include "MFCApplication4.h"
#include "afxdialogex.h"
#include "CMyDialog3.h"

// CMyDialog3クラスが動的生成可能であることをフレームワークに伝えます。
IMPLEMENT_DYNAMIC(CMyDialog3, CDialogEx)

/**
 * @brief CMyDialog3クラスのコンストラクタ
 * @param pParent 親ウィンドウへのポインタ
 */
CMyDialog3::CMyDialog3(CWnd *pParent /*=nullptr*/)
    : CDialogEx(IDD, pParent), m_nSelectedRow(-1), m_nCurrentPage(1)
{
}

/**
 * @brief CMyDialog3クラスのデストラクタ
 */
CMyDialog3::~CMyDialog3()
{
}

/**
 * @brief DDX/DDV（ダイアログデータエクスチェンジ/バリデーション）のサポート
 * @param pDX データエクスチェンジオブジェクトへのポインタ
 */
void CMyDialog3::DoDataExchange(CDataExchange *pDX)
{
    CDialogEx::DoDataExchange(pDX);
}

// BEGIN_MESSAGE_MAPブロック
// Windowsメッセージと、それを処理するクラスのメンバ関数（ハンドラ）を関連付けます。
BEGIN_MESSAGE_MAP(CMyDialog3, CDialogEx)
    // CGridCtrlからの選択変更通知(GCN_SELCHANGED)をOnGridSelChangedに紐付けます。
    ON_NOTIFY(GCN_SELCHANGED, IDC_GRID_CTRL, &CMyDialog3::OnGridSelChanged)
    // 次ページボタン(IDC_BTN_NEXTPAGE)のクリックをOnNextPageClickedに紐付けます。
    ON_BN_CLICKED(IDC_BTN_NEXTPAGE, &CMyDialog3::OnNextPageClicked)
    // IDC_BTN_FUNC1からIDC_BTN_FUNC8までの範囲のボタンクリックをOnFuncButtonClickedに一括で紐付けます。
    ON_COMMAND_RANGE(IDC_BTN_FUNC1, IDC_BTN_FUNC8, &CMyDialog3::OnFuncButtonClicked)
END_MESSAGE_MAP()


/**
 * @brief CGridCtrlからの行選択変更通知(WM_NOTIFY, GCN_SELCHANGED)を処理します。
 * @param pNMHDR 通知情報を含むNMHDR構造体へのポインタ。NM_GRIDVIEWにキャストして使用します。
 * @param pResult 結果コードへのポインタ。通常は0を設定します。
 */
void CMyDialog3::OnGridSelChanged(NMHDR *pNMHDR, LRESULT *pResult)
{
    NM_GRIDVIEW *pNMGV = (NM_GRIDVIEW *)pNMHDR;

    // 選択された行のインデックス(0始まり)をメンバ変数に保存します。
    m_nSelectedRow = pNMGV->iRow;
    // 新しい行が選択されたら、必ずページ1にリセットします。
    m_nCurrentPage = 1;
    // ボタンの表示状態を現在の選択状態に合わせて更新します。
    UpdateButtons();

    *pResult = 0;
}

/**
 * @brief Enterキーによるダイアログ終了を無効化するためにオーバーライドします。
 */
void CMyDialog3::OnOK()
{
    // この関数をオーバーライドし、中身を空にします。
    // 基底クラスの CDialogEx::OnOK() を呼び出さないことで、
    // Enterキーを押してもダイアログが閉じなくなります。
}

/**
 * @brief ダイアログの初期化処理(WM_INITDIALOG)をオーバーライドします。
 * @details グリッドやボタンコントロールの動的生成と初期設定を行います。
 * @return フォーカスをコントロールに設定しない場合はTRUE
 */
BOOL CMyDialog3::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    // --- レイアウト定数の定義 ---
    const int nMargin = 10;
    const int nFuncBtnWidth = 48;
    const int nFuncBtnHeight = 24;
    const int nNextBtnWidth = 24;
    const int nBtnSpacing = 1;

    // --- CGridCtrlのセットアップ ---
    m_gridCtrl.SetupGrid(16, 2); // 16行2列のグリッドを作成
    m_gridCtrl.SetColumnWidth(0, 120);
    m_gridCtrl.SetColumnWidth(1, 220);

    // --- CGridCtrlの生成 ---
    CRect gridRect;
    int gridHeight = m_gridCtrl.GetRequiredHeight();
    gridRect.SetRect(nMargin, nMargin, 120 + 220 + nMargin, nMargin + gridHeight);

    // 仕様書通りにセルを設定
    for (int i = 0; i < 16; ++i)
    {
        CString caption;
        caption.Format(_T("項目 %d"), i + 1);
        m_gridCtrl.SetCellText(i, 0, caption);
        m_gridCtrl.SetCellEditable(i, 0, FALSE); // 1列目は読み取り専用
        m_gridCtrl.SetCellEditable(i, 1, TRUE);  // 2列目は編集可能
    }
    // グリッドを生成
    m_gridCtrl.Create(gridRect, this, IDC_GRID_CTRL);
    m_gridCtrl.RedrawGrid();

    // --- ボタン群の生成 ---
    // ボタンを配置する開始Y座標（グリッドの下）
    int currentY = gridRect.bottom + nMargin;
    int currentX = nMargin;

    // 8個の機能ボタンをループで動的に生成
    for (int i = 0; i < 8; ++i)
    {
        CRect btnRect(currentX, currentY, currentX + nFuncBtnWidth, currentY + nFuncBtnHeight);
        UINT nID = IDC_BTN_FUNC_BASE + i;
        m_btnFunc[i].Create(_T(""), WS_CHILD | WS_VISIBLE, btnRect, this, nID);
        m_btnFunc[i].SetFont(GetFont());

        currentX += nFuncBtnWidth + nBtnSpacing;
    }

    // 次ページボタンを生成
    CRect nextBtnRect(currentX, currentY, currentX + nNextBtnWidth, currentY + nFuncBtnHeight);
    m_btnNextPage.Create(_T(""), WS_CHILD | WS_VISIBLE, nextBtnRect, this, IDC_BTN_NEXTPAGE);
    m_btnNextPage.SetFont(GetFont());

    // --- 初期状態の設定 ---
    m_nSelectedRow = -1; // 最初は何も選択されていない状態
    m_nCurrentPage = 1;
    UpdateButtons(); // ボタンを非表示/無効状態にする

    return TRUE;
}

/**
 * @brief キーボードメッセージをウィンドウにディスパッチされる前に横取りします。
 * @details EnterキーとEscapeキーによるダイアログ終了を無効化するために使用します。
 * @param pMsg メッセージ情報へのポインタ
 * @return メッセージを処理した場合はTRUE、デフォルト処理に任せる場合はFALSE。
 */
BOOL CMyDialog3::PreTranslateMessage(MSG *pMsg)
{
    // Escapeキーが押された場合、ダイアログが閉じるのを防ぎます。
    if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE)
    {
        return TRUE; // メッセージを処理済みとし、伝搬させない
    }

    // Enterキーが押された場合も同様に無効化
    if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN)
    {
        return TRUE; // メッセージを処理済みとし、伝搬させない
    }

    // その他のメッセージは、デフォルトの処理に任せます。
    return CDialogEx::PreTranslateMessage(pMsg);
}


/**
 * @brief 次ページボタンのクリックイベント(BN_CLICKED)を処理します。
 */
void CMyDialog3::OnNextPageClicked()
{
    // ページ番号をトグル（1なら2に、2なら1に）します。
    m_nCurrentPage = (m_nCurrentPage == 1) ? 2 : 1;
    // ボタンの表示を更新します。
    UpdateButtons();
    // フォーカスをグリッドに戻す
    m_gridCtrl.SetFocus();
}

/**
 * @brief 8つの機能ボタンのクリックイベント(BN_CLICKED)を一括で処理します。
 * @param nID クリックされたボタンのコントロールID
 */
void CMyDialog3::OnFuncButtonClicked(UINT nID)
{
    // 押されたボタンのインデックス(0-7)を取得
    int nButtonIndex = nID - IDC_BTN_FUNC1;

    // ボタンにGWLP_USERDATAとして保存しておいた「k」の値を取得します。
    int k = (int)::GetWindowLongPtr(m_btnFunc[nButtonIndex].GetSafeHwnd(), GWLP_USERDATA);

    // 現在選択されている行から「n」の値(1始まり)を取得します。
    int n = m_nSelectedRow + 1;

    // 親ウィンドウに「n」と「k」を別々にPostMessageで通知します。
    GetParent()->PostMessage(WM_EXECUTE_FUNCTION, (WPARAM)n, (LPARAM)k);
    // フォーカスをグリッドに戻す
    m_gridCtrl.SetFocus();
}

/**
 * @brief 現在の選択行とページ状態に基づき、全ボタンの表示/状態を更新します。
 */
void CMyDialog3::UpdateButtons()
{
    // 行が選択されていない場合
    if (m_nSelectedRow == -1)
    {
        // 全ての機能ボタンを「空」「無効」「表示」状態にする
        for (int i = 0; i < 8; ++i)
        {
            m_btnFunc[i].SetWindowText(_T(""));
            m_btnFunc[i].EnableWindow(FALSE);
            m_btnFunc[i].ShowWindow(SW_SHOW);
        }

        // 次ページボタンも同様に「空」「無効」「表示」状態にする
        m_btnNextPage.SetWindowText(_T(""));
        m_btnNextPage.EnableWindow(FALSE);
        m_btnNextPage.ShowWindow(SW_SHOW);

        return;
    }

    // --- 次ページボタンの状態制御 ---
    // 選択されている行の2列目のテキストを取得します。
    CString selectedCellText = m_gridCtrl.GetCellText(m_nSelectedRow, 1);
    // テキストが空欄の場合のみ、次ページボタンを有効にします。
    if (selectedCellText.IsEmpty())
    {
        m_btnNextPage.SetWindowText(_T(">>"));
        m_btnNextPage.EnableWindow(TRUE);
    }
    else
    {
        m_btnNextPage.SetWindowText(_T(""));
        m_btnNextPage.EnableWindow(FALSE);
    }
    m_btnNextPage.ShowWindow(SW_SHOW);

    // --- 機能ボタンの表示制御 ---
    if (m_nCurrentPage == 1) // ページ1の場合
    {
        for (int i = 0; i < 8; ++i)
        {
            int n = m_nSelectedRow + 1; // 1始まりの行番号
            int k = i + 1;              // 1から8までの機能インデックス

            // ラベルを "F(n-k)" の形式で設定します。
            CString label;
            label.Format(_T("F(%d-%d)"), n, k);
            m_btnFunc[i].SetWindowText(label);

            // ボタンウィンドウのユーザーデータ領域に、機能インデックス「k」の値を保存しておきます。
            // これにより、クリック時にどの機能かを簡単に特定できます。
            ::SetWindowLongPtr(m_btnFunc[i].GetSafeHwnd(), GWLP_USERDATA, (LONG_PTR)k);

            m_btnFunc[i].EnableWindow(TRUE);
            m_btnFunc[i].ShowWindow(SW_SHOW);
        }
    }
    else // ページ2の場合
    {
        // 先頭5ボタン (機能インデックス 9-13)
        for (int i = 0; i < 5; ++i)
        {
            int n = m_nSelectedRow + 1;
            int k = i + 9; // 機能インデックスは9から始まる

            CString label;
            label.Format(_T("F(%d-%d)"), n, k);
            m_btnFunc[i].SetWindowText(label);
            
            ::SetWindowLongPtr(m_btnFunc[i].GetSafeHwnd(), GWLP_USERDATA, (LONG_PTR)k);

            m_btnFunc[i].EnableWindow(TRUE);
            m_btnFunc[i].ShowWindow(SW_SHOW);
        }
        // 残り3ボタンは無効化
        for (int i = 5; i < 8; ++i)
        {
            m_btnFunc[i].SetWindowText(_T(""));
            m_btnFunc[i].EnableWindow(FALSE);
            m_btnFunc[i].ShowWindow(SW_SHOW);
        }
    }
}