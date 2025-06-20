// CMyDialog.cpp : 実装ファイル
//

#include "pch.h"
#include "MFCApplication4.h"
#include "afxdialogex.h"
#include "CMyDialog.h"

IMPLEMENT_DYNAMIC(CMyDialog, CDialogEx)

CMyDialog::CMyDialog(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_CMyDialog, pParent), m_pActiveGrid(nullptr)
    , m_nTotalWidth(0)
    , m_nTotalHeight(0)
    , m_nHScrollPos(0)
    , m_nVScrollPos(0)
{

}

CMyDialog::~CMyDialog()
{
}

void CMyDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CMyDialog, CDialogEx)
    ON_MESSAGE(WM_GRID_ACTIVATED, &CMyDialog::OnGridActivated)
	ON_MESSAGE(WM_GRID_CELL_CHANGED, &CMyDialog::OnGridCellChanged)
    ON_WM_HSCROLL()
    ON_WM_VSCROLL()
    ON_WM_SIZE()
    ON_WM_MOUSEWHEEL()
END_MESSAGE_MAP()

LRESULT CMyDialog::OnGridCellChanged(WPARAM wParam, LPARAM lParam)
{
	UINT nCtrlID = (UINT)wParam;
	int row = LOWORD(lParam);
	int col = HIWORD(lParam);
	// ここでセルの変更に対する処理を記述
	// (例) TRACE(L"Cell (%d, %d) in control %u changed.\n", row, col, nCtrlID);
	return 0;
}

// CMyDialog メッセージ ハンドラー

BOOL CMyDialog::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    // 水平・垂直両方のスクロールバースタイルを追加
    ModifyStyle(0, WS_VSCROLL | WS_HSCROLL);
    m_pActiveGrid = nullptr;

    int currentY = GRID_MARGIN_Y;
    CRect gridRect;

    // 10個のグリッドコントロールを動的に生成・配置
    for (int i = 0; i < GRID_COUNT; ++i)
    {
        UINT nID = AFX_IDW_PANE_FIRST + i;

		// 1. グリッドの行数・列数を設定
        // まず、グリッドの構成を設定します
        m_grids[i].SetupGrid(6, 2); // 例: 6行2列
        m_grids[i].SetRowHeight(22);
		// 2. 列の幅を設定
		m_grids[i].SetColumnWidth(0, 120);
		m_grids[i].SetColumnWidth(1, 220);

		// 3. 各セルのプロパティ（編集可否など）を設定
		// 仕様書に基づき、編集可能なセルを設定
		// SetCellEditableは、背景色も自動で白に変更します
		m_grids[i].SetCellEditable(1, 1, TRUE); // (行:2, 列:2)のセル
		m_grids[i].SetCellEditable(2, 1, TRUE); // (行:3, 列:2)のセル
		m_grids[i].SetCellEditable(3, 1, TRUE); // (行:4, 列:2)のセル
		m_grids[i].SetCellEditable(5, 1, TRUE); // (行:6, 列:2)のセル

		// 4. セルの初期テキストを設定 (オプション)
		for (int r = 0; r < 6; ++r)
		{
			for (int c = 0; c < 2; ++c)
			{
				CString str;
				str.Format(L"Cell (%d, %d)", r + 1, c + 1);
				m_grids[i].SetCellText(r, c, str);
			}
		}
			
		// CGridCtrlから1個あたりの正しい高さを取得する
		const int gridHeight = m_grids[i].GetRequiredHeight();
		const int gridWidth =  m_grids[i].GetRequiredWidth();

        // グリッドの位置とサイズを計算
        gridRect.SetRect(10, currentY, 10 + gridWidth, currentY + gridHeight);

        if (!m_grids[i].Create(gridRect, this, nID))
        {
            TRACE(L"Failed to create grid control #%d\n", i);
            continue; // 生成に失敗したら次へ
        }
        // 次のグリッドのY座標を、GetWindowRectに頼らずに直接計算する
        currentY += gridHeight + GRID_MARGIN_Y;
    }

    // 全てのコントロールを配置した後の合計の高さを保存
    m_nTotalHeight = currentY;

    // スクロール位置を初期化
    m_nHScrollPos = 0;
    m_nVScrollPos = 0;

    // スクロールバーの初期設定
    UpdateScrollInfo();

    return TRUE;
}

