/**
 * @file GridCtrl.h
 * @brief 汎用的な表形式（グリッド）カスタムコントロールのクラス宣言
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
     * @param[in] nMaxVisibleRows 一度に表示する最大行数（省略時10）
     */
    CGridCtrl(int nMaxVisibleRows = 10);
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
     * @return TRUE/成功時、FALSE/失敗時。
     */
    BOOL Create(const RECT& rect, CWnd* pParentWnd, UINT nID);
    
    /**
     * @brief グリッドの基本構成（行数・列数）を設定します。
     * @details この関数は、他のプロパティ設定の前に呼び出す必要があります。
     * @param[in] nRows 設定する行数。
     * @param[in] nCols 設定する列数。
     * @return セットアップが成功した場合はTRUE、失敗した場合はFALSE。
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
    
    /**
     * @brief 指定したセルにテキストを設定します。
     * @param[in] nRow 行インデックス (0始まり)
     * @param[in] nCol 列インデックス (0始まり)
     * @param[in] strText 設定するテキスト
     */
    void SetCellText(int nRow, int nCol, const CString& strText);

    /**
     * @brief 指定したセルのテキストを取得します。
     * @param[in] nRow 行インデックス (0始まり)
     * @param[in] nCol 列インデックス (0始まり)
     * @return セルのテキスト
     */
    CString GetCellText(int nRow, int nCol) const;
    
    /**
     * @brief 指定したセルの編集可否を設定します。
     * @param[in] nRow 行インデックス (0始まり)
     * @param[in] nCol 列インデックス (0始まり)
     * @param[in] bEditable 編集可能にする場合はTRUE
     */
    void SetCellEditable(int nRow, int nCol, BOOL bEditable);
    
    /**
     * @brief 指定したセルが編集可能かを取得します。
     * @param[in] nRow 行インデックス (0始まり)
     * @param[in] nCol 列インデックス (0始まり)
     * @return 編集可能な場合はTRUE
     */
    BOOL IsCellEditable(int nRow, int nCol) const;
    
    /**
     * @brief 指定したセルの背景色を設定します。
     * @param[in] nRow 行インデックス (0始まり)
     * @param[in] nCol 列インデックス (0始まり)
     * @param[in] color 設定する色 (COLORREF)
     */
    void SetCellBgColor(int nRow, int nCol, COLORREF color);
    
    // --- サイズ取得 ---
    
    /**
     * @brief グリッドの全コンテンツを表示するために必要な幅を返します。
     * @return 必要な幅(ピクセル)
     */
    int GetRequiredWidth() const;

    /**
     * @brief グリッドの全コンテンツを表示するために必要な高さを返します。
     * @details この実装では、内部スクロールバーで表示される最大行数までの高さを返します。
     * @return 必要な高さ(ピクセル)
     */
    int GetRequiredHeight() const;

    /**
     * @brief コントロールを強制的に再描画します。
     * @details Invalidate()を呼び出すラッパー関数です。
     */
    void RedrawGrid() { Invalidate(); }

    /**
     * @brief 一度に表示する最大行数を設定します。
     * @param[in] nMaxRows 最大行数
     */
    void SetMaxVisibleRows(int nMaxRows) { m_nMaxVisibleRows = nMaxRows; }
    /**
     * @brief 一度に表示する最大行数を取得します。
     * @return 最大行数
     */
    int GetMaxVisibleRows() const { return m_nMaxVisibleRows; }

    // --- 状態の操作/取得 ---
    
    /**
     * @brief このグリッドコントロールのアクティブ/非アクティブ状態を設定します。
     * @details アクティブ状態は、例えば複数のグリッドが配置されたダイアログで、
     * 現在どのグリッドがキーボードフォーカスを受け取る対象かを示すために使われます。
     * @param[in] bActive アクティブにする場合はTRUE
     */
    void SetActive(BOOL bActive);
    
    /**
     * @brief 現在選択されているセルの位置を取得します。
     * @return CPointオブジェクト。xが列、yが行を表します。非選択時は(-1, -1)。
     */
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

        CellInfo() : m_strText(_T("")), m_bgColor(RGB(255, 255, 255)), m_bEditable(FALSE) {}
    };

    /// @brief 内部スクロールバーで一度に表示する最大行数（デフォルト10、setterで変更可）
    int m_nMaxVisibleRows;
    
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

    /**
     * @brief 論理的な行・列インデックスから、1次元配列m_cellsのインデックスを計算します。
     * @param[in] nRow 行インデックス (0始まり)
     * @param[in] nCol 列インデックス (0始まり)
     * @return 配列のインデックス。範囲外の場合は-1。
     */
    int GetCellIndex(int nRow, int nCol) const;
    
    /**
     * @brief インプレイス編集用のエディットコントロールを生成し、表示します。
     */
    void CreateInPlaceEdit();

    /**
     * @brief インプレイス編集用のエディットコントロールを破棄します。
     * @param[in] bUpdate TRUEの場合、編集内容をセルに反映します。FALSEの場合は反映せずに破棄します。
     */
    void DestroyInPlaceEdit(BOOL bUpdate);
    
    /**
     * @brief 物理的な座標点から、それがどの論理セルに該当するかを計算します。
     * @param[in] point クライアント座標
     * @return セルの論理インデックス (列, 行)。見つからない場合は (-1, -1)。
     */
    CPoint HitTest(const CPoint& point) const;

    /**
     * @brief 論理的な行・列インデックスから、物理的な描画矩形を計算します。
     * @param[in] nRow 行インデックス (0始まり)
     * @param[in] nCol 列インデックス (0始まり)
     * @return セルの描画矩形 (クライアント座標)
     */
    CRect GetCellRect(int nRow, int nCol) const;

    /**
     * @brief カーソルキー入力に応じて、編集可能なセル間を選択移動します。
     * @param[in] dx 水平方向の移動量 (-1:左, 1:右)
     * @param[in] dy 垂直方向の移動量 (-1:上, 1:下)
     */
    void MoveSelection(int dx, int dy);
    
    /**
     * @brief 指定したセルが表示されるように、必要であればグリッドをスクロールします。
     * @param[in] nRow 表示させたいセルの行インデックス (0始まり)
     * @param[in] nCol 表示させたいセルの列インデックス (0始まり)
     */
    void EnsureCellVisible(int nRow, int nCol);

    /**
     * @brief 内部スクロールバーの状態を更新します。
     * @details グリッドの総行数と表示可能行数に基づいて、スクロールバーの範囲や表示/非表示を設定します。
     */
    void UpdateScrollbar();

    // --- メッセージハンドラ ---
    
    /**
     * @brief ウィンドウ作成時に呼び出されます (WM_CREATE)。
     * @param[in] lpCreateStruct 作成情報
     * @return 成功なら0、失敗なら-1
     */
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    
    /**
     * @brief 描画イベント(WM_PAINT)を処理します。
     * @details ダブルバッファリングを用いて、グリッドの内容全体を描画します。
     */
    afx_msg void OnPaint();
    
    /**
     * @brief マウス左ボタン押下イベント(WM_LBUTTONDOWN)を処理します。
     * @param[in] nFlags 修飾キーの状態
     * @param[in] point マウスカーソルのクライアント座標
     */
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);

    /**
     * @brief ダイアログナビゲーションのためのキー種別を返します (WM_GETDLGCODE)。
     * @details カーソルキーや文字キーをダイアログに奪われず、自前で処理するために必要です。
     * @return DLGC_WANTARROWS | DLGC_WANTCHARS
     */
    afx_msg UINT OnGetDlgCode();

    /**
     * @brief キー押下イベント(WM_KEYDOWN)を処理します。
     * @param[in] nChar 仮想キーコード
     * @param[in] nRepCnt キーのリピート回数
     * @param[in] nFlags 修飾キーの状態
     */
    afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

    /**
     * @brief フォーカスを受け取った際のイベントハンドラ (WM_SETFOCUS)。
     * @param[in] pOldWnd フォーカスを失ったウィンドウ
     */
    afx_msg void OnSetFocus(CWnd* pOldWnd);

    /**
     * @brief フォーカスを失った際のイベントハンドラ (WM_KILLFOCUS)。
     * @param[in] pNewWnd 新しくフォーカスを受け取るウィンドウ
     */
    afx_msg void OnKillFocus(CWnd* pNewWnd);
    
    /**
     * @brief 垂直スクロールイベント(WM_VSCROLL)を処理します。
     * @param[in] nSBCode スクロールバーのコード
     * @param[in] nPos スクロールボックスの位置
     * @param[in] pScrollBar スクロールバーコントロールへのポインタ
     */
    afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
    
    /**
     * @brief マウスホイールイベント(WM_MOUSEWHEEL)を処理します。
     * @param[in] nFlags 修飾キーの状態
     * @param[in] zDelta ホイールの回転量
     * @param[in] pt カーソルの位置
     * @return メッセージを処理した場合はTRUE
     */
    afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);

    /// @brief メッセージマップを宣言します。
    DECLARE_MESSAGE_MAP()
};