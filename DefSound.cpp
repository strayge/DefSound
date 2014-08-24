// ----------------------------------------------------------------------------
// SndDvSel.cpp
// Процессы выбора звукового устройтсва по-умолчанию
// @author EreTIk
// ----------------------------------------------------------------------------


#include "StdAfx.h"


// ----------------------------------------------------------------------------
// Отображение сообщения об ошибке в message box'е
// ----------------------------------------------------------------------------
ULONG ShowFormatError(
    __in PCWSTR wszAction,
    __in ULONG nErrorCode /* = GetLastError() */,
    __in_opt HWND hParentWindow /* = NULL */,
    __in_opt PCWSTR wszSuffix /* = NULL */
)
{
    PWCHAR pMsgBuff;
    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | 
        FORMAT_MESSAGE_FROM_SYSTEM,
        NULL,
        nErrorCode,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (PWCHAR)&pMsgBuff,
        0x00,NULL);
    CString strMessage;
    strMessage.Format(L"%s failed. Error %u: %s", wszAction, nErrorCode, pMsgBuff);
    if (wszSuffix)
        strMessage = strMessage + wszSuffix;
    else
        strMessage = strMessage + L"\n";
    LocalFree(pMsgBuff);
    MessageBox(hParentWindow, strMessage, APPL_TITLE L": Error", MB_ICONERROR);
    return nErrorCode;
}

// ----------------------------------------------------------------------------
ULONG Usage(HWND hParent /* = NULL */)
{
    // отображение help'а
    MessageBox(
        hParent, 
        L"Usage: DefSound.exe\n"
        L"Run and show system tray icon\n"
        L"\nOr: DefSound.exe <NUMBER> [STRING]\n"
        L"Change default sound render device and exit.\n"
        L"The device sets zero-based index (NUMBER)\n"
        L"STRING is a role, may be:\n"
        L"\tALL\t- all roles (default string value)\n"
        L"\tCON\t- console(system notification sounds and voice commands)\n"
        L"\tMMEDIA\t- multimedia (music, movies, etc...)\n"
        L"\tVOICE\t- voice communications\n"
        L"\nFor example: DefSound.exe 0\n"
        L"Select device with index 0 (first device) as default, all roles",
        APPL_TITLE L": Help",
        MB_OK);
    return ERROR_INVALID_PARAMETER;
}

// ----------------------------------------------------------------------------
// Точка входа в приложение
// ----------------------------------------------------------------------------
INT WINAPI WinMain(
    __in HINSTANCE hInstance,
    __in_opt HINSTANCE /* hPrevInstance */,
    __in_opt LPSTR /* lpCmdLine */,
    __in int /* nShowCmd */
)
{
    HRESULT hResult = CoInitialize(NULL);
    ATLASSERT(SUCCEEDED(hResult));

    INT nArgCount = 0;
    PWSTR *arrArguments = CommandLineToArgvW(GetCommandLineW(), &nArgCount);

    {
#ifndef _STANDALONE
        // получаем необходимые для работы интерфейсы
        CComPtr<IMMDeviceEnumerator> MmDevEnumerator;
        hResult = MmDevEnumerator.CoCreateInstance(__uuidof(MMDeviceEnumerator));
        ATLASSERT(SUCCEEDED(hResult));
        if (S_OK != hResult)
            return ShowFormatError(L"Query IMMDeviceEnumerator interface", hResult, NULL, L"Check version of your OS (required Vista+)");

        CComPtr<IPolicyConfigVista> PolicyConfigVista;
        hResult = PolicyConfigVista.CoCreateInstance(__uuidof(CPolicyConfigVistaClient));
        ATLASSERT(SUCCEEDED(hResult));
        if (!SUCCEEDED(hResult))
            return ShowFormatError(L"Query IPolicyConfigVista interface", hResult, NULL, L"Check version of your OS (required Vista+)");
#endif  // _STANDALONE


        if (nArgCount == 1)
        {        
#ifndef _STANDALONE
            TDefSoundTray DefSoundTray(*MmDevEnumerator, *PolicyConfigVista);
#else   // _STANDALONE
            TDefSoundTray DefSoundTray;
#endif  // _STANDALONE

            DefSoundTray.Run(hInstance);
        }
#ifndef _STANDALONE
        else
        {
            // запуск с параметрами: меняем устройство по умолчанию и выходим 
            if (nArgCount != 2 && nArgCount != 3)
                return Usage();

            ULONG nDeviceIndex = 0;
            swscanf_s(arrArguments[1], L"%i", &nDeviceIndex);

            TDeviceRole DeviceRole = drAllRoles;
            if (nArgCount == 3)
            {
                if (!_wcsicmp(arrArguments[2], L"ALL"))
                {
                    DeviceRole = drAllRoles;
                } 
                else if (!_wcsicmp(arrArguments[2], L"CON"))
                {
                    DeviceRole = drConsole;
                }
                else if (!_wcsicmp(arrArguments[2], L"MMEDIA"))
                {
                    DeviceRole = drMultimedia;
                }
                else if (!_wcsicmp(arrArguments[2], L"VOICE"))
                {
                    DeviceRole = drCommunications;
                }
                else
                {
                    ShowFormatError(L"Parse role string", ERROR_INVALID_PARAMETER);
                    return Usage();
                }
            }

            TAudioEndpointArray arrAudioEndpoint(*MmDevEnumerator);
            ;
            if (!SUCCEEDED(arrAudioEndpoint.RefreshList()))
            {
                ShowFormatError(L"Enumerate audio devices", hResult);
            }
            else 
            {
                if (nDeviceIndex >= arrAudioEndpoint.GetCount())
                    return MessageBox(NULL, L"Target device not found", APPL_TITLE L": Error", MB_OK);
                const TAudioEndpoint AudioEndpoint = arrAudioEndpoint[nDeviceIndex];
                switch (DeviceRole)
                {
                case drAllRoles:
                    hResult = PolicyConfigVista->SetDefaultEndpoint(AudioEndpoint.m_strDeviceId, eConsole);
                    ATLASSERT(SUCCEEDED(hResult));
                    hResult = PolicyConfigVista->SetDefaultEndpoint(AudioEndpoint.m_strDeviceId, eMultimedia);
                    ATLASSERT(SUCCEEDED(hResult));
                    hResult = PolicyConfigVista->SetDefaultEndpoint(AudioEndpoint.m_strDeviceId, eCommunications);
                    ATLASSERT(SUCCEEDED(hResult));
                    break;

                case drConsole:
                    hResult = PolicyConfigVista->SetDefaultEndpoint(AudioEndpoint.m_strDeviceId, eConsole);
                    ATLASSERT(SUCCEEDED(hResult));
                    break;
                
                case drMultimedia:
                    hResult = PolicyConfigVista->SetDefaultEndpoint(AudioEndpoint.m_strDeviceId, eMultimedia);
                    ATLASSERT(SUCCEEDED(hResult));
                    break;
                
                case drCommunications:
                    hResult = PolicyConfigVista->SetDefaultEndpoint(AudioEndpoint.m_strDeviceId, eCommunications);
                    ATLASSERT(SUCCEEDED(hResult));
                    break;

                default:
                    ATLASSERT(FALSE);
                    break;
                }
            }
        }
#endif  // _STANDALONE
    }

    CoUninitialize();
    return ERROR_SUCCESS;
}
