// CView2.cpp : 実装ファイル
//

#include "pch.h"
#include "MFCApplication4.h"
#include "CView2.h"


// CView2

IMPLEMENT_DYNCREATE(CView2, CView)

BEGIN_MESSAGE_MAP(CView2, CView)
END_MESSAGE_MAP()

void CView2::OnDraw(CDC* pDC)
{
	if (m_editCustom1 == nullptr)
	{

		m_editCustom1 = new CCenterEdit();
		CRect rect1(60, 60, 110, 110); // ダイアログ内の位置とサイズを指定
		m_editCustom1->Create(WS_CHILD | WS_VISIBLE, rect1, this, 1004);
		m_editCustom1->ShowWindow(TRUE);
		m_editCustom1->SetWindowTextW(_T("1234")); // 初期値を設定
		m_editCustom1->SetFont(GetFont()); // フォントをダイアログのフォントに設定
		m_editCustom1->SetLimitText(4); // 入力文字数を4文字に制限
		m_editCustom1->SetWindowPos(nullptr, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
	}
	if (m_editCustom2 == nullptr)
	{

		m_editCustom2 = new CCenterEdit();
		CRect rect1(10, 10, 50, 50); // ダイアログ内の位置とサイズを指定
		m_editCustom2->Create(WS_CHILD | WS_VISIBLE, rect1, this, 1006);
		m_editCustom2->ShowWindow(TRUE);
		m_editCustom2->SetWindowTextW(_T("5678")); // 初期値を設定
		m_editCustom2->SetFont(GetFont()); // フォントをダイアログのフォントに設定
		m_editCustom2->SetLimitText(4); // 入力文字数を4文字に制限
		m_editCustom2->SetWindowPos(nullptr, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
	}
}

// ★★★ PreCreateWindow関数の実装をここに追加 ★★★
BOOL CView2::PreCreateWindow(CREATESTRUCT& cs)
{
	// まず基底クラスの関数を呼び出すのが作法です
	if (!CView::PreCreateWindow(cs))
		return FALSE;

	// WS_CLIPSIBLINGS スタイルをビット演算で削除します。
	// これにより、CView1は兄弟ウィンドウの領域を無視して描画するようになります。
	cs.style &= ~WS_CLIPSIBLINGS;

	return TRUE;
}


// CView2 診断

#ifdef _DEBUG
void CView2::AssertValid() const
{
	CView::AssertValid();
}

#ifndef _WIN32_WCE
void CView2::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif
#endif //_DEBUG


// CView2 メッセージ ハンドラー
