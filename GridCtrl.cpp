// GridCtrl.cpp
#include "pch.h" // または stdafx.h
#include "GridCtrl.h"

// 色の定義
const COLORREF CLR_WHITE = RGB(255, 255, 255);
const COLORREF CLR_GRAY = RGB(240, 240, 240);
const COLORREF CLR_BLUE = RGB(173, 216, 230); // 選択色 (Light Blue)
const COLORREF CLR_YELLOW = RGB(255, 255, 224); // 編集色 (Light Yellow)
const COLORREF CLR_BORDER = RGB(192, 192, 192); // 枠線色

CGridCtrl::CGridCtrl()
    : m_nRows(0), m_nCols(0),
      m_nRowHeight(22),
      m_defaultBgColor(RGB(240, 240, 240)),
      m_selectedCell(-1, -1),
      m_bIsActive(FALSE),
      m_pEdit(nullptr)
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

    return TRUE;
}

void CGridCtrl::SetRowHeight(int nHeight)
{
    if (nHeight > 0) m_nRowHeight = nHeight;
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

void CGridCtrl::SetCellText(int nRow, int nCol, const CString& strText)
{
    int index = GetCellIndex(nRow, nCol);
    if (index != -1) m_cells[index].m_strText = strText;
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
    if (index != -1) m_cells[index].m_bgColor = color;
}

BEGIN_MESSAGE_MAP(CGridCtrl, CWnd)
    ON_WM_PAINT()
    ON_WM_LBUTTONDOWN()
    ON_WM_GETDLGCODE()
    ON_WM_KEYDOWN()
    ON_WM_SETFOCUS()
    ON_WM_KILLFOCUS()
END_MESSAGE_MAP()

BOOL CGridCtrl::Create(const RECT& rect, CWnd* pParentWnd, UINT nID)
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
    wndcls.lpszClassName = L"MyGridCtrl";

    if (!AfxRegisterClass(&wndcls))
    {
        TRACE(L"Failed to register window class\n");
        return FALSE;
    }

    // --- ▼▼▼ 修正 ▼▼▼ ---
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

    return CWnd::Create(L"MyGridCtrl", L"", WS_CHILD | WS_VISIBLE | WS_BORDER | WS_TABSTOP, newRect, pParentWnd, nID);
    // --- ▲▲▲ 修正 ▲▲▲ ---
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
    CBitmap* pOldBmp = memDC.SelectObject(&bmp);

    // 背景を白でクリア
    memDC.FillSolidRect(clientRect, CLR_WHITE);

    // 枠線とセルの描画
    CPen pen(PS_SOLID, 1, CLR_BORDER);
    CPen* pOldPen = memDC.SelectObject(&pen);

    // forループの境界をメンバ変数に変更
    for (int row = 0; row < m_nRows; ++row)
    {
        for (int col = 0; col < m_nCols; ++col)
        {
            CRect cellRect = GetCellRect(row, col);
            int index = GetCellIndex(row, col);
            
            // 背景色の決定
            COLORREF bgColor = m_cells[index].m_bgColor;
            if (m_selectedCell.x == col && m_selectedCell.y == row)
            {
                bgColor = (m_pEdit != nullptr) ? CLR_YELLOW : CLR_BLUE;
            }
            memDC.FillSolidRect(cellRect, bgColor);

            // 枠線
            memDC.MoveTo(cellRect.left, cellRect.top);
            memDC.LineTo(cellRect.right -1, cellRect.top);
            memDC.LineTo(cellRect.right -1, cellRect.bottom-1);
            memDC.LineTo(cellRect.left, cellRect.bottom-1);
            memDC.LineTo(cellRect.left, cellRect.top);

            // テキスト
            memDC.SetBkMode(TRANSPARENT);
            cellRect.DeflateRect(4, 2);
            memDC.DrawText(m_cells[index].m_strText, cellRect, DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
        }
    }

    memDC.SelectObject(pOldPen);
    
    // フォーカス枠の描画
    if (GetFocus() == this && m_selectedCell.x != -1)
    {
        CRect focusRect = GetCellRect(m_selectedCell.y, m_selectedCell.x);
        dc.DrawFocusRect(focusRect);
    }
    
    // --- ▼▼▼ ここから追加 ▼▼▼ ---
    // このグリッドがアクティブな場合、外枠を青で囲む
    if (m_bIsActive)
    {
        CPen borderPen(PS_SOLID, 3, RGB(0, 0, 255)); // 太さ3の青いペン
        CBrush* pOldBrush = (CBrush*)memDC.SelectStockObject(NULL_BRUSH); // 中は塗りつぶさない
        CPen* pOldPen = memDC.SelectObject(&borderPen);

        CRect rcClient;
        GetClientRect(&rcClient);
        // 枠線が完全に内側に描画されるように調整
        rcClient.DeflateRect(1, 1); 
        memDC.Rectangle(rcClient);

        memDC.SelectObject(pOldBrush);
        memDC.SelectObject(pOldPen);
    }
    // --- ▲▲▲ ここまで追加 ▲▲▲ ---
    dc.BitBlt(0, 0, clientRect.Width(), clientRect.Height(), &memDC, 0, 0, SRCCOPY);
    memDC.SelectObject(pOldBmp);
}


void CGridCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
    // --- ▼▼▼ この行を追加 ▼▼▼ ---
    // このグリッドがクリックされたことを親ウィンドウに通知する
    GetParent()->PostMessage(WM_GRID_ACTIVATED, GetDlgCtrlID());
    // --- ▲▲▲ この行を追加 ▲▲▲ ---
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
    if (m_selectedCell.x == -1) return;
    if (m_pEdit) return; //編集中はキー操作を無効

    switch (nChar)
    {
    case VK_UP:    MoveSelection(0, -1); break;
    case VK_DOWN:  MoveSelection(0, 1);  break;
    case VK_LEFT:  MoveSelection(-1, 0); break;
    case VK_RIGHT: MoveSelection(1, 0);  break;
    case VK_F2: // F2キーで編集開始
    case VK_RETURN:
        CreateInPlaceEdit();
        break;
    }
}

