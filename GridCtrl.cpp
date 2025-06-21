/**
 * @file GridCtrl.cpp
 * @brief 汎用的な表形式（グリッド）カスタムコントロールのクラス実装
 * @author C++/MFCコーディング (AI)
 * @date 2025/06/22
 */
#include "pch.h"
#include "GridCtrl.h"

 // --- 定数定義 ---

 // 色の定義
const COLORREF CLR_WHITE = RGB(255, 255, 255);       ///< 白色
const COLORREF CLR_GRAY = RGB(240, 240, 240);       ///< 編集不可セルのデフォルト背景色 (灰色)
const COLORREF CLR_BLUE_BG = RGB(0, 0, 230);        ///< 選択状態のセルの背景色 (青色)
const COLORREF CLR_YELLOW = RGB(255, 255, 224);   ///< 編集状態のセル or 空欄の編集可能セルの背景色 (薄黄色)
const COLORREF CLR_BORDER = RGB(192, 192, 192);   ///< グリッドの枠線色 (銀色)
const COLORREF CLR_ORANGE = RGB(255, 192, 128);   ///< 正の数の場合の背景色
const COLORREF CLR_BLUE_TEXT = RGB(0, 0, 255);    ///< 正の数の場合の文字色
const COLORREF CLR_BLUE2_BG = RGB(120, 210, 230); ///< 負の数の場合の背景色
const COLORREF CLR_RED_TEXT = RGB(255, 0, 0);     ///< 負の数の場合の文字色
const COLORREF CLR_BLACK = RGB(0, 0, 0);          ///< デフォルトの文字色

/**
 * @brief CGridCtrlクラスのコンストラクタ
 * @details 各メンバ変数を初期値に設定します。
 */
CGridCtrl::CGridCtrl(int nMaxVisibleRows)
    : m_nRows(0), m_nCols(0),
    m_nRowHeight(22),
    m_defaultBgColor(CLR_GRAY),
    m_selectedCell(-1, -1),
    m_bIsActive(FALSE),
    m_pEdit(nullptr),
    m_nTopRow(0)
{
    m_nMaxVisibleRows = nMaxVisibleRows;
}

/**
 * @brief CGridCtrlクラスのデストラクタ
 * @details インプレイス編集中のエディットコントロールが残っていれば破棄します。
 */
CGridCtrl::~CGridCtrl()
{
    DestroyInPlaceEdit(FALSE);
}


/**
 * @brief グリッドの基本構成（行数・列数）を設定します。
 * @details この関数は、他のプロパティ設定の前に呼び出す必要があります。
 * @param[in] nRows 設定する行数。
 * @param[in] nCols 設定する列数。
 * @return セットアップが成功した場合はTRUE。
 */
BOOL CGridCtrl::SetupGrid(int nRows, int nCols)
{
    if (nRows <= 0 || nCols <= 0)
    {
        ASSERT(FALSE);
        return FALSE;
    }
    m_nRows = nRows;
    m_nCols = nCols;

    // セルと列幅の情報を保持するベクターをリサイズ
    m_cells.resize(m_nRows * m_nCols);
    m_colWidths.resize(m_nCols);

    // デフォルト値で初期化
    for (int i = 0; i < m_nCols; ++i)
    {
        m_colWidths[i] = 80; // デフォルトの列幅
    }
    for (size_t i = 0; i < m_cells.size(); ++i)
    {
        m_cells[i].m_bgColor = m_defaultBgColor;
        m_cells[i].m_bEditable = FALSE;
    }

    m_nTopRow = 0;
    return TRUE;
}

/**
 * @brief 全ての行の高さを設定します。
 * @param[in] nHeight ピクセル単位での行の高さ。
 */
void CGridCtrl::SetRowHeight(int nHeight)
{
    if (nHeight > 0)
        m_nRowHeight = nHeight;
}

/**
 * @brief 指定した列の幅を設定します。
 * @param[in] nCol 幅を設定する列のインデックス (0始まり)。
 * @param[in] nWidth ピクセル単位での列の幅。
 */
void CGridCtrl::SetColumnWidth(int nCol, int nWidth)
{
    if (nCol >= 0 && nCol < m_nCols && nWidth > 0)
    {
        m_colWidths[nCol] = nWidth;
    }
}

/**
 * @brief 編集不可セルのデフォルト背景色を設定します。
 * @param[in] color 設定する色 (COLORREF)。
 */
void CGridCtrl::SetDefaultBgColor(COLORREF color)
{
    m_defaultBgColor = color;
}

/**
 * @brief 指定したセルにテキストを設定します。
 * @param[in] nRow 行インデックス (0始まり)
 * @param[in] nCol 列インデックス (0始まり)
 * @param[in] strText 設定するテキスト
 */
