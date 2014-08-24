// ----------------------------------------------------------------------------
// SndDvSel.h
// �������� ������������ ���� �������
// @author EreTIk
// ----------------------------------------------------------------------------


#pragma once


// Windowns SDK
#include <Windows.h>
#include <DeviceTopology.h>
#include <MmDeviceApi.h>
// #include <FunctionDiscoveryKeys_devpkey.h>

// ATL
#include <AtlBase.h>
#include <AtlStr.h>
#include <AtlColl.h>

// Default Sound Render Device
#include "PolicyConfig.h"
#define APPL_TITLE  L"Select Default Sound Render Device"
#include "Helpers.h"
#include "AudioEndpoint.h"
#include "DefSoundTray.h"

// ----------------------------------------------------------------------------
// ����������� ��������� �� ������ � message box'�
// ----------------------------------------------------------------------------
ULONG ShowFormatError(
    __in PCWSTR wszAction,
    __in ULONG nErrorCode = GetLastError(),
    __in_opt HWND hParentWindow = NULL,
    __in_opt PCWSTR wszSuffix = NULL
);

// ----------------------------------------------------------------------------
// ����������� ��������� ������������� ���������
// ----------------------------------------------------------------------------
ULONG Usage(HWND hParent = NULL);
