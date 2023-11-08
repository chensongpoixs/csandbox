#include <ntifs.h>
#include <ntddk.h>




// 线程结构体

struct ThreadData
{
	ULONG ThreadId;  //线程id 32 无符号
	int Priority;

};
#define PRIORITY_BOOSTER_DEVICE  0X8000

//#define CTL_CODE(DeviceType, Function, Method, Access) ( \
//	((DeviceType) << 16) | ((Access) << 14) | ((Function) << 2) | (Method))
//



#define IOCTL_PRIORITY_BOOSTER_SET_PRIORITY  CTL_CODE(PRIORITY_BOOSTER_DEVICE, \
	0X800, METHOD_NEITHER, FILE_ANY_ACCESS)



VOID ProorityBoosterUnload(_In_  PDRIVER_OBJECT pDriverObject);
 
NTSTATUS PriorityBoosterCreateClose( _In_ struct _DEVICE_OBJECT* DeviceObject, _Inout_ struct _IRP* Irp);

NTSTATUS PriorityBoosterDeviceControl(_In_ struct _DEVICE_OBJECT* DeviceObject, _Inout_ struct _IRP* Irp);

// main 
NTSTATUS DriverEntry(
	IN PDRIVER_OBJECT     DriverObject,
	IN PUNICODE_STRING    RegistryPath)
{

	DbgPrint("ProorityBoosterUnload[%s][%d] !!!\n", __FUNCTION__, __LINE__);
	DriverObject->DriverUnload = ProorityBoosterUnload;


	DriverObject->MajorFunction[IRP_MJ_CREATE] = PriorityBoosterCreateClose;
	DriverObject->MajorFunction[IRP_MJ_CLOSE] = PriorityBoosterCreateClose;
	DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = PriorityBoosterDeviceControl;

	//DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = PriorityBoosterDeviceControl;
	return STATUS_SUCCESS;
}
VOID ProorityBoosterUnload(_In_  PDRIVER_OBJECT pDriverObject)
{
	UNICODE_STRING symLink = RTL_CONSTANT_STRING(L"\\?\\PriorityBooster");
	// delete symbolic link
	IoDeleteSymbolicLink(&symLink);

	// delete device object
	IoDeleteDevice(pDriverObject->DeviceObject);

}


NTSTATUS PriorityBoosterCreateClose(_In_ struct _DEVICE_OBJECT* DeviceObject, _Inout_ struct _IRP* Irp)
{
	UNREFERENCED_PARAMETER(DeviceObject);


	Irp->IoStatus.Status = STATUS_SUCCESS;
	Irp->IoStatus.Information = 0;
	IoCompleteRequest(Irp, IO_NO_INCREMENT);


	return STATUS_SUCCESS;
}


NTSTATUS PriorityBoosterDeviceControl(_In_ struct _DEVICE_OBJECT* DeviceObject, _Inout_ struct _IRP* Irp)
{
	// get our IO_STACK_LOCATION

	PIO_STACK_LOCATION stack = IoGetCurrentIrpStackLocation(Irp);

	ULONG status =  STATUS_SUCCESS;


	switch (stack->Parameters.DeviceIoControl.IoControlCode)
	{
	case IOCTL_PRIORITY_BOOSTER_SET_PRIORITY : // 用户模式定义的枚举
	{
		// do the work


		int len = stack->Parameters.DeviceIoControl.InputBufferLength;
		if (len < sizeof(struct ThreadData))
		{
			status = STATUS_BUFFER_TOO_SMALL;
			break;
		}
		struct ThreadData* data = (stack->Parameters.DeviceIoControl.Type3InputBuffer);
		if (!data)
		{
			status = STATUS_INVALID_PARAMETER;
			break;
		}
		if (data->Priority < 1 || data->Priority > 31)
		{
			status = STATUS_INVALID_PARAMETER;
			break;
		}

		PETHREAD Thread;
		status = PsLookupThreadByThreadId(ULongToHandle(data->Priority), &Thread);
		if (!NT_SUCCESS(status))
		{
			// warr -->
			break;
		}

		KeSetPriorityThread((PKTHREAD)Thread, data->Priority);
		ObDereferenceObject(Thread);
		DbgPrint("Thread  priority change for %d ot %d succeeded !\n", data->ThreadId, data->Priority);
		break;


		break;
	}
	default:
		status = STATUS_INVALID_DEVICE_REQUEST;
		break;
	}

	Irp->IoStatus.Status = status;
	Irp->IoStatus.Information = 0;
	IoCompleteRequest(Irp, IO_NO_INCREMENT);

	return status;
}
