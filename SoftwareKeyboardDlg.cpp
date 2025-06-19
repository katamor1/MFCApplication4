// SoftwareKeyboardDlg.cpp
#include "pch.h" // or stdafx.h
#include "SoftwareKeyboardDlg.h"
#include "CenterEdit.h"
#include "Resource.h"
#include "afxdialogex.h"

// カラー定義
const COLORREF CLR_TITLE_BAR = RGB(0, 102, 204);
const COLORREF CLR_CLOSE_X = RGB(255, 255, 255);

// サイズ定義
const int KEY_WIDTH = 34;
const int KEY_HEIGHT = 34;
const int KEY_GAP = 1;
const int TITLE_BAR_HEIGHT = 34;
const int CLOSE_BTN_SIZE = 30;

IMPLEMENT_DYNAMIC(CSoftwareKeyboardDlg, CDialogEx)

CSoftwareKeyboardDlg::CSoftwareKeyboardDlg(CEdit* pTargetEdit, CWnd* pParent /*=nullptr*/)
    : CDialogEx(IDD_SW_KEYBOARD, pParent),
      m_pTargetEdit(pTargetEdit),
      m_bShiftOn(false),
      m_bCapsLockOn(false),
            // --- 修正箇所：ここから ---
      // m_bLCtrlOn(false), m_bRCtrlOn(false),
      // m_bLAltOn(false), m_bRAltOn(false),
      m_bCtrlOn(false),
      m_bAltOn(false),
      // --- 修正箇所：ここまで ---
      m_bFnOn(false),
      m_bDragging(false)
{
    // CapsLockの初期状態をシステムから取得
    m_bCapsLockOn = (GetKeyState(VK_CAPITAL) & 0x0001) != 0;
}

CSoftwareKeyboardDlg::~CSoftwareKeyboardDlg()
{
    for (auto& btn : m_KeyButtons)
    {
        delete btn;
    }
    m_KeyButtons.clear();
}

void CSoftwareKeyboardDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CSoftwareKeyboardDlg, CDialogEx)
    ON_WM_PAINT()
    ON_WM_LBUTTONDOWN()
    ON_WM_LBUTTONUP()
    ON_WM_MOUSEMOVE()
    // IDC_KEY_BASEから65個分のID範囲のコマンドメッセージをすべてOnKeyClickにマップする
    ON_COMMAND_RANGE(IDC_KEY_BASE, IDC_KEY_BASE + 100, &CSoftwareKeyboardDlg::OnKeyClick)
END_MESSAGE_MAP()

BOOL CSoftwareKeyboardDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    // ★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★
    // ★                                                   ★
    // ★             ↓↓↓ ここから修正 ↓↓↓             ★
    // ★                                                   ★
    // ★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★

    // --- 修正箇所：ここから ---
    // 各行の合計スパン（キー幅の合計）を計算し、最も幅が広い行に合わせて
    // ダイアログ全体の幅を決定します。
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
    
    // 動的に計算した幅でダイアログサイズを設定
    const int nRows = 5;
    int dlgWidth = (KEY_WIDTH * max_span_units) + (KEY_GAP * (max_span_units - 1));
    int dlgHeight = TITLE_BAR_HEIGHT + (KEY_HEIGHT * nRows) + (KEY_GAP * (nRows));
    SetWindowPos(nullptr, 0, 0, dlgWidth, dlgHeight, SWP_NOMOVE | SWP_NOZORDER);
    // --- 修正箇所：ここまで ---


    // タイトルバーと閉じるボタンの領域を設定
    m_rcTitleBar.SetRect(0, 0, dlgWidth, TITLE_BAR_HEIGHT);
    m_rcCloseBtn.SetRect(dlgWidth - CLOSE_BTN_SIZE - 2, (TITLE_BAR_HEIGHT - CLOSE_BTN_SIZE) / 2, dlgWidth - 2, (TITLE_BAR_HEIGHT + CLOSE_BTN_SIZE) / 2);

    // キーボードのボタンを動的に生成
    int y = TITLE_BAR_HEIGHT + KEY_GAP;
    for (int row = 0; row < nKeyArrayRows; ++row)
    {
        int x = 0;
        for (int col = 0; col < nKeyArrayCols; ++col)
        {
            const KEY_INFO& keyInfo = g_KeyLayout[row][col];
            if (keyInfo.uId == 0) continue; // スパンのプレースホルダはスキップ

            int keyWidth = (keyInfo.nColumnSpan * KEY_WIDTH) + ((keyInfo.nColumnSpan - 1) * KEY_GAP);
            CRect keyRect(x, y, x + keyWidth, y + KEY_HEIGHT);

            CKeyButton* pButton = new CKeyButton();
            pButton->Create(keyInfo.szLabel, WS_CHILD | WS_VISIBLE | BS_OWNERDRAW, keyRect, this, keyInfo.uId);
            pButton->SetFont(GetFont());
            pButton->SetKeyInfo(this, &keyInfo);

            m_KeyButtons.push_back(pButton);

            x += keyWidth + KEY_GAP;
        }
        y += KEY_HEIGHT + KEY_GAP;
    }

    // エディットコントロールの近くに表示
    CRect rcEdit;
    m_pTargetEdit->GetWindowRect(&rcEdit);
    SetWindowPos(nullptr, rcEdit.left, rcEdit.bottom + 5, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

    return TRUE;
}

