#pragma once


class MyQueue
{
private:
	HANDLE rankMutex;	                        //������
	void *p_rank[100];                       //��������ж�
	int p_start;                               //�ж���ʼλ��ָ��
	int p_end;                                    //�жӽ�βָ��
	int ranksize;                                 //�ж�����
public:
	MyQueue(void);
	~MyQueue(void);
	void rankinitial();
	bool pump(void *p);
	void destroyrank();
	void* pop();  
};
