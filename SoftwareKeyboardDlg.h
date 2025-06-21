/**
 * @file SoftwareKeyboardDlg.h
 * @brief ソフトウェアキーボードダイアログのクラス宣言
 * @author C++/MFCコーディング (AI)
 * @date 2025/06/22
 * @details このファイルは、マウスで文字入力を行うためのソフトウェアキーボードの
 * メインダイアログクラス CSoftwareKeyboardDlg の宣言を含みます。
 */
#pragma once

#include "KeyDefine.h"
#include "KeyButton.h"
#include <vector>
#include "Resource.h"

class CEdit; // CEditクラスの前方宣言

/**
 * @class CSoftwareKeyboardDlg
 * @brief ソフトウェアキーボードのダイアログクラス
 * @details CDialogExを継承し、キーの動的生成、描画、入力イベント処理、
 * ウィンドウのドラッグ移動などを担当します。
 */
class CSoftwareKeyboardDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CSoftwareKeyboardDlg)

public:
	/**
	 * @brief コンストラクタ
	 * @param[in] pTargetEdit キー入力の送信先となるエディットコントロールへのポインタ
	 * @param[in] pParent 親ウィンドウへのポインタ
	 */
	CSoftwareKeyboardDlg(CEdit *pTargetEdit, CWnd *pParent = nullptr);
	
	/**
	 * @brief デストラクタ
	 */
	virtual ~CSoftwareKeyboardDlg() override;

	/// @brief ダイアログ テンプレートのリソースID
	enum
	{
		IDD = IDD_SW_KEYBOARD
	};

	// --- 状態取得用 公開関数 ---
	bool IsShiftOn() const  noexcept { return m_bShiftOn; }     ///< ShiftキーがONかを取得します
	bool IsCapsLockOn() const  noexcept { return m_bCapsLockOn; } ///< CapsLockキーがONかを取得します
	bool IsCtrlOn() const  noexcept { return m_bCtrlOn; }       ///< CtrlキーがONかを取得します
	bool IsAltOn() const  noexcept { return m_bAltOn; }        ///< AltキーがONかを取得します
	bool IsFnOn() const  noexcept { return m_bFnOn; }         ///< FnキーがONかを取得します

protected:
	/**
	 * @brief ダイアログデータエクスチェンジ (DDX) および検証 (DDV) を行います。
	 * @param pDX データエクスチェンジオブジェクトへのポインタ
	 */
	virtual void DoDataExchange(CDataExchange *pDX);
	
	/**
	 * @brief ダイアログの初期化時にフレームワークから呼び出されます。
	 * @details コントロールの動的生成やメンバ変数の初期化を行います。
	 * @return 処理が成功し、フォーカスを設定した場合はTRUE。
	 */
	virtual BOOL OnInitDialog() override;

	/**
	 * @brief Enterキーによるダイアログ終了を無効化するためにオーバーライドします。
	 */
	virtual void OnOK() override;

	/**
	 * @brief Escapeキーによるダイアログ終了を無効化するためにオーバーライドします。
	 */
	virtual void OnCancel() override;

	// --- メンバ変数 ---
	CEdit *m_pTargetEdit; ///< キー入力の送信先となるエディットコントロール
	std::vector<CKeyButton *> m_KeyButtons; ///< 動的に生成したキーボタンのポインタを保持するコンテナ

	// --- 状態保持キーのフラグ ---
	bool m_bShiftOn;    ///< Shiftキーのトグル状態 (true: ON)
	bool m_bCapsLockOn; ///< CapsLockキーのトグル状態 (true: ON)
	bool m_bCtrlOn;     ///< Ctrlキーのトグル状態 (true: ON)
	bool m_bAltOn;      ///< Altキーのトグル状態 (true: ON)
	bool m_bFnOn;       ///< Fnキーのトグル状態 (true: ON)

	// --- ダイアログ移動用 ---
	bool m_bDragging;         ///< ウィンドウをドラッグ中かどうかのフラグ
	CPoint m_ptMouseOffset;   ///< ドラッグ開始時のマウスカーソルオフセット

	// --- カスタム描画領域 ---
	CRect m_rcTitleBar; ///< ドラッグ可能なタイトルバー領域
	CRect m_rcCloseBtn; ///< 閉じるボタンの領域

	// --- ヘルパー関数 ---
	void HandleKeyPress(const KEY_INFO *pKeyInfo);
	void SendChar(TCHAR ch);
	void SendKey(BYTE vk);
	void UpdateAllKeys();

	// --- メッセージハンドラ ---
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnKeyClick(UINT nID);
	DECLARE_MESSAGE_MAP()
};