void CSoftwareKeyboardDlg::OnPaint()
{
    CPaintDC dc(this);

    // 1. タイトルバーを描画
    dc.FillSolidRect(m_rcTitleBar, CLR_TITLE_BAR);

    // 2. 閉じるボタンを描画
    CPen pen(PS_SOLID, 2, CLR_CLOSE_X);
    CPen* pOldPen = dc.SelectObject(&pen);
    dc.MoveTo(m_rcCloseBtn.left + 8, m_rcCloseBtn.top + 8);
    dc.LineTo(m_rcCloseBtn.right - 8, m_rcCloseBtn.bottom - 8);
    dc.MoveTo(m_rcCloseBtn.left + 8, m_rcCloseBtn.bottom - 8);
    dc.LineTo(m_rcCloseBtn.right - 8, m_rcCloseBtn.top + 8);
    dc.SelectObject(pOldPen);
}

void CSoftwareKeyboardDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
    // 閉じるボタンがクリックされたかチェック
    if (m_rcCloseBtn.PtInRect(point))
    {
        OnCancel(); // キャンセル終了
        return;
    }

    // タイトルバーがクリックされたかチェック
    if (m_rcTitleBar.PtInRect(point))
    {
        m_bDragging = true;
        // 修正後：クリックされたクライアント座標をそのままオフセットとして保存する
        m_ptMouseOffset = point;
        SetCapture();
    }

    CDialogEx::OnLButtonDown(nFlags, point);
}

void CSoftwareKeyboardDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
    if (m_bDragging)
    {
        m_bDragging = false;
        ReleaseCapture();
    }
    CDialogEx::OnLButtonUp(nFlags, point);
}

