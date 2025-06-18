#pragma once

// CCenterEdit
// ����\���̃^�C�~���O�𒲐����邽�߂̃J�X�^�����b�Z�[�W
#define WM_APP_POST_INIT (WM_APP + 1)

class CCenterEdit : public CEdit
{
	DECLARE_DYNAMIC(CCenterEdit)

public:
	CCenterEdit();
	virtual ~CCenterEdit();

	// �I�[�o�[���C�h
protected:
	virtual void PreSubclassWindow();
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	// �����o�ϐ�
protected:
	CBrush m_brushFocus;    // �t�H�[�J�X���̔w�i�u���V
	CBrush m_brushNoFocus;  // ��t�H�[�J�X���̔w�i�u���V
	COLORREF m_colorTextFocus;   // �t�H�[�J�X���̕����F
	COLORREF m_colorTextNoFocus; // ��t�H�[�J�X���̕����F
	bool m_bIsFirstTimeInit = true;
	// ����
protected:
	void UpdateTextPosition(); // �e�L�X�g�̐����ʒu���X�V����w���p�[�֐�

	// ���b�Z�[�W �}�b�v
protected:
	afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnEnChange(); // �e�L�X�g�ύX�n���h��
	afx_msg LRESULT OnPostInit(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()
};
