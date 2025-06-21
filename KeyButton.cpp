/**
 * @file KeyButton.cpp
 * @brief ソフトウェアキーボードの個別キーを描画するカスタムボタンクラスの実装
 * @author C++/MFCコーディング (AI)
 * @date 2025/06/22
 */
#include "pch.h"
#include "KeyButton.h"
#include "SoftwareKeyboardDlg.h"

// --- 定数定義 ---

// カラー定義
const COLORREF CLR_KEY_BLUE = RGB(0, 102, 204);   ///< 修飾キーがONの時の背景色
const COLORREF CLR_KEY_GRAY = RGB(240, 240, 240);   ///< 通常時のキーの背景色
const COLORREF CLR_KEY_BORDER = RGB(173, 173, 173); ///< キーの枠線の色
const COLORREF CLR_TEXT_BLACK = RGB(0, 0, 0);       ///< 通常時の文字色
const COLORREF CLR_TEXT_WHITE = RGB(255, 255, 255); ///< 修飾キーがONの時の文字色
const COLORREF CLR_TEXT_GRAY = RGB(128, 128, 128);  ///< Shift時ラベルの文字色

// CKeyButtonクラスが動的生成可能であることをフレームワークに伝えます。
IMPLEMENT_DYNAMIC(CKeyButton, CButton)

/**
 * @brief CKeyButtonクラスのコンストラクタ
 */
CKeyButton::CKeyButton() : m_pKeyInfo(nullptr), m_pParentDlg(nullptr)
{
}

/**
 * @brief CKeyButtonクラスのデストラクタ
 */
CKeyButton::~CKeyButton()
{
}

/**
 * @brief キーボタンにキー情報と親ダイアログのポインタを設定します。
 * @param[in] pParent このボタンを所有する親ダイアログ(CSoftwareKeyboardDlg)へのポインタ
 * @param[in] pKeyInfo このボタンに対応するキー情報(KEY_INFO)へのポインタ
 */
void CKeyButton::SetKeyInfo(CSoftwareKeyboardDlg *pParent, const KEY_INFO *pKeyInfo)
{
    m_pParentDlg = pParent;
    m_pKeyInfo = pKeyInfo;
}

/**
 * @brief ボタンの描画処理を実装します (WM_DRAWITEM)。
 * @details BS_OWNERDRAWスタイルを持つボタンが再描画される必要があるときに
 * フレームワークから呼び出されます。親ダイアログの状態(ShiftキーON/OFFなど)を
 * 参照し、キーの背景、枠線、ラベルを適切に描画します。
 * @param[in] lpDrawItemStruct 描画に必要な情報を含む構造体へのポインタ
 */
