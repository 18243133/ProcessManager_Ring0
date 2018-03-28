
// ProcessManagerDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "ProcessManager.h"
#include "ProcessManagerDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

static UINT indicators[] =
{
	IDR_STATUSBAR_STRING
};

// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���


HANDLE g_hDevice = NULL;
HANDLE
OpenDevice(LPCTSTR lpDevicePath)
{
	HANDLE hDevice = CreateFile(lpDevicePath,
		GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);

	if (hDevice == INVALID_HANDLE_VALUE)
	{
	}

	return hDevice;

}

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

	// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	// ʵ��
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


// CProcessManagerDlg �Ի���


CProcessManagerDlg::CProcessManagerDlg(CWnd* pParent /*=NULL*/)
: CDialogEx(CProcessManagerDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);


	m_ulProcessCount = 0;
	m_ProcessInfor = NULL;
}

void CProcessManagerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST, m_List);
}

BEGIN_MESSAGE_MAP(CProcessManagerDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_SIZE()
END_MESSAGE_MAP()


// CProcessManagerDlg ��Ϣ�������

BOOL CProcessManagerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
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

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��


	
	

	CreatStatusBar();
	m_List.SetExtendedStyle(LVS_EX_FULLROWSELECT);
	m_List.InsertColumn(0, L"ID", LVCFMT_LEFT, 40);
	m_List.InsertColumn(1, L"����ID", LVCFMT_LEFT, 60);
	m_List.InsertColumn(2, L"����", LVCFMT_LEFT, 120);
	m_List.InsertColumn(3, L"·��", LVCFMT_LEFT, 300);

	CRect rect;
	GetWindowRect(&rect);
	rect.bottom += 20;
	MoveWindow(rect);

	//��ý��̸���
	if (GetProcessCount() == TRUE)
	{
		CString strStatusMsg;
		strStatusMsg.Format(L"��%d������", m_ulProcessCount);
		m_StatusBar.SetPaneText(0, strStatusMsg);   //��״̬������ʾ����
	}

	//��ý�����Ϣ
	if (GetProcessInfor() == TRUE)
	{
		//��ControlList����������
		SetDataToControlList();
	}

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

BOOL CProcessManagerDlg::GetProcessCount()
{
	DWORD dwReturnSize = 0;
	DWORD dwRet = 0;

	g_hDevice = OpenDevice(L"\\\\.\\ProcessManagerLink");

	if (g_hDevice == (HANDLE)-1)
	{
		AfxMessageBox(_T("������ʧ�ܣ��밲װ������������"));
		SendMessage(WM_CLOSE,0,0);
		return FALSE;
	}

	dwRet = DeviceIoControl(g_hDevice, CTL_PROCESS_COUNT,
		NULL,
		0,
		&m_ulProcessCount,
		sizeof(ULONG),
		&dwReturnSize,
		NULL);

	if (dwRet == 0)
	{
		CloseHandle(g_hDevice);
		return FALSE;
	}
	CloseHandle(g_hDevice);
	return  TRUE;
}

BOOL CProcessManagerDlg::GetProcessInfor()
{
	DWORD dwReturnSize = 0;
	DWORD dwRet = 0;

	if (m_ProcessInfor != NULL)
	{
		delete m_ProcessInfor;
		m_ProcessInfor = NULL;
	}

	m_ProcessInfor = new PROCESS_INFOR[m_ulProcessCount];
	if (m_ProcessInfor == NULL)
	{
		return FALSE;
	}

	memset(m_ProcessInfor, 0, sizeof(PROCESS_INFOR)*m_ulProcessCount);
	g_hDevice = OpenDevice(L"\\\\.\\ProcessManagerLink");

	if (g_hDevice == (HANDLE)-1)
	{
		delete m_ProcessInfor;
		m_ProcessInfor = NULL;
		return FALSE;
	}

	dwRet = DeviceIoControl(g_hDevice, CTL_PROCESS_INFOR,
		NULL,
		0,
		m_ProcessInfor,
		sizeof(PROCESS_INFOR)*m_ulProcessCount,
		&dwReturnSize,
		NULL);

	if (dwRet == 0)
	{
		delete m_ProcessInfor;
		m_ProcessInfor = NULL;
		CloseHandle(g_hDevice);
		return FALSE;
	}

	CloseHandle(g_hDevice);
	return  TRUE;
}

VOID CProcessManagerDlg::SetDataToControlList()
{
	int i = 0;

	if (m_ProcessInfor == NULL)
	{
		return;
	}
	CString strId;
	for (i = 0; i < m_ulProcessCount; i++)
	{
		CString  strProcessID;

		strProcessID.Format(L"%d", m_ProcessInfor[i].ulProcessID);
		strId.Format(L"%d",i+1);
		
		int nNewItem = m_List.InsertItem(m_List.GetItemCount(),strId);
		
		m_List.SetItemText(nNewItem, 1, strProcessID);
		FixProcessImageName(m_ProcessInfor[i].wzImageName, m_ProcessInfor[i].wzImagePath);
		m_List.SetItemText(nNewItem, 2, m_ProcessInfor[i].wzImageName);
		m_List.SetItemText(nNewItem, 3, m_ProcessInfor[i].wzImagePath);
	}
}

VOID CProcessManagerDlg::FixProcessImageName(WCHAR* wzImageName, WCHAR* wzImagePath)
{
	WCHAR* wzTemp = NULL;
	if (wzImagePath != NULL)
	{
		wzTemp = wcsrchr(wzImagePath, '\\');
		if (wzTemp != NULL)
		{
			wzTemp++;
			wcscpy(wzImageName, wzTemp);
		}
	}
}

void CProcessManagerDlg::CreatStatusBar(void)
{
	if (!m_StatusBar.Create(this) ||
		!m_StatusBar.SetIndicators(indicators,
		sizeof(indicators) / sizeof(UINT)))                    //����״̬���������ַ���Դ��ID
	{
		return;
	}
	CRect rc;
	::GetWindowRect(m_StatusBar.m_hWnd, rc);
	m_StatusBar.MoveWindow(rc);                           //�ƶ�״̬����ָ��λ��
}

void CProcessManagerDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CProcessManagerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CProcessManagerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CProcessManagerDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	if (m_StatusBar.m_hWnd != NULL){    //���Ի����С�ı�ʱ ״̬����СҲ��֮�ı�
		CRect rc;
		rc.top = cy - 20;
		rc.left = 0;
		rc.right = cx;
		rc.bottom = cy;
		m_StatusBar.MoveWindow(rc);
		m_StatusBar.SetPaneInfo(0, m_StatusBar.GetItemID(0), SBPS_POPOUT, cx - 10);
	}
}