// スクロール情報を計算・更新するヘルパー関数
void CMyDialog::UpdateScrollInfo()
{
    CRect clientRect;
    GetClientRect(&clientRect);
    SCROLLINFO si;
    si.cbSize = sizeof(SCROLLINFO);

    // --- 垂直スクロールバー ---
    if (m_nTotalHeight > clientRect.Height())
    {
        si.fMask = SIF_RANGE | SIF_PAGE | SIF_POS;
        si.nMin = 0;
        si.nMax = m_nTotalHeight;
        si.nPage = clientRect.Height();
        si.nPos = m_nVScrollPos;
        SetScrollInfo(SB_VERT, &si, TRUE);
        ShowScrollBar(SB_VERT, TRUE);
    }
    else
    {
        m_nVScrollPos = 0;
        ShowScrollBar(SB_VERT, FALSE);
    }

    // --- 水平スクロールバー ---
    if (m_nTotalWidth > clientRect.Width())
    {
        si.fMask = SIF_RANGE | SIF_PAGE | SIF_POS;
        si.nMin = 0;
        si.nMax = m_nTotalWidth;
        si.nPage = clientRect.Width();
        si.nPos = m_nHScrollPos;
        SetScrollInfo(SB_HORZ, &si, TRUE);
        ShowScrollBar(SB_HORZ, TRUE);
    }
    else
    {
        m_nHScrollPos = 0;
        ShowScrollBar(SB_HORZ, FALSE);
    }
}

// ダイアログのサイズが変更されたときの処理
void CMyDialog::OnSize(UINT nType, int cx, int cy)
{
    CDialogEx::OnSize(nType, cx, cy);
    
    // ウィンドウの準備ができていればスクロール情報を更新
    if (GetSafeHwnd() != nullptr)
    {
        UpdateScrollInfo();
    }
}

// 垂直スクロールバーが操作されたときの処理
void CMyDialog::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
    CRect clientRect;
    GetClientRect(&clientRect);
    int currentPos = m_nVScrollPos;
    int lineHeight = 40; // 1回の上下キーでスクロールする量

    switch (nSBCode)
    {
    case SB_LINEUP:    currentPos -= lineHeight; break;
    case SB_LINEDOWN:  currentPos += lineHeight; break;
    case SB_PAGEUP:    currentPos -= GetScrollLimit(SB_VERT) > 0 ? clientRect.Height() : lineHeight; break;
    case SB_PAGEDOWN:  currentPos += GetScrollLimit(SB_VERT) > 0 ? clientRect.Height() : lineHeight; break;
    case SB_THUMBTRACK: currentPos = nPos; break;
    case SB_TOP:       currentPos = 0; break;
    case SB_BOTTOM:    currentPos = GetScrollLimit(SB_VERT); break;
    }

    // --- ▼▼▼ この部分が重要です ▼▼▼ ---
    // 新しいスクロール位置が有効な範囲内に収まるように強制します (クランプ処理)。
    // これにより、スクロール範囲の限界を超える「バウンド」や「スナップ」を防ぎます。
    int maxScrollPos = GetScrollLimit(SB_VERT);
    if (currentPos < 0) currentPos = 0;
    if (currentPos > maxScrollPos) currentPos = maxScrollPos;
    // --- ▲▲▲ この部分が重要です ▲▲▲ ---


    // 位置に変化がなければ何もしない
    if (currentPos == m_nVScrollPos) return;

    // 画面を移動させる量を計算
    int scrollAmount = m_nVScrollPos - currentPos;
    m_nVScrollPos = currentPos;
    
    // ウィンドウをスクロールし、スクロールバーの位置を更新
    ScrollWindow(0, scrollAmount);
    SetScrollPos(SB_VERT, m_nVScrollPos, TRUE);

    CDialogEx::OnVScroll(nSBCode, nPos, pScrollBar);
}

