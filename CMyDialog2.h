/**
 * @file CMyDialog2.h
 * @brief 複数のCGridCtrlをスクロール可能なダイアログ上に配置し、連携させるクラスの宣言
 * @details このダイアログは、複数のグリッドコントロールを動的に生成し、
 * スクロールバーによって全体を表示領域内でナビゲーションする機能を提供します。
 * また、グリッド間のフォーカス移動や連携も管理します。
 */
#pragma once
#include "afxdialogex.h"
#include "GridCtrl.h"

/**
 * @class CMyDialog2
 * @brief 複数のグリッドを管理するスクロール可能なダイアログクラス
 * @details CGridCtrlの配列をメンバとして持ち、ダイアログのスクロール、
 * グリッド間のキーボードナビゲーション、アクティブなグリッドの管理などを行います。
 * このバージョンでは、CGridCtrlのセルはすべて読み取り専用です。
 */
class CMyDialog2 : public CDialogEx
{
    DECLARE_DYNAMIC(CMyDialog2)

    // --- 定数定義 ---
    /// @brief グリッド配列の行数
    static const int GRID_ARRAY_ROWS = 5;
    /// @brief グリッド配列の列数
    static const int GRID_ARRAY_COLS = 2;
    /// @brief 合計グリッド数
    static const int TOTAL_GRIDS = (GRID_ARRAY_ROWS * GRID_ARRAY_COLS);

public:
    /**
     * @brief 標準コンストラクタ
     * @param[in] pParent 親ウィンドウへのポインタ
     */
    CMyDialog2(CWnd *pParent = nullptr);

    /**
     * @brief デストラクタ
     */
    virtual ~CMyDialog2();

    /// @brief ダイアログ データのリソースID
    enum
    {
        IDD = IDD_CMyDialog2
    };

protected:
    // --- コントロール ---
    /// @brief グリッドコントロールの配列
    CGridCtrl m_grids[TOTAL_GRIDS];

    // --- 状態管理 ---
    /// @brief 現在アクティブ（フォーカスを持っている）なグリッドへのポインタ
    CGridCtrl *m_pActiveGrid;

    // --- スクロール関連メンバ ---
    /// @brief 全グリッドを配置した場合の合計の幅
    int m_nTotalWidth;
    /// @brief 全グリッドを配置した場合の合計の高さ
    int m_nTotalHeight;
    /// @brief 現在の水平スクロール位置 (ピクセル単位)
    int m_nHScrollPos;
    /// @brief 現在の垂直スクロール位置 (ピクセル単位)
    int m_nVScrollPos;
    
    // --- ヘルパー関数 ---

    /**
     * @brief ダイアログのスクロールバー情報を現在の状態に合わせて更新します。
     * @details 全コンテンツのサイズとクライアント領域のサイズを比較し、
     * スクロールバーの表示/非表示、範囲、ページサイズを適切に設定します。
     */
    void UpdateScrollInfo();

    /**
     * @brief 指定されたグリッドをアクティブ状態にします。
     * @param[in] pGridToActivate アクティブにするグリッドコントロールへのポインタ
     * @details 他のグリッドは非アクティブ化し、指定グリッドにフォーカスを移します。
     */
    void ActivateGrid(CGridCtrl *pGridToActivate);

    /**
     * @brief 指定されたグリッドが完全に表示されるようにダイアログを自動でスクロールします。
     * @param[in] pGrid 対象のグリッドコントロールへのポインタ
     */
    void EnsureGridVisible(CGridCtrl *pGrid);

    // --- DDX/DDV ---
    /**
     * @brief DDX/DDV（ダイアログデータエクスチェンジ/バリデーション）のサポート
     * @param pDX データエクスチェンジオブジェクトへのポインタ
     */
    virtual void DoDataExchange(CDataExchange *pDX);

    // --- メッセージハンドラ ---
    
    /**
     * @brief CGridCtrlからセルの内容変更通知(WM_GRID_CELL_CHANGED)を処理します。
     * @param wParam 通知元のコントロールID
     * @param lParam 変更されたセルの位置 (LOWORD:行, HIWORD:列)
     * @return 常に0
     */
    afx_msg LRESULT OnGridCellChanged(WPARAM wParam, LPARAM lParam);
    
    /**
     * @brief 水平スクロールイベント(WM_HSCROLL)を処理します。
     * @param nSBCode スクロールバーのコード
     * @param nPos スクロールボックスの位置
     * @param pScrollBar スクロールバーコントロールへのポインタ
     */
    afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar *pScrollBar);
    
    /**
     * @brief 垂直スクロールイベント(WM_VSCROLL)を処理します。
     * @param nSBCode スクロールバーのコード
     * @param nPos スクロールボックスの位置
     * @param pScrollBar スクロールバーコントロールへのポインタ
     */
    afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar *pScrollBar);
    
    /**
     * @brief ダイアログのサイズ変更イベント(WM_SIZE)を処理します。
     * @param nType サイズ変更の種類
     * @param cx クライアント領域の新しい幅
     * @param cy クライアント領域の新しい高さ
     */
    afx_msg void OnSize(UINT nType, int cx, int cy);
    
    /**
     * @brief マウスホイールイベント(WM_MOUSEWHEEL)を処理します。
     * @param nFlags 修飾キーの状態
     * @param zDelta ホイールの回転量
     * @param pt カーソルの位置
     * @return メッセージを処理した場合はTRUE
     */
    afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);

    /**
     * @brief ダイアログの初期化処理(WM_INITDIALOG)をオーバーライドします。
     * @details グリッドコントロールの動的生成と配置、スクロールバーの初期設定を行います。
     * @return フォーカスをコントロールに設定しない場合はTRUE
     */
    virtual BOOL OnInitDialog();
    
    /**
     * @brief キーボードメッセージをウィンドウにディスパッチされる前に横取りします。
     * @details EnterキーとEscapeキーによるダイアログ終了を無効化するために使用します。
     * @param[in] pMsg メッセージ情報へのポインタ
     * @return メッセージを処理した場合はTRUE、デフォルト処理に任せる場合はFALSE。
     */
    virtual BOOL PreTranslateMessage(MSG *pMsg);
    
    /**
     * @brief CGridCtrlからグリッドのアクティブ化通知(WM_GRID_ACTIVATED)を処理します。
     * @param wParam 通知元のコントロールID
     * @param lParam 未使用
     * @return 常に0
     */
    afx_msg LRESULT OnGridActivated(WPARAM wParam, LPARAM lParam);
    
    /**
     * @brief CGridCtrlからグリッド境界でのナビゲーション通知(WM_GRID_NAV_BOUNDARY_HIT)を処理します。
     * @param wParam 押されたカーソルキーの仮想キーコード (VK_UP, VK_DOWNなど)
     * @param lParam 通知元のコントロールID
     * @return 常に0
     */
    afx_msg LRESULT OnGridNavBoundaryHit(WPARAM wParam, LPARAM lParam);
    
    /**
     * @brief Enterキーによるダイアログ終了を無効化するためにオーバーライドします。
     */
    virtual void OnOK();

    /// @brief メッセージマップを宣言します。
    DECLARE_MESSAGE_MAP()
};