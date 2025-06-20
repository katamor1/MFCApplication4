
// MFCApplication4Dlg.h : ヘッダー ファイル
//

#pragma once
#include "CenterEdit.h"
#include "View1.h"
#include "CView2.h"
#include "CMyEdit.h"

// タイマーを識別するためのID
#define ID_TITLE_TIMER 1
// ワーカースレッドからView2の表示をトリガーするためのカスタムメッセージ
#define WM_SHOW_VIEW2 (WM_APP + 2)

// CMFCApplication4Dlg ダイアログ
class CMFCApplication4Dlg : public CDialogEx
{
// コンストラクション
public:
	CMFCApplication4Dlg(CWnd* pParent = nullptr);	// 標準コンストラクター

// ダイアログ データ
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MFCAPPLICATION4_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV サポート
	CCenterEdit* m_editCustom1 = nullptr;
	CCenterEdit* m_editCustom2 = nullptr;
	CView1* m_pView1 = nullptr; // View1へのポインタ
	CView2* m_pView2 = nullptr; // View2へのポインタ
	CMyEdit* m_editCustom3 = nullptr;

    // --- ▼▼▼ ここから追加 ▼▼▼ ---
    CString m_strOriginalTitle; // 元のタイトルを保存する変数
    // --- ▲▲▲ 追加 ▲▲▲ ---
// 実装
protected:
	HICON m_hIcon;

	// ★★★ この関数宣言を追加 ★★★
	void UpdateLayoutAndClipping();
	// ★★★ OnSizeメッセージハンドラの宣言を追加 ★★★
	afx_msg void OnSize(UINT nType, int cx, int cy);
	// 生成された、メッセージ割り当て関数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg LRESULT OnShowView2(WPARAM wParam, LPARAM lParam); // カスタムメッセージハンドラ
    // --- ▼▼▼ この行を追加 ▼▼▼ ---
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	// --- ▲▲▲ この行を追加 ▲▲▲ ---
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    afx_msg LRESULT OnShowOperationStatus(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
};
