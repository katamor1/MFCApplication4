
// MFCApplication4Dlg.h : ヘッダー ファイル
//

#pragma once
#include "CenterEdit.h"


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
	CCenterEdit* m_editCustom1;
	CCenterEdit* m_editCustom2;

// 実装
protected:
	HICON m_hIcon;

	// 生成された、メッセージ割り当て関数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
};