void CGridCtrl::OnSetFocus(CWnd* pOldWnd)
{
    CWnd::OnSetFocus(pOldWnd);
    // 最初の入力可能セルを選択状態にする
    if (m_selectedCell.x == -1)
    {
        MoveSelection(1,0); //仮。右に移動して最初のセルを探す
    }
    Invalidate();
}
void CGridCtrl::OnKillFocus(CWnd* pNewWnd)
{
    CWnd::OnKillFocus(pNewWnd);

    // ★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★
    // ★                                                   ★
    // ★             ↓↓↓ ここから修正 ↓↓↓             ★
    // ★                                                   ★
    // ★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★

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


// --- ヘルパー関数 ---

CRect CGridCtrl::GetCellRect(int nRow, int nCol) const
{
    if (nRow < 0 || nRow >= m_nRows || nCol < 0 || nCol >= m_nCols)
    {
        return CRect(0,0,0,0);
    }

    int left = 0;
    for (int i = 0; i < nCol; ++i)
    {
        left += m_colWidths[i];
    }
    int right = left + m_colWidths[nCol];
    int top = nRow * m_nRowHeight;
    int bottom = top + m_nRowHeight;

    return CRect(left, top, right, bottom);
}


// --- ▼▼▼ この関数をファイルの末尾などに追加 ▼▼▼ ---
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
// --- ▼▼▼ この関数をファイルの末尾などに追加 ▼▼▼ ---
int CGridCtrl::GetRequiredHeight() const
{
    if (m_nRows == 0)
    {
        return 0; // まだグリッドがセットアップされていない場合
    }
    return m_nRows * m_nRowHeight;
}
// --- ▲▲▲ 追加 ▲▲▲ ---
int CGridCtrl::GetCellIndex(int nRow, int nCol) const
{
    if (nRow >= 0 && nRow < m_nRows && nCol >= 0 && nCol < m_nCols)
    {
        return nRow * m_nCols + nCol;
    }
    return -1;
}
CPoint CGridCtrl::HitTest(const CPoint& point) const
{
    for (int row = 0; row < m_nRows; ++row)
    {
        for (int col = 0; col < m_nCols; ++col)
        {
            if (GetCellRect(row, col).PtInRect(point))
            {
                return CPoint(col, row);
            }
        }
    }
    return CPoint(-1, -1);
}

void CGridCtrl::CreateInPlaceEdit()
{
    if (m_pEdit) return;
    if (m_selectedCell.x == -1) return;
    int index = GetCellIndex(m_selectedCell.y, m_selectedCell.x);
    if (!m_cells[index].m_bEditable) return;

    CRect rect = GetCellRect(m_selectedCell.y, m_selectedCell.x);
    rect.DeflateRect(1, 1); // 枠線にかぶらないように少し小さくする

    m_pEdit = new CInPlaceEdit(this, m_selectedCell, m_cells[index].m_strText);

    // Create関数の戻り値をチェックし、失敗した場合は後続処理を行わないようにする
    if (!m_pEdit->Create(WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL, rect, this, 1))
    {
        // 失敗した場合、デバッグ出力にエラーコードを表示する
        TRACE(L"Failed to create InPlaceEdit control. Error: %d\n", GetLastError());
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
    if (!m_pEdit) return;

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
    if (m_pEdit) return; // 編集中は移動しない
    if (m_selectedCell.x == -1) return; // 未選択状態なら何もしない

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
                Invalidate();
                return; // 見つかったので選択を更新して終了
            }
            newRow += dy; // 見つからなければ、さらに同じ方向へ探索
        }
    }
    
    // ここに到達した場合、指定された方向に編集可能なセルは見つからなかった。
    // 何もせずに関数を終了する。
}


// --- ▼▼▼ この関数をまるごと追加 ▼▼▼ ---
void CGridCtrl::SetActive(BOOL bActive)
{
    if (m_bIsActive == bActive)
    {
        return; // 状態が変更されない場合は何もしない
    }

    m_bIsActive = bActive;

    // 非アクティブに設定された場合は、セルの選択状態と編集状態を解除する
    if (!bActive)
    {
        if (m_pEdit)
        {
            // 編集中の内容を破棄して終了
            DestroyInPlaceEdit(FALSE);
        }
        m_selectedCell = CPoint(-1, -1);
    }
    
    Invalidate(); // 再描画を促す
}
// --- ▲▲▲ この関数をまるごと追加 ▲▲▲ ---
