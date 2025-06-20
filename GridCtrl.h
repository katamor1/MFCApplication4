// GridCtrl.h
#pragma once

#include <vector>
#include "InPlaceEdit.h"
// --- ▼▼▼ ここから追加 ▼▼▼ ---
// 親ウィンドウへの通知メッセージID
#define WM_GRID_CELL_CHANGED (WM_USER + 100)
#define WM_GRID_ACTIVATED    (WM_USER + 101)
// --- ▲▲▲ ここまで追加 ▲▲▲ ---


// 親ウィンドウへの通知メッセージ
#define WM_GRID_CELL_CHANGED (WM_USER + 100)

// CGridCtrl クラス
class CGridCtrl : public CWnd
{
    friend class CInPlaceEdit; // CInPlaceEditクラスに、このクラスのprotected/privateメンバーへのアクセスを許可する
public:
    CGridCtrl();
    virtual ~CGridCtrl();

    // --- 公開インターフェース ---
    BOOL Create(const RECT& rect, CWnd* pParentWnd, UINT nID);
    
    // グリッドの基本構成を設定
    BOOL SetupGrid(int nRows, int nCols);

    // 外観の設定
    void SetRowHeight(int nHeight);
    void SetColumnWidth(int nCol, int nWidth);
    void SetDefaultBgColor(COLORREF color);

    // セルごとの設定
    void SetCellText(int nRow, int nCol, const CString& strText);
    CString GetCellText(int nRow, int nCol) const;
    void SetCellEditable(int nRow, int nCol, BOOL bEditable);
    BOOL IsCellEditable(int nRow, int nCol) const;
    void SetCellBgColor(int nRow, int nCol, COLORREF color);
    
    // --- ▼▼▼ ここを修正 ▼▼▼ ---
    // staticキーワードを削除し、通常のメンバ関数に変更します。
    // constを付け、この関数がメンバ変数を変更しないことを示します。
    int GetRequiredWidth() const;
    int GetRequiredHeight() const;
    // --- ▲▲▲ 修正 ▲▲▲ ---

    // 再描画
    void RedrawGrid() { Invalidate(); }

    // 状態の操作/取得
    void SetActive(BOOL bActive);
    CPoint GetSelectedCell() const { return m_selectedCell; }

protected:
    // セル情報を保持する構造体
    struct CellInfo
    {
        CString  m_strText;      // セルに表示するテキスト
        COLORREF m_bgColor;      // 通常時の背景色
        BOOL     m_bEditable;    // 編集可能フラグ

        CellInfo() : m_bgColor(RGB(255, 255, 255)), m_bEditable(FALSE) {}
    };

    // --- メンバ変数を動的なものに変更 ---
    int m_nRows;
    int m_nCols;
    int m_nRowHeight;
    std::vector<int> m_colWidths;
    std::vector<CellInfo> m_cells;
    COLORREF m_defaultBgColor;

    CPoint m_selectedCell;
    BOOL m_bIsActive;
    CInPlaceEdit* m_pEdit;

    // ヘルパー関数
    int GetCellIndex(int nRow, int nCol) const;
    void CreateInPlaceEdit();
    void DestroyInPlaceEdit(BOOL bUpdate);
    CPoint HitTest(const CPoint& point) const;
    CRect GetCellRect(int nRow, int nCol) const;
    void MoveSelection(int dx, int dy);
    
    // メッセージハンドラ
    afx_msg void OnPaint();
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    afx_msg UINT OnGetDlgCode();
    afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
    afx_msg void OnSetFocus(CWnd* pOldWnd);
    afx_msg void OnKillFocus(CWnd* pNewWnd);

    DECLARE_MESSAGE_MAP()
};