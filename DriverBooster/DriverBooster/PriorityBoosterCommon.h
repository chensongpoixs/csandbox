#ifndef _PRIORITY_BOOSTER_COMMON_H_
#define _PRIORITY_BOOSTER_COMMON_H_
//#include <ntifs.h>

// �߳̽ṹ��

struct ThreadData
{
	unsigned long ThreadId;  //�߳�id 32 �޷���
	int Priority;

};
#define PRIORITY_BOOSTER_DEVICE  0X8000


#define CTL_CODE( DeviceType, Function, Method, Access ) (                 \
    ((DeviceType) << 16) | ((Access) << 14) | ((Function) << 2) | (Method) \
)

#define IOCTL_PRIORITY_BOOSTER_SET_PRIORITY  CTL_CODE(PRIORITY_BOOSTER_DEVICE, \
	0X800, METHOD_NEITHER, FILE_ANY_ACCESS)

#endif // _PRIORITY_BOOSTER_COMMON_H_