// 水平スクロールバーが操作されたときの処理
void CMyDialog::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
    CRect clientRect;
    GetClientRect(&clientRect);
    int currentPos = m_nHScrollPos;
    int lineHeight = 40; // 1回の左右キーでスクロールする量

    switch (nSBCode)
    {
    case SB_LINELEFT:  currentPos -= lineHeight; break;
    case SB_LINERIGHT: currentPos += lineHeight; break;
    case SB_PAGELEFT:  currentPos -= GetScrollLimit(SB_HORZ) > 0 ? clientRect.Width() : lineHeight; break;
    case SB_PAGERIGHT: currentPos += GetScrollLimit(SB_HORZ) > 0 ? clientRect.Width() : lineHeight; break;
    case SB_THUMBTRACK: currentPos = nPos; break;
    case SB_LEFT:      currentPos = 0; break;
    case SB_RIGHT:     currentPos = GetScrollLimit(SB_HORZ); break;
    }

    // --- ▼▼▼ この部分が重要です ▼▼▼ ---
    // 新しいスクロール位置が有効な範囲内に収まるように強制します (クランプ処理)。
    int maxScrollPos = GetScrollLimit(SB_HORZ);
    if (currentPos < 0) currentPos = 0;
    if (currentPos > maxScrollPos) currentPos = maxScrollPos;
    // --- ▲▲▲ この部分が重要です ▲▲▲ ---


    // 位置に変化がなければ何もしない
    if (currentPos == m_nHScrollPos) return;

    // 画面を移動させる量を計算
    int scrollAmount = m_nHScrollPos - currentPos;
    m_nHScrollPos = currentPos;

    // ウィンドウをスクロールし、スクロールバーの位置を更新
    ScrollWindow(scrollAmount, 0);
    SetScrollPos(SB_HORZ, m_nHScrollPos, TRUE);

    CDialogEx::OnHScroll(nSBCode, nPos, pScrollBar);
}

// マウスホイールが操作されたときの処理
BOOL CMyDialog::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
    // Shiftキーが押されている場合は水平スクロール
    if (nFlags & MK_SHIFT)
    {
        if (zDelta > 0) OnHScroll(SB_LINELEFT, 0, nullptr);
        else OnHScroll(SB_LINERIGHT, 0, nullptr);
    }
    else // それ以外は垂直スクロール
    {
        if (zDelta > 0) OnVScroll(SB_LINEUP, 0, nullptr);
        else OnVScroll(SB_LINEDOWN, 0, nullptr);
    }

    return TRUE;
}

BOOL CMyDialog::PreTranslateMessage(MSG* pMsg)
{
    if (pMsg->message == WM_KEYDOWN)
    {
        // アクティブなグリッドがあり、かつ矢印キーが押された場合
        if (m_pActiveGrid &&
            (pMsg->wParam == VK_UP || pMsg->wParam == VK_DOWN || pMsg->wParam == VK_LEFT || pMsg->wParam == VK_RIGHT))
        {
            int currentIndex = -1;
            // 現在アクティブなグリッドのインデックスを探す
            for (int i = 0; i < GRID_COUNT; ++i)
            {
                if (&m_grids[i] == m_pActiveGrid)
                {
                    currentIndex = i;
                    break;
                }
            }

            if (currentIndex != -1)
            {
                int newIndex = -1;

                switch (pMsg->wParam)
                {
                case VK_UP:
                    if (currentIndex > 0) newIndex = currentIndex - 1;
                    break;
                case VK_DOWN:
                    if (currentIndex < GRID_COUNT - 1) newIndex = currentIndex + 1;
                    break;
                case VK_LEFT:
                    break;
                case VK_RIGHT:
                    break;
                }

                // 新しい移動先があれば、アクティブグリッドを更新
                if (newIndex != -1)
                {
                    ActivateGrid(&m_grids[newIndex]);
                }
            }
            return TRUE; // 矢印キーのメッセージを処理したので、ここで終了
        }
    }
    return CDialogEx::PreTranslateMessage(pMsg);
}

