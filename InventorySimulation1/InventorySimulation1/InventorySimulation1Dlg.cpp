
// InventorySimulation1Dlg.cpp : implementation file
//

#include "stdafx.h"
#include "InventorySimulation1.h"
#include "InventorySimulation1Dlg.h"
#include "afxdialogex.h"
#include <vector>

extern "C"
{
#include "lcgrand.h"
};
#include <math.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
using namespace std;

class inventorySim
{
public:
	int   m_amount, m_bigs, m_initial_inv_level, m_inv_level, m_next_event_type, m_num_events,
		m_num_months, m_num_values_demand, m_smalls;
	float m_area_holding, m_area_shortage, m_holding_cost, m_incremental_cost, m_maxlag,
		m_mean_interdemand, m_minlag, m_prob_distrib_demand[26], m_setup_cost,
		m_shortage_cost, m_sim_time, m_time_last_event, m_time_next_event[5],
		m_total_ordering_cost;
	int m_num_policies;

	void  initialize(void);
	void  timing(void);
	void  order_arrival(void);
	void  demand(void);
	void  evaluate(void);
	void  report(void);
	void  update_time_avg_stats(void);
	float expon(float mean);
	int   random_integer(float prob_distrib []);
	float uniform(float a, float b);

protected:
private:
}m_inventorySim;

int rando_seed = 1;
int click_run_time = 0;
vector<float> totalCost[10], orderingCost[10];
vector<float> holdingCost[10], shortageCost[10];

char * cs2ca(CString str)
{
	char *ptr;
#ifdef _UNICODE
	LONG len;
	len = WideCharToMultiByte(CP_ACP, 0, str, -1, NULL, 0, NULL, NULL);
	ptr = new char [len+1];
	memset(ptr,0,len + 1);
	WideCharToMultiByte(CP_ACP, 0, str, -1, ptr, len + 1, NULL, NULL);
#else
	ptr = new char [str.GetAllocLength()+1];
	sprintf(ptr,_T("%s"),str);
#endif
	return ptr;
}


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CInventorySimulation1Dlg dialog




CInventorySimulation1Dlg::CInventorySimulation1Dlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CInventorySimulation1Dlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CInventorySimulation1Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_listctrl);
	DDX_Control(pDX, IDC_BUTTON2, CalAvg_bt);
}

BEGIN_MESSAGE_MAP(CInventorySimulation1Dlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDOK, &CInventorySimulation1Dlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CInventorySimulation1Dlg::OnBnClickedCancel)
	//ON_EN_CHANGE(IDC_EDIT1, &CInventorySimulation1Dlg::OnEnChangeEdit1)
	ON_LBN_SELCHANGE(IDC_LIST1, &CInventorySimulation1Dlg::OnLbnSelchangeList1)
	ON_EN_CHANGE(num_policies, &CInventorySimulation1Dlg::OnEnChangepolicies)
	ON_BN_CLICKED(IDC_BUTTON1, &CInventorySimulation1Dlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CInventorySimulation1Dlg::OnBnClickedButton2)
END_MESSAGE_MAP()


// CInventorySimulation1Dlg message handlers