void CSoftwareKeyboardDlg::OnMouseMove(UINT nFlags, CPoint point)
{
    if (m_bDragging)
    {
        CPoint ptScreen = point;
        ClientToScreen(&ptScreen);
        SetWindowPos(nullptr, ptScreen.x - m_ptMouseOffset.x, ptScreen.y - m_ptMouseOffset.y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
    }
    CDialogEx::OnMouseMove(nFlags, point);
}

void CSoftwareKeyboardDlg::OnKeyClick(UINT nID)
{
    const KEY_INFO* pKeyInfo = nullptr;
    // クリックされたボタンの情報を探す
    for (int row = 0; row < 5; ++row)
    {
        for (int col = 0; col < 15; ++col)
        {
            if (g_KeyLayout[row][col].uId == nID)
            {
                pKeyInfo = &g_KeyLayout[row][col];
                goto key_found;
            }
        }
    }
key_found:
    if (pKeyInfo)
    {
        HandleKeyPress(pKeyInfo);
    }
}

void CSoftwareKeyboardDlg::HandleKeyPress(const KEY_INFO* pKeyInfo)
{
    switch (pKeyInfo->eKeyType)
    {
    case KT_NORMAL:
    {
        // --- 修正箇所：ここから ---
        // OSのレイアウト設定に依存せず、KeyDefine.hの定義に基づいて
        // 入力文字を決定するロジックに戻します。
        
        TCHAR ch = 0;
        bool bIsAlphabet = (pKeyInfo->bVirtKey >= 'A' && pKeyInfo->bVirtKey <= 'Z');

        if (bIsAlphabet)
        {
            // 英字キーの場合：ShiftとCaps Lockの両方を考慮
            if (m_bShiftOn ^ m_bCapsLockOn) 
            {
                // 大文字（szShiftLabelから取得）
                ch = pKeyInfo->szShiftLabel[0];
            }
            else
            {
                // 小文字（szLabelから取得）
                ch = pKeyInfo->szLabel[0];
            }
        }
        else
        {
            // 記号・数字キーの場合：Shiftの状態のみ考慮
            if (m_bShiftOn && pKeyInfo->szShiftLabel && pKeyInfo->szShiftLabel[0] != L'\0')
            {
                // ShiftがONの時の文字（szShiftLabelから取得）
                ch = pKeyInfo->szShiftLabel[0];
            }
            else
            {
                // 通常時の文字（szLabelから取得）
                ch = pKeyInfo->szLabel[0];
            }
        }

        // 決定した文字があれば送信
        if (ch != 0)
        {
            SendChar(ch);
        }
        // --- 修正箇所：ここまで ---


        // 通常キーを押したらShiftの状態は解除する
        if (m_bShiftOn)
        {
            m_bShiftOn = false;
            UpdateAllKeys();
        }
        break;
    }
    case KT_ACTION:
        if (pKeyInfo->bVirtKey == VK_RETURN) OnOK();
        else if (pKeyInfo->bVirtKey == VK_ESCAPE) OnCancel();
        else SendKey(pKeyInfo->bVirtKey); // Backspace, Delete, Tab, Spaceなど
        break;

    case KT_MODIFIER:
        switch (pKeyInfo->bVirtKey)
        {
        case VK_CAPITAL: m_bCapsLockOn = !m_bCapsLockOn; break;
        case VK_LSHIFT:
        case VK_RSHIFT:  m_bShiftOn = !m_bShiftOn; break;

        // --- 修正箇所：ここから ---
        // case VK_LCONTROL:m_bLCtrlOn = !m_bLCtrlOn; m_bRCtrlOn = false; break;
        // case VK_RCONTROL:m_bRCtrlOn = !m_bRCtrlOn; m_bLCtrlOn = false; break;
        // case VK_LMENU:   m_bLAltOn = !m_bLAltOn; m_bRAltOn = false; break;
        // case VK_RMENU:   m_bRAltOn = !m_bRAltOn; m_bLAltOn = false; break;
        
        // 左右どちらのCtrlでも、単一のm_bCtrlOnをトグルする
        case VK_LCONTROL:
        case VK_RCONTROL: 
            m_bCtrlOn = !m_bCtrlOn; 
            break;
        
        // 左右どちらのAltでも、単一のm_bAltOnをトグルする
        case VK_LMENU:
        case VK_RMENU:    
            m_bAltOn = !m_bAltOn; 
            break;
        // --- 修正箇所：ここまで ---
        default:         if (wcscmp(pKeyInfo->szLabel, L"Fn") == 0) m_bFnOn = !m_bFnOn; break;
        }
        UpdateAllKeys();
        break;
    }
    m_pTargetEdit->SetFocus(); // 操作後、フォーカスを戻す
}

// 文字を送信
void CSoftwareKeyboardDlg::SendChar(TCHAR ch)
{
    // Shift, Ctrl, Altの状態をチェックして、PostMessageでキー入力をシミュレート
    // SendInputの方がより忠実だが、フォーカスの問題があるため、
    // ここでは対象コントロールへ直接メッセージを送る
    m_pTargetEdit->SendMessage(WM_CHAR, (WPARAM)ch, 1);
}
// 特殊キーを送信
void CSoftwareKeyboardDlg::SendKey(BYTE vk)
{
    // ★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★
    // ★                                                   ★
    // ★             ↓↓↓ ここから修正 ↓↓↓             ★
    // ★                                                   ★
    // ★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★

    // Backspace (VK_BACK) は、文字入力の制御文字としてWM_CHARで送信すると、
    // CEditコントロールでより確実に処理されます。
    // 同様に、TabやSpaceもWM_CHARで送信するのが適切です。
    // 一方、Delete (VK_DELETE) のような非文字キーはWM_KEYDOWNで処理されます。
    if (vk == VK_BACK || vk == VK_TAB || vk == VK_SPACE)
    {
        m_pTargetEdit->SendMessage(WM_CHAR, (WPARAM)vk, 1);
    }
    else // VK_DELETE など、その他のアクションキー
    {
        m_pTargetEdit->SendMessage(WM_KEYDOWN, (WPARAM)vk, 0);
    }
}

// 全キーの再描画を要求
void CSoftwareKeyboardDlg::UpdateAllKeys()
{
    for (auto& btn : m_KeyButtons)
    {
        btn->Invalidate();
    }
}

void CSoftwareKeyboardDlg::OnOK()
{
    // Enterキーが押された: IDOKでダイアログを閉じる
    EndDialog(IDOK);
}

void CSoftwareKeyboardDlg::OnCancel()
{
    // Escキーまたは閉じるボタンが押された: IDCANCELでダイアログを閉じる
    EndDialog(IDCANCEL);
}