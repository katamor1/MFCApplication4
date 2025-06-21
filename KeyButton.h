/**
 * @file KeyButton.h
 * @brief ソフトウェアキーボードの個別キーを描画するカスタムボタンクラスの宣言
 */
#pragma once

#include "KeyDefine.h"

// CSoftwareKeyboardDlgクラスの前方宣言
// 循環参照を避けるために、ここでは実際のヘッダをインクルードしません。
class CSoftwareKeyboardDlg;

/**
 * @class CKeyButton
 * @brief オーナードローのカスタムキーボタンクラス
 * @details CButtonを継承し、BS_OWNERDRAWスタイルを使用します。
 * ソフトウェアキーボード上の個々のキーとして振る舞い、DrawItemメンバ関数内で
 * 自身の見た目（背景色、ラベル、状態）を動的に描画します。
 */
class CKeyButton : public CButton
{
    DECLARE_DYNAMIC(CKeyButton)

public:
    /**
     * @brief デフォルトコンストラクタ
     */
    explicit CKeyButton();

    /**
     * @brief デストラクタ
     */
    virtual ~CKeyButton() override;

    /**
     * @brief キーボタンにキー情報と親ダイアログのポインタを設定します。
     * @details この関数は、ボタンが生成された後に呼び出され、描画やイベント処理に必要な
     * 情報をボタン自身が保持するために使用されます。
     * @param[in] pParent このボタンを所有する親ダイアログ(CSoftwareKeyboardDlg)へのポインタ
     * @param[in] pKeyInfo このボタンに対応するキー情報(KEY_INFO)へのポインタ
     */
    void SetKeyInfo(CSoftwareKeyboardDlg *pParent, const KEY_INFO *pKeyInfo);

protected:
    /// @brief このボタンに対応するキー情報構造体へのポインタ
    const KEY_INFO *m_pKeyInfo;
    /// @brief 親ダイアログ(CSoftwareKeyboardDlg)へのポインタ
    CSoftwareKeyboardDlg *m_pParentDlg;

    /**
     * @brief ボタンの描画処理を実装します (WM_DRAWITEM)。
     * @details BS_OWNERDRAWスタイルを持つボタンが再描画される必要があるときに
     * フレームワークから呼び出されます。親ダイアログの状態(ShiftキーON/OFFなど)を
     * 参照し、キーの背景、枠線、ラベルを適切に描画します。
     * @param[in] lpDrawItemStruct 描画に必要な情報を含む構造体へのポインタ
     */
    virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) override;

    /// @brief メッセージマップを宣言します。
    DECLARE_MESSAGE_MAP()
};