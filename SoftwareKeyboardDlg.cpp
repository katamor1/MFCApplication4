/**
 * @file SoftwareKeyboardDlg.cpp
 * @brief ソフトウェアキーボードダイアログのクラス実装
 */
#include "pch.h"
#include "SoftwareKeyboardDlg.h"
#include "CenterEdit.h"
#include "Resource.h"
#include "afxdialogex.h"

// --- 定数定義 ---

// カラー定義
const COLORREF CLR_TITLE_BAR = RGB(0, 102, 204); ///< カスタムタイトルバーの背景色
const COLORREF CLR_CLOSE_X = RGB(255, 255, 255);   ///< 閉じるボタンの「×」マークの色

// サイズ定義
const int KEY_WIDTH = 34;         ///< 標準キーの幅
const int KEY_HEIGHT = 34;        ///< 標準キーの高さ
const int KEY_GAP = 1;            ///< キーとキーの間の隙間
const int TITLE_BAR_HEIGHT = 34;  ///< カスタムタイトルバーの高さ
const int CLOSE_BTN_SIZE = 30;    ///< 閉じるボタンのサイズ

// IMPLEMENT_DYNAMICマクロ
// CSoftwareKeyboardDlgクラスが動的生成可能であることをフレームワークに伝えます。
IMPLEMENT_DYNAMIC(CSoftwareKeyboardDlg, CDialogEx)

/**
 * @brief CSoftwareKeyboardDlgクラスのコンストラクタ
 * @param pTargetEdit キー入力の送信先となるエディットコントロールへのポインタ
 * @param pParent 親ウィンドウへのポインタ
 */
CSoftwareKeyboardDlg::CSoftwareKeyboardDlg(CEdit *pTargetEdit, CWnd *pParent /*=nullptr*/)
	: CDialogEx(IDD_SW_KEYBOARD, pParent),
	  m_pTargetEdit(pTargetEdit),
	  m_bShiftOn(false),
	  m_bCapsLockOn(false),
	  m_bCtrlOn(false),
	  m_bAltOn(false),
	  m_bFnOn(false),
	  m_bDragging(false)
{
	// 物理キーボードのCapsLockの初期状態を取得し、同期させます。
	m_bCapsLockOn = (GetKeyState(VK_CAPITAL) & 0x0001) != 0;
}

/**
 * @brief CSoftwareKeyboardDlgクラスのデストラクタ
 * @details 動的に生成したCKeyButtonオブジェクトを全て解放します。
 */
CSoftwareKeyboardDlg::~CSoftwareKeyboardDlg()
{
	for (auto &btn : m_KeyButtons)
	{
		delete btn;
	}
	m_KeyButtons.clear();
}