void CGridCtrl::SetCellText(int nRow, int nCol, const CString& strText)
{
    int index = GetCellIndex(nRow, nCol);
    if (index != -1)
        m_cells[index].m_strText = strText;
}

/**
 * @brief 指定したセルのテキストを取得します。
 * @param[in] nRow 行インデックス (0始まり)
 * @param[in] nCol 列インデックス (0始まり)
 * @return セルのテキスト
 */
CString CGridCtrl::GetCellText(int nRow, int nCol) const
{
    int index = GetCellIndex(nRow, nCol);
    return (index != -1) ? m_cells[index].m_strText : CString();
}

/**
 * @brief 指定したセルの編集可否を設定します。
 * @details 編集可能に設定すると、デフォルトで背景色が白になります。
 * @param[in] nRow 行インデックス (0始まり)
 * @param[in] nCol 列インデックス (0始まり)
 * @param[in] bEditable 編集可能にする場合はTRUE
 */
void CGridCtrl::SetCellEditable(int nRow, int nCol, BOOL bEditable)
{
    int index = GetCellIndex(nRow, nCol);
    if (index != -1)
    {
        m_cells[index].m_bEditable = bEditable;
        // 編集可能なセルは背景色を白にする（デフォルトの挙動）
        m_cells[index].m_bgColor = bEditable ? CLR_WHITE : m_defaultBgColor;
    }
}

/**
 * @brief 指定したセルが編集可能かを取得します。
 * @param[in] nRow 行インデックス (0始まり)
 * @param[in] nCol 列インデックス (0始まり)
 * @return 編集可能な場合はTRUE
 */
BOOL CGridCtrl::IsCellEditable(int nRow, int nCol) const
{
    int index = GetCellIndex(nRow, nCol);
    return (index != -1) ? m_cells[index].m_bEditable : FALSE;
}

/**
 * @brief 指定したセルの背景色を設定します。
 * @param[in] nRow 行インデックス (0始まり)
 * @param[in] nCol 列インデックス (0始まり)
 * @param[in] color 設定する色
 */
void CGridCtrl::SetCellBgColor(int nRow, int nCol, COLORREF color)
{
    int index = GetCellIndex(nRow, nCol);
    if (index != -1)
        m_cells[index].m_bgColor = color;
}


// BEGIN_MESSAGE_MAPブロック
// Windowsメッセージと、それを処理するクラスのメンバ関数（ハンドラ）を関連付けます。
BEGIN_MESSAGE_MAP(CGridCtrl, CWnd)
    ON_WM_PAINT()
    ON_WM_LBUTTONDOWN()
    ON_WM_GETDLGCODE()
    ON_WM_KEYDOWN()
    ON_WM_SETFOCUS()
    ON_WM_KILLFOCUS()
    ON_WM_VSCROLL()
    ON_WM_MOUSEWHEEL()
    ON_WM_CREATE()
END_MESSAGE_MAP()

/**
 * @brief ウィンドウ作成時に呼び出されます (WM_CREATE)。
 * @details ウィンドウハンドルが有効になるこのタイミングで、スクロールバーの初期化を行うのが最適です。
 * @param[in] lpCreateStruct 作成情報
 * @return 成功なら0、失敗なら-1
 */
int CGridCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CWnd::OnCreate(lpCreateStruct) == -1)
        return -1;

    UpdateScrollbar();
    return 0;
}

/**
 * @brief コントロールのウィンドウを生成・初期化します。
 * @param[in] rect 親ウィンドウのクライアント座標におけるコントロールの位置とサイズ。
 * @param[in] pParentWnd 親ウィンドウへのポインタ。
 * @param[in] nID コントロールID。
 * @return 成功した場合はTRUE、失敗した場合はFALSE。
 */
BOOL CGridCtrl::Create(const RECT& rect, CWnd* pParentWnd, UINT nID)
{
    // このコントロール用のウィンドウクラスを登録
    WNDCLASS wndcls;
    ::ZeroMemory(&wndcls, sizeof(wndcls));
    wndcls.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
    wndcls.lpfnWndProc = ::DefWindowProc;
    wndcls.hInstance = AfxGetInstanceHandle();
    wndcls.hCursor = ::LoadCursor(NULL, IDC_ARROW);
    wndcls.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wndcls.lpszMenuName = NULL;
    wndcls.lpszClassName = _T("MyGridCtrl");

    if (!AfxRegisterClass(&wndcls))
    {
        TRACE(_T("Failed to register window class\n"));
        return FALSE;
    }

    // 列幅と行高の合計から、コントロールの正しいサイズを計算
    int totalWidth = 0;
    for (int i = 0; i < m_nCols; ++i)
    {
        totalWidth += m_colWidths[i];
    }
    int totalHeight = m_nRows * m_nRowHeight;

    // 渡されたrectの左上座標は維持し、サイズを計算値で上書き
    CRect newRect = rect;
    newRect.right = newRect.left + totalWidth;
    newRect.bottom = newRect.top + totalHeight;

    return CWnd::Create(_T("MyGridCtrl"), _T(""), WS_CHILD | WS_VISIBLE | WS_BORDER | WS_TABSTOP | WS_VSCROLL, rect, pParentWnd, nID);
}

