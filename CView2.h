#pragma once

#include "CenterEdit.h"

// CView2 ビュー

class CView2 : public CView
{
protected:
	DECLARE_DYNCREATE(CView2)
	CCenterEdit *m_editCustom1 = nullptr;
	CCenterEdit *m_editCustom2 = nullptr;

	// 属性
public:
	// 操作
public:
	// オーバーライド
public:
	virtual void OnDraw(CDC *pDC);
	virtual BOOL PreCreateWindow(CREATESTRUCT &cs);

	// 実装
public:
	CView2() noexcept {};
	virtual ~CView2() = default;
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext &dc) const;
#endif

protected:
	DECLARE_MESSAGE_MAP()
};