void CSoftwareKeyboardDlg::DoDataExchange(CDataExchange *pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

// BEGIN_MESSAGE_MAPブロック
// Windowsメッセージと、それを処理するクラスのメンバ関数（ハンドラ）を関連付けます。
BEGIN_MESSAGE_MAP(CSoftwareKeyboardDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	// ON_COMMAND_RANGEは、指定したIDの範囲のボタンクリックイベントを単一のハンドラに送ります。
	ON_COMMAND_RANGE(IDC_KEY_BASE, IDC_KEY_BASE + 100, &CSoftwareKeyboardDlg::OnKeyClick)
END_MESSAGE_MAP()

/**
 * @brief ダイアログの初期化処理 (WM_INITDIALOG)
 * @details キー定義に基づき、キーボタンを動的に生成・配置し、ウィンドウサイズを調整します。
 * @return フォーカスをコントロールに設定しない場合はTRUE。
 */
BOOL CSoftwareKeyboardDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// キーレイアウト定義から、最も幅が必要な行のスパン数を計算します。
	int max_span_units = 0;
	const int nKeyArrayRows = sizeof(g_KeyLayout) / sizeof(g_KeyLayout[0]);
	const int nKeyArrayCols = sizeof(g_KeyLayout[0]) / sizeof(g_KeyLayout[0][0]);

	for (int row = 0; row < nKeyArrayRows; ++row)
	{
		int current_span = 0;
		for (int col = 0; col < nKeyArrayCols; ++col)
		{
			if (g_KeyLayout[row][col].uId != 0)
			{
				current_span += g_KeyLayout[row][col].nColumnSpan;
			}
		}
		if (current_span > max_span_units)
		{
			max_span_units = current_span;
		}
	}

	// 計算した最大スパン数に基づき、ダイアログのウィンドウサイズを動的に設定します。
	const int nRows = 5;
	int dlgWidth = (KEY_WIDTH * max_span_units) + (KEY_GAP * (max_span_units - 1));
	int dlgHeight = TITLE_BAR_HEIGHT + (KEY_HEIGHT * nRows) + (KEY_GAP * (nRows));
	SetWindowPos(nullptr, 0, 0, dlgWidth, dlgHeight, SWP_NOMOVE | SWP_NOZORDER);

	// カスタム描画するタイトルバーと閉じるボタンの矩形領域を計算します。
	m_rcTitleBar.SetRect(0, 0, dlgWidth, TITLE_BAR_HEIGHT);
	m_rcCloseBtn.SetRect(dlgWidth - CLOSE_BTN_SIZE - 2, (TITLE_BAR_HEIGHT - CLOSE_BTN_SIZE) / 2, dlgWidth - 2, (TITLE_BAR_HEIGHT + CLOSE_BTN_SIZE) / 2);

	// g_KeyLayout配列を元に、全てのキーボタンを動的に生成します。
	int y = TITLE_BAR_HEIGHT + KEY_GAP;
	for (int row = 0; row < nKeyArrayRows; ++row)
	{
		int x = 0;
		for (int col = 0; col < nKeyArrayCols; ++col)
		{
			const KEY_INFO &keyInfo = g_KeyLayout[row][col];
			if (keyInfo.uId == 0)
				continue; // nColumnSpan > 1 のキーのためのプレースホルダはスキップ

			// キーの幅をスパン数に応じて計算
			int keyWidth = (keyInfo.nColumnSpan * KEY_WIDTH) + ((keyInfo.nColumnSpan - 1) * KEY_GAP);
			CRect keyRect(x, y, x + keyWidth, y + KEY_HEIGHT);

			// ボタンを生成し、プロパティを設定
			CKeyButton *pButton = new CKeyButton();
			pButton->Create(keyInfo.szLabel, WS_CHILD | WS_VISIBLE | BS_OWNERDRAW, keyRect, this, keyInfo.uId);
			pButton->SetFont(GetFont());
			pButton->SetKeyInfo(this, &keyInfo);

			m_KeyButtons.push_back(pButton);

			x += keyWidth + KEY_GAP;
		}
		y += KEY_HEIGHT + KEY_GAP;
	}

	// ターゲットのエディットコントロールの真下にキーボードを配置します。
	CRect rcEdit;
	m_pTargetEdit->GetWindowRect(&rcEdit);
	SetWindowPos(nullptr, rcEdit.left, rcEdit.bottom + 5, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

	return TRUE;
}

/**
 * @brief 描画処理 (WM_PAINT)
 * @details カスタムタイトルバーと閉じるボタンの「×」マークを描画します。
 */
void CSoftwareKeyboardDlg::OnPaint()
{
	CPaintDC dc(this);

	// 1. タイトルバーを指定色で塗りつぶします。
	dc.FillSolidRect(m_rcTitleBar, CLR_TITLE_BAR);

	// 2. 閉じるボタンの領域に「×」を手動で描画します。
	CPen pen(PS_SOLID, 2, CLR_CLOSE_X);
	CPen *pOldPen = dc.SelectObject(&pen);
	dc.MoveTo(m_rcCloseBtn.left + 8, m_rcCloseBtn.top + 8);
	dc.LineTo(m_rcCloseBtn.right - 8, m_rcCloseBtn.bottom - 8);
	dc.MoveTo(m_rcCloseBtn.left + 8, m_rcCloseBtn.bottom - 8);
	dc.LineTo(m_rcCloseBtn.right - 8, m_rcCloseBtn.top + 8);
	dc.SelectObject(pOldPen);
}

/**
 * @brief マウスの左ボタンが押されたときの処理 (WM_LBUTTONDOWN)
 * @details 閉じるボタンのクリック判定と、タイトルバーのドラッグ開始処理を行います。
 * @param nFlags 修飾キーの状態
 * @param point マウスカーソルのクライアント座標
 */
void CSoftwareKeyboardDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	// 閉じるボタンの領域がクリックされたか判定
	if (m_rcCloseBtn.PtInRect(point))
	{
		EndDialog(IDCANCEL);
		return;
	}

	// タイトルバー領域がクリックされたか判定
	if (m_rcTitleBar.PtInRect(point))
	{
		m_bDragging = true;
		m_ptMouseOffset = point;
		SetCapture(); // マウスキャプチャを開始し、ウィンドウ外でもマウスメッセージを受け取る
	}

	CDialogEx::OnLButtonDown(nFlags, point);
}

/**
 * @brief マウスの左ボタンが離されたときの処理 (WM_LBUTTONUP)
 * @details ドラッグ状態を終了します。
 */
void CSoftwareKeyboardDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (m_bDragging)
	{
		m_bDragging = false;
		ReleaseCapture(); // マウスキャプチャを解放
	}
	CDialogEx::OnLButtonUp(nFlags, point);
}

