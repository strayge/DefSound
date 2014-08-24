// ----------------------------------------------------------------------------
// AudioEndpoint.h
// Список устройтсв аудио-воспроизведения
// @author EreTIk
// ----------------------------------------------------------------------------


#pragma once

// устройство аудио-воспроизведения
struct TAudioEndpoint {
    CString m_strDeviceId;
    CString m_strDeviceName;

    bool m_arrIsDefault[ERole_enum_count];

    TAudioEndpoint(
        PCWSTR wszDeviceId = NULL,
        PCWSTR wszDeviceName = NULL
    ) : m_strDeviceId(wszDeviceId)
      , m_strDeviceName(wszDeviceName)
    {
        for (ULONG i = 0; i < _countof(m_arrIsDefault); ++i)
            m_arrIsDefault[i] = false;
    }

    // проверка того, что утсроство является "по-умолчанию" во всех ролях
    bool IsDefaultAllRoles() const
    {
        for (ULONG i = 0; i < _countof(m_arrIsDefault); ++i)
            if (!m_arrIsDefault[i])
                return false;

        return true;
    }
};

// массив устройств аудио-воспроизведения
struct TAudioEndpointArray {
    TAudioEndpointArray(
        IMMDeviceEnumerator &MmDevEnumerator
    ) : m_MmDevEnumerator(MmDevEnumerator)
    {
    }

    // ----------------------------------------------------------------------------
    // Обновление списка устройтсв воспроизведения
    // ----------------------------------------------------------------------------
    HRESULT RefreshList();

    // ----------------------------------------------------------------------------
    // Получение количества устройтсв воспроизведения
    // ----------------------------------------------------------------------------
    SIZE_T GetCount()
    {
        return m_arrEndpoints.GetCount();
    }

    // ----------------------------------------------------------------------------
    // Получение устройтсва воспроизведения по его индексу в массиве
    // ----------------------------------------------------------------------------
    const TAudioEndpoint & operator[] (__in SIZE_T nIndex) const
    {
        return m_arrEndpoints[nIndex];
    }
    TAudioEndpoint & operator[] (__in SIZE_T nIndex)
    {
        return m_arrEndpoints[nIndex];
    }

private:

    // ----------------------------------------------------------------------------
    // Получение устройства по-умолчанию для указанной роли и пометка его
    // ----------------------------------------------------------------------------
    bool QueryAndMarkAsDefault(__in ERole eRole);

    // ----------------------------------------------------------------------------
    // Пометить устройтво как используемое по-умолчанию для указанной роли по его 
    // символическому идентификтору
    // ----------------------------------------------------------------------------
    bool MarkAsDefault(__in PCWSTR wszDeviceId, __in ERole eRole);

    IMMDeviceEnumerator &m_MmDevEnumerator;
    CAtlArray<TAudioEndpoint> m_arrEndpoints;
};
