#pragma once

// CCenterEdit
// 初回表示のタイミングを調整するためのカスタムメッセージ
#define WM_APP_POST_INIT (WM_APP + 1)

class CCenterEdit : public CEdit
{
	DECLARE_DYNAMIC(CCenterEdit)

public:
	CCenterEdit();
	virtual ~CCenterEdit();

	// オーバーライド
protected:
	virtual void PreSubclassWindow();
	virtual BOOL PreTranslateMessage(MSG *pMsg);

	// メンバ変数
protected:
	CBrush m_brushFocus;		 // フォーカス時の背景ブラシ
	CBrush m_brushNoFocus;		 // 非フォーカス時の背景ブラシ
	COLORREF m_colorTextFocus;	 // フォーカス時の文字色
	COLORREF m_colorTextNoFocus; // 非フォーカス時の文字色
	bool m_bIsFirstTimeInit = true;
	CString m_strOriginalText;
	// 実装
protected:
	void UpdateTextPosition(); // テキストの垂直位置を更新するヘルパー関数

	// メッセージ マップ
protected:
	afx_msg HBRUSH CtlColor(CDC *pDC, UINT nCtlColor);
	afx_msg void OnSetFocus(CWnd *pOldWnd);
	afx_msg void OnKillFocus(CWnd *pNewWnd);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnEnChange(); // テキスト変更ハンドラ
	afx_msg LRESULT OnPostInit(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()
};
