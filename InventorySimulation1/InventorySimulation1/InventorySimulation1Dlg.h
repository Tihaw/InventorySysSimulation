
// InventorySimulation1Dlg.h : header file
//

#pragma once
#include "afxwin.h"
#include "afxcmn.h"



// CInventorySimulation1Dlg dialog
class CInventorySimulation1Dlg : public CDialogEx
{
// Construction
public:
	CInventorySimulation1Dlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_INVENTORYSIMULATION1_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnEnChangeEdit1();
	afx_msg void OnLbnSelchangeList1();
	afx_msg void OnEnChangepolicies();
	afx_msg void OnBnClickedButton1();
	CListCtrl m_listctrl;
	afx_msg void OnBnClickedButton2();
	CButton CalAvg_bt;
};
