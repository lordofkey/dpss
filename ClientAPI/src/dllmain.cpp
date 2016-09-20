// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "stdafx.h"
#include "socket-lib/Socket.hpp"


typedef int ssize_t;


#define  PORT 8145
#define  IP_ADDRESS "172.1.10.134"

MyQueue que;
typedef void (*PF)(char* s,int len, void* param);

void process(MyImg* pimg, Socket::TCP server)
{
	vector<uchar> buff;//buffer for coding
	char token[] = "1234567890";
	int anthresult;
	try
	{
		server.send<char>(token, strlen(token));
		server.receive<int>(&anthresult, 1);
		if(anthresult == 0)
			return;
	}
	catch (Socket::SocketException &e)
	{
		cout << e << endl;
		return;
	}
//	cv::imencode(".tiff", pimg->img, buff);
	int len[3];
	len[0] = strlen(pimg->m_name);
//	len[1] = buff.size();
	len[1] = pimg->img.rows;
	len[2] = pimg->img.cols;

	try
	{
		server.send<int>(len, 3);
		server.send<char>(pimg->m_name, strlen(pimg->m_name));
		server.send_1dbuffer(pimg->img.data, len[1]*len[2]);
		server.receive<char>(pimg->result, 128);
	}
	catch (Socket::SocketException &e)
	{
		cout << e << endl;
	}
}




DWORD WINAPI SendFun(LPVOID pM)  
{
	char *pre;
	MyImg *pimg;
	Socket::TCP server;
	while (true)
	{
		pimg = (MyImg *)que.pop();
		if(pimg == NULL)
		{
			Sleep(200);
			continue;
		}
		if (!pimg->img.data)                              // Check for invalid input
		{
			cout << "Could not open or find the image" << std::endl;
			continue;
		}
		//TODO 图片检查
		try
		{
			server.connect_to(Socket::Address(IP_ADDRESS, PORT));
		}
		catch (Socket::SocketException &e)
		{
			cout << e << endl;
//TODO回掉处理函数
			server.close();
			continue;
		}
		process(pimg, server);
		pre = new char[128];
		strcpy(pre,pimg->result);
		PF proresul = NULL;
		proresul = (PF)(pimg->func);
		if(proresul != NULL)
			proresul(pre, 128, pimg->param);
		delete pimg;
		pimg = NULL;
		server.close();
	}

}  
DWORD WINAPI RevFun(LPVOID pM)  
{
	return 0;
} 


BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	HANDLE sendthread;
	HANDLE revthread;
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		Socket::TCP::initcri();
		for(int i = 0; i < 5; i++)
		{
			sendthread = CreateThread(NULL, 0, SendFun, NULL, 0, NULL);
//			revthread = CreateThread(NULL, 0, RevFun, NULL, 0, NULL);
		}
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}