/**
 * @brief マウスが移動したときの処理 (WM_MOUSEMOVE)
 * @details ドラッグ中であれば、ウィンドウをマウスに追従させて移動させます。
 */
void CSoftwareKeyboardDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	if (m_bDragging)
	{
		CPoint ptScreen = point;
		ClientToScreen(&ptScreen); // 現在のクライアント座標をスクリーン座標に変換
		// ウィンドウの新しい左上位置を計算し、移動
		SetWindowPos(nullptr, ptScreen.x - m_ptMouseOffset.x, ptScreen.y - m_ptMouseOffset.y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
	}
	CDialogEx::OnMouseMove(nFlags, point);
}

/**
 * @brief いずれかのキーボタンがクリックされたときの処理
 * @details ON_COMMAND_RANGEによって、全てのキーボタンクリックがこのハンドラに集約されます。
 * @param nID クリックされたボタンのコントロールID
 */
void CSoftwareKeyboardDlg::OnKeyClick(UINT nID)
{
	const KEY_INFO *pKeyInfo = nullptr;
	// クリックされたボタンのIDを元に、g_KeyLayout配列から対応するキー情報を探します。
	for (int row = 0; row < 5; ++row)
	{
		for (int col = 0; col < 15; ++col)
		{
			if (g_KeyLayout[row][col].uId == nID)
			{
				pKeyInfo = &g_KeyLayout[row][col];
				goto key_found; // 見つかったらループを抜ける
			}
		}
	}
key_found:
	if (pKeyInfo)
	{
		// キー情報が見つかった場合、キープレス処理を呼び出す
		HandleKeyPress(pKeyInfo);
	}
}

/**
 * @brief キーの種類に応じて、実際のキー入力シミュレーションや状態変更を行います。
 * @param pKeyInfo 押されたキーの情報を持つKEY_INFO構造体へのポインタ
 */
