// KeyButton.h
#pragma once

#include "KeyDefine.h"

// 前方宣言
class CSoftwareKeyboardDlg;

class CKeyButton : public CButton
{
    DECLARE_DYNAMIC(CKeyButton)

public:
    CKeyButton();
    virtual ~CKeyButton();

    void SetKeyInfo(CSoftwareKeyboardDlg* pParent, const KEY_INFO* pKeyInfo);

protected:
    const KEY_INFO* m_pKeyInfo;
    CSoftwareKeyboardDlg* m_pParentDlg; // 親ダイアログのポインタ

    virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);

    DECLARE_MESSAGE_MAP()
};