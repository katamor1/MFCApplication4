// GridCtrl.cpp
#include "pch.h" // または stdafx.h
#include "GridCtrl.h"

// 色の定義
const COLORREF CLR_WHITE = RGB(255, 255, 255);
const COLORREF CLR_GRAY = RGB(240, 240, 240);
const COLORREF CLR_BLUE = RGB(173, 216, 230);     // 選択色 (Light Blue)
const COLORREF CLR_YELLOW = RGB(255, 255, 224);   // 編集色 (Light Yellow)
const COLORREF CLR_BORDER = RGB(192, 192, 192);   // 枠線色
const COLORREF CLR_ORANGE = RGB(255, 192, 128);   // 正の数 背景色
const COLORREF CLR_BLUE_TEXT = RGB(0, 0, 255);    // 正の数 文字色
const COLORREF CLR_BLUE2_BG = RGB(120, 210, 230); // 負の数 背景色
const COLORREF CLR_BLUE_BG = RGB(0, 0, 230);      // 負の数 背景色
const COLORREF CLR_RED_TEXT = RGB(255, 0, 0);     // 負の数 文字色
const COLORREF CLR_BLACK = RGB(0, 0, 0);          // デフォルト文字色

CGridCtrl::CGridCtrl()
    : m_nRows(0), m_nCols(0),
      m_nRowHeight(22),
      m_defaultBgColor(RGB(240, 240, 240)),
      m_selectedCell(-1, -1),
      m_bIsActive(FALSE),
      m_pEdit(nullptr),
      m_nTopRow(0) // m_nTopRowを0で初期化
{
}

CGridCtrl::~CGridCtrl()
{
    DestroyInPlaceEdit(FALSE);
}

// --- 新しい公開関数 ---
BOOL CGridCtrl::SetupGrid(int nRows, int nCols)
{
    if (nRows <= 0 || nCols <= 0)
    {
        ASSERT(FALSE);
        return FALSE;
    }
    m_nRows = nRows;
    m_nCols = nCols;

    m_cells.resize(m_nRows * m_nCols);
    m_colWidths.resize(m_nCols);

    // デフォルト値で初期化
    for (int i = 0; i < m_nCols; ++i)
    {
        m_colWidths[i] = 80; // デフォルトの列幅
    }
    for (int i = 0; i < m_cells.size(); ++i)
    {
        m_cells[i].m_bgColor = m_defaultBgColor;
        m_cells[i].m_bEditable = FALSE;
    }

    m_nTopRow = 0;
    return TRUE;
}

void CGridCtrl::SetRowHeight(int nHeight)
{
    if (nHeight > 0)
        m_nRowHeight = nHeight;
}

void CGridCtrl::SetColumnWidth(int nCol, int nWidth)
{
    if (nCol >= 0 && nCol < m_nCols && nWidth > 0)
    {
        m_colWidths[nCol] = nWidth;
    }
}

void CGridCtrl::SetDefaultBgColor(COLORREF color)
{
    m_defaultBgColor = color;
}

void CGridCtrl::SetCellText(int nRow, int nCol, const CString &strText)
{
    int index = GetCellIndex(nRow, nCol);
    if (index != -1)
        m_cells[index].m_strText = strText;
}

CString CGridCtrl::GetCellText(int nRow, int nCol) const
{
    int index = GetCellIndex(nRow, nCol);
    return (index != -1) ? m_cells[index].m_strText : CString();
}

void CGridCtrl::SetCellEditable(int nRow, int nCol, BOOL bEditable)
{
    int index = GetCellIndex(nRow, nCol);
    if (index != -1)
    {
        m_cells[index].m_bEditable = bEditable;
        // 編集可能なセルは背景色を白にする（デフォルトの挙動）
        m_cells[index].m_bgColor = bEditable ? RGB(255, 255, 255) : m_defaultBgColor;
    }
}

BOOL CGridCtrl::IsCellEditable(int nRow, int nCol) const
{
    int index = GetCellIndex(nRow, nCol);
    return (index != -1) ? m_cells[index].m_bEditable : FALSE;
}