/**
 * @brief 描画イベント(WM_PAINT)を処理します。
 * @details ダブルバッファリングを用いて、グリッドの内容全体を描画します。
 * セルの状態（編集可否、内容、選択状態など）に応じて動的に色を変えて描画します。
 */
void CGridCtrl::OnPaint()
{
    CPaintDC dc(this);
    CRect clientRect;
    GetClientRect(&clientRect);

    // ダブルバッファリング開始（メモリDCに描画し、最後に一括で画面に転送）
    CDC memDC;
    memDC.CreateCompatibleDC(&dc);
    CBitmap bmp;
    bmp.CreateCompatibleBitmap(&dc, clientRect.Width(), clientRect.Height());
    CBitmap* pOldBmp = memDC.SelectObject(&bmp);

    memDC.FillSolidRect(clientRect, CLR_WHITE);

    CPen pen(PS_SOLID, 1, CLR_BORDER);
    CPen* pOldPen = memDC.SelectObject(&pen);

    // 表示する行の範囲を計算 (スクロール位置を考慮)
    int nStartRow = m_nTopRow;
    int nEndRow = min(m_nRows, m_nTopRow + m_nMaxVisibleRows);

    // 表示範囲のセルを描画
    for (int row = nStartRow; row < nEndRow; ++row)
    {
        for (int col = 0; col < m_nCols; ++col)
        {
            CRect cellRect = GetCellRect(row, col);
            int index = GetCellIndex(row, col);
            if (index == -1) continue;

            CellInfo& cell = m_cells[index];
            COLORREF bgColor = cell.m_bgColor;
            COLORREF textColor = CLR_BLACK;

            // 編集可能セルの場合、内容に応じて色を上書き
            if (cell.m_bEditable)
            {
                CString strText = cell.m_strText;
                strText.Trim();
                if (strText.IsEmpty())
                {
                    bgColor = CLR_YELLOW; // 空欄
                }
                else
                {
                    LPTSTR pEnd;
                    double dValue = _tcstod(strText, &pEnd);
                    if (*pEnd == _T('\0')) // 数値の場合
                    {
                        if (dValue < 0) { bgColor = CLR_BLUE2_BG; textColor = CLR_RED_TEXT; } // 負の数
                        else if (dValue > 0) { bgColor = CLR_ORANGE; textColor = CLR_BLUE_TEXT; } // 正の数
                    }
                }
            }

            // 選択/編集状態の色を最優先で適用
            if (m_selectedCell.x == col && m_selectedCell.y == row)
            {
                bgColor = (m_pEdit != nullptr) ? CLR_YELLOW : CLR_BLUE_BG;
                if (m_pEdit == nullptr) textColor = CLR_BLACK;
            }

            // セルの背景と枠線を描画
            memDC.FillSolidRect(cellRect, bgColor);
            memDC.MoveTo(cellRect.left, cellRect.top);
            memDC.LineTo(cellRect.right - 1, cellRect.top);
            memDC.LineTo(cellRect.right - 1, cellRect.bottom - 1);
            memDC.LineTo(cellRect.left, cellRect.bottom - 1);
            memDC.LineTo(cellRect.left, cellRect.top);

            // セルのテキストを描画
            memDC.SetBkMode(TRANSPARENT);
            memDC.SetTextColor(textColor);
            cellRect.DeflateRect(4, 2);
            memDC.DrawText(cell.m_strText, cellRect, DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
        }
    }
    memDC.SelectObject(pOldPen);

    // フォーカス枠を描画
    if (GetFocus() == this && m_selectedCell.x != -1)
    {
        CRect focusRect = GetCellRect(m_selectedCell.y, m_selectedCell.x);
        dc.DrawFocusRect(focusRect);
    }

    // このグリッドがアクティブな場合、外枠を青で囲む
    if (m_bIsActive)
    {
        CPen borderPen(PS_SOLID, 3, RGB(0, 0, 255));
        CBrush* pOldBrush = (CBrush*)memDC.SelectStockObject(NULL_BRUSH);
        CPen* pOldPenBorder = memDC.SelectObject(&borderPen);
        CRect rcClient;
        GetClientRect(&rcClient);
        rcClient.DeflateRect(1, 1);
        memDC.Rectangle(rcClient);
        memDC.SelectObject(pOldBrush);
        memDC.SelectObject(pOldPenBorder);
    }

    // メモリDCから画面DCへ一括転送
    dc.BitBlt(0, 0, clientRect.Width(), clientRect.Height(), &memDC, 0, 0, SRCCOPY);
    memDC.SelectObject(pOldBmp);
}

/**
 * @brief マウス左ボタン押下イベント(WM_LBUTTONDOWN)を処理します。
 * @details クリックされたセルを選択状態にし、編集可能であれば編集モードを開始します。
 * @param[in] nFlags 修飾キーの状態
 * @param[in] point マウスカーソルのクライアント座標
 */
void CGridCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
    // このグリッドがクリックされたことを親ウィンドウに通知
    GetParent()->PostMessage(WM_GRID_ACTIVATED, GetDlgCtrlID());
    SetFocus();

    CPoint cell = HitTest(point);
    if (cell.x == -1) // グリッド外
    {
        DestroyInPlaceEdit(TRUE);
        m_selectedCell = CPoint(-1, -1);
        Invalidate();
        return;
    }

    if (!IsCellEditable(cell.y, cell.x))
    {
        return; // 編集不可セルは選択しない
    }

    if (cell == m_selectedCell) // 選択中のセルを再クリック
    {
        if (m_pEdit == nullptr) CreateInPlaceEdit(); // 編集開始
    }
    else // 別の編集可能セルをクリック
    {
        DestroyInPlaceEdit(TRUE); // 前の編集を確定
        m_selectedCell = cell;

        // 親ウィンドウに行選択の変更を通知 (WM_NOTIFY)
        NM_GRIDVIEW nm;
        nm.hdr.hwndFrom = GetSafeHwnd();
        nm.hdr.idFrom = GetDlgCtrlID();
        nm.hdr.code = GCN_SELCHANGED;
        nm.iRow = m_selectedCell.y;
        nm.iCol = m_selectedCell.x;
        GetParent()->SendMessage(WM_NOTIFY, GetDlgCtrlID(), (LPARAM)&nm);

        Invalidate();
    }
}

