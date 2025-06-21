// CMyDialog3.cpp : 実装ファイル
//

#include "pch.h"
#include "resource.h"
#include "MFCApplication4.h"
#include "afxdialogex.h"
#include "CMyDialog3.h"

IMPLEMENT_DYNAMIC(CMyDialog3, CDialogEx)

CMyDialog3::CMyDialog3(CWnd *pParent /*=nullptr*/)
    : CDialogEx(IDD, pParent), m_nSelectedRow(-1), m_nCurrentPage(1)
{
}

CMyDialog3::~CMyDialog3()
{
}

void CMyDialog3::DoDataExchange(CDataExchange *pDX)
{
    CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CMyDialog3, CDialogEx)
ON_NOTIFY(GCN_SELCHANGED, IDC_GRID_CTRL, &CMyDialog3::OnGridSelChanged)
ON_BN_CLICKED(IDC_BTN_NEXTPAGE, &CMyDialog3::OnNextPageClicked)
ON_COMMAND_RANGE(IDC_BTN_FUNC1, IDC_BTN_FUNC8, &CMyDialog3::OnFuncButtonClicked)

END_MESSAGE_MAP()

void CMyDialog3::OnGridSelChanged(NMHDR *pNMHDR, LRESULT *pResult)
{
    NM_GRIDVIEW *pNMGV = (NM_GRIDVIEW *)pNMHDR;

    m_nSelectedRow = pNMGV->iRow; // 0始まりの行インデックスを保存
    m_nCurrentPage = 1;           // ページを1にリセット
    UpdateButtons();              // ボタンの状態を更新

    *pResult = 0;
}

// CMyDialog3 メッセージ ハンドラー

void CMyDialog3::OnOK()
{
    // この関数をオーバーライドし、中身を空にします。
    // 基底クラスの CDialogEx::OnOK() を呼び出さないことで、
    // Enterキーを押してもダイアログが閉じなくなります。
}

BOOL CMyDialog3::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    // --- レイアウト定数の定義 ---
    const int nMargin = 10;
    const int nFuncBtnWidth = 48;
    const int nFuncBtnHeight = 24;
    const int nNextBtnWidth = 24;
    const int nBtnSpacing = 1;

    m_gridCtrl.SetupGrid(16, 2);
    m_gridCtrl.SetColumnWidth(0, 120);
    m_gridCtrl.SetColumnWidth(1, 220);

    // --- CGridCtrlの生成とセットアップ ---
    CRect gridRect;
    // グリッドのサイズを計算 (幅はダイアログのクライアント領域に合わせる)
    int gridHeight = m_gridCtrl.GetRequiredHeight();                               // 16行分の高さを取得
    gridRect.SetRect(nMargin, nMargin, 120 + 220 + nMargin, nMargin + gridHeight); // 固定幅で設定

    // 仕様書通りにセルを設定
    for (int i = 0; i < 16; ++i)
    {
        CString caption;
        caption.Format(_T("項目 %d"), i + 1);
        m_gridCtrl.SetCellText(i, 0, caption);
        m_gridCtrl.SetCellEditable(i, 0, FALSE);
        m_gridCtrl.SetCellEditable(i, 1, TRUE);
    }
    // グリッドを生成
    m_gridCtrl.Create(gridRect, this, IDC_GRID_CTRL);
    m_gridCtrl.RedrawGrid();

    // --- ボタン群の生成 ---
    // ボタンを配置する開始Y座標（グリッドの下）
    int currentY = gridRect.bottom + nMargin;
    int currentX = nMargin;

    // 8個の機能ボタンをループで生成
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
    m_nSelectedRow = -1;
    m_nCurrentPage = 1;
    UpdateButtons(); // ボタンを非表示/無効状態にする

    return TRUE;
}

