
// serveDlg.h : 头文件
//
#include<queue>
#include<WinSock2.h>
#include "afxcmn.h"
#include "afxwin.h"
#pragma comment(lib,"ws2_32.lib")

#pragma once

//允许连接的玩家最大数量
#define online_max 20


// CserveDlg 对话框
class CserveDlg : public CDialogEx
{
// 构造
public:
	CserveDlg(CWnd* pParent = NULL);	// 标准构造函数
	in_addr gethostip() {
		char hostname[20];
		hostent *hostmsg;
		gethostname(hostname, sizeof(hostname));
		hostmsg = gethostbyname(hostname);
		return (*(in_addr*)(hostmsg->h_addr_list[0]));
	}//获取本机ip地址函数

	int startlisten();//开始监听
	//int serve_accept();//接收连接
	int creategame();//创建一个游戏房间
	//static DWORD WINAPI CserveDlg::recvmsg(LPVOID);//转发消息
	static DWORD WINAPI CserveDlg::serve_accept(LPVOID); 

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SERVE_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	

	//网络连接
	SOCKET m_listen;
	sockaddr_in m_lis_addr;
	SOCKET m_player[online_max];//连接玩家socket
	sockaddr_in m_pla_addr[online_max];//玩家ip端口信息
	int m_valid;//下一个可存储玩家信息的数组位置
	int m_last;//已存储玩家信息的数组的最后一个位置
	int m_total;//在线玩家总数
	char sendbuf[100];
	char recvbuf[100];
	std::queue<int> m_queue;//匹配队列
	int  m_match[online_max];//记录各个玩家的对手
	

	//FD_SET m_read;
	fd_set m_read;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonStart();
	CIPAddressCtrl m_ip;
	CEdit m_port;
	CEdit m_show;
	CEdit total_player;
	afx_msg void OnBnClickedOk();
	CButton button_start;
};
