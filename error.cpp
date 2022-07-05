#include <windows.h>
#include <tchar.h>
#include <cstdio>
#include "error.hpp"


char *get_api_err_msg()
{
    DWORD dwErr = GetLastError();
    char *wszMsgBuff = new char[512];  // Buffer for text.

    DWORD   dwChars;  // Number of chars returned.

    // Try to get the message from the system errors.
    dwChars = FormatMessage( FORMAT_MESSAGE_FROM_SYSTEM |
                             FORMAT_MESSAGE_IGNORE_INSERTS,
                             NULL,
                             dwErr,
                             0,
                             wszMsgBuff,
                             512,
                             NULL );

    if (0 == dwChars)
    {
        // The error code did not exist in the system errors.
        // Try Ntdsbmsg.dll for the error code.

        HINSTANCE hInst;

        // Load the library.
        hInst = LoadLibrary(TEXT("Ntdsbmsg.dll"));
        if ( NULL == hInst )
        {
            strcpy(wszMsgBuff, "cannot load Ntdsbmsg.dll");
            return wszMsgBuff;
        }

        // Try getting message text from ntdsbmsg.
        dwChars = FormatMessage( FORMAT_MESSAGE_FROM_HMODULE |
                                 FORMAT_MESSAGE_IGNORE_INSERTS,
                                 hInst,
                                 dwErr,
                                 0,
                                 wszMsgBuff,
                                 512,
                                 NULL );

        // Free the library.
        FreeLibrary( hInst );

    }

    if (dwChars == 0)
        strcpy(wszMsgBuff, "Error message not found");
    return wszMsgBuff;
}