/**
 * @file CView2.h
 * @brief CViewを派生したカスタムビュークラスの宣言
 * @details このビューは、メインダイアログに子ウィンドウとして配置され、
 * 他のビュー(CView1)の上に重ねて表示されることを想定しています。
 */
#pragma once

#include "CenterEdit.h"

/**
 * @class CView2
 * @brief CView1の上に重ねて表示されるカスタムビュー
 * @details CViewを継承し、内部にCCenterEditコントロールを動的に配置します。
 * ウィンドウ作成時にスタイルを調整し、兄弟ウィンドウとの描画の重なり方を制御します。
 */
class CView2 : public CView
{
protected:
    /**
     * @brief DYNCREATEマクロ
     * @details フレームワークがこのクラスのインスタンスを動的に生成できるようにします。
     */
    DECLARE_DYNCREATE(CView2)
    
    /// @brief ビュー内に配置されるカスタムエディットコントロールへのポインタ1
    CCenterEdit *m_editCustom1 = nullptr;
    /// @brief ビュー内に配置されるカスタムエディットコントロールへのポインタ2
    CCenterEdit *m_editCustom2 = nullptr;

    // 属性
public:
    // 操作
public:
    // オーバーライド
public:
    /**
     * @brief ビューの描画処理を行います (WM_PAINT)。
     * @details この実装では、初回描画時に子コントロールであるCCenterEditを動的に生成・配置します。
     * @param[in] pDC 描画に使用するデバイスコンテキストへのポインタ。
     */
    virtual void OnDraw(CDC *pDC) override;
    
    /**
     * @brief ウィンドウが作成される直前にフレームワークから呼び出されます。
     * @details ウィンドウのスタイル(CREATESTRUCT)を変更するためにオーバーライドされます。
     * @param[in,out] cs ウィンドウの作成パラメータを保持するCREATESTRUCT構造体。
     * @return ウィンドウ作成を続行する場合はTRUE、中止する場合はFALSE。
     */
    virtual BOOL PreCreateWindow(CREATESTRUCT &cs) override;

    // 実装
public:
    /**
     * @brief デフォルトコンストラクタ
     */
    CView2() noexcept = default;

    /**
     * @brief デストラクタ
     * @details 動的に確保したコントロールは、親ウィンドウが破棄される際に自動的にクリーンアップされます。
     */
    virtual ~CView2() noexcept override = default;
#ifdef _DEBUG
    /**
     * @brief オブジェクトの有効性を診断します (デバッグビルドのみ)。
     */
    virtual void AssertValid() const override;

    /**
     * @brief オブジェクトの状態をダンプします (デバッグビルドのみ)。
     * @param[in,out] dc ダンプ先のデバイスコンテキスト。
     */
    virtual void Dump(CDumpContext &dc) const override;
#endif

protected:
    /// @brief メッセージマップを宣言します。
    DECLARE_MESSAGE_MAP()
};