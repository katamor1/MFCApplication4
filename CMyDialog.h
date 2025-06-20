#pragma once
#include "afxdialogex.h"
#include "GridCtrl.h"

#define GRID_COUNT 10       // グリッドコントロールの数
#define GRID_MARGIN_Y 10    // 各グリッドコントロールの縦の間隔

// CMyDialog ダイアログ


class CMyDialog : public CDialogEx
{
	DECLARE_DYNAMIC(CMyDialog)

public:
	CMyDialog(CWnd* pParent = nullptr);   // 標準コンストラクター
	virtual ~CMyDialog();
// ダイアログ データ
	enum { IDD = IDD_CMyDialog };

protected:
    CGridCtrl m_grids[GRID_COUNT];
    
    CGridCtrl* m_pActiveGrid; // 最新のクリックされたグリッドを保持するポインタ

    // スクロール関連のメンバ変数
    int m_nTotalWidth;  // 全グリッドを配置した場合の合計の幅
    int m_nTotalHeight; // 全グリッドを配置した場合の合計の高さ
    int m_nHScrollPos;  // 現在の水平スクロール位置
    int m_nVScrollPos;  // 現在の垂直スクロール位置
    // スクロール情報を更新するためのヘルパー関数
    void UpdateScrollInfo();

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート
    void ActivateGrid(CGridCtrl* pGridToActivate);
    void EnsureGridVisible(CGridCtrl* pGrid); // 自動スクロール用ヘルパー関数
	afx_msg LRESULT OnGridCellChanged(WPARAM wParam, LPARAM lParam);
    afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
    afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	virtual BOOL OnInitDialog();
    virtual BOOL PreTranslateMessage(MSG* pMsg);
    afx_msg LRESULT OnGridActivated(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()
};