/**
 * @brief ダイアログナビゲーションのためのキー種別を返します (WM_GETDLGCODE)。
 * @details カーソルキーや文字キーをダイアログに奪われず、自前で処理するために必要です。
 * @return DLGC_WANTARROWS | DLGC_WANTCHARS
 */
UINT CGridCtrl::OnGetDlgCode()
{
    return DLGC_WANTARROWS | DLGC_WANTCHARS;
}

/**
 * @brief キー押下イベント(WM_KEYDOWN)を処理します。
 * @details カーソルキーによるセル移動、F2/Enterキーによる編集開始などを実装します。
 * グリッドの端に到達した場合は、親ウィンドウに通知します。
 * @param[in] nChar 仮想キーコード
 * @param[in] nRepCnt キーのリピート回数
 * @param[in] nFlags 修飾キーの状態
 */
void CGridCtrl::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
    if (m_pEdit)
    {
        CWnd::OnKeyDown(nChar, nRepCnt, nFlags);
        return;
    }

    switch (nChar)
    {
    case VK_UP:
    case VK_DOWN:
    case VK_LEFT:
    case VK_RIGHT:
    {
        if (m_selectedCell.x == -1) // 何も選択されていない場合
        {
            // 親ウィンドウにナビゲーションを依頼
            GetParent()->PostMessage(WM_GRID_NAV_BOUNDARY_HIT, (WPARAM)nChar, (LPARAM)GetDlgCtrlID());
            return;
        }

        BOOL bMoved = FALSE;
        int dx = (nChar == VK_LEFT) ? -1 : (nChar == VK_RIGHT) ? 1 : 0;
        int dy = (nChar == VK_UP) ? -1 : (nChar == VK_DOWN) ? 1 : 0;
        CPoint searchCell = m_selectedCell;

        // 押された方向に編集可能なセルを探す
        while (TRUE)
        {
            searchCell.x += dx;
            searchCell.y += dy;
            if (searchCell.x < 0 || searchCell.x >= m_nCols || searchCell.y < 0 || searchCell.y >= m_nRows) break;
            if (IsCellEditable(searchCell.y, searchCell.x))
            {
                m_selectedCell = searchCell;
                bMoved = TRUE;
                break;
            }
        }

        if (bMoved) // 移動できた場合
        {
            NM_GRIDVIEW nm;
            nm.hdr.hwndFrom = GetSafeHwnd();
            nm.hdr.idFrom = GetDlgCtrlID();
            nm.hdr.code = GCN_SELCHANGED;
            nm.iRow = m_selectedCell.y;
            nm.iCol = m_selectedCell.x;
            GetParent()->SendMessage(WM_NOTIFY, GetDlgCtrlID(), (LPARAM)&nm);
            EnsureCellVisible(m_selectedCell.y, m_selectedCell.x);
            Invalidate();
        }
        else // 端に到達した場合
        {
            GetParent()->PostMessage(WM_GRID_NAV_BOUNDARY_HIT, (WPARAM)nChar, (LPARAM)GetDlgCtrlID());
        }
        break;
    }
    case VK_PRIOR: // Page Up
    case VK_NEXT:  // Page Down
    {
        if (m_selectedCell.x == -1) break;
        int nStep = (nChar == VK_PRIOR) ? -m_nMaxVisibleRows : m_nMaxVisibleRows;
        int nDirection = (nChar == VK_PRIOR) ? -1 : 1;

        // 1. まず目標となる行を計算
        int targetRow = m_selectedCell.y + nStep;
        if (targetRow < 0)
            targetRow = 0;
        if (targetRow >= m_nRows)
            targetRow = m_nRows - 1;

        CPoint newSel(-1, -1);

        // 2. 同じ列内で、目標地点から近い編集可能セルを探す
        int searchRow = targetRow;
        while (searchRow >= 0 && searchRow < m_nRows)
        {
            if (IsCellEditable(searchRow, m_selectedCell.x))
            {
                newSel.x = m_selectedCell.x;
                newSel.y = searchRow;
                break;
            }
            searchRow += nDirection; // PageUpなら上へ, PageDownなら下へ探索
        }

        // 3. もし同じ列に見つからなければ、一番端の編集可能セルに移動する
        if (newSel.x == -1)
        {
            if (nChar == VK_PRIOR) // PageUpの場合は、一番最初の編集可能セルへ
            {
                for (int r = 0; r < m_nRows; ++r)
                {
                    for (int c = 0; c < m_nCols; ++c)
                    {
                        if (IsCellEditable(r, c))
                        {
                            newSel = CPoint(c, r);
                            goto found; // 二重ループを抜ける
                        }
                    }
                }
            }
            else // PageDownの場合は、一番最後の編集可能セルへ
            {
                for (int r = m_nRows - 1; r >= 0; --r)
                {
                    for (int c = m_nCols - 1; c >= 0; --c)
                    {
                        if (IsCellEditable(r, c))
                        {
                            newSel = CPoint(c, r);
                            goto found; // 二重ループを抜ける
                        }
                    }
                }
            }
        found:;
        }

        // 4. 移動先が見つかったら、選択を更新してスクロール
        if (newSel.x != -1 && newSel != m_selectedCell)
        {
            m_selectedCell = newSel;

            // 親ウィンドウに選択変更を通知
            NM_GRIDVIEW nm;
            nm.hdr.hwndFrom = GetSafeHwnd();
            nm.hdr.idFrom = GetDlgCtrlID();
            nm.hdr.code = GCN_SELCHANGED;
            nm.iRow = m_selectedCell.y;
            nm.iCol = m_selectedCell.x;
            GetParent()->SendMessage(WM_NOTIFY, GetDlgCtrlID(), (LPARAM)&nm);

            // 新しく選択されたセルが表示されるようにスクロール
            EnsureCellVisible(m_selectedCell.y, m_selectedCell.x);
            Invalidate();
        }
        break;
    }
    case VK_F2:
    case VK_RETURN:
        if (m_selectedCell.x != -1) CreateInPlaceEdit();
        break;
    default:
        CWnd::OnKeyDown(nChar, nRepCnt, nFlags);
        break;
    }
}

