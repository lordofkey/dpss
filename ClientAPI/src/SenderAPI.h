// ���� ifdef ���Ǵ���ʹ�� DLL �������򵥵�
// ��ı�׼�������� DLL �е������ļ��������������϶���� SENDERAPI_EXPORTS
// ���ű���ġ���ʹ�ô� DLL ��
// �κ�������Ŀ�ϲ�Ӧ����˷��š�������Դ�ļ��а������ļ����κ�������Ŀ���Ὣ
// SENDERAPI_API ������Ϊ�Ǵ� DLL ����ģ����� DLL ���ô˺궨���
// ������Ϊ�Ǳ������ġ�
#ifdef SENDERAPI_EXPORTS
#define SENDERAPI_API __declspec(dllexport)
#else
#define SENDERAPI_API __declspec(dllimport)
#endif

// �����Ǵ� SenderAPI.dll ������
class SENDERAPI_API CSenderAPI {
public:
	CSenderAPI(void);
	// TODO: �ڴ�������ķ�����
};

extern SENDERAPI_API int nSenderAPI;

SENDERAPI_API int fnSenderAPI(void);
SENDERAPI_API bool SendImg(cv::Mat imgin,const char name[],void *param = NULL, void *p = NULL);
SENDERAPI_API void SetCallBack(void *p);
