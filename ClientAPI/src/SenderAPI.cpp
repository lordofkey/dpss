// SenderAPI.cpp : ���� DLL Ӧ�ó���ĵ���������
//

#include "stdafx.h"
#include "SenderAPI.h"
extern MyQueue que;
extern PF proresul;
// ���ǵ���������һ��ʾ��
SENDERAPI_API int nSenderAPI=0;

// ���ǵ���������һ��ʾ����
SENDERAPI_API int fnSenderAPI(void)
{
	return 42;
}

// �����ѵ�����Ĺ��캯����
// �й��ඨ�����Ϣ������� SenderAPI.h
CSenderAPI::CSenderAPI()
{
	return;
}
SENDERAPI_API bool SendImg(cv::Mat imgin, const char name[],void *param)
{
	MyImg *aa;
	aa = new MyImg();
	aa->index = 0;
	aa->param = param;
	aa->img = imgin.clone();
	strcpy(aa->m_name,name);
	if(!que.pump(aa))
	{
		delete aa;
		return false;
	}
	return true;
}
SENDERAPI_API void SetCallBack(void *p)
{
	proresul = (PF)p;
}