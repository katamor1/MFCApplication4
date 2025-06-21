/**
 * @file View1.h
 * @brief CViewを派生したプライマリビューのクラス宣言
 * @author C++/MFCコーディング (AI)
 * @date 2025/06/22
 * @details このビューはメインダイアログに配置され、ワーカースレッドを起動して
 * 特定の条件を監視し、条件成立時に親ウィンドウへ通知する機能を持っています。
 */
#pragma once

#include "CenterEdit.h"

/// @brief ワーカースレッドからView2の表示をトリガーするためのカスタムメッセージ
#define WM_SHOW_VIEW2 (WM_APP + 2)

/**
 * @class CView1
 * @brief ワーカースレッドを持つプライマリカスタムビュー
 * @details CViewを継承し、インスタンス生成時にワーカースレッドを起動します。
 * スレッドは特定の条件（この実装では5秒の待機）を監視し、成立すると
 * 親ウィンドウにWM_SHOW_VIEW2メッセージを送信してCView2の表示を要求します。
 * ウィンドウ破棄時にはワーカースレッドを安全に停止させます。
 */
class CView1 : public CView
{
protected: // シリアル化からのみ作成します。
    /**
     * @brief DYNCREATEマクロ
     * @details フレームワークがこのクラスのインスタンスを動的に生成できるようにします。
     */
    DECLARE_DYNCREATE(CView1)
    
    /// @brief ビュー内に配置されるカスタムエディットコントロールへのポインタ1
    CCenterEdit* m_editCustom1 = nullptr;
    /// @brief ビュー内に配置されるカスタムエディットコントロールへのポインタ2
    CCenterEdit* m_editCustom2 = nullptr;

    // 属性

    // 操作
public:
    /**
     * @brief ワーカースレッドのエントリポイントとなる静的メンバ関数
     * @param[in] pParam スレッドに渡されるパラメータ。この実装ではCView1のインスタンスへのポインタ(this)。
     * @return スレッドの終了コード。
     */
    static UINT WorkerThreadProc(LPVOID pParam);

    // オーバーライド
public:
    /**
     * @brief ビューの描画処理を行います (WM_PAINT)。
     * @param[in] pDC 描画に使用するデバイスコンテキストへのポインタ。
     */
    virtual void OnDraw(CDC* pDC) override;
    
    /**
     * @brief ウィンドウが作成される直前にフレームワークから呼び出されます。
     * @details ウィンドウのスタイル(CREATESTRUCT)を変更するためにオーバーライドされます。
     * @param[in,out] cs ウィンドウの作成パラメータを保持するCREATESTRUCT構造体。
     * @return ウィンドウ作成を続行する場合はTRUE、中止する場合はFALSE。
     */
    virtual BOOL PreCreateWindow(CREATESTRUCT& cs) override;

    // 実装
public:
    /**
     * @brief コンストラクタ
     * @details このコンストラクタ内でワーカースレッドを起動します。
     */
    CView1() noexcept {
        // ワーカースレッドを起動
        m_pWorkerThread = AfxBeginThread(WorkerThreadProc, this);
        if (!m_pWorkerThread)
        {
            AfxMessageBox(_T("Failed to create worker thread."), MB_ICONERROR);
        }
        else
        {
            TRACE("Worker thread started.\n");
        }
    };
    
    /**
     * @brief デストラクタ
     * @details OnDestroyでワーカースレッドの停止処理が行われます。
     */
    virtual ~CView1() noexcept override = default;
#ifdef _DEBUG
    /**
     * @brief オブジェクトの有効性を診断します (デバッグビルドのみ)。
     */
    virtual void AssertValid() const override;
    
    /**
     * @brief オブジェクトの状態をダンプします (デバッグビルドのみ)。
     * @param[in,out] dc ダンプ先のデバイスコンテキスト。
     */
    virtual void Dump(CDumpContext& dc) const override;
#endif

protected:
    /// @brief ワーカースレッドのインスタンスを指すポインタ
    CWinThread* m_pWorkerThread;
    /// @brief ワーカースレッドに停止を通知するためのイベントオブジェクト
    CEvent      m_eventStopThread;

    // 生成された、メッセージ割り当て関数
protected:
    /**
     * @brief ウィンドウが破棄される際に呼び出されます (WM_DESTROY)。
     * @details ワーカースレッドを安全に停止させるために使用します。
     */
    afx_msg void OnDestroy();
    
    /// @brief メッセージマップを宣言します。
    DECLARE_MESSAGE_MAP()
};