#pragma once
#include "afxdialogex.h"

#include "GridCtrl.h"

// 親ウィンドウへ機能実行を通知するメッセージ
#define WM_EXECUTE_FUNCTION (WM_APP + 5)
// CMyDialog3 ダイアログ

class CMyDialog3 : public CDialogEx
{
    DECLARE_DYNAMIC(CMyDialog3)

public:
    CMyDialog3(CWnd *pParent = nullptr); // 標準コンストラクター
    virtual ~CMyDialog3();

    // ダイアログ データ
    enum
    {
        IDD = IDD_MyDialog3
    };

protected:
    virtual void DoDataExchange(CDataExchange *pDX); // DDX/DDV サポート
    virtual BOOL OnInitDialog();

    // コントロール
    CGridCtrl m_gridCtrl;
    CButton m_btnFunc[8];
    CButton m_btnNextPage;

    // 状態管理
    int m_nSelectedRow; // 現在選択されている行インデックス (0始まり, -1で非選択)
    int m_nCurrentPage; // 現在のページ番号 (1 or 2)

    // ヘルパー関数
    void UpdateButtons();

    // メッセージハンドラ
    afx_msg void OnGridSelChanged(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnNextPageClicked();
    afx_msg void OnFuncButtonClicked(UINT nID);
    virtual void OnOK();
    virtual BOOL PreTranslateMessage(MSG *pMsg);

    DECLARE_MESSAGE_MAP()
};
