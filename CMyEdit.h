// MyEdit.h
#pragma once

class CMyEdit : public CEdit
{
    DECLARE_DYNAMIC(CMyEdit)

public:
    CMyEdit();
    virtual ~CMyEdit();

protected:
    CString m_strOriginalText;

    afx_msg void OnSetFocus(CWnd *pOldWnd);
    DECLARE_MESSAGE_MAP()
};