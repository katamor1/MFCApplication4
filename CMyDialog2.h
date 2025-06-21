#pragma once
#include "afxdialogex.h"
#include "GridCtrl.h"

// MyDialog2.h

class CMyDialog2 : public CDialogEx
{
    DECLARE_DYNAMIC(CMyDialog2)

    static const int GRID_ARRAY_ROWS = 5;
    static const int GRID_ARRAY_COLS = 2;
    static const int TOTAL_GRIDS = (GRID_ARRAY_ROWS * GRID_ARRAY_COLS);

public:
    CMyDialog2(CWnd *pParent = nullptr); // 標準コンストラクタ
    virtual ~CMyDialog2();

    // ダイアログ データ
    enum
    {
        IDD = IDD_CMyDialog2
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

    virtual void DoDataExchange(CDataExchange *pDX); // DDX/DDV サポート
    void ActivateGrid(CGridCtrl *pGridToActivate);
    void EnsureGridVisible(CGridCtrl *pGrid); // 自動スクロール用ヘルパー関数
    afx_msg LRESULT OnGridCellChanged(WPARAM wParam, LPARAM lParam);
    afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar *pScrollBar);
    afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar *pScrollBar);
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
    virtual BOOL OnInitDialog();
    virtual BOOL PreTranslateMessage(MSG *pMsg);
    afx_msg LRESULT OnGridActivated(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnGridNavBoundaryHit(WPARAM wParam, LPARAM lParam);
    virtual void OnOK();

    DECLARE_MESSAGE_MAP()
};
