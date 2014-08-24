// ----------------------------------------------------------------------------
// DefSoundTray.cpp
// Обработка иконки в системном трее приложения
// @author EreTIk
// ----------------------------------------------------------------------------


#include "StdAfx.h"

// ----------------------------------------------------------------------------
void TDefSoundTray::Run(
    __in HINSTANCE hInstance
)
{
    // создаем невидимое окно
    PCWSTR wszInvWndClassName = APPL_TITLE L" Class";
    WNDCLASS InvWndClass;
    RtlZeroMemory(&InvWndClass, sizeof(InvWndClass));
    InvWndClass.lpfnWndProc = TDefSoundTray::InvWndProc;
    InvWndClass.hInstance = hInstance;
    InvWndClass.lpszClassName = wszInvWndClassName;
    InvWndClass.style = CS_GLOBALCLASS;
    RegisterClass(&InvWndClass);
    HWND hWindow = CreateWindow(
        wszInvWndClassName,
        APPL_TITLE,
        0,
        0,
        0,
        0,
        0,
        NULL,
        NULL,
        NULL,
        this);
    if (!hWindow)
        return (void)ShowFormatError(L"Initialization");
    SetWindowLongPtr(hWindow, GWLP_USERDATA, (LONG_PTR)this);

    // инициализируем данные иконки системного трея и отображаем ее
    HICON hIconSmall = (HICON)::LoadImage(
        hInstance,
        MAKEINTRESOURCE(IDI_ICON_MAIN32), 
		IMAGE_ICON,
        ::GetSystemMetrics(SM_CXSMICON),
        ::GetSystemMetrics(SM_CYSMICON),
        LR_DEFAULTCOLOR);
    ATLASSERT(hIconSmall);
    RtlZeroMemory(&m_TrayData, sizeof(m_TrayData));
    m_TrayData.cbSize = sizeof(m_TrayData);
    m_TrayData.uID = TrayIconId;
    m_TrayData.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
    m_TrayData.uCallbackMessage = TrayIconMessage;
    m_TrayData.hIcon = hIconSmall;
    *m_TrayData.szTip = 0;
    m_TrayData.hWnd = hWindow;
    wcscat_s(m_TrayData.szTip, _countof(m_TrayData.szTip), APPL_TITLE);
    Shell_NotifyIcon(NIM_ADD, &(m_TrayData));

    // уходим в цикл обработки сообщений
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0))
    {
        if (msg.message == WM_TIMER) 
            msg.hwnd = hWindow;

        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}

