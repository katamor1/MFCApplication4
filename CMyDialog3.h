/**
 * @file CMyDialog3.h
 * @brief CGridCtrlと機能ボタンを連携させるダイアログのクラス宣言
 * @author C++/MFCコーディング (AI)
 * @date 2025/06/22
 */
#pragma once
#include "afxdialogex.h"
#include "GridCtrl.h"

/// 親ウィンドウへ機能実行を通知するカスタムメッセージID
#define WM_EXECUTE_FUNCTION (WM_APP + 5)

/**
 * @class CMyDialog3
 * @brief グリッドの行選択に応じて動的に機能ボタンが変化するダイアログ
 * @details CGridCtrlの行選択とデータ内容に基づき、8つの機能ボタンと
 * ページ切り替えボタンの状態を動的に更新します。ボタンが押されると、
 * 対応する機能を親ウィンドウに通知します。
 */
class CMyDialog3 : public CDialogEx
{
    DECLARE_DYNAMIC(CMyDialog3)

public:
    /**
     * @brief 標準コンストラクタ
     * @param[in] pParent 親ウィンドウへのポインタ
     */
    explicit CMyDialog3(CWnd* pParent = nullptr);

    /**
     * @brief デストラクタ
     */
    virtual ~CMyDialog3() override;

    /// @brief ダイアログデータのリソースID
    enum { IDD = IDD_MyDialog3 };

protected:
    /**
     * @brief DDX/DDV（ダイアログデータエクスチェンジ/バリデーション）のサポート
     * @param pDX データエクスチェンジオブジェクトへのポインタ
     */
    virtual void DoDataExchange(CDataExchange* pDX) override;

    /**
     * @brief ダイアログの初期化処理(WM_INITDIALOG)をオーバーライドします。
     * @details グリッドやボタンコントロールの動的生成と初期設定を行います。
     * @return フォーカスをコントロールに設定しない場合はTRUE
     */
    virtual BOOL OnInitDialog() override;

    // --- コントロール ---
    /// @brief 表形式カスタムコントロール
    CGridCtrl m_gridCtrl;
    /// @brief 8つの機能ボタン
    CButton   m_btnFunc[8];
    /// @brief 次ページボタン
    CButton   m_btnNextPage;

    // --- 状態管理 ---
    /// @brief 現在選択されている行インデックス (0始まり, -1で非選択)
    int m_nSelectedRow;
    /// @brief 現在のページ番号 (1 or 2)
    int m_nCurrentPage;

    // --- ヘルパー関数 ---
    /**
     * @brief 現在の選択行とページ状態に基づき、全ボタンの表示/状態を更新します。
     */
    void UpdateButtons();

    // --- メッセージハンドラ ---
    
    /**
     * @brief CGridCtrlからの行選択変更通知(WM_NOTIFY, GCN_SELCHANGED)を処理します。
     * @param pNMHDR 通知情報を含むNMHDR構造体へのポインタ
     * @param pResult 結果コードへのポインタ
     */
    afx_msg void OnGridSelChanged(NMHDR* pNMHDR, LRESULT* pResult);
    
    /**
     * @brief 次ページボタンのクリックイベント(BN_CLICKED)を処理します。
     */
    afx_msg void OnNextPageClicked();
    
    /**
     * @brief 8つの機能ボタンのクリックイベント(BN_CLICKED)を一括で処理します。
     * @param nID クリックされたボタンのコントロールID
     */
    afx_msg void OnFuncButtonClicked(UINT nID);
    
    /**
     * @brief Enterキーによるダイアログ終了を無効化します。
     */
    virtual void OnOK() override;
    
    /**
     * @brief Escapeキーによるダイアログ終了を無効化します。
     * @param pMsg メッセージ情報へのポインタ
     * @return メッセージを処理した場合はTRUE
     */
    virtual BOOL PreTranslateMessage(MSG* pMsg) override;

    /// @brief メッセージマップを宣言します。
    DECLARE_MESSAGE_MAP()
};