void CKeyButton::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
    // 必要なポインタが設定されていることを確認
    ASSERT(m_pKeyInfo != nullptr);
    ASSERT(m_pParentDlg != nullptr);
    if (m_pKeyInfo == nullptr || m_pParentDlg == nullptr)
        return;

    // デバイスコンテキストと描画領域矩形を取得
    CDC *pDC = CDC::FromHandle(lpDrawItemStruct->hDC);
    CRect rect = lpDrawItemStruct->rcItem;

    // 親ダイアログから現在の修飾キーの状態を取得
    bool bShiftOn = m_pParentDlg->IsShiftOn();
    bool bCapsLockOn = m_pParentDlg->IsCapsLockOn();
    bool bCtrlOn = m_pParentDlg->IsCtrlOn();
    bool bAltOn = m_pParentDlg->IsAltOn();
    bool bFnOn = m_pParentDlg->IsFnOn();

    // このキー自身が現在ON状態（アクティブ状態）であるかを判定
    bool bIsOn = false;
    if (m_pKeyInfo->eKeyType == KT_MODIFIER)
    {
        switch (m_pKeyInfo->bVirtKey)
        {
        case VK_CAPITAL:  bIsOn = bCapsLockOn; break;
        case VK_LSHIFT:
        case VK_RSHIFT:   bIsOn = bShiftOn; break;
        case VK_LCONTROL:
        case VK_RCONTROL: bIsOn = bCtrlOn; break;
        case VK_LMENU:
        case VK_RMENU:    bIsOn = bAltOn; break;
        default:
            // Fnキーは仮想キーコードを持たないため、ラベルで判定
            if (wcscmp(m_pKeyInfo->szLabel, L"Fn") == 0)
            {
                bIsOn = bFnOn;
            }
            break;
        }
    }

    // --- 1. 背景の描画 ---
    // ON状態なら青、通常ならグレーで塗りつぶす
    CBrush brushBack;
    if (bIsOn)
    {
        brushBack.CreateSolidBrush(CLR_KEY_BLUE);
    }
    else
    {
        brushBack.CreateSolidBrush(CLR_KEY_GRAY);
    }
    pDC->FillRect(rect, &brushBack);

    // --- 2. 枠線の描画 ---
    CPen penBorder(PS_SOLID, 1, CLR_KEY_BORDER);
    CPen *pOldPen = pDC->SelectObject(&penBorder);
    pDC->MoveTo(rect.left, rect.bottom - 1);
    pDC->LineTo(rect.left, rect.top);
    pDC->LineTo(rect.right - 1, rect.top);
    pDC->LineTo(rect.right - 1, rect.bottom - 1);
    pDC->LineTo(rect.left, rect.bottom - 1);
    pDC->SelectObject(pOldPen);

    // --- 3. ラベルの描画 ---
    pDC->SetBkMode(TRANSPARENT);
    // ON状態なら白、通常なら黒の文字色を設定
    COLORREF clrText = bIsOn ? CLR_TEXT_WHITE : CLR_TEXT_BLACK;
    pDC->SetTextColor(clrText);

    CFont *pOldFont = pDC->SelectObject(GetParent()->GetFont());

    // 表示するラベルの文字列を決定
    CString strLabel = m_pKeyInfo->szLabel;
    CString strShiftLabel = m_pKeyInfo->szShiftLabel;
    bool bIsAlphabet = (m_pKeyInfo->bVirtKey >= 'A' && m_pKeyInfo->bVirtKey <= 'Z');

    // ShiftまたはCaps Lockの状態に応じて、英字キーの表示を大文字/小文字に切り替え
    if (bIsAlphabet && (bShiftOn ^ bCapsLockOn)) // XOR: ShiftとCapsLockの片方だけがONの場合
    {
        strLabel = m_pKeyInfo->szShiftLabel; // 大文字
    }

    // ShiftキーがONの場合、記号・数字キーはShift時のラベルを使用
    if (!bIsAlphabet && bShiftOn && !strShiftLabel.IsEmpty())
    {
        strLabel = strShiftLabel;
        strShiftLabel.Empty(); // Shift時ラベルはメインとして描画したので、右下の表示はしない
    }

    // メインラベルを描画
    UINT uFormat = DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX;
    CRect textRect = rect;
    // Shift時ラベルがある場合（記号キーなど）は、メインラベルを左上に描画
    if (!strShiftLabel.IsEmpty() && !bIsAlphabet)
    {
        uFormat = DT_LEFT | DT_TOP | DT_SINGLELINE | DT_NOPREFIX;
        textRect.DeflateRect(4, 4);
    }
    pDC->DrawText(strLabel, textRect, uFormat);

    // Shift時ラベルを描画（右下）
    if (!strShiftLabel.IsEmpty() && !bIsAlphabet)
    {
        pDC->SetTextColor(CLR_TEXT_GRAY);
        pDC->DrawText(strShiftLabel, textRect, DT_RIGHT | DT_BOTTOM | DT_SINGLELINE | DT_NOPREFIX);
    }

    pDC->SelectObject(pOldFont);
}

// BEGIN_MESSAGE_MAPブロック
// CKeyButtonクラス用のメッセージマップ。現在、特定のメッセージ処理は追加されていません。
BEGIN_MESSAGE_MAP(CKeyButton, CButton)
END_MESSAGE_MAP()