/**
 * @brief 指定したセルが表示されるように、必要であればグリッドをスクロールします。
 * @param[in] nRow 行インデックス (0始まり)
 * @param[in] nCol 列インデックス (0始まり)
 */
void CGridCtrl::EnsureCellVisible(int nRow, int nCol)
{
    if (m_nRows <= m_nMaxVisibleRows) return;

    int newTopRow = m_nTopRow;
    if (nRow < m_nTopRow)
    {
        newTopRow = nRow;
    }
    else if (nRow >= m_nTopRow + m_nMaxVisibleRows)
    {
        newTopRow = nRow - m_nMaxVisibleRows + 1;
    }

    if (newTopRow != m_nTopRow)
    {
        int maxScrollPos = m_nRows - m_nMaxVisibleRows;
        newTopRow = max(0, min(newTopRow, maxScrollPos));
        m_nTopRow = newTopRow;
        SetScrollPos(SB_VERT, m_nTopRow, TRUE);
        Invalidate();
    }
}


/**
 * @brief フォーカスを受け取った際のイベントハンドラ (WM_SETFOCUS)。
 * @details まだ何も選択されていなければ、最初の編集可能セルを選択します。
 * @param[in] pOldWnd フォーカスを失ったウィンドウ
 */
void CGridCtrl::OnSetFocus(CWnd* pOldWnd)
{
    CWnd::OnSetFocus(pOldWnd);
    if (m_selectedCell.x == -1)
    {
        MoveSelection(1, 0);
    }
    Invalidate();
}