// ----------------------------------------------------------------------------
LRESULT TDefSoundTray::InvWndProc(
    __in HWND hWindow,
    __in UINT nMessage,
    __in WPARAM wParam,
    __in LPARAM lParam
)
{
    LRESULT nResult = 0;
    bool bHandled = false;
    TDefSoundTray *pDefSoundTray = (TDefSoundTray *)
        ((LONG_PTR)GetWindowLongPtr(hWindow, GWLP_USERDATA));
    switch (nMessage)
    {
    // уничтожение окна
    case WM_DESTROY:
		PostQuitMessage(0);
		break;

    // обработка командных сообщений
    case WM_COMMAND:
        if (pDefSoundTray)
        {
            ULONG nId = LOWORD(wParam);
            switch (nId)
            {
            // отображение системных настроек звука
            case tmiSoundOptions:
                pDefSoundTray->DoSoundOptions();
                break;

            // отображение помощи
            case tmiHelp:
                Usage();
                break;

            // выход из приложения
            case tmiExit:
                pDefSoundTray->RemoveSystemTrayIcon();
                // уничтожаем невидимое коно
                SendMessage(hWindow, WM_CLOSE, 0, 0);
                DestroyWindow(hWindow);
                break;

            default:
#ifndef _STANDALONE
                if (nId >= tmiFirstEndpoint)
                {
                    ULONG nIndex = nId - tmiFirstEndpoint;
                    if (nIndex < pDefSoundTray->m_arrEndpoints.GetCount() * CountOfDeviceRole)
                    {
                        // индекс попадпет в диапазн:
                        // [tmiFirstEndpoint, tmiFirstEndpoint + (4 * количество устройтсв)]
                        // 
                        // это команда установки нового аудио-устройтсва по-умолчанию
                        TDeviceRole DeviceRole = TDeviceRole(nIndex % CountOfDeviceRole);
                        nIndex = nIndex / CountOfDeviceRole;
                        const TAudioEndpoint &AudioEndpoint = pDefSoundTray->m_arrEndpoints[nIndex];
                        HRESULT hResult;
                        switch (DeviceRole)
                        {
                        case drAllRoles:
                            hResult = pDefSoundTray->m_PolicyConfigVista.SetDefaultEndpoint(AudioEndpoint.m_strDeviceId, eConsole);
                            ATLASSERT(SUCCEEDED(hResult));
                            hResult = pDefSoundTray->m_PolicyConfigVista.SetDefaultEndpoint(AudioEndpoint.m_strDeviceId, eMultimedia);
                            ATLASSERT(SUCCEEDED(hResult));
                            hResult = pDefSoundTray->m_PolicyConfigVista.SetDefaultEndpoint(AudioEndpoint.m_strDeviceId, eCommunications);
                            ATLASSERT(SUCCEEDED(hResult));
                            break;

                        case drConsole:
                            hResult = pDefSoundTray->m_PolicyConfigVista.SetDefaultEndpoint(AudioEndpoint.m_strDeviceId, eConsole);
                            ATLASSERT(SUCCEEDED(hResult));
                            break;
                        
                        case drMultimedia:
                            hResult = pDefSoundTray->m_PolicyConfigVista.SetDefaultEndpoint(AudioEndpoint.m_strDeviceId, eMultimedia);
                            ATLASSERT(SUCCEEDED(hResult));
                            break;
                        
                        case drCommunications:
                            hResult = pDefSoundTray->m_PolicyConfigVista.SetDefaultEndpoint(AudioEndpoint.m_strDeviceId, eCommunications);
                            ATLASSERT(SUCCEEDED(hResult));
                            break;

                        default:
                            ATLASSERT(FALSE);
                            break;
                        }
                    }
                }
#endif  // _STANDALONE
                break;
            }
        }
        break;

    // обработка событий от иконки в системном трее
    case TrayIconMessage:
		if (lParam == WM_LBUTTONDBLCLK && pDefSoundTray)
		{
			// двойной клик мыши по значку в трее
			HRESULT hResult = pDefSoundTray->m_arrEndpoints.RefreshList();
			ATLASSERT(SUCCEEDED(hResult));
			if (!SUCCEEDED(hResult))
			{
				ShowFormatError(L"Enumerate audio devices", hResult);
				break;
			}
			SIZE_T nCount = 0;
			nCount = pDefSoundTray->m_arrEndpoints.GetCount();
			if (nCount >= 2)
			{
				const TAudioEndpoint &AudioEndpoint0 = pDefSoundTray->m_arrEndpoints[0];
				const TAudioEndpoint &AudioEndpoint1 = pDefSoundTray->m_arrEndpoints[1];
				bool isMmediaDefault0 = AudioEndpoint0.m_arrIsDefault[eMultimedia];
				
				if (!isMmediaDefault0)
				{
					hResult = pDefSoundTray->m_PolicyConfigVista.SetDefaultEndpoint(AudioEndpoint0.m_strDeviceId, eMultimedia);
					ATLASSERT(SUCCEEDED(hResult));
				}
				else
				{
					hResult = pDefSoundTray->m_PolicyConfigVista.SetDefaultEndpoint(AudioEndpoint1.m_strDeviceId, eMultimedia);
					ATLASSERT(SUCCEEDED(hResult));
				}
			}
		}
		else if (lParam == WM_RBUTTONUP && pDefSoundTray)
        {
            // клик правой кнопкой мыши по значку в трее
            HMENU hTrayMenu = CreatePopupMenu();
            ATLVERIFY(AppendMenu(hTrayMenu, 0, tmiSoundOptions, L"System Sound Options..."));
            SIZE_T nCount = 0;
            ATLVERIFY(AppendMenu(hTrayMenu, MF_MENUBREAK, 0, NULL));
#ifndef _STANDALONE
            HRESULT hResult = pDefSoundTray->m_arrEndpoints.RefreshList();
            ATLASSERT(SUCCEEDED(hResult));
            if (!SUCCEEDED(hResult))
            {
                ShowFormatError(L"Enumerate audio devices", hResult);
                break;
            }
            nCount = pDefSoundTray->m_arrEndpoints.GetCount();
            ULONG nIdIndex = tmiFirstEndpoint;
            if (nCount)
            {
                for (SIZE_T i = 0; i < nCount; ++i)
                {
                    const TAudioEndpoint &AudioEndpoint = pDefSoundTray->m_arrEndpoints[i];
                    HMENU hDeviceMenu = CreatePopupMenu();
                    ATLVERIFY(AppendMenu(
                        hDeviceMenu, 
                        AudioEndpoint.IsDefaultAllRoles() ? MF_CHECKED : MF_UNCHECKED, 
                        nIdIndex++,
                        L"All Roles"));
                    ATLVERIFY(AppendMenu(
                        hDeviceMenu, 
                        AudioEndpoint.m_arrIsDefault[eConsole] ? MF_CHECKED : MF_UNCHECKED, 
                        nIdIndex++,
                        L"Console"));
                    ATLVERIFY(AppendMenu(
                        hDeviceMenu, 
                        AudioEndpoint.m_arrIsDefault[eMultimedia] ? MF_CHECKED : MF_UNCHECKED, 
                        nIdIndex++,
                        L"Multimedia"));
                    ATLVERIFY(AppendMenu(
                        hDeviceMenu, 
                        AudioEndpoint.m_arrIsDefault[eCommunications] ? MF_CHECKED : MF_UNCHECKED, 
                        nIdIndex++,
                        L"Communications"));

                    ATLVERIFY(AppendMenu(
                        hTrayMenu,
                        MF_POPUP | (AudioEndpoint.IsDefaultAllRoles() ? MF_CHECKED : MF_UNCHECKED),
                        (UINT_PTR)hDeviceMenu,
                        AudioEndpoint.m_strDeviceName));
                }
            }
            else
#endif  // _STANDALONE
            if (!nCount)
            {
                ATLVERIFY(AppendMenu(hTrayMenu, MF_GRAYED, 0, L"No audio endpoints"));
            }

            ATLVERIFY(AppendMenu(hTrayMenu, MF_MENUBREAK, 0, NULL));
            ATLVERIFY(AppendMenu(hTrayMenu, 0, tmiHelp, L"Using..."));
            ATLVERIFY(AppendMenu(hTrayMenu, 0, tmiExit, L"Exit"));

            // SetMenuDefaultItem(hTrayMenu, tmiSoundOptions, MF_BYCOMMAND);

            POINT CurPoint;
            ATLVERIFY(GetCursorPos(&CurPoint));
            SetForegroundWindow(hWindow);
            TrackPopupMenu(hTrayMenu, 0, CurPoint.x, CurPoint.y, 0, hWindow, NULL);
            PostMessage(hWindow, WM_NULL, 0, 0);
            Shell_NotifyIcon(NIM_SETFOCUS, &pDefSoundTray->m_TrayData);
        } 
        break;
    }

    if (bHandled)
        return nResult;
    return DefWindowProc(hWindow, nMessage, wParam, lParam);
}

