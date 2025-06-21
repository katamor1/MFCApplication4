#pragma once
#include "afxdialogex.h"
#include "GridCtrl.h"

// CMyDialog ダイアログ

class CMyDialog : public CDialogEx
{
    DECLARE_DYNAMIC(CMyDialog)

    static const int GRID_ARRAY_ROWS = 10;
    static const int GRID_ARRAY_COLS = 1;
    static const int TOTAL_GRIDS = (GRID_ARRAY_ROWS * GRID_ARRAY_COLS);

public:
    explicit CMyDialog(CWnd *pParent = nullptr); // 標準コンストラクター
    virtual ~CMyDialog() override;

    // ダイアログ データ
    enum
    {
        IDD = IDD_CMyDialog
    };

protected:
    CGridCtrl m_grids[TOTAL_GRIDS];

    CGridCtrl *m_pActiveGrid; // 最新のクリックされたグリッドを保持するポインタ

    // スクロール関連のメンバ変数
    int m_nTotalWidth;  // 全グリッドを配置した場合の合計の幅
    int m_nTotalHeight; // 全グリッドを配置した場合の合計の高さ
    int m_nHScrollPos;  // 現在の水平スクロール位置
    int m_nVScrollPos;  // 現在の垂直スクロール位置
    // スクロール情報を更新するためのヘルパー関数
    void UpdateScrollInfo();

    virtual void DoDataExchange(CDataExchange *pDX) override; // DDX/DDV サポート
    void ActivateGrid(CGridCtrl *pGridToActivate);
    void EnsureGridVisible(CGridCtrl *pGrid); // 自動スクロール用ヘルパー関数
    afx_msg LRESULT OnGridCellChanged(WPARAM wParam, LPARAM lParam);
    afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar *pScrollBar);
    afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar *pScrollBar);
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
    virtual BOOL OnInitDialog() override;
    virtual BOOL PreTranslateMessage(MSG *pMsg) override;
    afx_msg LRESULT OnGridActivated(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnGridNavBoundaryHit(WPARAM wParam, LPARAM lParam);
    virtual void OnOK() override;

    DECLARE_MESSAGE_MAP()
};
