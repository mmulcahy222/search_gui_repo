#include <iostream>
#include <Windows.h>
#include <Commctrl.h>
#include <shellapi.h>


#include "windows_gui.hpp"


///////////////////////////
//
// NOTES:
//
//		- The following .hpp files are included in this file:
//			- windows_gui.hpp
//			     - windows_gui.hpp in turn calls the following .hpp file:
//					- search_strategy_factory.hpp
//
///////////////////////////



using namespace std;
HWND get_hwnd_by_title_part(const char*);
HHOOK _hook;
HMENU menu;
HWND hwnd_main_window;
LRESULT __stdcall MouseCallback(int nCode, WPARAM wParam, LPARAM lParam)
{   
    switch(wParam)
    {
        case WM_LBUTTONUP:
        {
            // get the text of the tree item
            DWORD pid = 0;
            DWORD bytes_read = 0;
            DWORD bytes_written = 0;
            BOOL debug = false;
            int text_buffer_size = 256;
            char text[256];

            //get the point & get the hwnd from that point
            POINT pt;
            GetCursorPos(&pt);
            HWND hwnd_clicked = WindowFromPoint(pt);            
            char class_name[256];
            GetClassName(hwnd_clicked, class_name, 256);
            //do not go any further if it's not a TreeViewList item
            if (strcmp(class_name, "SysTreeView32") != 0) break;

            //get HTREEITEM from hwnd
            TreeView_GetSelection(hwnd_clicked);
            HTREEITEM htreeitem_selected = (HTREEITEM)SendMessage(hwnd_clicked, TVM_GETNEXTITEM, TVGN_DROPHILITE, 0);

            
            //new TVITEM to place in the remote prcess
            TVITEM item;
            item.hItem = htreeitem_selected;
            item.mask = TVIF_TEXT | TVIF_HANDLE;
            item.pszText = new TCHAR[256];
            item.cchTextMax = 256;
            //open process
            GetWindowThreadProcessId(hwnd_main_window, &pid);
            HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);

            //VirtualAllocEx (treeview)
            //VirtualAllocEx (text)
            //
            // Both functions return a pointer to the allocated memory. Since it has EX at the end, it goes to another process.
            LPVOID remote_item_address = VirtualAllocEx(hProcess, 0, sizeof(TVITEM), MEM_COMMIT, PAGE_READWRITE);
            LPVOID remote_text_address = VirtualAllocEx(hProcess, 0, text_buffer_size, MEM_COMMIT, PAGE_READWRITE);
            //
            //
            //This following line is very important. The whole TVITEM  structure will be sent to the process. The TVITEM.psText will have the results of the the second call we made to VirtualAllocEx, which is for text. It's like a double pointer.
            //
            // According to https://docs.microsoft.com/en-us/windows/win32/controls/tvm-getitem
            // "If the TVIF_TEXT flag is set in the mask member of the TVITEM or TVITEMEX structure, the pszText member must point to a valid buffer and the cchTextMax member must be set to the number of characters in that buffer"
            item.pszText = (LPTSTR)remote_text_address;

            //WriteProcessMemory    (Puts OUR local TVITEM object into the remote process)
            //SendMessageA          (message is TVM_GETITEM, and fills out the TVItem structure we put in)
            //GetLastError          (error checking)
            //ReadProcessMemory     (gets the remote TVITEM object at remote_item_address, and puts it in local &item)
            //ReadProcessMemory     (gets the remote text object at remote_text_address, and puts it in local &buffer)
            BOOL write_process = WriteProcessMemory(hProcess, remote_item_address, &item, sizeof(TVITEM), &bytes_written);
            BOOL sent_message = SendMessageA(hwnd_clicked, TVM_GETITEM, TVGN_CARET, (LPARAM)remote_item_address);
            // get last error
            DWORD last_error = GetLastError();
            ReadProcessMemory(hProcess, remote_item_address, &item, sizeof(TVITEM), &bytes_read);
            CHAR buffer[256];
            ReadProcessMemory(hProcess, remote_text_address, &buffer, text_buffer_size - 1, &bytes_read);
            

            //print out 
            if(debug)
            {
                cout << "write process: " << write_process << endl;
                cout << "sent message: " << sent_message << endl;
                cout << "last error: " << last_error << endl;
                cout << "bytes_written: " << bytes_written << endl;
                cout << "bytes_read: " << bytes_read << endl;
                cout << "Text: " << text << endl;
                cout << "Buffer: " << buffer << endl;
                cout << endl;
            }
           
            //free memory 
            VirtualFreeEx(hProcess, remote_item_address, sizeof(TVITEM), MEM_RELEASE);
            VirtualFreeEx(hProcess, remote_text_address, text_buffer_size, MEM_RELEASE);
            CloseHandle(hProcess);

            //search
            if(strlen(buffer) < 1) break;

            //search instance is defined in 
            search_instance.search(buffer);
            

            break;
        }
        case WM_COMMAND:
        {
            cout << "Command" << endl;
            break;
        }
        case WM_INITMENUPOPUP:
        {
            cout << "Goon" << endl;
            break;
        }
    }
    return CallNextHookEx(_hook, nCode, wParam, lParam);
}

HWND get_hwnd_by_title_part(const char* title_part)
{
    HWND hwnd = NULL;
    while (hwnd = FindWindowEx(NULL, hwnd, NULL, NULL))
    {
        char title[256];
        GetWindowText(hwnd, title, 256);
        if (strstr(title, title_part))
        {
            return hwnd;
        }
    }
    return NULL;
}
int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow )
{
    // get the hwnd of the main_window window
    hwnd_main_window = get_hwnd_by_title_part("main_window");
    cout << "hwnd_main_window: " << hwnd_main_window << endl;
    // set the hook for left mouse clicks
    if (!(_hook = SetWindowsHookEx(WH_MOUSE_LL, MouseCallback, hInstance, 0)))
    {
        MessageBox(NULL, "Failed to install hook!", "Error", MB_ICONERROR);
    }

    // create the window to search in different pages (windows_gui.hpp)
    HWND hwnd_search_window = create_basic_window();


    // getdlgitem at 5 (the text box that is not the buttons). The reason it is 5 is because it corresponds to the index of the text box in the windows_gui.hpp file when it is created by the CreateWindow function.
    HWND hwnd_search_text = GetDlgItem(hwnd_search_window, 5);
    char hwnd_main_window_hex[256];
    sprintf(hwnd_main_window_hex, "Window found in: 0x%x", hwnd_main_window);
    SetWindowText(hwnd_search_text, hwnd_main_window_hex);

    ShowWindow(hwnd_search_window, SW_SHOW);
    UpdateWindow(hwnd_search_window);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage( &msg );
        DispatchMessage( &msg );
    }
    return 0;
}