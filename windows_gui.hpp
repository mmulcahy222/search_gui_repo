#include <iostream>
#include <Windows.h>
#include <Commctrl.h>
using namespace std;

#include "search_strategy_factory.hpp"

//search instance to be used for the radio push buttons in the window
Search search_instance = Search();

LRESULT APIENTRY WindowProcedure(HWND , UINT , WPARAM , LPARAM );


HWND create_basic_window()
{
    int window_width = 400;
    int window_height = 250;

    WNDCLASSEX wc;
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = 0;
    wc.lpfnWndProc = WindowProcedure;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = GetModuleHandle(NULL);
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)CreateSolidBrush(RGB(191,184,191));
    wc.lpszMenuName = NULL;
    wc.lpszClassName = "WindowClass";
    wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
    if (!RegisterClassEx(&wc))
    {
        MessageBox(NULL, "Window Registration Failed!", "Error!",
                   MB_ICONEXCLAMATION | MB_OK);
        return NULL;
    }
    HWND hwnd = CreateWindowEx(WS_EX_CLIENTEDGE, "WindowClass", "Search Tree View",
                               WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, window_width, window_height, NULL, NULL, GetModuleHandle(NULL), NULL);
    if (hwnd == NULL)
    {
        MessageBox(NULL, "Window Creation Failed!", "Error!",
                   MB_ICONEXCLAMATION | MB_OK);
        return NULL;
    }
    return hwnd;
}


LRESULT APIENTRY WindowProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
        case WM_COMMAND:
        {
            if (LOWORD(wParam) == 1)
            {
                search_instance.setStrategy("youtube");
            }
            else if (LOWORD(wParam) == 2)
            {
                search_instance.setStrategy("spotify");
            }
            else if (LOWORD(wParam) == 3)
            {
                search_instance.setStrategy("discogs");
            }
            else if (LOWORD(wParam) == 4)
            {
                search_instance.setStrategy("google");
            }

            return 0;
        }
        case WM_CREATE:
        {
            int window_width = 400;
            int window_height = 250;

            //create two buttons in this window with padding of 10 pixels on every side
            short int top_padding = 10;
            short int left_padding = 50;
            short int right_padding = 50;
            short int bottom_padding = 10;
            short int button_bottom_margin = 10;
            // I don't know why I needed to put -20 for the width, because it should have worked without it. 
            short int button_width = (window_width - (left_padding + right_padding)) - 20;
            short int button_height = 30;

            //Create three buttons and attach them to the window. They will show up as RADIO BUTTONS in the menu. The BS_PUSHLIKE style is used to make the radio buttons look like push buttons.
            HWND button1 = CreateWindowEx(0, "BUTTON", "YouTube", WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON | BS_PUSHLIKE, left_padding, top_padding, button_width, button_height, hwnd, (HMENU)1, NULL, NULL);
            HWND button2 = CreateWindowEx(0, "BUTTON", "Spotify", WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON | BS_PUSHLIKE, left_padding, top_padding + button_height + button_bottom_margin, button_width, button_height, hwnd, (HMENU)2, NULL, NULL);
            HWND button3 = CreateWindowEx(0, "BUTTON", "Discogs", WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON | BS_PUSHLIKE, left_padding, top_padding + (button_height * 2) + (button_bottom_margin * 2), button_width, button_height, hwnd, (HMENU)3, NULL, NULL);
            HWND button4 = CreateWindowEx(0, "BUTTON", "Google", WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON | BS_PUSHLIKE, left_padding, top_padding + (button_height * 3) + (button_bottom_margin * 3), button_width, button_height, hwnd, (HMENU)4, NULL, NULL);

            //Run the code that would usually happen when clicking the button. The 1 corresponds to the first button when the radio buttons are created.
            SendMessage(hwnd, WM_COMMAND, 1, 0);
            //Make button 1 the default selection (pushed down)
            SendMessage(button1, BM_SETCHECK, BST_CHECKED, 0);


            //put some text 25 pixels below button4 with background color of RGB(191,184,191)
            //background color is determined by the case in the switch statement in the WindowProcedure function below for the case called WM_CTLCOLORSTATIC
            //the (+5) is to make the text appear below the button
            HWND text_box = CreateWindowEx(0, "STATIC", "Goon", WS_VISIBLE | WS_CHILD | SS_LEFT | SS_CENTER, left_padding, top_padding + (button_height * 4) + (button_bottom_margin * 4) + 5, button_width, button_height, hwnd, (HMENU)5, NULL, NULL);

            break;
        }
        //Sole reason is to make the background color of Text Label with the same background as the rest of the window
        case WM_CTLCOLORSTATIC:
        {
            HDC hdcStatic = (HDC)wParam;
            SetTextColor(hdcStatic, RGB(0, 0, 0));
            SetBkColor(hdcStatic, RGB(191, 184, 191));
            //create brush
            HBRUSH hBrush = CreateSolidBrush(RGB(191, 184, 191));
            return (LRESULT)hBrush;
        }         
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
        default:
            return DefWindowProc(hwnd, message, wParam, lParam);
    }

    return 0;
}
