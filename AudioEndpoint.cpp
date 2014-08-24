// ----------------------------------------------------------------------------
// AudioEndpoint.cpp
// Список устройтсв аудио-воспроизведения
// @author EreTIk
// ----------------------------------------------------------------------------


#include "StdAfx.h"
#pragma hdrstop

// build-in property key: device description string
#undef DEFINE_PROPERTYKEY
#define DEFINE_PROPERTYKEY(name, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8, pid) EXTERN_C const PROPERTYKEY name = { { l, w1, w2, { b1, b2,  b3,  b4,  b5,  b6,  b7,  b8 } }, pid }
DEFINE_PROPERTYKEY(PKEY_Device_DeviceDesc,  0xa45c254e, 0xdf1c, 0x4efd, 0x80, 0x20, 0x67, 0xd1, 0x46, 0xa8, 0x50, 0xe0, 2);     // DEVPROP_TYPE_STRING


// ----------------------------------------------------------------------------
HRESULT TAudioEndpointArray::RefreshList()
{
    m_arrEndpoints.RemoveAll();

    CComPtr<IMMDeviceCollection> AudioEndpoints;
    HRESULT hResult = m_MmDevEnumerator.EnumAudioEndpoints(eRender, DEVICE_STATE_ACTIVE, &AudioEndpoints);
    ATLASSERT(SUCCEEDED(hResult));
    if (S_OK != hResult)
        return hResult;

    UINT nCount = 0;
    hResult = AudioEndpoints->GetCount(&nCount);
    ATLASSERT(SUCCEEDED(hResult));
    if (S_OK != hResult)
        return hResult;

    // перечисялем полученные устройтсва
    for (UINT i = 0; i < nCount; ++i)
    {
        CComPtr<IMMDevice> ifMmDevice;
        CComPtr<IPropertyStore> ifDevProperty;

        hResult = AudioEndpoints->Item(i, &ifMmDevice);
        ATLASSERT(SUCCEEDED(hResult));
        if (S_OK != hResult)
            return hResult;

        // получаем идентификатор устройства
        TCoAllocatedStr<WCHAR> DeviceId;

        hResult = ifMmDevice->GetId(DeviceId);
        ATLASSERT(SUCCEEDED(hResult));
        if (S_OK != hResult)
            return hResult;

        // получаем имя устройтсва
        hResult = ifMmDevice->OpenPropertyStore(STGM_READ, &ifDevProperty);
        ATLASSERT(SUCCEEDED(hResult));
        if (S_OK != hResult)
            return hResult;

        TPropvarString DeviceName;
        hResult = ifDevProperty->GetValue(PKEY_Device_DeviceDesc, DeviceName);
        ATLASSERT(SUCCEEDED(hResult));
        if (S_OK != hResult)
            return hResult;
        m_arrEndpoints.Add(TAudioEndpoint(DeviceId, PROPVARIANT(DeviceName).pwszVal));
    }

    ATLVERIFY(QueryAndMarkAsDefault(eConsole));
    ATLVERIFY(QueryAndMarkAsDefault(eMultimedia));
    ATLVERIFY(QueryAndMarkAsDefault(eCommunications));

    return hResult;
}

// ----------------------------------------------------------------------------
bool TAudioEndpointArray::QueryAndMarkAsDefault(
    __in ERole eRole
)
{
    CComPtr<IMMDevice> ifDefaultDevice;
    HRESULT hResult = m_MmDevEnumerator.GetDefaultAudioEndpoint(eRender, eRole, &ifDefaultDevice);
    ATLASSERT(SUCCEEDED(hResult));
    if (S_OK != hResult)
        return false;

    CComPtr<IPropertyStore> ifDevProperty;

    // получаем идентификатор устройства
    TCoAllocatedStr<WCHAR> DeviceId;

    hResult = ifDefaultDevice->GetId(DeviceId);
    ATLASSERT(SUCCEEDED(hResult));
    if (S_OK != hResult)
        return false;

    // помечаем устройтсво по-умолчанию
    return MarkAsDefault(DeviceId, eRole);
}

// ----------------------------------------------------------------------------
bool TAudioEndpointArray::MarkAsDefault(
    __in PCWSTR wszDeviceId,
    __in ERole eRole
)
{
    const SIZE_T nCount = m_arrEndpoints.GetCount();
    bool bFounded = false;
    for (SIZE_T i = 0; i < nCount; ++i)
    {
        TAudioEndpoint &AudioEndpoint = m_arrEndpoints[i];
        if (!AudioEndpoint.m_strDeviceId.CompareNoCase(wszDeviceId))
        {
            AudioEndpoint.m_arrIsDefault[eRole] = true;
            bFounded = true;
        }
        else
        {
            AudioEndpoint.m_arrIsDefault[eRole] = false;
        }
    }
    return bFounded;
}