BOOL CMyDialog3::PreTranslateMessage(MSG *pMsg)
{
    // Escapeキーが押された場合、メッセージをここで処理済み（TRUE）とし、
    // それ以上伝搬させないことで、ダイアログが閉じるのを防ぎます。
    if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE)
    {
        return TRUE; // メッセージを「食べた」ことにして、何もしない
    }

    // Enterキーが押された場合も同様に無効化
    if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN)
    {
        return TRUE; // Enterキーもここで握りつぶす
    }

    // その他のメッセージは、デフォルトの処理に任せます。
    return CDialogEx::PreTranslateMessage(pMsg);
}

void CMyDialog3::OnNextPageClicked()
{
    // ページをトグル
    m_nCurrentPage = (m_nCurrentPage == 1) ? 2 : 1;
    UpdateButtons();
    m_gridCtrl.SetFocus();
}

void CMyDialog3::OnFuncButtonClicked(UINT nID)
{
    // 押されたボタンのインデックス(0-7)を取得
    int nButtonIndex = nID - IDC_BTN_FUNC1;

    // ボタンから「k」の値を取得
    int k = (int)::GetWindowLongPtr(m_btnFunc[nButtonIndex].GetSafeHwnd(), GWLP_USERDATA);

    // 現在選択されている行から「n」の値を取得
    int n = m_nSelectedRow + 1;

    // 親ウィンドウに「n」と「k」を別々に通知する
    GetParent()->PostMessage(WM_EXECUTE_FUNCTION, (WPARAM)n, (LPARAM)k);
    m_gridCtrl.SetFocus();
}

void CMyDialog3::UpdateButtons()
{
    if (m_nSelectedRow == -1) // 行が選択されていない場合
    {
        // 全ての機能ボタンを「空」「無効」「表示」状態にする
        for (int i = 0; i < 8; ++i)
        {
            m_btnFunc[i].SetWindowText(_T("")); // 1. ボタンの文字列を空にする
            m_btnFunc[i].EnableWindow(FALSE);   // 2. ボタンを押せないように無効化する
            m_btnFunc[i].ShowWindow(SW_SHOW);   // 3. ボタンを表示状態にする
        }

        // 次ページボタンも同様に「空」「無効」「表示」状態にする
        m_btnNextPage.SetWindowText(_T(""));
        m_btnNextPage.EnableWindow(FALSE);
        m_btnNextPage.ShowWindow(SW_SHOW);

        return;
    }

    // --- 次ページボタンの状態制御 ---
    CString selectedCellText = m_gridCtrl.GetCellText(m_nSelectedRow, 1);
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
    if (m_nCurrentPage == 1)
    {
        for (int i = 0; i < 8; ++i)
        {
            int n = m_nSelectedRow + 1; // 1始まりの行番号
            int k = i + 1;              // ハイフンの右側の数値

            CString label;
            label.Format(_T("F(%d-%d)"), n, k);
            m_btnFunc[i].SetWindowText(label);

            // ボタンには、計算結果ではなく「k」の値を保存します
            ::SetWindowLongPtr(m_btnFunc[i].GetSafeHwnd(), GWLP_USERDATA, (LONG_PTR)k);

            m_btnFunc[i].EnableWindow(TRUE);
            m_btnFunc[i].ShowWindow(SW_SHOW);
        }
    }
    else // ページ2の場合
    {
        // 先頭5ボタン
        for (int i = 0; i < 5; ++i)
        {
            int n = m_nSelectedRow + 1;
            int k = i + 9;

            CString label;
            label.Format(_T("F(%d-%d)"), n, k);
            m_btnFunc[i].SetWindowText(label);

            // ボタンには、計算結果ではなく「k」の値を保存します
            ::SetWindowLongPtr(m_btnFunc[i].GetSafeHwnd(), GWLP_USERDATA, (LONG_PTR)k);

            m_btnFunc[i].EnableWindow(TRUE);
            m_btnFunc[i].ShowWindow(SW_SHOW);
        }
        // 残り3ボタン
        for (int i = 5; i < 8; ++i)
        {
            m_btnFunc[i].SetWindowText(_T(""));
            m_btnFunc[i].EnableWindow(FALSE);
            m_btnFunc[i].ShowWindow(SW_SHOW);
        }
    }
}
