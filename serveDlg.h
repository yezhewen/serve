
// serveDlg.h : ͷ�ļ�
//
#include<queue>
#include<WinSock2.h>
#include "afxcmn.h"
#include "afxwin.h"
#pragma comment(lib,"ws2_32.lib")

#pragma once

//�������ӵ�����������
#define online_max 20


// CserveDlg �Ի���
class CserveDlg : public CDialogEx
{
// ����
public:
	CserveDlg(CWnd* pParent = NULL);	// ��׼���캯��
	in_addr gethostip() {
		char hostname[20];
		hostent *hostmsg;
		gethostname(hostname, sizeof(hostname));
		hostmsg = gethostbyname(hostname);
		return (*(in_addr*)(hostmsg->h_addr_list[0]));
	}//��ȡ����ip��ַ����

	int startlisten();//��ʼ����
	//int serve_accept();//��������
	int creategame();//����һ����Ϸ����
	//static DWORD WINAPI CserveDlg::recvmsg(LPVOID);//ת����Ϣ
	static DWORD WINAPI CserveDlg::serve_accept(LPVOID); 

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SERVE_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	

	//��������
	SOCKET m_listen;
	sockaddr_in m_lis_addr;
	SOCKET m_player[online_max];//�������socket
	sockaddr_in m_pla_addr[online_max];//���ip�˿���Ϣ
	int m_valid;//��һ���ɴ洢�����Ϣ������λ��
	int m_last;//�Ѵ洢�����Ϣ����������һ��λ��
	int m_total;//�����������
	char sendbuf[100];
	char recvbuf[100];
	std::queue<int> m_queue;//ƥ�����
	int  m_match[online_max];//��¼������ҵĶ���
	

	//FD_SET m_read;
	fd_set m_read;

	// ���ɵ���Ϣӳ�亯��
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