// ----------------------------------------------------------------------------
void TDefSoundTray::DoSoundOptions()
{
    STARTUPINFO StartupInfo;
    RtlZeroMemory(&StartupInfo, sizeof(StartupInfo));
    StartupInfo.cb =  sizeof(StartupInfo);
    PROCESS_INFORMATION ProcessInfo;
    RtlZeroMemory(&ProcessInfo, sizeof(ProcessInfo));
    WCHAR wszCommandLine[] = L"control.exe mmsys.cpl";
    if (CreateProcess(
            NULL,
            wszCommandLine,
            NULL,
            NULL,
            FALSE,
            0,
            NULL,
            NULL,
            &StartupInfo,
            &ProcessInfo))
    {
        CloseHandle(ProcessInfo.hProcess);
        CloseHandle(ProcessInfo.hThread);
    }
}

// ----------------------------------------------------------------------------
void TDefSoundTray::RemoveSystemTrayIcon()
{
    // удаляем иконку из трея
    HWND hWindow = m_TrayData.hWnd;
    RtlZeroMemory(&m_TrayData, sizeof(m_TrayData));
    m_TrayData.cbSize = sizeof(m_TrayData);
    m_TrayData.hWnd = hWindow;
    m_TrayData.uID = TrayIconId;
    Shell_NotifyIcon(NIM_DELETE, &m_TrayData);
}