void CSoftwareKeyboardDlg::HandleKeyPress(const KEY_INFO *pKeyInfo)
{
	switch (pKeyInfo->eKeyType)
	{
	case KT_NORMAL: // 通常の文字・数字・記号キー
	{
		TCHAR ch = 0;
		bool bIsAlphabet = (pKeyInfo->bVirtKey >= 'A' && pKeyInfo->bVirtKey <= 'Z');

		// KeyDefine.hの定義に基づき、ShiftやCapsLockの状態を考慮して入力文字を決定
		if (bIsAlphabet)
		{
			if (m_bShiftOn ^ m_bCapsLockOn) // XOR: ShiftとCapsLockの片方だけがONの場合
				ch = pKeyInfo->szShiftLabel[0]; // 大文字
			else
				ch = pKeyInfo->szLabel[0]; // 小文字
		}
		else // 記号・数字キー
		{
			if (m_bShiftOn && pKeyInfo->szShiftLabel && pKeyInfo->szShiftLabel[0] != L'\0')
				ch = pKeyInfo->szShiftLabel[0]; // Shift時の文字
			else
				ch = pKeyInfo->szLabel[0]; // 通常時の文字
		}

		if (ch != 0)
			SendChar(ch);

		// Shiftキーは、他の通常キーが押されたらOFFに戻る
		if (m_bShiftOn)
		{
			m_bShiftOn = false;
			UpdateAllKeys();
		}
		break;
	}
	case KT_ACTION: // Enter, Esc, Backspaceなどのアクションキー
		if (pKeyInfo->bVirtKey == VK_RETURN)
			EndDialog(IDOK);
		else if (pKeyInfo->bVirtKey == VK_ESCAPE)
			EndDialog(IDCANCEL);
		else
			SendKey(pKeyInfo->bVirtKey);
		break;

	case KT_MODIFIER: // Shift, Ctrl, Alt, CapsLockなどの状態保持キー
		// 各フラグをトグル（反転）させる
		switch (pKeyInfo->bVirtKey)
		{
		case VK_CAPITAL:  m_bCapsLockOn = !m_bCapsLockOn; break;
		case VK_LSHIFT:
		case VK_RSHIFT:   m_bShiftOn = !m_bShiftOn; break;
		case VK_LCONTROL:
		case VK_RCONTROL: m_bCtrlOn = !m_bCtrlOn; break;
		case VK_LMENU:
		case VK_RMENU:    m_bAltOn = !m_bAltOn; break;
		default:
			if (wcscmp(pKeyInfo->szLabel, _T("Fn")) == 0)
				m_bFnOn = !m_bFnOn;
			break;
		}
		UpdateAllKeys(); // 状態が変わったので、全キーの表示を更新
		break;
	}
	m_pTargetEdit->SetFocus(); // 操作後、フォーカスをターゲットのエディットコントロールに戻す
}

/**
 * @brief 文字キー入力をターゲットのエディットコントロールに送信します。
 * @param ch 送信する文字
 */
void CSoftwareKeyboardDlg::SendChar(TCHAR ch)
{
	m_pTargetEdit->SendMessage(WM_CHAR, (WPARAM)ch, 1);
}

/**
 * @brief 特殊キー（非文字キー）入力をターゲットのエディットコントロールに送信します。
 * @param vk 送信するキーの仮想キーコード
 */
void CSoftwareKeyboardDlg::SendKey(BYTE vk)
{
	// Backspace, Tab, Spaceは制御文字としてWM_CHARで送信するのが確実
	if (vk == VK_BACK || vk == VK_TAB || vk == VK_SPACE)
	{
		m_pTargetEdit->SendMessage(WM_CHAR, (WPARAM)vk, 1);
	}
	else // DeleteなどはアクションとしてWM_KEYDOWNで送信
	{
		m_pTargetEdit->SendMessage(WM_KEYDOWN, (WPARAM)vk, 0);
	}
}

/**
 * @brief 全てのキーボタンに再描画を要求します。
 * @details Shiftキーなどが押されて、全キーの表示を更新する必要がある場合に呼び出されます。
 */
void CSoftwareKeyboardDlg::UpdateAllKeys()
{
	for (auto &btn : m_KeyButtons)
	{
		btn->Invalidate();
	}
}

/**
 * @brief Enterキーによるダイアログ終了を無効化します。
 */
void CSoftwareKeyboardDlg::OnOK()
{
	// 基底クラスのOnOK()を呼び出さないことで、何もしないようにする。
}

/**
 * @brief Escキーによるダイアログ終了を無効化します。
 */
void CSoftwareKeyboardDlg::OnCancel()
{
	// 基底クラスのOnCancel()を呼び出さないことで、何もしないようにする。
}