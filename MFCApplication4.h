
// MFCApplication4.h : PROJECT_NAME アプリケーションのメイン ヘッダー ファイルです
//

#pragma once

#ifndef __AFXWIN_H__
	#error "PCH に対してこのファイルをインクルードする前に 'pch.h' をインクルードしてください"
#endif

#include "resource.h"		// メイン シンボル


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

// 実装

	DECLARE_MESSAGE_MAP()
};

extern CMFCApplication4App theApp;
