// ----------------------------------------------------------------------------
// DefSoundTray.h
// ��������� ������ � ��������� ���� ����������
// @author EreTIk
// ----------------------------------------------------------------------------


#pragma once

// ���� ����������, � ������� ��� ��������� ����������� ��-���������
enum TDeviceRole {
    drAllRoles = 0,
    drConsole,
    drMultimedia,
    drCommunications,
    CountOfDeviceRole
};

// ----------------------------------------------------------------------------
// ����� ���������� ���� � ������ � ��������� ����
// ----------------------------------------------------------------------------
class TDefSoundTray {
public:
    TDefSoundTray(
#ifndef _STANDALONE
        IMMDeviceEnumerator &MmDevEnumerator, 
        IPolicyConfigVista &PolicyConfigVista
#endif  // _STANDALONE
    )
#ifndef _STANDALONE
      : m_arrEndpoints(MmDevEnumerator) 
      , m_PolicyConfigVista(PolicyConfigVista)
#endif  // _STANDALONE
    {
    }

    // �������� ������������ ���� ���������� ����
    enum TTrayMenuItems {
        tmiSoundOptions = 10201,
        tmiHelp,
        tmiExit,
        tmiFirstEndpoint
    };

    // ----------------------------------------------------------------------------
    // ������ ������ ����������
    // ----------------------------------------------------------------------------
    void Run(
        __in HINSTANCE hInstance
    );

private:
    // ������� ������� ���������� ����
    static LRESULT CALLBACK InvWndProc(
        __in HWND hWindow,
        __in UINT nMessage,
        __in WPARAM wParam,
        __in LPARAM lParam
    );

    // ----------------------------------------------------------------------------
    // ��������� ������� ����������� ��������� �������� �����
    // ----------------------------------------------------------------------------
    void DoSoundOptions();

    // ----------------------------------------------------------------------------
    // �������� ������ ���������� ����
    // ----------------------------------------------------------------------------
    void RemoveSystemTrayIcon();

    // ������ � ��������� ����
    static const UINT TrayIconId = 100;
    static const UINT TrayIconMessage = WM_USER + TrayIconId + 1;
    NOTIFYICONDATA m_TrayData;

#ifndef _STANDALONE
    // ��������� ���������� ��������� ����� ���������
    IPolicyConfigVista &m_PolicyConfigVista;
    // ������ ��������� �����-���������������
    TAudioEndpointArray m_arrEndpoints;
#endif  // _STANDALONE
};