BOOL CInventorySimulation1Dlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	//EXTRA INIT!!
// 4	&initial_inv_level, &num_months, &num_policies, &num_values_demand,
// 	4	&mean_interdemand, &setup_cost, &incremental_cost, &holding_cost,
	// 	3	&shortage_cost, &minlag, &maxlag
	SetDlgItemText(initial_inv_level,L"60");
	SetDlgItemText(num_months,L"120");
	SetDlgItemText(num_policies,L"9");
	SetDlgItemText(num_values_demand,L"4");
	SetDlgItemText(mean_interdemand,L"0.1");
	SetDlgItemText(setup_cost,L"32");
	SetDlgItemText(incremental_cost,L"3.0");
	SetDlgItemText(holding_cost,L"1.0");
	SetDlgItemText(shortage_cost,L"5.0");
	SetDlgItemText(minlag,L"0.5");
	SetDlgItemText(maxlag,L"1.0");
	SetDlgItemText(prob_distrib_demand1,L"0.167");
	SetDlgItemText(prob_distrib_demand2,L"0.500");
	SetDlgItemText(prob_distrib_demand3,L"0.833");
	SetDlgItemText(prob_distrib_demand5,L"1.0");
	//set policies:
	SetDlgItemText(poli_s1,L"20");
	SetDlgItemText(poli_sBig1,L"40");
	SetDlgItemText(poli_s2,L"20");
	SetDlgItemText(poli_sBig2,L"60");
	SetDlgItemText(poli_s3,L"20");
	SetDlgItemText(poli_sBig3,L"80");
	SetDlgItemText(poli_s4,L"20");
	SetDlgItemText(poli_sBig4,L"100");
	SetDlgItemText(poli_s5,L"40");
	SetDlgItemText(poli_sBig5,L"60");
	SetDlgItemText(poli_s6,L"40");
	SetDlgItemText(poli_sBig6,L"80");
	SetDlgItemText(poli_s7,L"40");
	SetDlgItemText(poli_sBig7,L"100");
	SetDlgItemText(poli_s8,L"60");
	SetDlgItemText(poli_sBig8,L"80");
	SetDlgItemText(poli_s9,L"60");
	SetDlgItemText(poli_sBig9,L"100");
	SetDlgItemText(seed,L"1");
	CString t;
	t.Format(_T("U've clicked %d times Run button)"), click_run_time);
	SetDlgItemText(prompt,t);

	m_listctrl.InsertColumn(0,L"Policy", 1, 100, -1);
	m_listctrl.InsertColumn(1,L"Average total cost", 1, 120, -1);
	m_listctrl.InsertColumn(2,L"Average ordering cost", 1, 120, -1);
	m_listctrl.InsertColumn(3,L"Average holding cost", 1, 120, -1);
	m_listctrl.InsertColumn(4,L"Average shortage cost", 1, 130, -1);
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CInventorySimulation1Dlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CInventorySimulation1Dlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CInventorySimulation1Dlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CInventorySimulation1Dlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	CDialogEx::OnOK();
}


void CInventorySimulation1Dlg::OnBnClickedCancel()
{
	// TODO: 在此添加控件通知处理程序代码
	CDialogEx::OnCancel();
}


void CInventorySimulation1Dlg::OnEnChangeEdit1()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CDialogEx::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
}


void CInventorySimulation1Dlg::OnLbnSelchangeList1()
{
	// TODO: 在此添加控件通知处理程序代码
}


void CInventorySimulation1Dlg::OnEnChangepolicies()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CDialogEx::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
}

void inventorySim::initialize(void)  /* Initialization function. */
{
    /* Initialize the simulation clock. */

    m_sim_time = 0.0;

    /* Initialize the state variables. */

    m_inv_level       = m_initial_inv_level;
    m_time_last_event = 0.0;

    /* Initialize the statistical counters. */

    m_total_ordering_cost = 0.0;
    m_area_holding        = 0.0;
    m_area_shortage       = 0.0;

    /* Initialize the event list.  Since no order is outstanding, the order-
       arrival event is eliminated from consideration. */

    m_time_next_event[1] = 1.0e+30;
    m_time_next_event[2] = m_sim_time + expon(m_mean_interdemand);
    m_time_next_event[3] = m_num_months;
    m_time_next_event[4] = 0.0;
}


void inventorySim::timing(void)  /* Timing function. */
{
    int   i;
    float min_time_next_event = 1.0e+29;

    m_next_event_type = 0;

    /* Determine the event type of the next event to occur. */

    for (i = 1; i <= m_num_events; ++i)
        if (m_time_next_event[i] < min_time_next_event) {
            min_time_next_event = m_time_next_event[i];
            m_next_event_type     = i;
        }

    /* Check to see whether the event list is empty. */

    if (m_next_event_type == 0) {

        /* The event list is empty, so stop the simulation */

        //fprintf(outfile, "\nEvent list empty at time %f", m_sim_time);
        exit(1);
    }

    /* The event list is not empty, so advance the simulation clock. */

    m_sim_time = min_time_next_event;
}


void inventorySim::order_arrival(void)  /* Order arrival event function. */
{
    /* Increment the inventory level by the amount ordered. */

    m_inv_level += m_amount;

    /* Since no order is now outstanding, eliminate the order-arrival event from
       consideration. */

    m_time_next_event[1] = 1.0e+30;
}


void inventorySim::demand(void)  /* Demand event function. */
{
    /* Decrement the inventory level by a generated demand size. */

    m_inv_level -= random_integer(m_prob_distrib_demand);

    /* Schedule the time of the next demand. */

    m_time_next_event[2] = m_sim_time + expon(m_mean_interdemand);
}


