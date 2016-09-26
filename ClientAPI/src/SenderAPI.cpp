// SenderAPI.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "SenderAPI.h"
extern MyQueue que;
// 这是导出变量的一个示例
SENDERAPI_API int nSenderAPI=0;

// 这是导出函数的一个示例。
SENDERAPI_API int fnSenderAPI(void)
{
	return 42;
}

// 这是已导出类的构造函数。
// 有关类定义的信息，请参阅 SenderAPI.h
CSenderAPI::CSenderAPI()
{
	return;
}
SENDERAPI_API bool SendImg(cv::Mat imgin, const char name[],void *param, void *p)
{
	MyImg *aa;
	aa = new MyImg();
	aa->index = 0;
	aa->param = param;
	aa->img = imgin.clone();
	aa->func = p;
	cv::cvtColor(aa->img, aa->img, CV_RGB2GRAY);
	strcpy_s(aa->m_name, 128, name);
	if(!que.pump(aa))
	{
		delete aa;
		return false;
	}
	return true;
}