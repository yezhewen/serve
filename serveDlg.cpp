
// serveDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "serve.h"
#include "serveDlg.h"
#include "afxdialogex.h"

//CRITICAL_SECTION cs;//全局临界区结构
#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CserveDlg 对话框


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


// CserveDlg 消息处理程序

BOOL CserveDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标
	
									
	// TODO: 在此添加额外的初始化代码char hostname[20];
	


	//socket库启动	
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
	m_port.SetWindowTextW(_T("端口：8888"));
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CserveDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CserveDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

//开始监听连接 函数
int CserveDlg::startlisten()
{
	int ret;
	m_listen = socket(AF_INET, SOCK_STREAM, 0); 

		if (m_listen == INVALID_SOCKET)
		{

			MessageBox(_T("创建监听套接字失败"));
			return -1;
         }

		m_lis_addr.sin_addr = gethostip();
		m_lis_addr.sin_family = AF_INET;
		m_lis_addr.sin_port = htons(8888);

	//绑定服务器地址
	ret = bind(m_listen, (sockaddr*)&m_lis_addr, sizeof(m_lis_addr));
	if (ret != 0)
	{
		MessageBox(_T("绑定地址失败"));
		return -1;
	}
	//开始监听
	
	ret = listen(m_listen, 5);//最大连接数量
	if (ret != 0)
	{
		MessageBox(_T("开启监听失败"));
		return -1;
	}
	//MessageBox(_T("开启监听"));

	//fd_set结构初始化
	FD_ZERO(&m_read);
	FD_SET(m_listen, &m_read);
}
	

DWORD WINAPI CserveDlg::serve_accept(LPVOID para)
{
	
	CserveDlg *p = (CserveDlg*)para;
	int ret ;
	int nstart, nend;
	CString str;
	char buffer[100];//发送接收缓冲区
	while (1)
	{
		

		//= WSAGetLastError();
	/*	CString str;
		str.Format(_T("%d-%d"), ret, ret);
		pdlg->MessageBox(_T(" ") + str);
*/

        ret = select(0, &(p->m_read), NULL, NULL, NULL); //一直等待直到收到消息或者连接请求
		int *buf = (int*)buffer;
		if (ret <= 0)
		{
			return ret;
		}
		if (FD_ISSET(p->m_listen, &(p->m_read)))//收到的是连接请求
		{
			//sockaddr_in temp_addr;
			int len = sizeof(sockaddr_in);
			p->m_player[p->m_valid] = accept(p->m_listen, (sockaddr*)&p->m_pla_addr[p->m_valid], &len);
			if (p->m_player[p->m_valid] != INVALID_SOCKET)
			{
				p->m_queue.push(p->m_valid);//进入匹配队列
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
                    //玩家主动退出或者接收失败
				    if (ret <= 0)
					{
						p->m_player[i] = INVALID_SOCKET;
						buf[0] = 4;
						ret = send(p->m_player[p->m_match[i]], buffer, sizeof(buffer),0);
						if (ret <= 0)
						{
							//如果给对手发送消息失败，说明对手也离开了游戏
							p->m_player[p->m_match[i]] = INVALID_SOCKET;
						}
						else//不然重新将其对手加入匹配队列
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

		if (p->m_queue.size() > 1)//匹配队列大于一个人，则出队两个人开始游戏
		{
			p->creategame();
		}
		//更新 FD_SET结构
		FD_ZERO(&(p->m_read));
		FD_SET(p->m_listen, &(p->m_read));
		p->m_total = 0;
		p->m_valid =-1;
		for (int i = 0; i < p->m_last; i++)
		{
			if (p->m_player[i] == INVALID_SOCKET)
			{
				if (p->m_valid <0)//标记第一个不可用的套接字坐标来存储新的玩家
					p->m_valid = i;
				while (p->m_player[p->m_last - 1]== INVALID_SOCKET)//将最后一个socket提到当前失效的socket位置
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

		str.Format(_T("在线玩家:%d"), p->m_total);
    	p->total_player.SetWindowTextW(str);
	
	}

	
}


int CserveDlg::creategame()//创建一个匹配游戏
{
	//MessageBox(_T("创建一个房间"));
	int a = m_queue.front();
	m_queue.pop();
	int b = m_queue.front();
	m_queue.pop();
	//ZeroMemory(sendbuf, sizeof(sendbuf));
	/*CString str;
	str.Format(_T("%d %d"), a, b);
	MessageBox(str);*/
	//如果两人中有人是已退出的 重加匹配队列
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

	if (ret <= 0 || ret1 <= 0)//如果有发送失败，就重新加入队列匹配
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
		MessageBox(_T("开启监听失败"));
	}
	else
	{
		CreateThread(0, NULL, serve_accept, this, NULL, NULL);
	}
	button_start.EnableWindow(false);
	button_start.SetWindowTextW(_T("服务器已开启"));
	// TODO: 在此添加控件通知处理程序代码
}

//获取本机IP

//in_addr CserveDlg::gethostip()//获取本机ip地址函数
//{
//	char hostname[20];
//	hostent *hostmsg;
//	gethostname(hostname, sizeof(hostname));
//	hostmsg = gethostbyname(hostname);
//	return (*(in_addr*)(hostmsg->h_addr_list[0]));
//}

void CserveDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	for (int i = 0; i < m_last; i++)
	{
		if (m_player[i] != INVALID_SOCKET)
			closesocket(m_player[1]);
	}
	closesocket(m_listen);
	WSACleanup();
	CDialogEx::OnOK();
}
