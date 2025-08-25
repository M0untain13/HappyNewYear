#include <windows.h>
#include <string>
#include <chrono>
#include <iomanip>
#include <sstream>

// Глобальные переменные
HINSTANCE hInst;
const wchar_t szWindowClass[] = L"NewYearCountdown";
const wchar_t szTitle[] = L"До Нового Года осталось:";

// Функция для расчета дней до Нового Года
int GetDaysUntilNewYear() {
    auto now = std::chrono::system_clock::now();
    time_t now_time = std::chrono::system_clock::to_time_t(now);
    tm* local_time = localtime(&now_time);
    
    // Устанавливаем дату следующего Нового Года
    tm new_year = *local_time;
    new_year.tm_year = local_time->tm_year + 1; // Следующий год
    new_year.tm_mon = 0;  // Январь
    new_year.tm_mday = 1; // 1 число
    new_year.tm_hour = 0;
    new_year.tm_min = 0;
    new_year.tm_sec = 0;
    
    time_t new_year_time = mktime(&new_year);
    
    // Разница в секундах
    double diff_seconds = difftime(new_year_time, now_time);
    
    // Конвертируем в дни
    int days = static_cast<int>(diff_seconds / (60 * 60 * 24));
    
    return days;
}

// Функция для получения текущей даты
std::wstring GetCurrentDate() {
    auto now = std::chrono::system_clock::now();
    time_t now_time = std::chrono::system_clock::to_time_t(now);
    tm* local_time = localtime(&now_time);
    
    wchar_t buffer[80];
    wcsftime(buffer, sizeof(buffer), L"%d.%m.%Y %H:%M:%S", local_time);
    
    return std::wstring(buffer);
}

// Обработчик сообщений для главного окна
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    static HWND hDaysLabel, hDateLabel;
    static UINT_PTR timerId;
    
    switch (message) {
    case WM_CREATE:
        // Создаем таймер для обновления каждую секунду
        timerId = SetTimer(hWnd, 1, 1000, NULL);
        
        // Создаем статический текст для отображения дней
        hDaysLabel = CreateWindowW(L"STATIC", L"", 
            WS_CHILD | WS_VISIBLE | SS_CENTER,
            10, 10, 280, 60, hWnd, NULL, hInst, NULL);
        
        // Создаем статический текст для отображения текущей даты
        hDateLabel = CreateWindowW(L"STATIC", L"", 
            WS_CHILD | WS_VISIBLE | SS_CENTER,
            10, 80, 280, 20, hWnd, NULL, hInst, NULL);
        
        // Устанавливаем шрифт для основного текста
        HFONT hFont;
        hFont = CreateFont(36, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, 
                          DEFAULT_CHARSET, OUT_OUTLINE_PRECIS,
                          CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, 
                          VARIABLE_PITCH, L"Arial");
        SendMessage(hDaysLabel, WM_SETFONT, (WPARAM)hFont, TRUE);
        
        // Шрифт для даты
        HFONT hSmallFont;
        hSmallFont = CreateFont(14, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, 
                               DEFAULT_CHARSET, OUT_OUTLINE_PRECIS,
                               CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, 
                               VARIABLE_PITCH, L"Arial");
        SendMessage(hDateLabel, WM_SETFONT, (WPARAM)hSmallFont, TRUE);
        break;
        
    case WM_TIMER:
        // Обновляем отображение при срабатывании таймера
        {
            int days = GetDaysUntilNewYear();
            std::wstring daysText;
            
            if (days == 0) {
                daysText = L"С НОВЫМ ГОДОМ! 🎄";
            } else {
                daysText = std::to_wstring(days) + L" дней";
            }
            
            SetWindowTextW(hDaysLabel, daysText.c_str());
            SetWindowTextW(hDateLabel, GetCurrentDate().c_str());
        }
        break;
        
    case WM_CTLCOLORSTATIC:
        {
            HDC hdcStatic = (HDC)wParam;
            SetTextColor(hdcStatic, RGB(255, 255, 255));
            SetBkColor(hdcStatic, RGB(25, 25, 112)); // Темно-синий фон
            
            static HBRUSH hBrush = CreateSolidBrush(RGB(25, 25, 112));
            return (LRESULT)hBrush;
        }
        break;
        
    case WM_ERASEBKGND:
        {
            RECT rect;
            GetClientRect(hWnd, &rect);
            HBRUSH hBrush = CreateSolidBrush(RGB(25, 25, 112));
            FillRect((HDC)wParam, &rect, hBrush);
            DeleteObject(hBrush);
            return 1;
        }
        break;
        
    case WM_DESTROY:
        KillTimer(hWnd, timerId);
        PostQuitMessage(0);
        break;
        
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Регистрация класса окна
ATOM RegisterWindowClass(HINSTANCE hInstance) {
    WNDCLASSEXW wcex;
    
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, IDI_APPLICATION);
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, IDI_APPLICATION);
    
    return RegisterClassExW(&wcex);
}

// Точка входа
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    hInst = hInstance;
    
    RegisterWindowClass(hInstance);
    
    // Создаем главное окно
    HWND hWnd = CreateWindowW(szWindowClass, szTitle, 
        WS_OVERLAPPEDWINDOW & ~WS_MAXIMIZEBOX & ~WS_SIZEBOX,
        CW_USEDEFAULT, 0, 320, 180, nullptr, nullptr, hInstance, nullptr);
    
    if (!hWnd) {
        return FALSE;
    }
    
    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);
    
    // Цикл сообщений
    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    
    return (int)msg.wParam;
}