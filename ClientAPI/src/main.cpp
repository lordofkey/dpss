#include <opencv2\opencv.hpp>
#include "SenderAPI.h"
#include <windows.h>


void receive(char* s,int len, void* param)
{
	LARGE_INTEGER endCount;
	LARGE_INTEGER freq;
	LARGE_INTEGER startCount = *(LARGE_INTEGER*)param;
	QueryPerformanceFrequency(&freq);
	QueryPerformanceCounter(&endCount);
	double elapsed =1000.0* (double)(endCount.QuadPart - startCount.QuadPart) / freq.QuadPart;
	printf("time: %f  ms \n",elapsed);
	printf("%s",s);
}

int main()
{
	int index = 0;
	cv::Mat image, imageIn;
	image = cv::imread("1.jpg");
	cv::resize(image, image, cv::Size(227, 227));
	int i = 0;
	while(true)
	{
		i ++;

		LARGE_INTEGER* startCount = new LARGE_INTEGER();

		QueryPerformanceCounter(startCount);

		if(SendImg(image, "yw_c",startCount,receive))
		{
		}
		Sleep(10);
	}
	
}