void inventorySim::evaluate(void)  /* Inventory-evaluation event function. */
{
    /* Check whether the inventory level is less than smalls. */

    if (m_inv_level < m_smalls) {

        /* The inventory level is less than smalls, so place an order for the
           appropriate amount. */

        m_amount               = m_bigs - m_inv_level;
        m_total_ordering_cost += m_setup_cost + m_incremental_cost * m_amount;

        /* Schedule the arrival of the order. */

        m_time_next_event[1] = m_sim_time + uniform(m_minlag, m_maxlag);
    }

    /* Regardless of the place-order decision, schedule the next inventory
       evaluation. */

    m_time_next_event[4] = m_sim_time + 1.0;
}


void inventorySim::report(void)  /* Report generator function. */
{
    /* Compute and write estimates of desired measures of performance. */

    float avg_holding_cost, avg_ordering_cost, avg_shortage_cost;

    avg_ordering_cost = m_total_ordering_cost / m_num_months;
    avg_holding_cost  = m_holding_cost * m_area_holding / m_num_months;
    avg_shortage_cost = m_shortage_cost * m_area_shortage / m_num_months;
/*
    fprintf(outfile, "\n\n(%3d,%3d)%15.2f%15.2f%15.2f%15.2f",
            m_smalls, m_bigs,
            avg_ordering_cost + avg_holding_cost + avg_shortage_cost,
            avg_ordering_cost, avg_holding_cost, avg_shortage_cost);*/

}


void inventorySim::update_time_avg_stats(void)  /* Update area accumulators for time-average
                                     statistics. */
{
    float time_since_last_event;

    /* Compute time since last event, and update last-event-time marker. */

    time_since_last_event = m_sim_time - m_time_last_event;
    m_time_last_event       = m_sim_time;

    /* Determine the status of the inventory level during the previous interval.
       If the inventory level during the previous interval was negative, update
       area_shortage.  If it was positive, update area_holding.  If it was zero,
       no update is needed. */

    if (m_inv_level < 0)
        m_area_shortage -= m_inv_level * time_since_last_event;
    else if (m_inv_level > 0)
        m_area_holding  += m_inv_level * time_since_last_event;
}


float inventorySim::expon(float mean)  /* Exponential variate generation function. */
{
    /* Return an exponential random variate with mean "mean". */

    return -mean * log(lcgrand(rando_seed));
}


int inventorySim::random_integer(float prob_distrib[])  /* Random integer generation
                                             function. */
{
    int   i;
    float u;

    /* Generate a U(0,1) random variate. */

    u = lcgrand(rando_seed);

    /* Return a random integer in accordance with the (cumulative) distribution
       function prob_distrib. */

    for (i = 1; u >= prob_distrib[i]; ++i)
        ;
    return i;
}


float inventorySim::uniform(float a, float b)  /* Uniform variate generation function. */
{
    /* Return a U(a,b) random variate. */

    return a + lcgrand(rando_seed) * (b - a);
}

