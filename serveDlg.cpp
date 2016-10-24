
// serveDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "serve.h"
#include "serveDlg.h"
#include "afxdialogex.h"

//CRITICAL_SECTION cs;//ȫ���ٽ����ṹ
#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CserveDlg �Ի���


CserveDlg::CserveDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_SERVE_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	m_last = 0;
	m_valid = 0;
	m_total = 0;
}

void CserveDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_IPADDRESS1, m_ip);
	DDX_Control(pDX, IDC_EDIT1, m_port);
	DDX_Control(pDX, IDC_msgshow, m_show);
	DDX_Control(pDX, IDC_EDIT2, total_player);
	DDX_Control(pDX, IDC_BUTTON_START, button_start);
}

BEGIN_MESSAGE_MAP(CserveDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_START, &CserveDlg::OnBnClickedButtonStart)
	ON_BN_CLICKED(IDOK, &CserveDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CserveDlg ��Ϣ�������

BOOL CserveDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ���ô˶Ի����ͼ�ꡣ  ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��
	
									
	// TODO: �ڴ���Ӷ���ĳ�ʼ������char hostname[20];
	


	//socket������	
	//u_long nonblock = 1;
	//ioctlsocket(m_listen, FIONBIO, &nonblock);
	WSADATA wsadata;
	WSAStartup(MAKEWORD(2, 2), &wsadata);

	char hostname[20];
	hostent *hostmsg;
	gethostname(hostname, sizeof(hostname));
	hostmsg = gethostbyname(hostname);
	m_lis_addr.sin_addr = *((in_addr*)(hostmsg->h_addr_list[0]));
	m_lis_addr.sin_family = AF_INET;
	m_lis_addr.sin_port = htons(8888);

	m_ip.SetAddress(hostmsg->h_addr_list[0][0],
		hostmsg->h_addr_list[0][1],
		hostmsg->h_addr_list[0][2],
		hostmsg->h_addr_list[0][3]);
	m_port.SetWindowTextW(_T("�˿ڣ�8888"));
	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ  ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CserveDlg::OnPaint()
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
HCURSOR CserveDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

//��ʼ�������� ����
int CserveDlg::startlisten()
{
	int ret;
	m_listen = socket(AF_INET, SOCK_STREAM, 0); 

		if (m_listen == INVALID_SOCKET)
		{

			MessageBox(_T("���������׽���ʧ��"));
			return -1;
         }

		m_lis_addr.sin_addr = gethostip();
		m_lis_addr.sin_family = AF_INET;
		m_lis_addr.sin_port = htons(8888);

	//�󶨷�������ַ
	ret = bind(m_listen, (sockaddr*)&m_lis_addr, sizeof(m_lis_addr));
	if (ret != 0)
	{
		MessageBox(_T("�󶨵�ַʧ��"));
		return -1;
	}
	//��ʼ����
	
	ret = listen(m_listen, 5);//�����������
	if (ret != 0)
	{
		MessageBox(_T("��������ʧ��"));
		return -1;
	}
	//MessageBox(_T("��������"));

	//fd_set�ṹ��ʼ��
	FD_ZERO(&m_read);
	FD_SET(m_listen, &m_read);
}
	

DWORD WINAPI CserveDlg::serve_accept(LPVOID para)
{
	
	CserveDlg *p = (CserveDlg*)para;
	int ret ;
	int nstart, nend;
	CString str;
	char buffer[100];//���ͽ��ջ�����
	while (1)
	{
		

		//= WSAGetLastError();
	/*	CString str;
		str.Format(_T("%d-%d"), ret, ret);
		pdlg->MessageBox(_T(" ") + str);
*/

        ret = select(0, &(p->m_read), NULL, NULL, NULL); //һֱ�ȴ�ֱ���յ���Ϣ������������
		int *buf = (int*)buffer;
		if (ret <= 0)
		{
			return ret;
		}
		if (FD_ISSET(p->m_listen, &(p->m_read)))//�յ�������������
		{
			//sockaddr_in temp_addr;
			int len = sizeof(sockaddr_in);
			p->m_player[p->m_valid] = accept(p->m_listen, (sockaddr*)&p->m_pla_addr[p->m_valid], &len);
			if (p->m_player[p->m_valid] != INVALID_SOCKET)
			{
				p->m_queue.push(p->m_valid);//����ƥ�����
				if(p->m_valid==p->m_last) (p->m_last)++;
			}
			
		}
		else
		{
			for (int i = 0; i <(p->m_last); i++)
			{
			    if (FD_ISSET(p->m_player[i], &(p->m_read)))
				{		
				    int ret = recv(p->m_player[i],buffer, sizeof(buffer), 0);
                    //��������˳����߽���ʧ��
				    if (ret <= 0)
					{
						p->m_player[i] = INVALID_SOCKET;
						buf[0] = 4;
						ret = send(p->m_player[p->m_match[i]], buffer, sizeof(buffer),0);
						if (ret <= 0)
						{
							//��������ַ�����Ϣʧ�ܣ�˵������Ҳ�뿪����Ϸ
							p->m_player[p->m_match[i]] = INVALID_SOCKET;
						}
						else//��Ȼ���½�����ּ���ƥ�����
							p->m_queue.push(p->m_match[i]);
					}
					else	
					{
						ret = send(p->m_player[(p->m_match[i])], buffer, sizeof(buffer), 0);
						if (ret <= 0)
						{
                         p->m_player[(p->m_match[i])] = INVALID_SOCKET;
						 buf[0] = 4;
						 ret = send(p->m_player[i], buffer, sizeof(buffer),0);
						 if (ret <= 0)
						 {
							 p->m_player[i] = INVALID_SOCKET;
						 }
						 else
							 p->m_queue.push(i);
						}	
					}
					break;
				}

			}
		}

		if (p->m_queue.size() > 1)//ƥ����д���һ���ˣ�����������˿�ʼ��Ϸ
		{
			p->creategame();
		}
		//���� FD_SET�ṹ
		FD_ZERO(&(p->m_read));
		FD_SET(p->m_listen, &(p->m_read));
		p->m_total = 0;
		p->m_valid =-1;
		for (int i = 0; i < p->m_last; i++)
		{
			if (p->m_player[i] == INVALID_SOCKET)
			{
				if (p->m_valid <0)//��ǵ�һ�������õ��׽����������洢�µ����
					p->m_valid = i;
				while (p->m_player[p->m_last - 1]== INVALID_SOCKET)//�����һ��socket�ᵽ��ǰʧЧ��socketλ��
				{
					(p->m_last)--;
				}
			}
			else
			{
				FD_SET(p->m_player[i], &(p->m_read));
				p->m_total++;

				//p->m_show.GetSel(nstart, nend);
				//p->m_total.SetSel(nend, nend);
				//p->m_total.ReplaceSel(str);
			}
		}
		if (p->m_valid<0) p->m_valid = p->m_last;

		str.Format(_T("�������:%d"), p->m_total);
    	p->total_player.SetWindowTextW(str);
	
	}

	
}


int CserveDlg::creategame()//����һ��ƥ����Ϸ
{
	//MessageBox(_T("����һ������"));
	int a = m_queue.front();
	m_queue.pop();
	int b = m_queue.front();
	m_queue.pop();
	//ZeroMemory(sendbuf, sizeof(sendbuf));
	/*CString str;
	str.Format(_T("%d %d"), a, b);
	MessageBox(str);*/
	//������������������˳��� �ؼ�ƥ�����
	if (m_player[a] == INVALID_SOCKET || m_player[b] == INVALID_SOCKET)
	{
		if (m_player[a] != INVALID_SOCKET) m_queue.push(a);
		else  if (m_player[b] != INVALID_SOCKET) m_queue.push(b);
		return -1;

	}

	int ret, ret1;
	int *buf = (int*)sendbuf;
	m_match[a] = b;
	m_match[b] = a;
	buf[0] = 0; buf[1] = 1;
    ret=send(m_player[a], sendbuf, sizeof(sendbuf), 0);
	buf[0]=0,buf[1] = 2;  
	ret1=send(m_player[b], sendbuf, sizeof(sendbuf), 0);

	if (ret <= 0 || ret1 <= 0)//����з���ʧ�ܣ������¼������ƥ��
	{
		if (ret > 0)
		{
			m_queue.push(a);
			m_player[b] = INVALID_SOCKET;
		}
		else 	if (ret1 > 0)
		{
			m_queue.push(b);
			m_player[a] = INVALID_SOCKET; 
		}
		else
		{
			m_player[a] = INVALID_SOCKET; 
			m_player[b] = INVALID_SOCKET;
		}
	}
	return 0;
}



void CserveDlg::OnBnClickedButtonStart()
{
	int ret = startlisten();
	if (ret == -1)
	{
		MessageBox(_T("��������ʧ��"));
	}
	else
	{
		CreateThread(0, NULL, serve_accept, this, NULL, NULL);
	}
	button_start.EnableWindow(false);
	button_start.SetWindowTextW(_T("�������ѿ���"));
	// TODO: �ڴ���ӿؼ�֪ͨ����������
}

//��ȡ����IP

//in_addr CserveDlg::gethostip()//��ȡ����ip��ַ����
//{
//	char hostname[20];
//	hostent *hostmsg;
//	gethostname(hostname, sizeof(hostname));
//	hostmsg = gethostbyname(hostname);
//	return (*(in_addr*)(hostmsg->h_addr_list[0]));
//}

void CserveDlg::OnBnClickedOk()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	for (int i = 0; i < m_last; i++)
	{
		if (m_player[i] != INVALID_SOCKET)
			closesocket(m_player[1]);
	}
	closesocket(m_listen);
	WSACleanup();
	CDialogEx::OnOK();
}
