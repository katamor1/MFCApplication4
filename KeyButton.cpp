// KeyButton.cpp
#include "pch.h" // or stdafx.h
#include "KeyButton.h"
#include "SoftwareKeyboardDlg.h"

// カラー定義
const COLORREF CLR_KEY_BLUE = RGB(0, 102, 204);
const COLORREF CLR_KEY_GRAY = RGB(240, 240, 240);
const COLORREF CLR_KEY_BORDER = RGB(173, 173, 173);
const COLORREF CLR_TEXT_BLACK = RGB(0, 0, 0);
const COLORREF CLR_TEXT_WHITE = RGB(255, 255, 255);
const COLORREF CLR_TEXT_GRAY = RGB(128, 128, 128);

IMPLEMENT_DYNAMIC(CKeyButton, CButton)

CKeyButton::CKeyButton() : m_pKeyInfo(nullptr), m_pParentDlg(nullptr)
{
}

CKeyButton::~CKeyButton()
{
}

void CKeyButton::SetKeyInfo(CSoftwareKeyboardDlg *pParent, const KEY_INFO *pKeyInfo)
{
    m_pParentDlg = pParent;
    m_pKeyInfo = pKeyInfo;
}
void CKeyButton::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
    ASSERT(m_pKeyInfo != nullptr);
    ASSERT(m_pParentDlg != nullptr);
    if (m_pKeyInfo == nullptr || m_pParentDlg == nullptr)
        return;

    CDC *pDC = CDC::FromHandle(lpDrawItemStruct->hDC);
    CRect rect = lpDrawItemStruct->rcItem;

    // 親ダイアログから現在の状態を取得
    bool bShiftOn = m_pParentDlg->IsShiftOn();
    bool bCapsLockOn = m_pParentDlg->IsCapsLockOn();
    bool bCtrlOn = m_pParentDlg->IsCtrlOn(); // 引数を削除
    bool bAltOn = m_pParentDlg->IsAltOn();   // 引数を削除
    bool bFnOn = m_pParentDlg->IsFnOn();

    bool bIsOn = false; // このキーがON状態か
    if (m_pKeyInfo->eKeyType == KT_MODIFIER)
    {
        switch (m_pKeyInfo->bVirtKey)
        {
        case VK_CAPITAL:
            bIsOn = bCapsLockOn;
            break;
        case VK_LSHIFT:
        case VK_RSHIFT:
            bIsOn = bShiftOn;
            break;
        case VK_LCONTROL:
        case VK_RCONTROL:
            bIsOn = bCtrlOn;
            break;
        case VK_LMENU:
        case VK_RMENU:
            bIsOn = bAltOn;
            break;
        default:
            bIsOn = (m_pKeyInfo->szLabel == CString(_T("Fn"))) ? bFnOn : false;
            break;
        }
    }

    // 1. 背景の描画
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

    // 2. 枠線の描画
    CPen penBorder(PS_SOLID, 1, CLR_KEY_BORDER);
    CPen *pOldPen = pDC->SelectObject(&penBorder);
    pDC->MoveTo(rect.left, rect.bottom - 1);
    pDC->LineTo(rect.left, rect.top);
    pDC->LineTo(rect.right - 1, rect.top);
    pDC->LineTo(rect.right - 1, rect.bottom - 1);
    pDC->LineTo(rect.left, rect.bottom - 1);
    pDC->SelectObject(pOldPen);

    // 3. ラベルの描画
    pDC->SetBkMode(TRANSPARENT);
    COLORREF clrText = bIsOn ? CLR_TEXT_WHITE : CLR_TEXT_BLACK;
    pDC->SetTextColor(clrText);

    CFont *pOldFont = pDC->SelectObject(GetParent()->GetFont());

    // ラベルの文字列を決定
    CString strLabel = m_pKeyInfo->szLabel;
    CString strShiftLabel = m_pKeyInfo->szShiftLabel;
    bool bIsChar = (m_pKeyInfo->bVirtKey >= 'A' && m_pKeyInfo->bVirtKey <= 'Z');

    // ShiftまたはCaps LockがONの場合、英字キーは大文字に
    if (bIsChar && (bShiftOn ^ bCapsLockOn))
    {
        strLabel = m_pKeyInfo->szShiftLabel;
    }

    // ShiftキーがONの場合、記号キーはShift時のラベルを使用
    if (!bIsChar && bShiftOn && !strShiftLabel.IsEmpty())
    {
        strLabel = strShiftLabel;
        strShiftLabel.Empty(); // 右下の文字は描画しない
    }

    // メインラベルを描画
    UINT uFormat = DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX; // DT_NOPREFIX を追加
    if (!strShiftLabel.IsEmpty() && !bIsChar)
    {
        // Shift時ラベルがある場合は左上に描画
        uFormat = DT_LEFT | DT_TOP | DT_SINGLELINE | DT_NOPREFIX; // DT_NOPREFIX を追加
        rect.DeflateRect(4, 4);
    }
    pDC->DrawText(strLabel, rect, uFormat);

    // Shift時ラベルを描画（右下）
    if (!strShiftLabel.IsEmpty() && !bIsChar)
    {
        pDC->SetTextColor(CLR_TEXT_GRAY);
        pDC->DrawText(strShiftLabel, rect, DT_RIGHT | DT_BOTTOM | DT_SINGLELINE | DT_NOPREFIX); // DT_NOPREFIX を追加
    }

    pDC->SelectObject(pOldFont);
}

BEGIN_MESSAGE_MAP(CKeyButton, CButton)
END_MESSAGE_MAP()