/**
 * @brief フォーカスを失った際のイベントハンドラ (WM_KILLFOCUS)。
 * @details インプレイス編集中のエディットコントロールがあれば破棄します。
 * @param[in] pNewWnd 新しくフォーカスを受け取るウィンドウ
 */
void CGridCtrl::OnKillFocus(CWnd* pNewWnd)
{
    CWnd::OnKillFocus(pNewWnd);
    if (m_pEdit != nullptr && pNewWnd != nullptr)
    {
        HWND hEdit = m_pEdit->GetSafeHwnd();
        HWND hNew = pNewWnd->GetSafeHwnd();
        if (hEdit != nullptr && hNew == hEdit)
        {
            return; // フォーカスがインプレイスエディット自身に移る場合は何もしない
        }
    }
    DestroyInPlaceEdit(TRUE);
    Invalidate();
}

/**
 * @brief 論理的な行・列インデックスから、物理的な描画矩形を計算します。
 * @param[in] nRow 行インデックス (0始まり)
 * @param[in] nCol 列インデックス (0始まり)
 * @return セルの描画矩形 (クライアント座標)
 */
CRect CGridCtrl::GetCellRect(int nRow, int nCol) const
{
    if (nRow < m_nTopRow || nRow >= m_nTopRow + m_nMaxVisibleRows)
    {
        return CRect(0, 0, 0, 0); // 画面外
    }
    int left = 0;
    for (int i = 0; i < nCol; ++i) left += m_colWidths[i];
    int right = left + m_colWidths[nCol];
    int top = (nRow - m_nTopRow) * m_nRowHeight;
    int bottom = top + m_nRowHeight;
    return CRect(left, top, right, bottom);
}

/**
 * @brief 物理的な座標点から、それがどの論理セルに該当するかを計算します。
 * @param[in] point クライアント座標
 * @return セルの論理インデックス (列, 行)。見つからない場合は (-1, -1)。
 */
CPoint CGridCtrl::HitTest(const CPoint& point) const
{
    int row = (point.y / m_nRowHeight) + m_nTopRow;
    if (row < 0 || row >= m_nRows) return CPoint(-1, -1);

    int currentX = 0;
    for (int col = 0; col < m_nCols; ++col)
    {
        currentX += m_colWidths[col];
        if (point.x < currentX)
        {
            return CPoint(col, row);
        }
    }
    return CPoint(-1, -1);
}

/**
 * @brief グリッドの全コンテンツを表示するために必要な高さを返します。
 * @details m_nMaxVisibleRowsまでの高さを返します。
 * @return 必要な高さ(ピクセル)
 */
int CGridCtrl::GetRequiredHeight() const
{
    if (m_nRows == 0) return 0;
    int nVisibleRows = min(m_nRows, m_nMaxVisibleRows);
    return nVisibleRows * m_nRowHeight;
}

/**
 * @brief グリッドの全コンテンツを表示するために必要な幅を返します。
 * @return 必要な幅(ピクセル)
 */
int CGridCtrl::GetRequiredWidth() const
{
    if (m_nCols == 0) return 0;
    int width = 0;
    for (size_t i = 0; i < m_colWidths.size(); ++i)
    {
        width += m_colWidths[i];
    }
    return width;
}


/**
 * @brief 論理的な行・列インデックスから、1次元配列m_cellsのインデックスを計算します。
 * @param[in] nRow 行インデックス (0始まり)
 * @param[in] nCol 列インデックス (0始まり)
 * @return 配列のインデックス。範囲外の場合は-1。
 */
int CGridCtrl::GetCellIndex(int nRow, int nCol) const
{
    if (nRow >= 0 && nRow < m_nRows && nCol >= 0 && nCol < m_nCols)
    {
        return nRow * m_nCols + nCol;
    }
    return -1;
}

