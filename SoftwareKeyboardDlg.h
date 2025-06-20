// SoftwareKeyboardDlg.h
#pragma once

#include "KeyDefine.h"
#include "KeyButton.h"
#include <vector>
#include "Resource.h"

class CEdit; // 前方宣言

class CSoftwareKeyboardDlg : public CDialogEx
{
    DECLARE_DYNAMIC(CSoftwareKeyboardDlg)

public:
    // コンストラクタで入力対象のエディットコントロールを受け取る
    CSoftwareKeyboardDlg(CEdit* pTargetEdit, CWnd* pParent = nullptr);
    virtual ~CSoftwareKeyboardDlg();

    // ダイアログ データ
    enum { IDD = IDD_SW_KEYBOARD };

    // 状態取得用public関数
    bool IsShiftOn() const { return m_bShiftOn; }
    bool IsCapsLockOn() const { return m_bCapsLockOn; }

    // --- 修正箇所：ここから ---
    // bool IsCtrlOn(BYTE vk) const { return (vk == VK_LCONTROL && m_bLCtrlOn) || (vk == VK_RCONTROL && m_bRCtrlOn); }
    // bool IsAltOn(BYTE vk) const { return (vk == VK_LMENU && m_bLAltOn) || (vk == VK_RMENU && m_bRAltOn); }
    bool IsCtrlOn() const { return m_bCtrlOn; } // 引数をなくし、単一のフラグを返す
    bool IsAltOn() const { return m_bAltOn; }   // 引数をなくし、単一のフラグを返す
    // --- 修正箇所：ここまで ---
    bool IsFnOn() const { return m_bFnOn; }

protected:
    virtual void DoDataExchange(CDataExchange* pDX);
    virtual BOOL OnInitDialog();
    virtual void OnOK();
    virtual void OnCancel();

    // メンバ変数
    CEdit* m_pTargetEdit; // 入力対象のエディット
    std::vector<CKeyButton*> m_KeyButtons;

    // --- 修正箇所：ここから ---
    // 状態保持キーのフラグ
    bool m_bShiftOn;
    bool m_bCapsLockOn;
    // bool m_bLCtrlOn, m_bRCtrlOn;
    // bool m_bLAltOn, m_bRAltOn;
    bool m_bCtrlOn; // 左右の区別をなくす
    bool m_bAltOn;  // 左右の区別をなくす
    bool m_bFnOn;
    // --- 修正箇所：ここまで ---
    
    // ダイアログ移動用
    bool m_bDragging;
    CPoint m_ptMouseOffset;

    // カスタム描画領域
    CRect m_rcTitleBar;
    CRect m_rcCloseBtn;

    // ヘルパー関数
    void HandleKeyPress(const KEY_INFO* pKeyInfo);
    void SendChar(TCHAR ch);
    void SendKey(BYTE vk);
    void UpdateAllKeys();

    DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnPaint();
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
    afx_msg void OnMouseMove(UINT nFlags, CPoint point);
    afx_msg void OnKeyClick(UINT nID);
};