void CMyDialog::ActivateGrid(CGridCtrl* pGridToActivate)
{
    if (pGridToActivate && pGridToActivate != m_pActiveGrid)
    {
        // 新しいグリッドをアクティブとして記録
        m_pActiveGrid = pGridToActivate;

        // 全てのグリッドコントロールをループし、アクティブ/非アクティブを設定
        for (int i = 0; i < GRID_COUNT; ++i)
        {
            m_grids[i].SetActive(&m_grids[i] == m_pActiveGrid);
        }
        // 新しくアクティブになったグリッドにフォーカスを移動
        m_pActiveGrid->SetFocus();
    }
    // 新しくアクティブになったグリッドが表示されるようにスクロールさせる
    EnsureGridVisible(pGridToActivate);
}

LRESULT CMyDialog::OnGridActivated(WPARAM wParam, LPARAM lParam)
{
    UINT nCtrlID = (UINT)wParam;
    CGridCtrl* pActivatedGrid = (CGridCtrl*)GetDlgItem(nCtrlID);
    ActivateGrid(pActivatedGrid);
    return 0;
}


void CMyDialog::EnsureGridVisible(CGridCtrl* pGrid)
{
    if (!pGrid || !pGrid->GetSafeHwnd()) return;

    CRect clientRect;
    GetClientRect(&clientRect); // ダイアログの現在の表示領域

    CRect gridRect;
    pGrid->GetWindowRect(&gridRect); // グリッドのスクリーン座標での位置
    ScreenToClient(&gridRect);       // ダイアログのクライアント座標に変換（スクロール後の見た目上の位置）

    // --- 垂直スクロール量の計算 ---
    int newVPos = m_nVScrollPos;
    if (gridRect.top < 0) // 上にはみ出している場合
    {
        // はみ出した分だけスクロール位置を戻す
        newVPos += gridRect.top;
    }
    else if (gridRect.bottom > clientRect.bottom) // 下にはみ出している場合
    {
        // はみ出した分だけスクロール位置を進める
        newVPos += (gridRect.bottom - clientRect.bottom);
    }
    
    // --- 水平スクロール量の計算 ---
    int newHPos = m_nHScrollPos;
    if (gridRect.left < 0) // 左にはみ出している場合
    {
        newHPos += gridRect.left;
    }
    else if (gridRect.right > clientRect.right) // 右にはみ出している場合
    {
        newHPos += (gridRect.right - clientRect.right);
    }

    // スクロール位置が変化した場合のみ、実際のスクロール処理を実行
    if (newVPos != m_nVScrollPos || newHPos != m_nHScrollPos)
    {
        // 範囲内に収める
        newHPos = max(0, min(newHPos, GetScrollLimit(SB_HORZ)));
        newVPos = max(0, min(newVPos, GetScrollLimit(SB_VERT)));

        // 画面を移動させる量を計算
        int dx = m_nHScrollPos - newHPos;
        int dy = m_nVScrollPos - newVPos;

        // 新しい位置を保存
        m_nHScrollPos = newHPos;
        m_nVScrollPos = newVPos;

        // ウィンドウをスクロール
        ScrollWindow(dx, dy);
        
        // スクロールバーの位置を更新
        SetScrollPos(SB_HORZ, m_nHScrollPos, TRUE);
        SetScrollPos(SB_VERT, m_nVScrollPos, TRUE);
    }
}
