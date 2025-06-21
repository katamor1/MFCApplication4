#pragma once

#include "CenterEdit.h"

// View1 ビュー

// ワーカースレッドからView2の表示をトリガーするためのカスタムメッセージ
#define WM_SHOW_VIEW2 (WM_APP + 2)

class CView1 : public CView
{
	DECLARE_DYNCREATE(CView1)
	CCenterEdit *m_editCustom1 = nullptr;
	CCenterEdit *m_editCustom2 = nullptr;
	CWinThread *m_pWorkerThread;				 // ワーカースレッドのポインタ
	CEvent m_eventStopThread;					 // スレッド停止用イベント
	static UINT WorkerThreadProc(LPVOID pParam); // ワーカースレッド関数

public:
	CView1() noexcept
	{
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
	virtual ~CView1() = default;
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext &dc) const;
#endif

protected:
	afx_msg void OnDestroy();
	virtual void OnDraw(CDC *pDC);
	virtual BOOL PreCreateWindow(CREATESTRUCT &cs);
	DECLARE_MESSAGE_MAP()
};