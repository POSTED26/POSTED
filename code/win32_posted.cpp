/*  =========================================================
    Author: Jacob Tillett
    Date: 8/5/2018

    This is the entry point.

    So far this file has an entry point, it creates a window
    that processes a few messages with a callback function.
    This file also creates a buffer to fill and them paint to
    screen.
    =========================================================*/

#include <windows.h>
#include <stdint.h>

// Definitions. 
#define internal static
#define local_persist static
#define global_var static



global_var bool isRunning; // TODO: this is a global for now.

global_var BITMAPINFO BitmapInfo;
global_var void *BitmapMemory;

global_var int BitmapWidth;
global_var int BitmapHeight;
global_var int BytesPerPixel = 4;



internal void RenderGradient(int XOffset, int YOffset)
{
    int Width = BitmapWidth;
    int Height = BitmapHeight;

    int Pitch = Width * BytesPerPixel;
    uint8_t *Row = (uint8_t *)BitmapMemory;
    for(int Y = 0; Y < BitmapHeight; ++Y)
    {
        uint32_t *Pixel = (uint32_t *)Row;
        for(int X = 0; X < BitmapWidth; ++X)
        { // Pixels in memory BB GG RR XX

            uint8_t Blue = (X + XOffset);
            uint8_t Green = (Y + YOffset);
            uint8_t Red = ((uint8_t)(X + XOffset) / 2) + ((uint8_t)(Y + -YOffset) / 2);

            *Pixel++ = ((Red << 16) | (Green << 8) | Blue);

        }
        Row += Pitch;
    }
    
}



internal void Win32ResizeDIBSection(int Width, int Height)
{
    //TODO: maybe free first , free after, then free first if fails
 

    if(BitmapMemory)
    {
        VirtualFree(BitmapMemory, 0, MEM_RELEASE);
    }

    BitmapWidth = Width;
    BitmapHeight = Height;

    // Filling out BitmapInfo header with important information
    BitmapInfo.bmiHeader.biSize = sizeof(BitmapInfo.bmiHeader);
    BitmapInfo.bmiHeader.biWidth = BitmapWidth;
    BitmapInfo.bmiHeader.biHeight = -BitmapHeight;
    BitmapInfo.bmiHeader.biPlanes = 1;
    BitmapInfo.bmiHeader.biBitCount = 32;
    BitmapInfo.bmiHeader.biCompression = BI_RGB;

    //int BytesPerPixel = 4;
    int BitmapMemorySize = (Width * Height) * BytesPerPixel;
    BitmapMemory = VirtualAlloc(0, BitmapMemorySize, MEM_COMMIT, PAGE_READWRITE);


    //TODO: Possibly clear to black. It kind of seem like it does already but who knows.

}

// Called in the paint message in the callback, used to take what is in our buffer and 
// put it on the screen.
internal void Win32UpdateWindow(HDC DeviceContext, RECT *ClientRect, int X, int Y, int Width, int Height)
{
    int WindowWidth = ClientRect->right - ClientRect->left;
    int WindowHeight = ClientRect->bottom - ClientRect->top;
    StretchDIBits(DeviceContext,
                //   X, Y, Width, Height,          // Destination
                //   X, Y, Width, Height,          // Source
                  0, 0, BitmapWidth, BitmapHeight,
                  0, 0, WindowWidth, WindowHeight,
                  BitmapMemory, 
                  &BitmapInfo, DIB_RGB_COLORS, SRCCOPY);
}

// Processes the messages sent to the window
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

            RECT ClientRect;
            GetClientRect(Window, &ClientRect);

            Win32UpdateWindow(DeviceContext, &ClientRect, X, Y, Width, Height);
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


// Entry point into the program
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
        HWND Window = CreateWindowExA(0,
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
        if(Window)
        {
            isRunning = true;
            int XOffset = 0;
            int YOffset = 0;
            while(isRunning) //loop forever till we get a positve message from reuslt
            {
                
                MSG Message;
                while(PeekMessageA(&Message, 0, 0, 0, PM_REMOVE))
                {

                    if(Message.message == WM_QUIT)
                    {
                        isRunning = false;
                    }
                    TranslateMessage(&Message);
                    DispatchMessageA(&Message);
                }
                RenderGradient(XOffset, YOffset);
                HDC DeviceContext = GetDC(Window);

                RECT ClientRect;
                GetClientRect(Window, &ClientRect);
                int WindowWidth = ClientRect.right - ClientRect.left;
                int WindowHeight = ClientRect.bottom - ClientRect.top;
                Win32UpdateWindow(DeviceContext, &ClientRect, 0, 0, WindowWidth, WindowHeight);
                ReleaseDC(Window, DeviceContext);
                ++XOffset;
                --YOffset;
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