/*  =========================================================
    Author: Jacob Tillett
    Date: 8/5/2018

    This is the entry point.
    =========================================================*/

#include <windows.h>


LRESULT CALLBACK MainWindowCallback(HWND Window,
                                    UINT Message,
                                    WPARAM WParam,
                                    LPARAM LParam)
{
    LRESULT Result = 0;

    switch(Message)
    {
        case WM_SIZE:
        {
            OutputDebugStringA("WM_SIZE\n");
        } break;

        case WM_DESTROY:
        {
            OutputDebugStringA("WM_DESTROY\n");
        } break;

        case WM_CLOSE:
        {
            OutputDebugStringA("WM_CLOSE\n");
        } break;

        case WM_ACTIVATEAPP:
        {
            OutputDebugStringA("WM_ACTIVATEAPP\n");
        } break;

        case WM_PAINT:
        {
            PAINTSTRUCT Paint;
            HDC DeviceContext = BeginPaint(Window, &Paint);
            int X = Paint.rcPaint.left;
            int Y = Paint.rcPaint.top;
            int Width = Paint.rcPaint.right - Paint.rcPaint.left;
            int Height = Paint.rcPaint.bottom - Paint.rcPaint.top;
            static DWORD Operation = WHITENESS;
            PatBlt(DeviceContext, X, Y, Width, Height, Operation);
            if(Operation == WHITENESS)
            {
                Operation = BLACKNESS;
            }
            else
            {
                Operation = WHITENESS;
            }
            EndPaint(Window, &Paint);

        } break;

        default:
        {
//            OutputDebugStringA("default\n");
            Result = DefWindowProc(Window, Message, WParam, LParam);
        } break;
    }
    
    return(Result);
}


//Entry point into the program
int CALLBACK WinMain(HINSTANCE Instance,
                     HINSTANCE PrevInstance,
                     LPSTR CommandLine,
                     int ShowCode)
{

    /*
        Creating a window class struct and filling out the fields 
        that we need to use.

        The empty curly braces set all the values to 0
    */
    WNDCLASS WindowClass = {}; 
    // Telling the window to redraw on horizontal and vertical resizing
    WindowClass.style = CS_OWNDC|CS_HREDRAW|CS_VREDRAW;
    // Passing the window callback
    WindowClass.lpfnWndProc = MainWindowCallback;
    // Passing our instance
    WindowClass.hInstance = Instance;
//  WindowClass.hIcon; // we do not need a icon yet

    // Gicing the window class a name
    WindowClass.lpszClassName = "PostedWindowClass";

    if(RegisterClassA(&WindowClass))
    {
        HWND WindowHandle = CreateWindowExA(0,
                                           WindowClass.lpszClassName,
                                           "POSTED",
                                           WS_OVERLAPPEDWINDOW|WS_VISIBLE,
                                           CW_USEDEFAULT,
                                           CW_USEDEFAULT,
                                           CW_USEDEFAULT,
                                           CW_USEDEFAULT,
                                           0,
                                           0,
                                           Instance,
                                           0);
        if(WindowHandle)
        {
            MSG Message;
            for(;;) //loop forever till we get a positve message from reuslt
            {
                BOOL MessageResult = GetMessageA(&Message, 0, 0, 0);
                if(MessageResult > 0)
                {
                    TranslateMessage(&Message);
                    DispatchMessage(&Message);
                }
                else
                {
                    break;
                }
            }
        }
        else
        {
            // TODO: LOGGING
        }
    }
    else
    {
        // TODO: Log 
    }


    return(0);
}