void CGridCtrl::SetCellBgColor(int nRow, int nCol, COLORREF color)
{
    int index = GetCellIndex(nRow, nCol);
    if (index != -1)
        m_cells[index].m_bgColor = color;
}

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

int CGridCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CWnd::OnCreate(lpCreateStruct) == -1)
        return -1;

    // ウィンドウハンドルが有効になった、この最適なタイミングで
    // スクロールバーの初期化処理を呼び出します。
    UpdateScrollbar();

    return 0;
}

BOOL CGridCtrl::Create(const RECT &rect, CWnd *pParentWnd, UINT nID)
{
    // ウィンドウクラスを登録
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

void CGridCtrl::OnPaint()
{
    CPaintDC dc(this);
    CRect clientRect;
    GetClientRect(&clientRect);

    CDC memDC;
    memDC.CreateCompatibleDC(&dc);
    CBitmap bmp;
    bmp.CreateCompatibleBitmap(&dc, clientRect.Width(), clientRect.Height());
    CBitmap *pOldBmp = memDC.SelectObject(&bmp);

    memDC.FillSolidRect(clientRect, CLR_WHITE);

    CPen pen(PS_SOLID, 1, CLR_BORDER);
    CPen *pOldPen = memDC.SelectObject(&pen);

    // 表示する行の範囲を計算
    int nStartRow = m_nTopRow;
    int nEndRow = min(m_nRows, m_nTopRow + MAX_VISIBLE_ROWS);

    for (int row = nStartRow; row < nEndRow; ++row)
    {
        for (int col = 0; col < m_nCols; ++col)
        {
            // 物理的な描画位置を計算（rowからm_nTopRowを引く）
            CRect cellRect = GetCellRect(row, col);
            int index = GetCellIndex(row, col);
            if (index == -1)
                continue;

            CellInfo &cell = m_cells[index];

            // 1. デフォルトの色を決定
            COLORREF bgColor = cell.m_bgColor;
            COLORREF textColor = CLR_BLACK;

            // 2. 編集可能なセルの場合、内容に応じて色を上書き
            if (cell.m_bEditable)
            {
                CString strText = cell.m_strText;
                strText.Trim(); // 前後の空白を除去して判定

                if (strText.IsEmpty())
                {
                    // ルール1: 空欄の場合
                    bgColor = CLR_YELLOW;
                }
                else
                {
                    // 文字列が数値かどうかを判定
                    LPTSTR pEnd;
                    double dValue = _tcstod(strText, &pEnd);

                    // 文字列の最後まで正常に数値として変換できたかチェック
                    if (*pEnd == _T('\0'))
                    {
                        if (dValue < 0)
                        {
                            // ルール2: 負の数の場合
                            bgColor = CLR_BLUE2_BG;
                            textColor = CLR_RED_TEXT;
                        }
                        else if (dValue > 0)
                        {
                            // ルール3: 正の数の場合
                            bgColor = CLR_ORANGE;
                            textColor = CLR_BLUE_TEXT;
                        }
                        // dValueが0の場合は、ルール4（その他）に該当
                    }
                    // 数値でない文字列の場合も、ルール4（その他）に該当
                }
            }

            // 3. 選択状態/編集状態の色を最優先で適用
            if (m_selectedCell.x == col && m_selectedCell.y == row)
            {
                bgColor = (m_pEdit != nullptr) ? CLR_YELLOW : CLR_BLUE_BG;
                // 編集状態の文字色は黒のまま
                if (m_pEdit == nullptr)
                {
                    textColor = CLR_BLACK;
                }
            }

            // 決定した色でセルを描画
            memDC.FillSolidRect(cellRect, bgColor);
            memDC.MoveTo(cellRect.left, cellRect.top);
            memDC.LineTo(cellRect.right - 1, cellRect.top);
            memDC.LineTo(cellRect.right - 1, cellRect.bottom - 1);
            memDC.LineTo(cellRect.left, cellRect.bottom - 1);
            memDC.LineTo(cellRect.left, cellRect.top);

            // 決定した色でテキストを描画
            memDC.SetBkMode(TRANSPARENT);
            memDC.SetTextColor(textColor);
            cellRect.DeflateRect(4, 2);
            memDC.DrawText(cell.m_strText, cellRect, DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
        }
    }

    memDC.SelectObject(pOldPen);

    // フォーカス枠の描画
    if (GetFocus() == this && m_selectedCell.x != -1)
    {
        CRect focusRect = GetCellRect(m_selectedCell.y, m_selectedCell.x);
        dc.DrawFocusRect(focusRect);
    }

    // このグリッドがアクティブな場合、外枠を青で囲む
    if (m_bIsActive)
    {
        CPen borderPen(PS_SOLID, 3, RGB(0, 0, 255));                       // 太さ3の青いペン
        CBrush *pOldBrush = (CBrush *)memDC.SelectStockObject(NULL_BRUSH); // 中は塗りつぶさない
        CPen *pOldPen = memDC.SelectObject(&borderPen);

        CRect rcClient;
        GetClientRect(&rcClient);
        // 枠線が完全に内側に描画されるように調整
        rcClient.DeflateRect(1, 1);
        memDC.Rectangle(rcClient);

        memDC.SelectObject(pOldBrush);
        memDC.SelectObject(pOldPen);
    }
    dc.BitBlt(0, 0, clientRect.Width(), clientRect.Height(), &memDC, 0, 0, SRCCOPY);
    memDC.SelectObject(pOldBmp);
}

void CGridCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
    // このグリッドがクリックされたことを親ウィンドウに通知する
    GetParent()->PostMessage(WM_GRID_ACTIVATED, GetDlgCtrlID());
    SetFocus();

    CPoint cell = HitTest(point);
    if (cell.x == -1) // グリッド外
    {
        // 編集中の場合は確定
        DestroyInPlaceEdit(TRUE);
        m_selectedCell = CPoint(-1, -1);
        Invalidate();
        return;
    }

    // IsCellEditableヘルパー関数を使う
    if (!IsCellEditable(cell.y, cell.x))
    {
        // 編集不可セルをクリックしても選択状態は変わらない
        return;
    }

    // 選択中のセルを再クリックした場合
    if (cell == m_selectedCell)
    {
        if (m_pEdit == nullptr)
        {
            CreateInPlaceEdit();
        }
    }
    else // 別の編集可能セルをクリックした場合
    {
        DestroyInPlaceEdit(TRUE); // 前の編集を確定
        m_selectedCell = cell;
        // 親ウィンドウに行選択の変更を通知する
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

UINT CGridCtrl::OnGetDlgCode()
{
    // カーソルキーをダイアログに奪われないようにする
    return DLGC_WANTARROWS | DLGC_WANTCHARS;
}

void CGridCtrl::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
    // 編集モードの場合は、すべてのキー操作を無効化
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
        // --- 1. グリッド内に選択セルがない場合の処理 ---
        if (m_selectedCell.x == -1)
        {
            // 内部に移動すべきセルがないため、どの方向キーが押されても
            // 「境界に到達した」とみなし、親ウィンドウにナビゲーションを依頼します。
            GetParent()->PostMessage(WM_GRID_NAV_BOUNDARY_HIT, (WPARAM)nChar, (LPARAM)GetDlgCtrlID());
            return; // このキーに対する処理は完了
        }

        // --- 2. グリッド内に選択セルがある場合の処理 (既存のロジック) ---
        BOOL bMoved = FALSE; // グリッド内で移動できたかどうかのフラグ
        int dx = (nChar == VK_LEFT) ? -1 : (nChar == VK_RIGHT) ? 1
                                                               : 0;
        int dy = (nChar == VK_UP) ? -1 : (nChar == VK_DOWN) ? 1
                                                            : 0;

        CPoint searchCell = m_selectedCell;
        while (TRUE)
        {
            searchCell.x += dx;
            searchCell.y += dy;

            // グリッドの範囲外に出たら探索終了
            if (searchCell.x < 0 || searchCell.x >= m_nCols || searchCell.y < 0 || searchCell.y >= m_nRows)
            {
                break;
            }

            if (IsCellEditable(searchCell.y, searchCell.x))
            {
                // 移動先が見つかった場合
                m_selectedCell = searchCell;
                bMoved = TRUE;
                break;
            }
        }

        if (bMoved)
        {
            // 内部で移動できた場合：親に選択変更を通知し、再描画
            NM_GRIDVIEW nm;
            nm.hdr.hwndFrom = GetSafeHwnd();
            nm.hdr.idFrom = GetDlgCtrlID();
            nm.hdr.code = GCN_SELCHANGED;
            nm.iRow = m_selectedCell.y;
            nm.iCol = m_selectedCell.x;
            GetParent()->SendMessage(WM_NOTIFY, GetDlgCtrlID(), (LPARAM)&nm);
            // 新しく選択されたセルが表示されるように、ビューをスクロールさせる
            EnsureCellVisible(m_selectedCell.y, m_selectedCell.x);
            Invalidate();
        }
        else
        {
            // 内部で移動できなかった（端に到達した）場合：親に通知
            GetParent()->PostMessage(WM_GRID_NAV_BOUNDARY_HIT, (WPARAM)nChar, (LPARAM)GetDlgCtrlID());
        }
        break;
    }

    case VK_PRIOR: // Page Up
    case VK_NEXT:  // Page Down
    {
        // セルが選択されていなければ何もしない
        if (m_selectedCell.x == -1)
        {
            break;
        }

        int nStep = (nChar == VK_PRIOR) ? -MAX_VISIBLE_ROWS : MAX_VISIBLE_ROWS;
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
        // セルが選択されていれば編集開始を試みる
        if (m_selectedCell.x != -1)
        {
            CreateInPlaceEdit();
        }
        break;

    default:
        CWnd::OnKeyDown(nChar, nRepCnt, nFlags);
        break;
    }
}

void CGridCtrl::EnsureCellVisible(int nRow, int nCol)
{
    // スクロールバーがない場合は何もしない
    if (m_nRows <= MAX_VISIBLE_ROWS)
    {
        return;
    }

    int newTopRow = m_nTopRow;

    if (nRow < m_nTopRow)
    {
        // 選択セルが、現在表示されている領域よりも上にある場合
        // -> 選択セルが一番上に表示されるようにスクロール
        newTopRow = nRow;
    }
    else if (nRow >= m_nTopRow + MAX_VISIBLE_ROWS)
    {
        // 選択セルが、現在表示されている領域よりも下にある場合
        // -> 選択セルが一番下に表示されるようにスクロール
        newTopRow = nRow - MAX_VISIBLE_ROWS + 1;
    }

    // スクロール位置に変化があった場合のみ、実際のスクロール処理を実行
    if (newTopRow != m_nTopRow)
    {
        // OnVScrollのロジックと同様の処理
        int maxScrollPos = m_nRows - MAX_VISIBLE_ROWS;
        if (newTopRow < 0)
            newTopRow = 0;
        if (newTopRow > maxScrollPos)
            newTopRow = maxScrollPos;

        m_nTopRow = newTopRow;

        // スクロールバーのつまみの位置を更新
        SetScrollPos(SB_VERT, m_nTopRow, TRUE);

        // 再描画して、新しいスクロール位置のコンテンツを表示
        Invalidate();
    }
}

void CGridCtrl::OnSetFocus(CWnd *pOldWnd)
{
    CWnd::OnSetFocus(pOldWnd);
    // 最初の入力可能セルを選択状態にする
    if (m_selectedCell.x == -1)
    {
        MoveSelection(1, 0); // 仮。右に移動して最初のセルを探す
    }
    Invalidate();
}
void CGridCtrl::OnKillFocus(CWnd *pNewWnd)
{
    CWnd::OnKillFocus(pNewWnd);

    // 新しいフォーカスを受け取ったウィンドウ(pNewWnd)が、
    // インプレイスエディット(m_pEdit)自身である場合は、
    // エディットコントロールを破棄せずに処理を抜けます。
    // これにより、SetFocusの副作用による意図しない破棄を防ぎます。
    if (pNewWnd != nullptr && m_pEdit != nullptr && pNewWnd->GetSafeHwnd() == m_pEdit->GetSafeHwnd())
    {
        return;
    }

    // 上記以外（フォーカスがグリッドやエディットの外に移動した）の場合、
    // 編集中のエディットコントロールがあれば確定・破棄します。
    if (m_pEdit)
    {
        DestroyInPlaceEdit(TRUE);
    }

    Invalidate();
}

// GetCellRect: 論理的な行番号から、物理的な描画矩形を計算
CRect CGridCtrl::GetCellRect(int nRow, int nCol) const
{
    if (nRow < m_nTopRow || nRow >= m_nTopRow + MAX_VISIBLE_ROWS)
    {
        return CRect(0, 0, 0, 0); // 画面外のセル
    }

    int left = 0;
    for (int i = 0; i < nCol; ++i)
        left += m_colWidths[i];
    int right = left + m_colWidths[nCol];

    // 物理的なY座標を計算
    int top = (nRow - m_nTopRow) * m_nRowHeight;
    int bottom = top + m_nRowHeight;

    return CRect(left, top, right, bottom);
}

// HitTest: 物理的なY座標から、論理的な行番号を計算
CPoint CGridCtrl::HitTest(const CPoint &point) const
{
    // point.yは物理的なクリック位置
    int row = (point.y / m_nRowHeight) + m_nTopRow;

    CRect clientRect;
    GetClientRect(&clientRect);
    int cellWidth = 0;
    for (int col = 0; col < m_nCols; ++col)
    {
        cellWidth += m_colWidths[col];
        if (point.x < cellWidth)
        {
            if (row >= 0 && row < m_nRows)
                return CPoint(col, row);
            else
                break;
        }
    }
    return CPoint(-1, -1);
}

// GetRequiredHeight: コントロールの物理的な高さを返すように修正
int CGridCtrl::GetRequiredHeight() const
{
    if (m_nRows == 0)
    {
        return 0; // まだグリッドがセットアップされていない場合
    }
    // 表示行数（最大10行）分の高さを返す
    int nVisibleRows = min(m_nRows, MAX_VISIBLE_ROWS);
    return nVisibleRows * m_nRowHeight;
}

int CGridCtrl::GetRequiredWidth() const
{
    if (m_nCols == 0)
    {
        return 0; // まだグリッドがセットアップされていない場合
    }
    int width = 0;

    for (int i = 0; i < m_nCols; ++i)
    {
        width += m_colWidths[i];
    }
    return width;
}

int CGridCtrl::GetCellIndex(int nRow, int nCol) const
{
    if (nRow >= 0 && nRow < m_nRows && nCol >= 0 && nCol < m_nCols)
    {
        return nRow * m_nCols + nCol;
    }
    return -1;
}

void CGridCtrl::CreateInPlaceEdit()
{
    if (m_pEdit)
        return;
    if (m_selectedCell.x == -1)
        return;
    int index = GetCellIndex(m_selectedCell.y, m_selectedCell.x);
    if (!m_cells[index].m_bEditable)
        return;

    CRect rect = GetCellRect(m_selectedCell.y, m_selectedCell.x);
    rect.DeflateRect(1, 1); // 枠線にかぶらないように少し小さくする

    m_pEdit = new CInPlaceEdit(this, m_selectedCell, m_cells[index].m_strText);

    // Create関数の戻り値をチェックし、失敗した場合は後続処理を行わないようにする
    if (!m_pEdit->Create(WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL, rect, this, 1))
    {
        // 失敗した場合、デバッグ出力にエラーコードを表示する
        TRACE(_T("Failed to create InPlaceEdit control. Error: %d\n"), GetLastError());
        delete m_pEdit;
        m_pEdit = nullptr;
        return; // ウィンドウ生成に失敗したので、ここで処理を中断する
    }
    m_pEdit->SetFont(GetFont());
    m_pEdit->SetFocus();
    m_pEdit->SetSel(0, -1);

    Invalidate(); // 背景を黄色にするために再描画
}

void CGridCtrl::DestroyInPlaceEdit(BOOL bUpdate)
{
    if (!m_pEdit)
        return;

    if (bUpdate)
    {
        CString text;
        m_pEdit->GetWindowText(text);
        int index = GetCellIndex(m_selectedCell.y, m_selectedCell.x);
        m_cells[index].m_strText = text;

        // 親ウィンドウに変更を通知
        GetParent()->PostMessage(WM_GRID_CELL_CHANGED, GetDlgCtrlID(), MAKELPARAM(m_selectedCell.y, m_selectedCell.x));
    }

    m_pEdit->DestroyWindow();
    delete m_pEdit;
    m_pEdit = nullptr;

    SetFocus(); // フォーカスをグリッド自体に戻す
    Invalidate();
}

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

void CGridCtrl::SetActive(BOOL bActive)
{
    if (m_bIsActive == bActive)
    {
        return; // 状態が変更されない場合は何もしない
    }

    m_bIsActive = bActive;

    if (bActive)
    {
        // グリッドがアクティブになった際の処理
        // もし、まだセルが何も選択されていない場合
        if (m_selectedCell.x == -1)
        {
            // 編集可能な最初のセルを探して、自動的に選択状態にする
            bool bFound = false;
            for (int row = 0; row < m_nRows && !bFound; ++row)
            {
                for (int col = 0; col < m_nCols && !bFound; ++col)
                {
                    if (IsCellEditable(row, col))
                    {
                        m_selectedCell = CPoint(col, row);
                        bFound = true; // 見つかったのでループを抜ける
                    }
                }
            }

            // 編集可能なセルが見つかって自動選択した場合、親に選択変更を通知する
            if (bFound)
            {
                NM_GRIDVIEW nm;
                nm.hdr.hwndFrom = GetSafeHwnd();
                nm.hdr.idFrom = GetDlgCtrlID();
                nm.hdr.code = GCN_SELCHANGED;
                nm.iRow = m_selectedCell.y;
                nm.iCol = m_selectedCell.x;
                GetParent()->SendMessage(WM_NOTIFY, GetDlgCtrlID(), (LPARAM)&nm);
            }
        }
    }
    else
    {
        // 非アクティブにされた場合、セルの選択状態と編集状態を解除する
        if (m_pEdit)
        {
            DestroyInPlaceEdit(FALSE);
        }
        m_selectedCell = CPoint(-1, -1);
    }

    Invalidate(); // 再描画を促す
}

void CGridCtrl::UpdateScrollbar()
{
    if (GetSafeHwnd() == nullptr)
        return;

    if (m_nRows > MAX_VISIBLE_ROWS)
    {
        SCROLLINFO si;
        si.cbSize = sizeof(SCROLLINFO);
        si.fMask = SIF_RANGE | SIF_PAGE | SIF_POS;
        si.nMin = 0;
        si.nMax = m_nRows - 1;
        si.nPage = MAX_VISIBLE_ROWS;
        si.nPos = m_nTopRow;
        SetScrollInfo(SB_VERT, &si, TRUE);
        ShowScrollBar(SB_VERT, TRUE);
    }
    else
    {
        ShowScrollBar(SB_VERT, FALSE);
    }
}

void CGridCtrl::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar *pScrollBar)
{
    int newTopRow = m_nTopRow;

    switch (nSBCode)
    {
    case SB_LINEUP:
        newTopRow--;
        break;
    case SB_LINEDOWN:
        newTopRow++;
        break;
    case SB_PAGEUP:
        newTopRow -= MAX_VISIBLE_ROWS;
        break;
    case SB_PAGEDOWN:
        newTopRow += MAX_VISIBLE_ROWS;
        break;
    case SB_THUMBTRACK:
        newTopRow = nPos;
        break;
    }

    // スクロール可能な最大位置を計算
    int maxScrollPos = m_nRows - MAX_VISIBLE_ROWS;
    if (newTopRow < 0)
        newTopRow = 0;
    if (newTopRow > maxScrollPos)
        newTopRow = maxScrollPos;
    if (newTopRow == m_nTopRow)
        return;

    m_nTopRow = newTopRow;
    SetScrollPos(SB_VERT, m_nTopRow, TRUE);
    Invalidate(); // スクロールしたので全体を再描画

    CWnd::OnVScroll(nSBCode, nPos, pScrollBar);
}

BOOL CGridCtrl::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
    if (m_nRows > MAX_VISIBLE_ROWS)
    {
        if (zDelta > 0)
            OnVScroll(SB_LINEUP, 0, nullptr);
        else
            OnVScroll(SB_LINEDOWN, 0, nullptr);
    }
    return CWnd::OnMouseWheel(nFlags, zDelta, pt);
}
