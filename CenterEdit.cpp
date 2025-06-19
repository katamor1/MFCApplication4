#include "pch.h"
#include "CenterEdit.h"
#include "SoftwareKeyboardDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CCenterEdit

IMPLEMENT_DYNAMIC(CCenterEdit, CEdit)

CCenterEdit::CCenterEdit()
{
	// ブラシと色の初期化
	m_brushFocus.CreateSolidBrush(RGB(255, 255, 0));  // 黄色
	m_brushNoFocus.CreateSolidBrush(RGB(0, 255, 255)); // 白色
	m_colorTextFocus = RGB(255, 0, 0);     // 赤色
	m_colorTextNoFocus = RGB(0, 0, 0);       // 黒色
}

CCenterEdit::~CCenterEdit()
{
}

BEGIN_MESSAGE_MAP(CCenterEdit, CEdit)
	// ON_WM_CTLCOLOR_REFLECT() は、親ウィンドウに送られるWM_CTLCOLORメッセージを
	// このコントロール自身で処理するためのマクロです。これにより、コントロールが自己完結します。
	ON_WM_CTLCOLOR_REFLECT()
	ON_WM_SETFOCUS()
	ON_WM_KILLFOCUS()
	ON_WM_SIZE()
	ON_CONTROL_REFLECT(EN_CHANGE, &CCenterEdit::OnEnChange)
	ON_MESSAGE(WM_APP_POST_INIT, &CCenterEdit::OnPostInit)
END_MESSAGE_MAP()


// CCenterEdit メッセージ ハンドラー

void CCenterEdit::PreSubclassWindow()
{
	// 1. 水平方向の配置を右寄せに設定
	ModifyStyle(0, ES_RIGHT | ES_MULTILINE, 0);

	// 2. 入力文字数を4文字に制限
	SetLimitText(4);

	// ダイアログリソースからコントロールがサブクラス化される際に一度だけ呼ばれる
	CEdit::PreSubclassWindow();
}


BOOL CCenterEdit::PreTranslateMessage(MSG* pMsg)
{
	// ES_MULTILINE使用時にEnterキーで改行されるのを防ぐ
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN)
	{
		GetParent()->SetFocus(); // 親ウィンドウにフォーカスを移動
		return TRUE; // メッセージを破棄
	}
	return CEdit::PreTranslateMessage(pMsg);
}
void CCenterEdit::OnSetFocus(CWnd* pOldWnd)
{
	CEdit::OnSetFocus(pOldWnd);

	// 1. 現在のテキストを保存
	GetWindowText(m_strOriginalText);

	// 2. ソフトウェアキーボードをモーダルで表示
	//    コンストラクタに自身(this)を渡し、どのエディットへの入力か伝える
	CSoftwareKeyboardDlg dlg(this);
	INT_PTR nResponse = dlg.DoModal();

	// 3. IDCANCELで閉じられた場合、テキストを元に戻す
	if (nResponse == IDCANCEL)
	{
		SetWindowText(m_strOriginalText);
	}

	// 4. キーボードが閉じたら、親ウィンドウにフォーカスを戻す
	//    これにより、エディットコントロールからフォーカスが外れる
	if (GetParent())
	{
		GetParent()->SetFocus();
	}
	Invalidate(); // 再描画を促す
}

void CCenterEdit::OnKillFocus(CWnd* pNewWnd)
{
	CEdit::OnKillFocus(pNewWnd);
	Invalidate(); // 再描画を促す
}

void CCenterEdit::OnEnChange()
{
	// テキストが変更されたときも念のため矩形を再設定
	UpdateTextPosition();
	// Invalidateはテキスト変更時に自動で再描画がかかるので、通常は不要
}

void CCenterEdit::UpdateTextPosition()
{
	CRect rectClient;
	GetClientRect(&rectClient);

	if (rectClient.IsRectEmpty())
	{
		return;
	}

	CDC* pDC = GetDC();
	if (pDC)
	{
		CFont* pFont = GetFont();
		CFont* pOldFont = pDC->SelectObject(pFont);
		TEXTMETRIC tm;
		pDC->GetTextMetrics(&tm);
		pDC->SelectObject(pOldFont);
		ReleaseDC(pDC);

		int topMargin = (rectClient.Height() - tm.tmHeight) / 2;
		if (topMargin < 0)
		{
			topMargin = 0;
		}

		CRect rectFormat = rectClient;
		rectFormat.top = topMargin;

		SetRect(&rectFormat);
	}
}

void CCenterEdit::OnSize(UINT nType, int cx, int cy)
{
	// ★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★
	// 初回のリサイズ時のみ、自分自身にメッセージをポストする
	// ★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★
	if (m_bIsFirstTimeInit && cx > 0 && cy > 0)
	{
		m_bIsFirstTimeInit = false;
		// これで、すべての初期化が終わった後にOnPostInitが呼ばれる
		PostMessage(WM_APP_POST_INIT, 0, 0);
	}

	CEdit::OnSize(nType, cx, cy);
	UpdateTextPosition();
	Invalidate();
}


LRESULT CCenterEdit::OnPostInit(WPARAM wParam, LPARAM lParam)
{
	// ★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★
	// すべての初期化が終わった、完璧なタイミングで呼ばれるハンドラ
	// ★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★
	UpdateTextPosition();
	Invalidate();
	return 0;
}
HBRUSH CCenterEdit::CtlColor(CDC* pDC, UINT nCtlColor)
{
	// このハンドラは、DefWindowProcが背景を描画する際に使用するブラシを返す
	// 役割だけに限定する。文字色の設定はOnPaintで行う。
	CWnd* pFocusWnd = GetFocus();
	bool bHasFocus = (pFocusWnd && pFocusWnd->GetSafeHwnd() == GetSafeHwnd());

	if (bHasFocus)
	{
		LOGBRUSH LogBrush;
		m_brushFocus.GetLogBrush(&LogBrush);
		pDC->SetBkColor(LogBrush.lbColor);
		pDC->SetTextColor(m_colorTextFocus);
		return (HBRUSH)m_brushFocus.GetSafeHandle();
	}
	else
	{
		LOGBRUSH LogBrush;
		m_brushNoFocus.GetLogBrush(&LogBrush);
		pDC->SetBkColor(LogBrush.lbColor);
		pDC->SetTextColor(m_colorTextNoFocus);
		return (HBRUSH)m_brushNoFocus.GetSafeHandle();
	}
}
