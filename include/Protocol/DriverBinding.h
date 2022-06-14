#pragma once

typedef struct _EFI_DRIVER_BINDING_PROTOCOL EFI_DRIVER_BINDING_PROTOCOL;

typedef VOID (EFIAPI *EFI_DRIVER_BINDING_SUPPORTED)();
typedef VOID (EFIAPI *EFI_DRIVER_BINDING_START)();
typedef VOID (EFIAPI *EFI_DRIVER_BINDING_STOP)();
/// 
/// This protocol provides the services required to determine if a driver
/// supports a given controller. If a controller is supported, then it
/// also provides routines to start and stop the controller.
///
struct _EFI_DRIVER_BINDING_PROTOCOL {
	EFI_DRIVER_BINDING_SUPPORTED Supported;
	EFI_DRIVER_BINDING_START Start;
	EFI_DRIVER_BINDING_STOP Stop;

	///
	/// The version number of the UEFI driver that produced the
	/// EFI_DRIVER_BINDING_PROTOCOL. This field is used by
	/// the EFI boot service ConnectController() to determine
	/// the order that driver's Supported() service will be used when
	/// a controller needs to be started. EFI Driver Binding Protocol
	/// instances with higher Version values will be used before ones
	/// with lower Version values. The Version values of 0x0-
	/// 0x0f and 0xfffffff0-0xffffffff are reserved for
	/// platform/OEM specific drivers. The Version values of 0x10-
	/// 0xffffffef are reserved for IHV-developed drivers.
	///
	UINT32 Version;

	///
	/// The image handle of the UEFI driver that produced this instance
	/// of the EFI_DRIVER_BINDING_PROTOCOL.
	///
	EFI_HANDLE ImageHandle;

	///
	/// The handle on which this instance of the
	/// EFI_DRIVER_BINDING_PROTOCOL is installed. In most
	/// cases, this is the same handle as ImageHandle. However, for
	/// UEFI drivers that produce more than one instance of the
	/// EFI_DRIVER_BINDING_PROTOCOL, this value may not be
	/// the same as ImageHandle.
	///
	EFI_HANDLE DriverBindingHandle;
};