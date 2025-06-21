
// MFCApplication4.h : PROJECT_NAME アプリケーションのメイン ヘッダー ファイルです
//

#pragma once

#ifndef __AFXWIN_H__
#error "PCH に対してこのファイルをインクルードする前に 'pch.h' をインクルードしてください"
#endif

#include "resource.h" // メイン シンボル

#define WM_APP_SHOW_OPERATION_STATUS (WM_APP + 3)

// CMFCApplication4App:
// このクラスの実装については、MFCApplication4.cpp を参照してください
//

class CMFCApplication4App : public CWinApp
{
public:
	CMFCApplication4App();

	// オーバーライド
public:
	virtual BOOL InitInstance();
	virtual BOOL PreTranslateMessage(MSG *pMsg);

	// 実装

	DECLARE_MESSAGE_MAP()
};

extern CMFCApplication4App theApp;
