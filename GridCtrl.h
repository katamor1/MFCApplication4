// GridCtrl.h
#pragma once

#include <vector>
#include "InPlaceEdit.h"
// 親ウィンドウへの通知メッセージID
#define WM_GRID_CELL_CHANGED (WM_USER + 100)
#define WM_GRID_ACTIVATED (WM_USER + 101)
#define WM_GRID_NAV_BOUNDARY_HIT (WM_USER + 102)
// 通知コード
#define GCN_SELCHANGED (NM_FIRST - 1)

// WM_NOTIFYで送信する構造体
struct NM_GRIDVIEW
{
    NMHDR hdr; // NMHDR構造体を最初のメンバーとして含めます
    int iRow;
    int iCol;
};

// CGridCtrl クラス
class CGridCtrl : public CWnd
{
    friend class CInPlaceEdit; // CInPlaceEditクラスに、このクラスのprotected/privateメンバーへのアクセスを許可する
public:
    CGridCtrl();
    virtual ~CGridCtrl();

    BOOL Create(const RECT &rect, CWnd *pParentWnd, UINT nID);

    // グリッドの基本構成を設定
    BOOL SetupGrid(int nRows, int nCols);

    // 外観の設定
    void SetRowHeight(int nHeight);
    void SetColumnWidth(int nCol, int nWidth);
    void SetDefaultBgColor(COLORREF color);

    // セルごとの設定
    void SetCellText(int nRow, int nCol, const CString &strText);
    CString GetCellText(int nRow, int nCol) const;
    void SetCellEditable(int nRow, int nCol, BOOL bEditable);
    BOOL IsCellEditable(int nRow, int nCol) const;
    void SetCellBgColor(int nRow, int nCol, COLORREF color);
    int GetRequiredWidth() const;
    int GetRequiredHeight() const;

    // 再描画
    void RedrawGrid() { Invalidate(); }

    // 状態の操作/取得
    void SetActive(BOOL bActive);
    CPoint GetSelectedCell() const { return m_selectedCell; }

protected:
    // セル情報を保持する構造体
    struct CellInfo
    {
        CString m_strText;  // セルに表示するテキスト
        COLORREF m_bgColor; // 通常時の背景色
        BOOL m_bEditable;   // 編集可能フラグ

        CellInfo() : m_bgColor(RGB(255, 255, 255)), m_bEditable(FALSE) {}
    };

    static const int MAX_VISIBLE_ROWS = 10; // 最大表示行数
    int m_nTopRow;                          // 表示領域の一番上に表示されている行のインデックス
    int m_nRows;
    int m_nCols;
    int m_nRowHeight;
    std::vector<int> m_colWidths;
    std::vector<CellInfo> m_cells;
    COLORREF m_defaultBgColor;

    CPoint m_selectedCell;
    BOOL m_bIsActive;
    CInPlaceEdit *m_pEdit;

    // ヘルパー関数
    int GetCellIndex(int nRow, int nCol) const;
    void CreateInPlaceEdit();
    void DestroyInPlaceEdit(BOOL bUpdate);
    CPoint HitTest(const CPoint &point) const;
    CRect GetCellRect(int nRow, int nCol) const;
    void MoveSelection(int dx, int dy);
    void EnsureCellVisible(int nRow, int nCol);
    void UpdateScrollbar(); // スクロールバーの状態を更新するヘルパー

    // メッセージハンドラ
    afx_msg void OnPaint();
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    afx_msg UINT OnGetDlgCode();
    afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
    afx_msg void OnSetFocus(CWnd *pOldWnd);
    afx_msg void OnKillFocus(CWnd *pNewWnd);
    afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar *pScrollBar);
    afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

    DECLARE_MESSAGE_MAP()
};