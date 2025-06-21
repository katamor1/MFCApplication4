/**
 * @file MFCApplication4.h
 * @brief アプリケーションクラスのメインヘッダーファイル
 * @details このファイルは、MFCアプリケーション全体の動作を管理する
 * CMFCApplication4App クラスを宣言します。
 */
#pragma once

#ifndef __AFXWIN_H__
#error "PCH に対してこのファイルをインクルードする前に 'pch.h' をインクルードしてください"
#endif

#include "resource.h" // メイン シンボル

/// @brief メインダイアログに「操作中」と表示させるためのカスタムメッセージID
#define WM_APP_SHOW_OPERATION_STATUS (WM_APP + 3)

/**
 * @class CMFCApplication4App
 * @brief アプリケーションのメインクラス
 * @details CWinAppを継承し、アプリケーションのインスタンス化、初期化、終了処理、
 * そしてアプリケーション全体のメッセージフィルタリングを担当します。
 */
class CMFCApplication4App : public CWinApp
{
public:
    /**
     * @brief デフォルトコンストラクタ
     */
    CMFCApplication4App();

    // オーバーライド
public:
    /**
     * @brief アプリケーションのインスタンスが初期化される際にフレームワークから呼び出されます。
     * @details メインダイアログの生成と表示など、アプリケーションの起動シーケンスを定義します。
     * @return アプリケーションを即座に終了させるため、常にFALSEを返します。
     */
    virtual BOOL InitInstance() override;

    /**
     * @brief アプリケーションのインスタンスが終了する際にフレームワークから呼び出されます。
     * @details アプリケーション終了時のクリーンアップ処理を行います。
     * @return 親クラスの終了コードを返します。
     */
    virtual int ExitInstance() override;

    /**
     * @brief アプリケーションのメッセージループでメッセージがディスパッチされる前に呼び出されます。
     * @details アプリケーション全体のキーボード・マウスメッセージを横取り（フィルタリング）し、
     * 特定の操作を無効化するなどのグローバルな処理を実装します。
     * @param[in] pMsg メッセージ情報へのポインタ
     * @return メッセージをここで処理した場合はTRUE、通常のディスパッチを続ける場合はFALSE。
     */
    virtual BOOL PreTranslateMessage(MSG *pMsg) override;

    // 実装

    /// @brief メッセージマップを宣言します。
    DECLARE_MESSAGE_MAP()
};

/// @brief アプリケーションの唯一のインスタンスを指すグローバルオブジェクト
extern CMFCApplication4App theApp;