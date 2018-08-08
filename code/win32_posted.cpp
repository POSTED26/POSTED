/*  =========================================================
    Author: Jacob Tillett
    Date: 8/5/2018

    This is the entry point.
    =========================================================*/

#include <windows.h>

#define internal static
#define local_persist static
#define global_var static

global_var bool isRunning; // TODO: this is a global for now.

global_var BITMAPINFO BitmapInfo;
global_var void *BitmapMemory;
global_var HBITMAP BitmapHandle;
global_var HDC BitmapDeviceContext;


internal void Win32ResizeDIBSection(int Width, int Height)
{
    //TODO: maybe free first , free after, then free first if fails

    
    
    if(BitmapHandle)
    {
        DeleteObject(BitmapHandle);
    }
    
    if(!BitmapDeviceContext)
    {
        //TODO: should we recreate under certain circumstances???
        BitmapDeviceContext = CreateCompatibleDC(0);
    }
    
    BitmapInfo.bmiHeader.biSize = sizeof(BitmapInfo.bmiHeader);
    BitmapInfo.bmiHeader.biWidth = Width;
    BitmapInfo.bmiHeader.biHeight = Height;
    BitmapInfo.bmiHeader.biPlanes = 1;
    BitmapInfo.bmiHeader.biBitCount = 32;
    BitmapInfo.bmiHeader.biCompression = BI_RGB;
    // BitmapInfo.biSizeImage = 0;
    // BitmapInfo.biXPelsPerMeter = 0;
    // BitmapInfo.biYPelsPerMeter = 0;
    // BitmapInfo.biClrUsed = 0;
    // BitmapInfo.biClrImportant = 0;



    BitmapHandle = CreateDIBSection(BitmapDeviceContext,
                                    &BitmapInfo, 
                                    DIB_RGB_COLORS, 
                                    &BitmapMemory, 
                                    0,
                                    0);

    
}

internal void Win32UpdateWindow(HDC DeviceContext, int X, int Y, int Width, int Height)
{
    StretchDIBits(DeviceContext,
                  X, Y, Width, Height,          // Destination
                  X, Y, Width, Height,          // Source
                  BitmapMemory, 
                  &BitmapInfo, DIB_RGB_COLORS, SRCCOPY);
}

LRESULT CALLBACK Win32MainWindowCallback(HWND Window,
                                    UINT Message,
                                    WPARAM WParam,
                                    LPARAM LParam)
{
    LRESULT Result = 0;

    switch(Message)
    {
        case WM_SIZE:
        {
            RECT ClientRect;
            GetClientRect(Window, &ClientRect);
            int Width = ClientRect.right - ClientRect.left;
            int Height = ClientRect.bottom - ClientRect.top;
            Win32ResizeDIBSection(Width, Height);
            //OutputDebugStringA("WM_SIZE\n");
        } break;

        case WM_DESTROY:
        {
            isRunning = false; // TODO: handle with and error recreate window??
            //OutputDebugStringA("WM_DESTROY\n");
        } break;

        case WM_CLOSE:
        {
            isRunning = false; // TODO: handle with a message to user??
            //OutputDebugStringA("WM_CLOSE\n");
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
            local_persist DWORD Operation = WHITENESS;
            Win32UpdateWindow(DeviceContext, X, Y, Width, Height);
            PatBlt(DeviceContext, X, Y, Width, Height, Operation);
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
    WindowClass.lpfnWndProc = Win32MainWindowCallback;
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
            isRunning = true;
            MSG Message;
            while(isRunning) //loop forever till we get a positve message from reuslt
            {
                BOOL MessageResult = GetMessageA(&Message, 0, 0, 0);
                if(MessageResult > 0)
                {
                    TranslateMessage(&Message);
                    DispatchMessageA(&Message);
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