#include <ntifs.h>
#include "PriorityBoosterCommon.h"
#include <ntddk.h>




//#define CTL_CODE(DeviceType, Function, Method, Access) ( \
//	((DeviceType) << 16) | ((Access) << 14) | ((Function) << 2) | (Method))
//



//#define IOCTL_PRIORITY_BOOSTER_SET_PRIORITY  CTL_CODE(PRIORITY_BOOSTER_DEVICE, \
//	0X800, METHOD_NEITHER, FILE_ANY_ACCESS)



VOID PriorityBoosterUnload(_In_  PDRIVER_OBJECT pDriverObject);
NTSTATUS PrioitityBoosterDriverInit( _In_ PDRIVER_OBJECT DriverObject, _In_ PUNICODE_STRING RegistryPath);
NTSTATUS PriorityBoosterCreateClose( _In_ struct _DEVICE_OBJECT* DeviceObject, _Inout_ struct _IRP* Irp);

NTSTATUS PriorityBoosterDeviceControl(_In_ struct _DEVICE_OBJECT* DeviceObject, _Inout_ struct _IRP* Irp);

// main 
NTSTATUS DriverEntry(
	IN PDRIVER_OBJECT     DriverObject,
	IN PUNICODE_STRING    RegistryPath)
{

	DbgPrint("[%s][%d] !!!\n", __FUNCTION__, __LINE__);
	PrioitityBoosterDriverInit(DriverObject, RegistryPath);
	//DriverObject->DriverInit = PrioitityBoosterDriverInit;
	DriverObject->DriverUnload = PriorityBoosterUnload;





	DriverObject->MajorFunction[IRP_MJ_CREATE] = PriorityBoosterCreateClose;
	DriverObject->MajorFunction[IRP_MJ_CLOSE] = PriorityBoosterCreateClose;
	DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = PriorityBoosterDeviceControl;

	//DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = PriorityBoosterDeviceControl;
	return STATUS_SUCCESS;
}


NTSTATUS PrioitityBoosterDriverInit(_In_   PDRIVER_OBJECT  DriverObject, _In_ PUNICODE_STRING RegistryPath)
{
	DbgPrint("[%s][%d] !!!\n", __FUNCTION__, __LINE__);
	UNICODE_STRING devName = RTL_CONSTANT_STRING(L"\\Device\\PriorityBooster");
	PDEVICE_OBJECT pDeviceObject;
	NTSTATUS status = IoCreateDevice(DriverObject, 0, &devName, FILE_DEVICE_UNKNOWN, 0, FALSE, &pDeviceObject);
	if (!NT_SUCCESS(status))
	{
		DbgPrint("[%s][%d] !!!\n", __FUNCTION__, __LINE__);
		DbgPrint(("Failed to create device object (0X%08X)\n"), status);
		return status;
	}

	DbgPrint("[%s][%d] !!!\n", __FUNCTION__, __LINE__);
	UNICODE_STRING symLink = RTL_CONSTANT_STRING(L"\\??\\PriorityBooster");
	status = IoCreateSymbolicLink(&symLink, &devName);
	if (!NT_SUCCESS(status))
	{
		DbgPrint("[%s][%d] !!!\n", __FUNCTION__, __LINE__);
		DbgPrint("Failed to create symbolic link (0X%08X)\n", status);
		IoDeleteDevice(pDeviceObject);
	}
	DbgPrint("[%s][%d] !!!\n", __FUNCTION__, __LINE__);
			return 0;
}
VOID PriorityBoosterUnload(_In_  PDRIVER_OBJECT pDriverObject)
{
	DbgPrint("[%s][%d] !!!\n", __FUNCTION__, __LINE__);
	UNICODE_STRING symLink = RTL_CONSTANT_STRING(L"\\??\\PriorityBooster");
	// delete symbolic link
	IoDeleteSymbolicLink(&symLink);

	// delete device object
	IoDeleteDevice(pDriverObject->DeviceObject);

}

_Use_decl_annotations_
NTSTATUS PriorityBoosterCreateClose(_In_ struct _DEVICE_OBJECT* DeviceObject, _Inout_ struct _IRP* Irp)
{
	UNREFERENCED_PARAMETER(DeviceObject);
	DbgPrint("[%s][%d] !!!\n", __FUNCTION__, __LINE__);

	Irp->IoStatus.Status = STATUS_SUCCESS;
	Irp->IoStatus.Information = 0;
	IoCompleteRequest(Irp, IO_NO_INCREMENT);


	return STATUS_SUCCESS;
}

_Use_decl_annotations_
NTSTATUS PriorityBoosterDeviceControl(_In_ struct _DEVICE_OBJECT* DeviceObject, _Inout_ struct _IRP* Irp)
{
	// get our IO_STACK_LOCATION
	DbgPrint("[%s][%d] !!!\n", __FUNCTION__, __LINE__);
	PIO_STACK_LOCATION stack = IoGetCurrentIrpStackLocation(Irp);

	ULONG status =  STATUS_SUCCESS;


	switch (stack->Parameters.DeviceIoControl.IoControlCode)
	{
	case IOCTL_PRIORITY_BOOSTER_SET_PRIORITY : // 用户模式定义的枚举
	{
		// do the work

		DbgPrint("[%s][%d] !!!\n", __FUNCTION__, __LINE__);
		int len = stack->Parameters.DeviceIoControl.InputBufferLength;
		if (len < sizeof(struct ThreadData))
		{
			DbgPrint("[%s][%d] !!!\n", __FUNCTION__, __LINE__);
			status = STATUS_BUFFER_TOO_SMALL;
			break;
		}
		struct ThreadData* data = (stack->Parameters.DeviceIoControl.Type3InputBuffer);
		if (!data)
		{
			DbgPrint("[%s][%d] !!!\n", __FUNCTION__, __LINE__);
			status = STATUS_INVALID_PARAMETER;
			break;
		}
		if (data->Priority < 1 || data->Priority > 31)
		{
			status = STATUS_INVALID_PARAMETER;
			break;
		}
		DbgPrint("[%s][%d] !!!\n", __FUNCTION__, __LINE__);
		PETHREAD Thread;
		status = PsLookupThreadByThreadId(ULongToHandle(data->ThreadId), &Thread);
		if (!NT_SUCCESS(status))
		{
			DbgPrint("[%s][%d] !!!\n", __FUNCTION__, __LINE__);
			// warr -->
			break;
		}
		DbgPrint("[%s][%d] !!!\n", __FUNCTION__, __LINE__);
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
	DbgPrint("[%s][%d] !!!\n", __FUNCTION__, __LINE__);
	Irp->IoStatus.Status = status;
	Irp->IoStatus.Information = 0;
	IoCompleteRequest(Irp, IO_NO_INCREMENT);

	return status;
}
