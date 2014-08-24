// ----------------------------------------------------------------------------
// AudioEndpoint.h
// ������ ��������� �����-���������������
// @author EreTIk
// ----------------------------------------------------------------------------


#pragma once

// ���������� �����-���������������
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

    // �������� ����, ��� ��������� �������� "��-���������" �� ���� �����
    bool IsDefaultAllRoles() const
    {
        for (ULONG i = 0; i < _countof(m_arrIsDefault); ++i)
            if (!m_arrIsDefault[i])
                return false;

        return true;
    }
};

// ������ ��������� �����-���������������
struct TAudioEndpointArray {
    TAudioEndpointArray(
        IMMDeviceEnumerator &MmDevEnumerator
    ) : m_MmDevEnumerator(MmDevEnumerator)
    {
    }

    // ----------------------------------------------------------------------------
    // ���������� ������ ��������� ���������������
    // ----------------------------------------------------------------------------
    HRESULT RefreshList();

    // ----------------------------------------------------------------------------
    // ��������� ���������� ��������� ���������������
    // ----------------------------------------------------------------------------
    SIZE_T GetCount()
    {
        return m_arrEndpoints.GetCount();
    }

    // ----------------------------------------------------------------------------
    // ��������� ���������� ��������������� �� ��� ������� � �������
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
    // ��������� ���������� ��-��������� ��� ��������� ���� � ������� ���
    // ----------------------------------------------------------------------------
    bool QueryAndMarkAsDefault(__in ERole eRole);

    // ----------------------------------------------------------------------------
    // �������� ��������� ��� ������������ ��-��������� ��� ��������� ���� �� ��� 
    // �������������� �������������
    // ----------------------------------------------------------------------------
    bool MarkAsDefault(__in PCWSTR wszDeviceId, __in ERole eRole);

    IMMDeviceEnumerator &m_MmDevEnumerator;
    CAtlArray<TAudioEndpoint> m_arrEndpoints;
};