void CInventorySimulation1Dlg::OnBnClickedButton1()
{
	// TODO: 在此添加控件通知处理程序代码
	//read params
	CString tmp;
	char *str="";

	m_listctrl.DeleteAllItems();

	//first obtain the seed;
	rando_seed = GetDlgItemInt(seed);

	m_inventorySim.m_initial_inv_level = GetDlgItemInt(initial_inv_level);
	m_inventorySim.m_num_months = GetDlgItemInt(num_months);
	m_inventorySim.m_num_policies = GetDlgItemInt(num_policies);
	m_inventorySim.m_num_values_demand = GetDlgItemInt(num_values_demand);
	GetDlgItemText(mean_interdemand, tmp);
	str = cs2ca(tmp);
	m_inventorySim.m_mean_interdemand = atof(str);
	GetDlgItemText(setup_cost, tmp);
	str = cs2ca(tmp);
	m_inventorySim.m_setup_cost = atof(str);
	GetDlgItemText(incremental_cost, tmp);
	str = cs2ca(tmp);
	m_inventorySim.m_incremental_cost = atof(str);
	GetDlgItemText(holding_cost, tmp);
	str = cs2ca(tmp);
	m_inventorySim.m_holding_cost = atof( str);
	GetDlgItemText(shortage_cost, tmp);
	str = cs2ca(tmp);
	m_inventorySim.m_shortage_cost = atof(str);
	GetDlgItemText(minlag, tmp);
	str = cs2ca(tmp);
	m_inventorySim.m_minlag = atof( str);
	GetDlgItemText(maxlag, tmp);
	str = cs2ca(tmp);
	m_inventorySim.m_maxlag = atof( str);
	GetDlgItemText(prob_distrib_demand1, tmp);
	str = cs2ca(tmp);
	m_inventorySim.m_prob_distrib_demand[1] = atof( str);
	GetDlgItemText(prob_distrib_demand2, tmp);			
	str = cs2ca(tmp);														   
	m_inventorySim.m_prob_distrib_demand[2] = atof( str);
	GetDlgItemText(prob_distrib_demand3, tmp);			
	str = cs2ca(tmp);														   
	m_inventorySim.m_prob_distrib_demand[3] = atof( str);
	GetDlgItemText(prob_distrib_demand5, tmp);			
	str = cs2ca(tmp);														   
	m_inventorySim.m_prob_distrib_demand[4] = atof( str);

	for (int i = 1; i <= m_inventorySim.m_num_policies; i++)
	{
		switch(i)
		{
		case 1:
			m_inventorySim.m_smalls = GetDlgItemInt(poli_s1);
			m_inventorySim.m_bigs = GetDlgItemInt(poli_sBig1);
			break;
		case 2:
			m_inventorySim.m_smalls = GetDlgItemInt(poli_s2);
			m_inventorySim.m_bigs = GetDlgItemInt(poli_sBig2);
			break;
		case 3:
			m_inventorySim.m_smalls = GetDlgItemInt(poli_s3);
			m_inventorySim.m_bigs = GetDlgItemInt(poli_sBig3);
			break;
		case 4:
			m_inventorySim.m_smalls = GetDlgItemInt(poli_s4);
			m_inventorySim.m_bigs = GetDlgItemInt(poli_sBig4);
			break;
		case 5:
			m_inventorySim.m_smalls = GetDlgItemInt(poli_s5);
			m_inventorySim.m_bigs = GetDlgItemInt(poli_sBig5);
			break;
		case 6:
			m_inventorySim.m_smalls = GetDlgItemInt(poli_s6);
			m_inventorySim.m_bigs = GetDlgItemInt(poli_sBig6);
			break;
		case 7:
			m_inventorySim.m_smalls = GetDlgItemInt(poli_s7);
			m_inventorySim.m_bigs = GetDlgItemInt(poli_sBig7);
			break;
		case 8:
			m_inventorySim.m_smalls = GetDlgItemInt(poli_s8);
			m_inventorySim.m_bigs = GetDlgItemInt(poli_sBig8);
			break;
		case 9:
			m_inventorySim.m_smalls = GetDlgItemInt(poli_s9);
			m_inventorySim.m_bigs = GetDlgItemInt(poli_sBig9);
			break;
		}

		m_inventorySim.m_num_events = 4;

		 m_inventorySim.initialize();

        /* Run the simulation until it terminates after an end-simulation event
           (type 3) occurs. */

        do {

            /* Determine the next event. */

            m_inventorySim.timing();

            /* Update time-average statistical accumulators. */

            m_inventorySim.update_time_avg_stats();

            /* Invoke the appropriate event function. */

            switch (m_inventorySim.m_next_event_type) {
                case 1:
                    m_inventorySim.order_arrival();
                    break;
                case 2:
                    m_inventorySim.demand();
                    break;
                case 4:
                    m_inventorySim.evaluate();
                    break;
                case 3:
                    //m_inventorySim.report();
					{
						float avg_holding_cost, avg_ordering_cost, avg_shortage_cost;

						avg_ordering_cost = 
							m_inventorySim.m_total_ordering_cost / 
							m_inventorySim.m_num_months;
						avg_holding_cost  = 
							m_inventorySim.m_holding_cost * 
							m_inventorySim.m_area_holding / 
							m_inventorySim.m_num_months;
						avg_shortage_cost = 
							m_inventorySim.m_shortage_cost * 
							m_inventorySim.m_area_shortage / 
							m_inventorySim.m_num_months;
						//show in the list
						CString t;
						t.Format(_T("(%d, %d)"), m_inventorySim.m_smalls, m_inventorySim.m_bigs);
						m_listctrl.InsertItem(0, t);
						t.Format(_T("%.2f"), 
							avg_ordering_cost+
							avg_holding_cost +avg_shortage_cost);
						m_listctrl.SetItemText(0,1,t);
						t.Format(_T("%.2f"), avg_ordering_cost);
						m_listctrl.SetItemText(0,2,t);
						t.Format(_T("%.2f"), avg_holding_cost);
						m_listctrl.SetItemText(0,3,t);
						t.Format(_T("%.2f"), avg_shortage_cost);
						m_listctrl.SetItemText(0,4,t);

						//remember the  avgs
						totalCost[i].push_back(avg_ordering_cost+
							avg_holding_cost +avg_shortage_cost);
						orderingCost[i].push_back(avg_ordering_cost);
						holdingCost[i].push_back(avg_holding_cost);
						shortageCost[i].push_back(avg_shortage_cost);
					}
                    break;
            }

        /* If the event just executed was not the end-simulation event (type 3),
           continue simulating.  Otherwise, end the simulation for the current
           (s,S) pair and go on to the next pair (if any). */

        } while (m_inventorySim.m_next_event_type != 3);
	}
	click_run_time++;
	CString t;
	t.Format(_T("U've clicked %d times Run button)"), click_run_time);
	SetDlgItemText(prompt,t);
	CalAvg_bt.EnableWindow(TRUE);

}