/**
 * @brief インプレイス編集用のエディットコントロールを生成し、表示します。
 */
void CGridCtrl::CreateInPlaceEdit()
{
    if (m_pEdit || m_selectedCell.x == -1 || !IsCellEditable(m_selectedCell.y, m_selectedCell.x))
        return;

    CRect rect = GetCellRect(m_selectedCell.y, m_selectedCell.x);
    rect.DeflateRect(1, 1);

    m_pEdit = new CInPlaceEdit(this, m_selectedCell, GetCellText(m_selectedCell.y, m_selectedCell.x));

    if (!m_pEdit->Create(WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL, rect, this, 1))
    {
        TRACE(_T("Failed to create InPlaceEdit control. Error: %d\n"), GetLastError());
        delete m_pEdit;
        m_pEdit = nullptr;
        return;
    }
    m_pEdit->SetFont(GetFont());
    m_pEdit->SetFocus();
    m_pEdit->SetSel(0, -1);
    Invalidate();
}


/**
 * @brief インプレイス編集用のエディットコントロールを破棄します。
 * @param[in] bUpdate TRUEの場合、編集内容をセルに反映します。FALSEの場合は反映せずに破棄します。
 */
void CGridCtrl::DestroyInPlaceEdit(BOOL bUpdate)
{
    if (!m_pEdit) return;

    if (bUpdate)
    {
        CString text;
        m_pEdit->GetWindowText(text);
        int index = GetCellIndex(m_selectedCell.y, m_selectedCell.x);
        if (index != -1 && m_cells[index].m_strText != text)
        {
            m_cells[index].m_strText = text;
            // 親ウィンドウに変更を通知
            GetParent()->PostMessage(WM_GRID_CELL_CHANGED, GetDlgCtrlID(), MAKELPARAM(m_selectedCell.y, m_selectedCell.x));
        }
    }

    m_pEdit->DestroyWindow();
    delete m_pEdit;
    m_pEdit = nullptr;

    SetFocus();
    Invalidate();
}

/**
 * @brief カーソルキー入力に応じて、編集可能なセル間を選択移動します。
 * @param[in] dx 水平方向の移動量 (-1:左, 1:右, 0:移動なし)
 * @param[in] dy 垂直方向の移動量 (-1:上, 1:下, 0:移動なし)
 */
void CGridCtrl::MoveSelection(int dx, int dy)
{
    if (m_pEdit)
        return; // 編集中は移動しない
    if (m_selectedCell.x == -1)
        return; // 未選択状態なら何もしない

    CPoint currentCell = m_selectedCell;

    if (dx != 0) // 左右移動の場合
    {
        int newCol = currentCell.x + dx;
        // 現在の行の中で、指定された方向に編集可能セルを探す
        while (newCol >= 0 && newCol < m_nCols)
        {
            int index = GetCellIndex(currentCell.y, newCol);
            if (m_cells[index].m_bEditable)
            {
                m_selectedCell = CPoint(newCol, currentCell.y);
                // 親ウィンドウに行選択の変更を通知する
                NM_GRIDVIEW nm;
                nm.hdr.hwndFrom = GetSafeHwnd();
                nm.hdr.idFrom = GetDlgCtrlID();
                nm.hdr.code = GCN_SELCHANGED;
                nm.iRow = m_selectedCell.y;
                nm.iCol = m_selectedCell.x;
                GetParent()->SendMessage(WM_NOTIFY, GetDlgCtrlID(), (LPARAM)&nm);

                Invalidate();
                return; // 見つかったので選択を更新して終了
            }
            newCol += dx; // 見つからなければ、さらに同じ方向へ探索
        }
    }
    else if (dy != 0) // 上下移動の場合
    {
        int newRow = currentCell.y + dy;
        // 現在の列の中で、指定された方向に編集可能セルを探す
        while (newRow >= 0 && newRow < m_nRows)
        {
            int index = GetCellIndex(newRow, currentCell.x);

            if (m_cells[index].m_bEditable)
            {
                m_selectedCell = CPoint(currentCell.x, newRow);
                // 親ウィンドウに行選択の変更を通知する
                NM_GRIDVIEW nm;
                nm.hdr.hwndFrom = GetSafeHwnd();
                nm.hdr.idFrom = GetDlgCtrlID();
                nm.hdr.code = GCN_SELCHANGED;
                nm.iRow = m_selectedCell.y;
                nm.iCol = m_selectedCell.x;
                GetParent()->SendMessage(WM_NOTIFY, GetDlgCtrlID(), (LPARAM)&nm);

                Invalidate();
                return; // 見つかったので選択を更新して終了
            }
            newRow += dy; // 見つからなければ、さらに同じ方向へ探索
        }
    }

    // ここに到達した場合、指定された方向に編集可能なセルは見つからなかった。
    // 何もせずに関数を終了する。
}

