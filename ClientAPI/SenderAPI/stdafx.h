// stdafx.h : ��׼ϵͳ�����ļ��İ����ļ���
// ���Ǿ���ʹ�õ��������ĵ�
// �ض�����Ŀ�İ����ļ�
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             //  �� Windows ͷ�ļ����ų�����ʹ�õ���Ϣ
// Windows ͷ�ļ�:
#define WINDOWS



#include <windows.h>
#include <opencv2\opencv.hpp>
#include "MyQueue.h"
#include "MyImg.h"
typedef void (*PF)(char* s,int len, void* param);

// TODO: �ڴ˴����ó�����Ҫ������ͷ�ļ�
