#include "StdAfx.h"
#include "MyQueue.h"


MyQueue::MyQueue(void)
{
	rankinitial();
}


MyQueue::~MyQueue(void)
{
}

#pragma region �ж�ά��
//�жӳ�ʼ��
void MyQueue::rankinitial()
{
	rankMutex = CreateMutex(NULL,false,NULL);
	p_start = p_end = 0;
	ranksize = 100;
	for(int i = 0;i < 100;i++)
	{
		p_rank[i] = NULL;
	}
}

//��ջ
bool MyQueue::pump(void *p)
{
	WaitForSingleObject(rankMutex,INFINITE);
	if(((p_end+1)%ranksize) == p_start)
	{
		ReleaseMutex(rankMutex);
		return false;
	}
	p_rank[p_end] = p;
	//��������
	//
	//
	//
	p_end = (p_end+1)%ranksize;
	ReleaseMutex(rankMutex);
	return true;
}

//�����ж�
void MyQueue::destroyrank()  
{
	p_start = p_end = 0;
	for(int i = 0;i < 100;i++)
	{
		if(p_rank[i] != NULL)
		{
			delete p_rank[i];
			p_rank[i] = NULL;
		}
	}
}
/*��ջ*/  
void *MyQueue::pop()  
{  
	WaitForSingleObject(rankMutex,INFINITE);
	void *output;  
	if(p_start == p_end)
	{
		ReleaseMutex(rankMutex);
		return NULL;
	}
	output = p_rank[p_start];
	/////////////////////////////////////��ջʱһ��Ҫ��ָ���NULL
	p_rank[p_start] = NULL;
	p_start = (p_start + 1)%ranksize;
	ReleaseMutex(rankMutex);
	return output;
}  
#pragma endregion