/**
 * @brief このグリッドコントロールのアクティブ/非アクティブ状態を設定します。
 * @details アクティブになると外枠が青くなり、フォーカスを受け取れるようになります。
 * 非アクティブになると選択状態が解除されます。
 * @param[in] bActive アクティブにする場合はTRUE
 */
void CGridCtrl::SetActive(BOOL bActive)
{
    if (m_bIsActive == bActive) return;
    m_bIsActive = bActive;

    if (bActive)
    {
        // アクティブになった際、何も選択されていなければ最初の編集可能セルを選択
        if (m_selectedCell.x == -1)
        {
            bool found = false;
            for (int r = 0; r < m_nRows && !found; ++r) {
                for (int c = 0; c < m_nCols; ++c) {
                    if (IsCellEditable(r, c)) {
                        m_selectedCell = CPoint(c, r);
                        NM_GRIDVIEW nm;
                        nm.hdr.hwndFrom = GetSafeHwnd();
                        nm.hdr.idFrom = GetDlgCtrlID();
                        nm.hdr.code = GCN_SELCHANGED;
                        nm.iRow = m_selectedCell.y;
                        nm.iCol = m_selectedCell.x;
                        GetParent()->SendMessage(WM_NOTIFY, GetDlgCtrlID(), (LPARAM)&nm);
                        found = true;
                        break;
                    }
                }
            }
        }
    }
    else
    {
        // 非アクティブにされた場合、選択と編集を解除
        DestroyInPlaceEdit(FALSE);
        m_selectedCell = CPoint(-1, -1);
    }
    Invalidate();
}

/**
 * @brief 内部スクロールバーの状態を更新します。
 * @details グリッドの総行数と表示可能行数に基づいて、スクロールバーの範囲や表示/非表示を設定します。
 */
void CGridCtrl::UpdateScrollbar()
{
    if (GetSafeHwnd() == nullptr) return;

    if (m_nRows > m_nMaxVisibleRows)
    {
        SCROLLINFO si;
        si.cbSize = sizeof(SCROLLINFO);
        si.fMask = SIF_RANGE | SIF_PAGE | SIF_POS;
        si.nMin = 0;
        si.nMax = m_nRows - 1;
        si.nPage = m_nMaxVisibleRows;
        si.nPos = m_nTopRow;
        SetScrollInfo(SB_VERT, &si, TRUE);
        ShowScrollBar(SB_VERT, TRUE);
    }
    else
    {
        m_nTopRow = 0;
        ShowScrollBar(SB_VERT, FALSE);
    }
}


/**
 * @brief 垂直スクロールイベント(WM_VSCROLL)を処理します。
 * @param[in] nSBCode スクロールバーのコード
 * @param[in] nPos スクロールボックスの位置
 * @param[in] pScrollBar スクロールバーコントロールへのポインタ
 */
void CGridCtrl::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
    int newTopRow = m_nTopRow;

    switch (nSBCode)
    {
    case SB_LINEUP: newTopRow--; break;
    case SB_LINEDOWN: newTopRow++; break;
    case SB_PAGEUP: newTopRow -= m_nMaxVisibleRows; break;
    case SB_PAGEDOWN: newTopRow += m_nMaxVisibleRows; break;
    case SB_THUMBTRACK: newTopRow = nPos; break;
    }

    int maxScrollPos = m_nRows - m_nMaxVisibleRows;
    newTopRow = max(0, min(newTopRow, maxScrollPos));

    if (newTopRow == m_nTopRow) return;

    m_nTopRow = newTopRow;
    SetScrollPos(SB_VERT, m_nTopRow, TRUE);
    Invalidate();

    CWnd::OnVScroll(nSBCode, nPos, pScrollBar);
}

/**
 * @brief マウスホイールイベント(WM_MOUSEWHEEL)を処理します。
 * @details マウスホイールの回転に応じて垂直スクロールを行います。
 * @param[in] nFlags 修飾キーの状態
 * @param[in] zDelta ホイールの回転量
 * @param[in] pt カーソルの位置
 * @return メッセージを処理した場合はTRUE
 */
BOOL CGridCtrl::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
    if (m_nRows > m_nMaxVisibleRows)
    {
        if (zDelta > 0) OnVScroll(SB_LINEUP, 0, nullptr);
        else OnVScroll(SB_LINEDOWN, 0, nullptr);
    }
    return CWnd::OnMouseWheel(nFlags, zDelta, pt);
}