void CInventorySimulation1Dlg::OnBnClickedButton2()
{
	m_listctrl.DeleteAllItems();
	// TODO: 在此添加控件通知处理程序代码
	float avg_holding_cost, avg_ordering_cost, avg_shortage_cost;

	for (int i = 1; i <= m_inventorySim.m_num_policies;i++)
	{
		avg_holding_cost = 0;
		avg_ordering_cost =0;
		avg_shortage_cost = 0;

		for (int j = 0; j <holdingCost[1].size(); j++ )
		{
			avg_holding_cost += holdingCost[i][j];
			avg_ordering_cost += orderingCost[i][j];
			avg_shortage_cost += shortageCost[i][j];
		}
		// divide by size..... the sizes are =same :)
		avg_holding_cost /= holdingCost[1].size();
		avg_ordering_cost /= orderingCost[1].size();
		avg_shortage_cost /= shortageCost[1].size();

		CString t;
		switch(i)
		{
		case 1:
			m_inventorySim.m_smalls = GetDlgItemInt(poli_s1);
			m_inventorySim.m_bigs = GetDlgItemInt(poli_sBig1);
			break;
		case 2:
			m_inventorySim.m_smalls = GetDlgItemInt(poli_s2);
			m_inventorySim.m_bigs = GetDlgItemInt(poli_sBig2);
			break;
		case 3:
			m_inventorySim.m_smalls = GetDlgItemInt(poli_s3);
			m_inventorySim.m_bigs = GetDlgItemInt(poli_sBig3);
			break;
		case 4:
			m_inventorySim.m_smalls = GetDlgItemInt(poli_s4);
			m_inventorySim.m_bigs = GetDlgItemInt(poli_sBig4);
			break;
		case 5:
			m_inventorySim.m_smalls = GetDlgItemInt(poli_s5);
			m_inventorySim.m_bigs = GetDlgItemInt(poli_sBig5);
			break;
		case 6:
			m_inventorySim.m_smalls = GetDlgItemInt(poli_s6);
			m_inventorySim.m_bigs = GetDlgItemInt(poli_sBig6);
			break;
		case 7:
			m_inventorySim.m_smalls = GetDlgItemInt(poli_s7);
			m_inventorySim.m_bigs = GetDlgItemInt(poli_sBig7);
			break;
		case 8:
			m_inventorySim.m_smalls = GetDlgItemInt(poli_s8);
			m_inventorySim.m_bigs = GetDlgItemInt(poli_sBig8);
			break;
		case 9:
			m_inventorySim.m_smalls = GetDlgItemInt(poli_s9);
			m_inventorySim.m_bigs = GetDlgItemInt(poli_sBig9);
			break;
		}
		t.Format(_T("(%d, %d)"), m_inventorySim.m_smalls, m_inventorySim.m_bigs);
		m_listctrl.InsertItem(0, t);
		t.Format(_T("%.2f"), 
			avg_ordering_cost+
			avg_holding_cost +avg_shortage_cost);
		m_listctrl.SetItemText(0,1,t);
		t.Format(_T("%.2f"), avg_ordering_cost);
		m_listctrl.SetItemText(0,2,t);
		t.Format(_T("%.2f"), avg_holding_cost);
		m_listctrl.SetItemText(0,3,t);
		t.Format(_T("%.2f"), avg_shortage_cost);
		m_listctrl.SetItemText(0,4,t);
	}

}
