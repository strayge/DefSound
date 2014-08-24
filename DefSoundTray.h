// ----------------------------------------------------------------------------
// DefSoundTray.h
// Обработка иконки в системном трее приложения
// @author EreTIk
// ----------------------------------------------------------------------------


#pragma once

// роли устройтсва, в которых оно выступает устройством по-умолчанию
enum TDeviceRole {
    drAllRoles = 0,
    drConsole,
    drMultimedia,
    drCommunications,
    CountOfDeviceRole
};

// ----------------------------------------------------------------------------
// Класс невидимого кона и иконки в системном трее
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

    // элементы контекстного меню системного трея
    enum TTrayMenuItems {
        tmiSoundOptions = 10201,
        tmiHelp,
        tmiExit,
        tmiFirstEndpoint
    };

    // ----------------------------------------------------------------------------
    // Начало работы приложения
    // ----------------------------------------------------------------------------
    void Run(
        __in HINSTANCE hInstance
    );

private:
    // оконная функция невидимого окна
    static LRESULT CALLBACK InvWndProc(
        __in HWND hWindow,
        __in UINT nMessage,
        __in WPARAM wParam,
        __in LPARAM lParam
    );

    // ----------------------------------------------------------------------------
    // Обработка команды отображения системных настроек звука
    // ----------------------------------------------------------------------------
    void DoSoundOptions();

    // ----------------------------------------------------------------------------
    // Удаление иконки системного трея
    // ----------------------------------------------------------------------------
    void RemoveSystemTrayIcon();

    // иконка в системном трее
    static const UINT TrayIconId = 100;
    static const UINT TrayIconMessage = WM_USER + TrayIconId + 1;
    NOTIFYICONDATA m_TrayData;

#ifndef _STANDALONE
    // интерфейс управления политикой аудио устройтсв
    IPolicyConfigVista &m_PolicyConfigVista;
    // список устройств аудио-воспроизведения
    TAudioEndpointArray m_arrEndpoints;
#endif  // _STANDALONE
};
