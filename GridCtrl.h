/**
 * @file GridCtrl.h
 * @brief 汎用的な表形式（グリッド）カスタムコントロールのクラス宣言
 * @author C++/MFCコーディング (AI)
 * @date 2025/06/22
 * @details 行数、列数、セルの大きさやプロパティを動的に設定可能で、
 * 内部に垂直スクロール機能を持つ自己完結型のコントロールです。
 * 親ウィンドウとは定義されたカスタムメッセージや通知コードで連携します。
 */
#pragma once

#include "InPlaceEdit.h"
#include <vector>

// --- 親ウィンドウへの通知メッセージ ---

/// @brief 親ウィンドウへセルの内容変更を通知します。wParam:コントロールID, lParam:MAKELPARAM(row, col)
#define WM_GRID_CELL_CHANGED (WM_USER + 100)
/// @brief 親ウィンドウへこのグリッドがクリックされ、アクティブになったことを通知します。wParam:コントロールID
#define WM_GRID_ACTIVATED    (WM_USER + 101)
/// @brief 親ウィンドウへキーボードナビゲーションがグリッドの端に到達したことを通知します。wParam:押されたキーコード, lParam:コントロールID
#define WM_GRID_NAV_BOUNDARY_HIT (WM_USER + 102)

// --- CGridCtrlから親への通知コード (WM_NOTIFYで使用) ---

/// @brief 行選択が変更されたことを示す通知コード
#define GCN_SELCHANGED (NM_FIRST - 1)

/**
 * @struct NM_GRIDVIEW
 * @brief WM_NOTIFYメッセージで送信するカスタム通知構造体
 * @details NMHDRを最初のメンバーとして持ち、選択されたセルの行・列情報を格納します。
 */
struct NM_GRIDVIEW
{
    NMHDR   hdr;    ///< Windows標準の通知ヘッダー
    int     iRow;   ///< 選択された行インデックス (0始まり)
    int     iCol;   ///< 選択された列インデックス (0始まり)
};


/**
 * @class CGridCtrl
 * @brief 高度にカスタマイズ可能な表形式のカスタムコントロール
 * @details CWndを基底クラスとし、スクロール、インプレイス編集、動的なスタイル変更など、豊富な機能を持ちます。
 * ちらつき防止のためにダブルバッファリングで描画されます。
 */
class CGridCtrl : public CWnd
{
    // CInPlaceEditクラスに、このクラスのprotected/privateメンバーへのアクセスを許可します。
    // これにより、CInPlaceEditは自身の破棄を親であるCGridCtrlに通知できます。
    friend class CInPlaceEdit;

public:
    /**
     * @brief デフォルトコンストラクタ
     */
    CGridCtrl();
    /**
     * @brief デストラクタ
     */
    virtual ~CGridCtrl() override;

    // --- 公開インターフェース ---

    /**
     * @brief コントロールのウィンドウを生成・初期化します。
     * @param[in] rect 親ウィンドウのクライアント座標におけるコントロールの位置とサイズ。
     * @param[in] pParentWnd 親ウィンドウへのポインタ。
     * @param[in] nID コントロールID。
     * @return 成功した場合は0以外、失敗した場合は0。
     */
    BOOL Create(const RECT& rect, CWnd* pParentWnd, UINT nID);
    
    /**
     * @brief グリッドの基本構成（行数・列数）を設定します。
     * @details この関数は、他のプロパティ設定の前に呼び出す必要があります。
     * @param[in] nRows 設定する行数。
     * @param[in] nCols 設定する列数。
     * @return セットアップが成功した場合はTRUE。
     */
    BOOL SetupGrid(int nRows, int nCols);

    /**
     * @brief 全ての行の高さを設定します。
     * @param[in] nHeight ピクセル単位での行の高さ。
     */
    void SetRowHeight(int nHeight);

    /**
     * @brief 指定した列の幅を設定します。
     * @param[in] nCol 幅を設定する列のインデックス (0始まり)。
     * @param[in] nWidth ピクセル単位での列の幅。
     */
    void SetColumnWidth(int nCol, int nWidth);
    
    /**
     * @brief 編集不可セルのデフォルト背景色を設定します。
     * @param[in] color 設定する色 (COLORREF)。
     */
    void SetDefaultBgColor(COLORREF color);

    // --- セルごとの設定 ---
    void SetCellText(int nRow, int nCol, const CString& strText);
    CString GetCellText(int nRow, int nCol) const;
    void SetCellEditable(int nRow, int nCol, BOOL bEditable);
    BOOL IsCellEditable(int nRow, int nCol) const;
    void SetCellBgColor(int nRow, int nCol, COLORREF color);
    
    // --- サイズ取得 ---
    int GetRequiredWidth() const;
    int GetRequiredHeight() const;

    /**
     * @brief コントロールを強制的に再描画します。
     * @details Invalidate()を呼び出すラッパー関数です。
     */
    void RedrawGrid() { Invalidate(); }

    // --- 状態の操作/取得 ---
    void SetActive(BOOL bActive);
    CPoint GetSelectedCell() const { return m_selectedCell; }

protected:
    /**
     * @struct CellInfo
     * @brief 個々のセルの情報を保持する内部構造体。
     */
    struct CellInfo
    {
        CString  m_strText;      ///< セルに表示するテキスト
        COLORREF m_bgColor;      ///< 通常時の背景色
        BOOL     m_bEditable;    ///< 編集可能フラグ

        CellInfo() : m_bgColor(RGB(255, 255, 255)), m_bEditable(FALSE) {}
    };

    /// @brief 内部スクロールバーで一度に表示する最大行数
    static const int MAX_VISIBLE_ROWS = 10;
    
    // --- 状態変数 ---
    /// @brief 表示領域の一番上に表示されている行のインデックス (0始まり)
    int m_nTopRow;
    /// @brief グリッドの総行数
    int m_nRows;
    /// @brief グリッドの総列数
    int m_nCols;
    /// @brief 1行の高さ
    int m_nRowHeight;
    /// @brief デフォルトの背景色
    COLORREF m_defaultBgColor;

    // --- データコンテナ ---
    /// @brief 各列の幅を保持する動的配列
    std::vector<int> m_colWidths;
    /// @brief 全セルの情報を保持する動的配列 (1次元配列で2次元を表現)
    std::vector<CellInfo> m_cells;

    // --- UI状態 ---
    /// @brief 現在選択されているセルの位置 (-1,-1で非選択)
    CPoint m_selectedCell;
    /// @brief このグリッドがアクティブかどうかのフラグ
    BOOL m_bIsActive;
    /// @brief インプレイス編集用のエディットコントロールのポインタ
    CInPlaceEdit* m_pEdit;

    // --- ヘルパー関数 ---
    int GetCellIndex(int nRow, int nCol) const;
    void CreateInPlaceEdit();
    void DestroyInPlaceEdit(BOOL bUpdate);
    CPoint HitTest(const CPoint& point) const;
    CRect GetCellRect(int nRow, int nCol) const;
    void MoveSelection(int dx, int dy);
    void EnsureCellVisible(int nRow, int nCol);
    void UpdateScrollbar();

    // --- メッセージハンドラ ---
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnPaint();
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    afx_msg UINT OnGetDlgCode();
    afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
    afx_msg void OnSetFocus(CWnd* pOldWnd);
    afx_msg void OnKillFocus(CWnd* pNewWnd);
    afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
    afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);

    /// @brief メッセージマップを宣言します。
    DECLARE_MESSAGE_MAP()
};