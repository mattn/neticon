/******************************************************************************
    �l�b�g�A�C�R��
        �l�b�g�A�C�R�� �\�[�X�t�@�C��
            Copyright(C) 2007 Wraith.   All rights reserved.
                                            Coded by Wraith in Jul 17, 2007.
******************************************************************************/

///////////////////////////////////////////////////////////////////////////////
//
//  �� neticon.cpp
//      http://tricklib.com/cxx/ex/neticon/neticon.cpp
//
//  �� �֘A�t�@�C��
//      ���\�[�X�w�b�_�t�@�C��
//      http://tricklib.com/cxx/ex/neticon/neticon.rh
//      ���\�[�X�t�@�C��
//      http://tricklib.com/cxx/ex/neticon/neticon.rc
//      �A�v���P�[�V�����A�C�R��
//      http://tricklib.com/cxx/ex/neticon/neticon.ico
//      �f�t�H���g�l�b�g�A�C�R��
//      http://tricklib.com/cxx/ex/neticon/svc.ico
//      "start"�A�C�R��
//      http://tricklib.com/cxx/ex/neticon/start.ico
//      "stop"�A�C�R��
//      http://tricklib.com/cxx/ex/neticon/stop.ico
//      "log"�A�C�R��
//      http://tricklib.com/cxx/ex/neticon/log.ico
//      "����"�A�C�R��
//      http://tricklib.com/cxx/ex/neticon/close.ico
//      "OK"�I�[�o�[���C�A�C�R��
//      http://tricklib.com/cxx/ex/neticon/ok.ico
//      "NG"�I�[�o�[���C�A�C�R��
//      http://tricklib.com/cxx/ex/neticon/ng.ico
//      �`�F�b�N�}�[�N�I�[�o�[���C�A�C�R��
//      http://tricklib.com/cxx/ex/neticon/check.ico
//      �S�t�@�C���p�b�N
//      http://tricklib.com/cxx/ex/neticon/neticon.zip
//
//  �� ���t�@�����X�E�T�|�[�g�y�[�W
//      http://tricklib.com/cxx/ex/neticon/
//
//  �� ���C�Z���X���
//      http://tricklib.com/license.htm
//
//  �� �R���p�C�����@
//
//      case Borland C++
//          bcc32 -tWM -O1 -c neticon.cpp
//          brcc32 neticon.rc
//          ilink32 -aa neticon.obj, neticon.exe, , c0w32.obj import32.lib cw32mt.lib, , neticon.res
//
//      case Visual C++
//          rc neticon.rc
//          cl /EHsc /O1 neticon.cpp /link /machine:IX86 /SUBSYSTEM:WINDOWS neticon.res
//

#include <assert.h>
#include <winsock2.h> // windows.h ����ɃC���N���[�h���Ȃ��� vc �ł͔ߎS�Ȃ��ƂɂȂ�B
#include <windows.h>
#include <commctrl.h>
#include <tchar.h>
#include <time.h>
#include <sys/timeb.h>
#include <string>
#include <map>

#if !defined(_WIN32_WINNT)
#define _WIN32_WINNT WINVER
#endif

#if 0x0501 <= WINVER
#define NETICON_IPV6_READY
#endif

#if defined(NETICON_IPV6_READY)
#include <ws2tcpip.h>
#endif

#if 0x0600 <= WINVER // 0x0600 <= _WIN32_WINNT
#if defined(__BORLANDC__)
#pragma warn -8017
#endif
#include <initguid.h>
#include <wincodec.h>
#if defined(__BORLANDC__)
#pragma warn .8017
#endif
#endif

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(X) (sizeof(X) /sizeof(X[0]))
#endif

#include "neticon.rh"

#if defined(__BORLANDC__)
#pragma comment(lib, "ws2_32.lib")
#endif

#if defined(_MSC_VER)
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "comctl32.lib")
#pragma comment(lib, "KERNEL32.lib")
#pragma comment(lib, "GDI32.lib")
#pragma comment(lib, "SHELL32.lib")
#pragma comment(lib, "USER32.lib")
#pragma comment(lib, "ADVAPI32.lib")
#if 0x0600 <= WINVER
#pragma comment(lib, "OLE32.lib")
#pragma comment(lib, "WindowsCodecs")
#endif
#endif

LPCWSTR application_name = L"�l�b�g �A�C�R��";

//////////////////////////////////////////////////////////////////////////////
//
//  for DEBUG and Error Information
//

void debug_print(const char *format, ...)
{
    char buffer[1024];
    va_list arg_list;
    va_start(arg_list, format);
    wvsprintfA(buffer, format, arg_list);
    va_end(arg_list);
    OutputDebugStringA(buffer);
}
void debug_print(const wchar_t *format, ...)
{
    wchar_t buffer[1024];
    va_list arg_list;
    va_start(arg_list, format);
    wvsprintfW(buffer, format, arg_list);
    va_end(arg_list);
    OutputDebugStringW(buffer);
}

const std::string make_error_messageA(DWORD error_code = GetLastError())
{
    LPVOID  message_buffer;
    FormatMessageA
    (
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
        NULL, error_code, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPSTR)&message_buffer, 0, NULL
    );
    const std::string result = (const char *)message_buffer;
    LocalFree(message_buffer);
    return result;
}

const std::wstring make_error_messageW(DWORD error_code = GetLastError())
{
    LPVOID  message_buffer;
    FormatMessageW
    (
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
        NULL, error_code, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPWSTR)&message_buffer, 0, NULL
    );
    const std::wstring result = (const wchar_t *)message_buffer;
    LocalFree(message_buffer);
    return result;
}
#if defined(_UNICODE) || defined(UNICODE)
#define make_error_message make_error_messageW
#else
#define make_error_message make_error_messageA
#endif


//////////////////////////////////////////////////////////////////////////////
//
//  Declare auto closers
//

template<class T> class demi
{
    public:
        T value;

        demi() {}
        demi(const T &X) :value(X) {}

        operator T& () { return value; }
        operator const T& () const { return value; }

        T * operator & () { return &value; }
        const T * operator & () const { return &value; }

        T & operator () () { return value; }
        const T & operator () () const { return value; }
};

#define DECLARE_AUTO_CLOSE(name, type, param, init, close_command) \
class name :public demi<type> \
{ \
  public: \
    name(param) :demi<type>(init) { } \
    ~name() \
    { \
        close(); \
    } \
    void close() \
    { \
        if (value) \
        { \
            close_command; \
            value = NULL; \
        } \
    } \
};

DECLARE_AUTO_CLOSE(AUTO_COMPATIBLE_DC, HDC, HDC dc, CreateCompatibleDC(dc), DeleteDC(value))

class AUTO_WINDOW_DC :public demi<HDC>
{
  public:
    HWND window;
    AUTO_WINDOW_DC(HWND a_window = 0) :demi<HDC>(GetWindowDC(a_window)), window(a_window) { }
    ~AUTO_WINDOW_DC()
    {
        close();
    }
    void close()
    {
        if (value)
        {
            ReleaseDC(window, value);
            value = NULL;
        }
    }
};


//////////////////////////////////////////////////////////////////////////////
//
//  Windows Version Checker
//

class AUTO_OSVERSIONINFO :public OSVERSIONINFO
{
public:
    AUTO_OSVERSIONINFO()
    {
        dwOSVersionInfoSize = sizeof(*this);
        GetVersionEx(this);
    }
    bool is_target_or_later(DWORD major_vesion, DWORD minor_version = 0)
    {
        return (major_vesion == dwMajorVersion && minor_version <= dwMinorVersion) || major_vesion < dwMajorVersion;
    }
    bool is_windows_vista_or_later()
    {
        return is_target_or_later(6, 0);
    }
    bool is_windows_xp_or_later()
    {
        return is_target_or_later(5, 1);
    }
};


///////////////////////////////////////////////////////////////////////////////
//
//  �ʐM
//

namespace net
{
    typedef struct ip_option_information
    {
        UCHAR   Ttl;                // Time To Live
        UCHAR   Tos;                // Type Of Service
        UCHAR   Flags;              // IP header flags
        UCHAR   OptionsSize;        // Size in bytes of options data
        PUCHAR  OptionsData;        // Pointer to options data
    } IP_OPTION_INFORMATION, *PIP_OPTION_INFORMATION;

    typedef struct
    {
        DWORD           Address;        // Replying address
        unsigned long   Status;         // Reply status
        unsigned long   RoundTripTime;  // RTT in milliseconds
        unsigned short  DataSize;       // Echo data size
        unsigned short  Reserved;       // Reserved for system use
        void *          Data;           // Pointer to the echo data
        IP_OPTION_INFORMATION Options;  // Reply options
    } IP_ECHO_REPLY, *PIP_ECHO_REPLY;
    
    HINSTANCE icmp_lib = NULL;
    HINSTANCE iphlpapi_lib = NULL;
    
    typedef HANDLE  (WINAPI* IcmpCreateFile_PROC_TYPE)  (VOID);
    typedef DWORD   (WINAPI* IcmpSendEcho_PROC_TYPE)    (HANDLE, DWORD, LPVOID, WORD, PIP_OPTION_INFORMATION, LPVOID, DWORD, DWORD);
    typedef BOOL    (WINAPI* IcmpCloseHandle_PROC_TYPE) (HANDLE);
    IcmpCreateFile_PROC_TYPE    IcmpCreateFile  = NULL;
    IcmpSendEcho_PROC_TYPE      IcmpSendEcho    = NULL;
    IcmpCloseHandle_PROC_TYPE   IcmpCloseHandle = NULL;

    typedef HANDLE  (WINAPI* Icmp6CreateFile_PROC_TYPE)  (VOID);
    typedef DWORD   (WINAPI* Icmp6SendEcho2_PROC_TYPE)    (HANDLE, HANDLE, FARPROC, PVOID, sockaddr *, sockaddr *, LPVOID, WORD, PIP_OPTION_INFORMATION, LPVOID, DWORD, DWORD);
    Icmp6CreateFile_PROC_TYPE   Icmp6CreateFile = NULL;
    Icmp6SendEcho2_PROC_TYPE    Icmp6SendEcho2  = NULL;
    
#if defined(NETICON_IPV6_READY)
    typedef ADDRINFO *addr6_type;
#endif
    typedef unsigned int addr4_type;
    
    class lib_power
    {
    public:
        lib_power()
        {
            WSADATA wsa;
            if (0 != WSAStartup(0x0101, &wsa))
            {
                OutputDebugString("WSAStartup: faild");
                OutputDebugString(make_error_message(WSAGetLastError()).c_str());
            }
            
            icmp_lib = LoadLibrary("ICMP.DLL");
            if (icmp_lib)
            {
                IcmpCreateFile  = (IcmpCreateFile_PROC_TYPE)    GetProcAddress(icmp_lib,"IcmpCreateFile");
                IcmpSendEcho    = (IcmpSendEcho_PROC_TYPE)      GetProcAddress(icmp_lib,"IcmpSendEcho");
                IcmpCloseHandle = (IcmpCloseHandle_PROC_TYPE)   GetProcAddress(icmp_lib,"IcmpCloseHandle");
            }
            
            iphlpapi_lib = LoadLibrary("IPHLPAPI.DLL");
            if (iphlpapi_lib)
            {
                Icmp6CreateFile = (Icmp6CreateFile_PROC_TYPE)   GetProcAddress(iphlpapi_lib,"Icmp6CreateFile");
                Icmp6SendEcho2  = (Icmp6SendEcho2_PROC_TYPE)    GetProcAddress(iphlpapi_lib,"Icmp6SendEcho2");
            }
        }
        ~lib_power()
        {
            WSACleanup();
        }
    };
    
#if defined(NETICON_IPV6_READY)
    addr6_type get_addr6(const char *hostname, const char *servicename = NULL)
    {
        ADDRINFO hints = { 0, };
        hints.ai_flags = 0;
        hints.ai_family = AF_UNSPEC;
        addr6_type result = NULL;
        getaddrinfo(hostname, servicename, &hints, &result);
        return result;
    }
    addr6_type get_addr6(const char *hostname, int portnum)
    {
        char buffer[32];
        wsprintfA(buffer, "%d", portnum);
        return get_addr6(hostname, buffer);
    }
#endif
    addr4_type get_addr4(const char *hostname)
    {
        addr4_type addr = inet_addr(hostname);
        if (INADDR_NONE == addr)
        {
            struct hostent *host_data = gethostbyname(hostname);
            if (host_data) {
                addr = *((addr4_type *)(host_data->h_addr_list[0]));
            }
        }
        return addr;
    }

#if defined(NETICON_IPV6_READY)
    bool ping(addr6_type from_addr, addr6_type to_addr, DWORD wait = 4000, int *RTT = NULL, int *TTL = NULL)
    {
        //
        //  ICMP.DLL ����� IPHLPAPI.DLL �̃��[�h�`�F�b�N
        //
        if (NULL == icmp_lib)
        {
            OutputDebugString("LoadLibrary(\"icmp.dll\"): faild");
            return false;
        }
        if (NULL == iphlpapi_lib)
        {
            OutputDebugString("LoadLibrary(\"iphlpapi.dll\"): faild");
            return false;
        }
        
        if (NULL == Icmp6CreateFile)
        {
            OutputDebugString("GetProcAddress(LoadLibrary(\"iphlpapi.dll\"), \"Icmp6CreateFile\"): faild");
            return false;
        }
        if (NULL == Icmp6SendEcho2)
        {
            OutputDebugString("GetProcAddress(LoadLibrary(\"iphlpapi.dll\"), \"Icmp6SendEcho2\"): faild");
            return false;
        }
        if (NULL == IcmpCloseHandle)
        {
            OutputDebugString("GetProcAddress(LoadLibrary(\"icmp.dll\"), \"IcmpCloseHandle\"): faild");
            return false;
        }

        //
        //  open
        //
        HANDLE icmp_handle = Icmp6CreateFile();
        if (icmp_handle == INVALID_HANDLE_VALUE)
        {
            OutputDebugString("Icmp6CreateFile(): faild");
            return false;
        }

        //
        //  PING�p�P�b�g����
        //
#if 0
//  Windows XP �� Icmp6SendEcho2() ���Ăяo�����ۂɈُ�I�����������������ׂ̃q���g�ɂȂ邩������Ȃ��̂Ŏc���Ă����B
        struct MY_IP_ECHO_REPLY
        {
            //  IPV6_ADDRESS_EX
            USHORT sin6_port;
            ULONG sin6_flowinfo;
            USHORT sin6_addr[8];
            ULONG sin6_scope_id;
            
            //  ICMPV6_ECHO_REPLY
            ULONG Status;
            unsigned int RoundTripTime;
            
            //  MY_IP_ECHO_REPLY
            BYTE buffer[250];
        };
#else
        struct MY_IP_ECHO_REPLY :public IP_ECHO_REPLY
        {
            BYTE buffer[250];
        };
#endif
        MY_IP_ECHO_REPLY ping_packet;
        ZeroMemory(&ping_packet, sizeof(ping_packet));
        BYTE ping_packet_data[32] = "Hello";
        
        //
        //  PING�p�P�b�g���M
        //
        DWORD dwStatus = 0;
        for(addr6_type rp = to_addr; NULL != rp && !dwStatus; rp = rp->ai_next)
        {
            dwStatus = Icmp6SendEcho2(icmp_handle, NULL, NULL, NULL, from_addr->ai_addr, rp->ai_addr, ping_packet_data, sizeof(ping_packet_data), NULL, &ping_packet, sizeof(ping_packet), wait);
        }
        
        //
        //  ...
        //
        bool result = dwStatus;
        if (result)
        {
#if 0
//  Windows XP �� Icmp6SendEcho2() ���Ăяo�����ۂɈُ�I�����������������ׂ̃q���g�ɂȂ邩������Ȃ��̂Ŏc���Ă����B
            if (RTT)
            {
                *RTT = (int)(ping_packet.RoundTripTime);
            }
            if (TTL)
            {
                *TTL = ((IP_ECHO_REPLY*)&ping_packet)->Options.Ttl;
            }
#else
            if (RTT)
            {
                *RTT = (int)(ping_packet.RoundTripTime);
            }
            if (TTL)
            {
                *TTL = (int)(ping_packet.Options.Ttl);
            }
#endif
        }
        else
        {
            DWORD error = GetLastError();
            debug_print("0x%08.8X:%s", error, make_error_messageA().c_str());
        }
        
        //
        //  close
        //
        IcmpCloseHandle(icmp_handle);
        
        //
        //  ...
        //
        return result;
    }
#endif
    bool ping(addr4_type addr, DWORD wait = 4000, int *RTT = NULL, int *TTL = NULL)
    {
        //
        //  ICMP.DLL �̃��[�h�`�F�b�N
        //
        if (NULL == icmp_lib)
        {
            OutputDebugString("LoadLibrary(\"icmp.dll\"): faild");
            return false;
        }
        if (NULL == IcmpCreateFile)
        {
            OutputDebugString("GetProcAddress(LoadLibrary(\"icmp.dll\"), \"IcmpCreateFile\"): faild");
            return false;
        }
        if (NULL == IcmpSendEcho)
        {
            OutputDebugString("GetProcAddress(LoadLibrary(\"icmp.dll\"), \"IcmpSendEcho\"): faild");
            return false;
        }
        if (NULL == IcmpCloseHandle)
        {
            OutputDebugString("GetProcAddress(LoadLibrary(\"icmp.dll\"), \"IcmpCloseHandle\"): faild");
            return false;
        }

        //
        //  open
        //
        HANDLE icmp_handle = IcmpCreateFile();
        if (icmp_handle == INVALID_HANDLE_VALUE)
        {
            OutputDebugString("IcmpCreateFile(): faild");
            return false;
        }

        //
        //  PING�p�P�b�g����
        //
        struct MY_IP_ECHO_REPLY :public IP_ECHO_REPLY
        {
            BYTE buffer[250];
        };
        MY_IP_ECHO_REPLY ping_packet;
        ZeroMemory(&ping_packet, sizeof(ping_packet));
        BYTE ping_packet_data[32] = "Hello";
        
        //
        //  PING�p�P�b�g���M
        //
        DWORD dwStatus = IcmpSendEcho(icmp_handle, *((DWORD*)&addr), ping_packet_data, sizeof(ping_packet_data), NULL, &ping_packet, sizeof(ping_packet), wait);
        
        //
        //  ...
        //
        bool result = dwStatus;
        if (result)
        {
            if (RTT)
            {
                *RTT = (int)(ping_packet.RoundTripTime);
            }
            if (TTL)
            {
                *TTL = (int)(ping_packet.Options.Ttl);
            }
        }
        
        //
        //  close
        //
        IcmpCloseHandle(icmp_handle);
        
        //
        //  ...
        //
        return result;
    }

#if defined(NETICON_IPV6_READY)
    bool port_scan(addr6_type addr)
    {
        bool result = false;
        if (NULL != addr)
        {
            for (addr6_type rp = addr; NULL != rp && !result; rp = rp->ai_next)
            {
                int socket_handle = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
                if (0 <= socket_handle)
                {
                    if (0 <= connect(socket_handle, rp->ai_addr, rp->ai_addrlen))
                    {
                        result = true;
                        shutdown(socket_handle, SD_BOTH);
                    }
                    closesocket(socket_handle);
                }
            }
        }
        return result;
    }
#endif
    bool port_scan(addr4_type addr, int portnum)
    {
        bool result = false;
        if (INADDR_NONE != addr)
        {
            int socket_handle = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
            if (0 <= socket_handle)
            {
                struct sockaddr_in sai;
                sai.sin_family = AF_INET;
                sai.sin_addr.s_addr = addr;
                sai.sin_port = htons(portnum);
                if (0 <= connect(socket_handle, (const struct sockaddr *)&sai, sizeof(sai)))
                {
                    result = true;
                    shutdown(socket_handle, SD_BOTH);
                }
                closesocket(socket_handle);
            }
        }
        return result;
    }
}

class watch_base
{
public:
    bool last_result;
    watch_base() :last_result(false)
    {
    }
    virtual ~watch_base()
    {
    }
    
    bool check()
    {
        return last_result = check_target();
    }
    virtual bool check_target() = 0;
};


class ping_watch :public watch_base
{
public:
    LPCSTR hostname;
    ping_watch(LPCSTR a_hostname) :hostname(a_hostname)
    {
    }
    bool check_target()
    {
        bool result = false;
#if 0 && defined(NETICON_IPV6_READY)
//  ���̃v���O������ IPv6 �x�[�X�� ping �͂܂��s����Ȃ̂� ping �Ɋւ��Ă͂��΂� IPv4 �Œ�̂܂܂ɂ��Ă����B
        if (AUTO_OSVERSIONINFO().is_windows_xp_or_later())
        {
            net::addr6_type from_addr = net::get_addr6("");
            if (NULL != from_addr)
            {
                net::addr6_type to_addr = net::get_addr6(hostname);
                if (NULL != to_addr)
                {
                    result = net::ping(from_addr, to_addr, 1500);
                    freeaddrinfo(to_addr);
                }
                freeaddrinfo(from_addr);
            }
        }
        else
        {
#endif
            net::addr4_type addr = net::get_addr4(hostname);
            if (INADDR_NONE != addr)
            {
                result = net::ping(addr, 1500);
            }
#if 0 && defined(NETICON_IPV6_READY)
//  ���̃v���O������ IPv6 �x�[�X�� ping �͂܂��s����Ȃ̂� ping �Ɋւ��Ă͂��΂� IPv4 �Œ�̂܂܂ɂ��Ă����B
        }
#endif
        return result;
    }
};
class port_watch :public ping_watch
{
public:
    UINT portnum;
    port_watch(LPCSTR a_hostname, UINT a_portnum) :ping_watch(a_hostname), portnum(a_portnum)
    {
    }
    bool check_target()
    {
        bool result = false;
#if defined(NETICON_IPV6_READY)
        if (AUTO_OSVERSIONINFO().is_windows_xp_or_later())
        {
            net::addr6_type addr = net::get_addr6(hostname, portnum);
            if (NULL != addr)
            {
                result = net::port_scan(addr);
                freeaddrinfo(addr);
            }
        }
        else
        {
#endif
            net::addr4_type addr = net::get_addr4(hostname);
            if (INADDR_NONE != addr)
            {
                result = net::port_scan(addr, portnum);
            }
#if defined(NETICON_IPV6_READY)
        }
#endif
        return result;
    }
};


///////////////////////////////////////////////////////////////////////////////
//
//  GDI
//

UINT get_ILC_color_flag(HDC dc)
{
    int color_resolution = GetDeviceCaps(dc, COLORRES);
    
    if (32 <= color_resolution)
    {
        return ILC_COLOR32;
    }
    else
    if (24 <= color_resolution)
    {
        return ILC_COLOR24;
    }
    else
    if (16 <= color_resolution)
    {
        return ILC_COLOR16;
    }
    else
    if (8 <= color_resolution)
    {
        return ILC_COLOR8;
    }
    else
    if (4 <= color_resolution)
    {
        return ILC_COLOR4;
    }
    else
    {
        return ILC_COLOR;
    }
}


///////////////////////////////////////////////////////////////////////////////
//
//  �A�C�R��
//

HICON create_merge_icon(SIZE icon_size, HICON icon1, HICON icon2)
{
#if 0
// �A���t�@�������ރo�[�W����
    //  �C���[�W���X�g���g���ăA�C�R�����}�[�W
    HIMAGELIST source_image = ImageList_Create(icon_size.cx, icon_size.cy, get_ILC_color_flag(AUTO_WINDOW_DC()) |ILC_MASK, 1, 1);
#else
// �A���t�@�������ޖ��̉��΍�(��������)�o�[�W����
    // Windows XP �� ILC_COLOR32 ���w�肷��ƃI�[�o�[���C�C���[�W����镔�����������肷��E�E�E�Ȃ��H
    HIMAGELIST source_image = ImageList_Create(icon_size.cx, icon_size.cy, ILC_COLOR24 |ILC_MASK, 2, 1);
    ImageList_SetBkColor(source_image, 0x00DDDDDD);
#endif
    ImageList_AddIcon(source_image, icon1);
    ImageList_AddIcon(source_image, icon2);
    HIMAGELIST merge_image = ImageList_Merge(source_image, 0, source_image, 1, 0, 0);
    HICON merge_icon = ImageList_GetIcon(merge_image, 0, ILD_TRANSPARENT);
    ImageList_Destroy(source_image);
    ImageList_Destroy(merge_image);
    return merge_icon;
}

#if 0x0600 <= WINVER

//
//  �ȉ��� InitBitmapInfo(), Create32BitHBITMAP(), IconToBitmap() �̃p�N������...
//  http://shellrevealed.com/blogs/shellblog/archive/2007/02/06/Vista-Style-Menus_2C00_-Part-1-_2D00_-Adding-icons-to-standard-menus.aspx
//

void InitBitmapInfo(__out_bcount(cbInfo) BITMAPINFO *pbmi, ULONG cbInfo, LONG cx, LONG cy, WORD bpp)
{
    ZeroMemory(pbmi, cbInfo);
    pbmi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    pbmi->bmiHeader.biPlanes = 1;
    pbmi->bmiHeader.biCompression = BI_RGB;

    pbmi->bmiHeader.biWidth = cx;
    pbmi->bmiHeader.biHeight = cy;
    pbmi->bmiHeader.biBitCount = bpp;
}

HRESULT Create32BitHBITMAP(HDC hdc, const SIZE *psize, __deref_opt_out void **ppvBits, __out HBITMAP* phBmp)
{
    *phBmp = NULL;

    BITMAPINFO bmi;
    InitBitmapInfo(&bmi, sizeof(bmi), psize->cx, psize->cy, 32);

    HDC hdcUsed = hdc ? hdc : GetDC(NULL);
    if (hdcUsed)
    {
        *phBmp = CreateDIBSection(hdcUsed, &bmi, DIB_RGB_COLORS, ppvBits, NULL, 0);
        if (hdc != hdcUsed)
        {
            ReleaseDC(NULL, hdcUsed);
        }
    }
    return (NULL == *phBmp) ? E_OUTOFMEMORY : S_OK;
}

HBITMAP IconToBitmap(HICON icon)
{
    HBITMAP hbmp = NULL;
    IWICImagingFactory *pFactory;
    HRESULT hr = CoCreateInstance(CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER, IID_IWICImagingFactory,(LPVOID*)&pFactory);
    
    if (SUCCEEDED(hr))
    {
        IWICBitmap *pBitmap;
        hr = pFactory->CreateBitmapFromHICON(icon, &pBitmap);
        if (SUCCEEDED(hr))
        {
            UINT cx, cy;
            hr = pBitmap->GetSize(&cx, &cy);
            if (SUCCEEDED(hr))
            {
                const SIZE sizIcon = { (int)cx, -(int)cy };
                BYTE *pbBuffer;
                hr = Create32BitHBITMAP(NULL, &sizIcon, reinterpret_cast<void **>(&pbBuffer), &hbmp);
                if (SUCCEEDED(hr))
                {
                    const UINT cbStride = cx * 4;// == sizeof(ARGB);
                    const UINT cbBuffer = cy * cbStride;
                    hr = pBitmap->CopyPixels(NULL, cbStride, cbBuffer, pbBuffer);
                    if (SUCCEEDED(hr))
                    {
                    }
                    else
                    {
                        DeleteObject(hbmp);
                        hbmp = NULL;
                    }
                }
            }
            pBitmap->Release();
        }
        pFactory->Release();
    }
    return hbmp;
}
#endif


///////////////////////////////////////////////////////////////////////////////
//
//  �ʒm�A�C�R��
//

const UINT WM_NOTIFYICON_BASE = WM_APP;

BOOL control_notify_icon(DWORD message, HWND owner, UINT id, LPCWSTR caption = NULL, HICON icon = NULL)
{
    NOTIFYICONDATAW notify_icon_data =
    {
        sizeof(NOTIFYICONDATA),
        owner,
        id,
        ((NIM_DELETE != message) ? NIF_MESSAGE: 0) | ((caption) ? NIF_TIP: 0) | ((icon) ? NIF_ICON: 0),
        WM_NOTIFYICON_BASE +id,
        icon,
    };
    if (caption)
    {
        assert((int)wcslen(caption) < (int)ARRAY_SIZE(notify_icon_data.szTip));
        lstrcpyW(notify_icon_data.szTip, caption);
    }
    
    BOOL result;
    int try_count = 0;
    const int max_try_count = 150;
    do
    {
        result = Shell_NotifyIconW(message, &notify_icon_data);
    }
    while
    (
        FALSE == result &&
//      ERROR_TIMEOUT == GetLastError() && ��Shell_NotifyIcon()�̖߂�l�� FALSE �Ȃ̂� GetLastError() �� ERROR_SUCCESS ��Ԃ����Ƃ�����̂Ń`�F�b�N�͂��Ȃ��B
        NIM_ADD == message &&
        FALSE == (result = Shell_NotifyIconW(NIM_MODIFY, &notify_icon_data)) &&
        ++try_count < max_try_count &&
        (Sleep(1000), true)
    );
    //  cf. KB418138
    
    return result;
}


///////////////////////////////////////////////////////////////////////////////
//
//  net icon
//

namespace net_icon
{
    //
    //  system
    //
    HINSTANCE hInstance = NULL;
    LPCTSTR lpszClassName = _T("notify_icon_owner");
#if 0x0600 <= WINVER
    HMODULE uxtheme_dll = NULL;
    BOOL (*IsThemeActive)() = NULL;
    bool is_enabled_theme = false;
#endif
    bool on_auto_check = false;
    volatile bool on_output_log = false;
    const UINT TIMER_ID = 100;
    
    //
    //  notify icon
    //
    UINT WM_taskbarcreated = WM_APP;
    const UINT NOTIFYICON_ID = 10;

    //
    //  commandline
    //
    LPCWSTR this_file_name = NULL;
    LPCWSTR icon_file_name = NULL;
    LPCWSTR icon_index = NULL;
    LPCWSTR display_name = NULL;
    std::wstring display_message;
    LPCWSTR port_str = NULL;
    LPCWSTR interval_str = NULL;
    WCHAR host_name[1024];
    char ansi_host_name[1024];
    UINT interval = 1000;

    //
    //  icon
    //
    SIZE smallicon_size;
    HICON base_icon = NULL;
    std::map<DWORD, HICON> icon_table;
    HICON checkmark_icon = NULL;
    
    //
    //  net
    //
    watch_base *target_net = NULL;
    std::map<DWORD, LPCWSTR> text_table;

    //
    //  thread
    //
    volatile HANDLE thread_handle = 0;
    volatile bool continue_watch = false;

    
#if 0x0600 <= WINVER
    inline bool is_theme_active()
    {
        return IsThemeActive && IsThemeActive();
    }
#endif
    inline HICON load_icon(int id)
    {
        return (HICON)LoadImage(hInstance, MAKEINTRESOURCE(id), IMAGE_ICON, smallicon_size.cx, smallicon_size.cy, LR_DEFAULTCOLOR);
    }

    inline HICON make_status_icon(DWORD icon_id)
    {
        return create_merge_icon(smallicon_size, base_icon, load_icon(icon_id));
    }
    HWND create()
    {
        //
        //  �T�[�r�X�̎擾
        //
        if (NULL == port_str)
        {
            target_net = new ping_watch(ansi_host_name);
        }
        else
        {
            target_net = new port_watch(ansi_host_name, _wtoi(port_str));
        }
        
        if (!target_net)
        {
            WCHAR buffer[1024];
            wsprintfW(buffer, L"�w�肳�ꂽ�T�[�r�X %s ���J���܂���ł����B\n%s", display_name, make_error_messageW().c_str());
            MessageBoxW(NULL, buffer, application_name, MB_ICONSTOP |MB_OK);
            return NULL;
        }
        
        //
        //  �X�e�[�^�X�\���p�A�C�R���̍쐬
        //
        icon_table[false]     = make_status_icon(STATUS_NG_ICON);
        icon_table[true]      = make_status_icon(STATUS_OK_ICON);
        
        //
        //  �X�e�[�^�X�\���p�e�L�X�g�̏���
        //
        text_table[false]     = L"[NG]";
        text_table[true]      = L"[OK]";
        
        //
        //  ���j���[�p�A�C�R���̐ݒ�
        //
        checkmark_icon = load_icon(MENU_CHECKMARK_ICON);
        HMENU window_menu = LoadMenu(hInstance, MAKEINTRESOURCE(NOTIFYICON_MENU));
        HMENU notify_icon_menu = GetSubMenu(window_menu, 0);
        MENUITEMINFO mii = { sizeof(MENUITEMINFO), };
        int i = 0;
#if 0x0600 <= WINVER
        is_enabled_theme = AUTO_OSVERSIONINFO().is_windows_vista_or_later() && is_theme_active();
#endif
        while(true)
        {
            mii.fMask = MIIM_ID |MIIM_FTYPE;
            if (!GetMenuItemInfo(notify_icon_menu, i, TRUE, &mii))
            {
                break;
            }
            
            if
            (
                MFT_SEPARATOR != (mii.fType &MFT_SEPARATOR)
            )
            {
                HICON icon = load_icon(mii.wID);
                if (!icon)
                {
                    OutputDebugString("LoadImage: faild");
                    OutputDebugString(make_error_message().c_str());
                    continue;
                }
                mii.fMask = MIIM_DATA |MIIM_BITMAP;
                mii.dwItemData = (DWORD)icon; // ��mii.hbmpItem �Ńr�b�g�}�b�v���g���ꍇ�ɂ͂���Ȃ����AWM_THEMECHANGED �ɔ����ĕۑ����Ă����B
#if 0x0600 <= WINVER
                if (is_enabled_theme)
                {
                    mii.hbmpItem = IconToBitmap(icon);
                }
                else
#endif
                {
                    mii.hbmpItem = HBMMENU_CALLBACK;
                }
                if (!SetMenuItemInfo(notify_icon_menu, i, TRUE, &mii))
                {
                    OutputDebugString("SetMenuItemInfo: faild");
                    OutputDebugString(make_error_message().c_str());
                }
            }
            ++i;
        }
        
        //
        //  �E�B���h�E�̍쐬
        //
        return CreateWindow
        (
            lpszClassName,
            _T("�l�b�g�A�C�R��"),
            WS_POPUPWINDOW,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            NULL,
            window_menu,
            hInstance,
            NULL
        );
    }
    
    //
    //  �ʒm�A�C�R��TIP�p�e�L�X�g�̍쐬
    //
    LPCWSTR make_icon_caption(DWORD current_status)
    {
        //const int max_caption_length = 64; // ARRAYA_SIZE(((NOTIFYICONDATA*)0)->szTip);
        //  �V�F���̃o�[�W�����ɂ���Ă�128�����܂ň����邱�ƂɂȂ��Ă���̂���
        //  ���ۂɂ�64�����܂ł����\������Ȃ��E�E�E�Ȃɂ��ǉ��Ńt���O���̎w��
        //  ���K�v�H
        
        display_message = host_name;
        if (port_str)
        {
            display_message += L":";
            display_message += port_str;
        }
        std::map<DWORD, LPCWSTR>::const_iterator text_i = text_table.find(current_status);
        if (text_i != text_table.end())
        {
            display_message = text_i->second +(L" " +display_message);
        }
        else
        {
            display_message = L"(�L�E�ցE�M)�H " +display_message;
        }
        return display_message.c_str();
    }
    
    //
    //  ���j���[������Ԃ̍X�V
    //
    void update_menu_status(HWND hwnd)
    {
        HMENU window_menu = GetMenu(hwnd);
        HMENU notify_icon_menu = GetSubMenu(window_menu, 0);
        EnableMenuItem(notify_icon_menu, DO_START_ICON, MF_BYCOMMAND |((!on_auto_check) ?MF_ENABLED :MF_GRAYED));
        EnableMenuItem(notify_icon_menu, DO_STOP_ICON, MF_BYCOMMAND |((on_auto_check) ?MF_ENABLED :MF_GRAYED));
    }
    
    //
    //  �ʒm�A�C�R���p�A�C�R���̎擾
    //
    HICON get_status_icon(DWORD status)
    {
        std::map<DWORD, HICON>::const_iterator icon_i = icon_table.find(status);
        if (icon_i != icon_table.end())
        {
            return icon_i->second;
        }
        else
        {
            return base_icon;
        }
    }
    
    //
    //  �ʒm�A�C�R���̍X�V
    //
    void update_status(HWND hwnd, bool current_status)
    {
        //
        //  �ʒm�A�C�R���̍X�V
        //
        control_notify_icon(NIM_MODIFY, hwnd, NOTIFYICON_ID, make_icon_caption(current_status), get_status_icon(current_status));
    }

    void purge_thread()
    {
        continue_watch = false;
        if (thread_handle)
        {
            if (WAIT_OBJECT_0 != WaitForSingleObject(thread_handle, 4000))
            {
                TerminateThread(thread_handle, 0);
            }
            CloseHandle(thread_handle);
            thread_handle = NULL;
        }
    }
    
    static DWORD WINAPI watch_thread_proc(LPVOID lpParam)
    {
        net::lib_power net_power_on;
        continue_watch = true;
        DWORD pace = GetTickCount();
        while(continue_watch)
        {
            //
            //  ��Ԋm�F�ƒʒm�A�C�R���ւ̔��f
            //
            update_status((HWND)lpParam, target_net->check());
            
            //
            //  ���O�o��
            //
            if (on_output_log)
            {
                struct timeb now;
                ftime(&now);
                const struct tm *tblock = localtime(&now.time);
                int year = (tblock->tm_year < 70) ? tblock->tm_year +2000: tblock->tm_year +1900;
                int month = tblock->tm_mon +1;
                int day = tblock->tm_mday;
                int hour = tblock->tm_hour;
                int min = tblock->tm_min;
                int sec = tblock->tm_sec;
                int millisec = now.millitm;
                debug_print(L"[neticon] %04.4d/%02.2d/%02.2d %02.2d:%02.2d:%02.2d.%03.3d %s", year, month, day, hour, min, sec, millisec, display_message.c_str());
            }
            
            //
            //  �C���^�[�o���y�[�X����
            //
            DWORD current_time = GetTickCount();
            do
            {
                pace += interval;
            }
            while(pace < current_time && interval <= pace);
            UINT rest = pace -GetTickCount();
            if ((rest +interval) < interval)
            {
                rest = 0 -rest;
            }
            while(0 < rest && continue_watch)
            {
                UINT current_wait = 300;
                if (rest < current_wait)
                {
                    current_wait = rest;
                }
                Sleep(current_wait);
                rest -= current_wait;
            }
        }
        return  0;
    }
    
    LRESULT CALLBACK window_procedure
    (
        HWND hwnd,
        UINT message,
        WPARAM wParam,
        LPARAM lParam
    )
    {
        switch(message)
        {
        
        case WM_CREATE:
            {
                if (control_notify_icon(NIM_ADD, hwnd, NOTIFYICON_ID, make_icon_caption(false), base_icon))
                {
                    PostMessage(hwnd, WM_COMMAND, MAKEWPARAM(DO_START_ICON, 0), NULL);
                }
                else
                {
                    MessageBoxW
                    (
                        hwnd,
                        L"�ʒm�A�C�R����o�^�ł��܂���ł����B",
                        application_name,
                        MB_ICONSTOP |MB_OK
                    );
                    PostMessage(hwnd, WM_CLOSE, 0, 0);
                }
            }
            break;

        case WM_MEASUREITEM:
            {
                MEASUREITEMSTRUCT *mi = (MEASUREITEMSTRUCT*)lParam;
                if (mi != NULL)
                {
                    int diff_cx = smallicon_size.cx -GetSystemMetrics(SM_CXMENUCHECK);
                    if (0 < diff_cx)
                    {
                        mi->itemWidth += diff_cx;
                    }
                    mi->itemHeight = smallicon_size.cy;
                    return TRUE;
                    //  ���j���[�̃I�[�i�[�h���[�܂��Ɋւ��Ă͐̂��� Windows �̎d�l��������������
                    //  �Ȃ̂ŏ����̊��ł��܂Ƃ��ɓ�������҂��邱�Ƃ͖����B���̓s�x�������K�v
                    //  �E�E�E�Ƃ��������܂̂��̍��W�v�Z�������̊����ׂĂŖ��Ȃ��\������邩
                    //  �Ƃ����Ƃ��Ԃ�A�m���BMENUITEMINFO::hbmpItem �Ńr�b�g�}�b�v���g���`������
                    //  �ς܂���̂��ȒP�����A�g�B
                }
            }
            break;

        case WM_DRAWITEM:
            {
                DRAWITEMSTRUCT *di = (DRAWITEMSTRUCT*)lParam;
                if (di != NULL)
                {
                    switch(di->CtlType)
                    {
                    case ODT_MENU:
                        {
                            if (di->itemData)
                            {
                                UINT is_checked = ODS_CHECKED == (di->itemState &ODS_CHECKED);
                                UINT is_disabled = ODS_DISABLED == (di->itemState &ODS_DISABLED);
#if 0
                                if (is_checked)
                                {
                                    RECT EdgeRect;
                                    EdgeRect.left = di->rcItem.left +(di->rcItem.right -di->rcItem.left) -smallicon_size.cx -4;
                                    EdgeRect.top = di->rcItem.top +(di->rcItem.bottom -di->rcItem.top -smallicon_size.cy) /2 -1;
                                    EdgeRect.right = EdgeRect.left +smallicon_size.cx;
                                    EdgeRect.bottom = EdgeRect.top +smallicon_size.cy +2;
                                    FillRect
                                    (
                                        di->hDC,
                                        &EdgeRect,
                                        GetSysColorBrush(COLOR_HIGHLIGHT)
                                    );
                                    DrawEdge
                                    (
                                        di->hDC,
                                        &EdgeRect,
                                        BDR_SUNKENINNER,
                                        BDR_SUNKENOUTER
                                    );
                                }
#endif
                                DrawState
                                (
                                    di->hDC,
                                    GetSysColorBrush(COLOR_MENU),
                                    NULL,
                                    (LPARAM)(di->itemData),
                                    (WPARAM)0,
                                    di->rcItem.left +(di->rcItem.right -di->rcItem.left) -smallicon_size.cx -4,
                                    di->rcItem.top +(di->rcItem.bottom -di->rcItem.top -smallicon_size.cy) /2,
                                    smallicon_size.cx,
                                    smallicon_size.cy,
                                    DST_ICON |DSS_NORMAL |(is_disabled ? DSS_DISABLED: 0)
                                );
                                if (is_checked)
                                {
                                    DrawState
                                    (
                                        di->hDC,
                                        GetSysColorBrush(COLOR_MENU),
                                        NULL,
                                        (LPARAM)checkmark_icon,
                                        (WPARAM)0,
                                        di->rcItem.left +(di->rcItem.right -di->rcItem.left) -smallicon_size.cx -4,
                                        di->rcItem.top +(di->rcItem.bottom -di->rcItem.top -smallicon_size.cy) /2,
                                        smallicon_size.cx,
                                        smallicon_size.cy,
                                        DST_ICON |DSS_NORMAL |(is_disabled ? DSS_DISABLED: 0)
                                    );
                                }
                                return TRUE;
                                //  ���j���[�̃I�[�i�[�h���[�܂��Ɋւ��Ă͐̂��� Windows �̎d�l��������������
                                //  �Ȃ̂ŏ����̊��ł��܂Ƃ��ɓ�������҂��邱�Ƃ͖����B���̓s�x�������K�v
                                //  �E�E�E�Ƃ��������܂̂��̍��W�v�Z�������̊����ׂĂŖ��Ȃ��\������邩
                                //  �Ƃ����Ƃ��Ԃ�A�m���BMENUITEMINFO::hbmpItem �Ńr�b�g�}�b�v���g���`������
                                //  �ς܂���̂��ȒP�����A�g�B
                            }
                        }
                    }
                }
            }
            break;
            
        case WM_TIMER:
            update_status(hwnd, target_net->check());
            break;
        
        case WM_COMMAND:
        {
            assert(NULL != target_net);
            UINT wID = LOWORD(wParam);
            //HWND hwndCtl = (HWND)lParam;
            //UINT wNotifyCode = HIWORD(wParam);
            switch(wID)
            {
//            case DO_REFRESH_ICON:
//                update_status(hwnd, target_net->check());
//                break;
            
            case DO_START_ICON:
                purge_thread();
                thread_handle = CreateThread(NULL, 0, watch_thread_proc, hwnd, 0, NULL);
                on_auto_check = true;
                update_menu_status(hwnd);
                break;

            case DO_STOP_ICON:
                purge_thread();
                on_auto_check = false;
                update_menu_status(hwnd);
                control_notify_icon(NIM_MODIFY, hwnd, NOTIFYICON_ID, make_icon_caption(false), base_icon);
                break;
                
            case DO_LOG_ICON:
                {
                    HMENU window_menu = GetMenu(hwnd);
                    HMENU notify_icon_menu = GetSubMenu(window_menu, 0);
                    on_output_log = !on_output_log;
                    CheckMenuItem(notify_icon_menu, DO_LOG_ICON, MF_BYCOMMAND |(on_output_log ? MF_CHECKED: MF_UNCHECKED));
                }
                break;
                
            case DO_CLOSE_ICON:
                PostMessage(hwnd, WM_CLOSE, 0, 0);
                break;
            }
            break;
        }
        
        case WM_NOTIFYICON_BASE +NOTIFYICON_ID:
            switch(lParam)
            {
                case WM_LBUTTONDOWN:
                case WM_RBUTTONDOWN:
                {
                    SetForegroundWindow(hwnd);
                    POINT pt;
                    GetCursorPos(&pt);
                    TrackPopupMenu(GetSubMenu(GetMenu(hwnd), 0), TPM_RIGHTBUTTON, pt.x, pt.y, 0, hwnd, NULL);
                    PostMessage(hwnd, WM_NULL, 0, 0);
                    //  cf. KB135788
                    break;
                }
            }
            break;
            
        case WM_DESTROY:
            purge_thread();
            control_notify_icon(NIM_DELETE, hwnd, NOTIFYICON_ID);
            if (target_net)
            {
                delete target_net;
                target_net = NULL;
            }
            PostQuitMessage(0);
            break;
            
#if 0x0600 <= WINVER
        case WM_THEMECHANGED:
            {
                bool is_enabled_theme_now = AUTO_OSVERSIONINFO().is_windows_vista_or_later() && is_theme_active();
                if (is_enabled_theme_now != is_enabled_theme)
                {
                    //
                    //  ���j���[�`����̍X�V
                    //
                    HMENU window_menu = GetMenu(hwnd);
                    HMENU notify_icon_menu = GetSubMenu(window_menu, 0);
                    MENUITEMINFO mii = { sizeof(MENUITEMINFO), };
                    int i = 0;
                    while(true)
                    {
                        mii.fMask = MIIM_FTYPE |MIIM_DATA |MIIM_BITMAP;
                        if (!GetMenuItemInfo(notify_icon_menu, i, TRUE, &mii))
                        {
                            break;
                        }
                        
                        if
                        (
                            MFT_SEPARATOR != (mii.fType &MFT_SEPARATOR) &&
                            NULL != mii.dwItemData
                        )
                        {
                            mii.fMask = MIIM_BITMAP;
                            if (is_enabled_theme_now)
                            {
                                if (NULL == mii.hbmpItem || HBMMENU_CALLBACK == mii.hbmpItem)
                                {
                                    mii.hbmpItem = IconToBitmap((HICON)mii.dwItemData);
                                }
                            }
                            else
                            {
                                if (NULL != mii.hbmpItem && HBMMENU_CALLBACK != mii.hbmpItem)
                                {
                                    DeleteObject((HBITMAP)(mii.hbmpItem));
                                }
                                mii.hbmpItem = HBMMENU_CALLBACK;
                            }
                            if (!SetMenuItemInfo(notify_icon_menu, i, TRUE, &mii))
                            {
                                OutputDebugString("SetMenuItemInfo: faild");
                                OutputDebugString(make_error_message().c_str());
                            }
                        }
                        ++i;
                    }
                    is_enabled_theme = is_enabled_theme_now;
                }
            }
            break;
#endif

        default:
            if (WM_taskbarcreated == message)
            {
                if (!control_notify_icon(NIM_ADD, hwnd, NOTIFYICON_ID, display_name, base_icon))
                {
                    //  ���̏󋵉��Ŏ��s�����ꍇ�̓T�C�����g�ɏI������B
                    PostMessage(hwnd, WM_CLOSE, 0, 0);
                }
            }
            return DefWindowProc(hwnd, message, wParam, lParam);
        }
        return 0;
    }
    
    bool regist()
    {
        WNDCLASS this_window_class;
        
        this_window_class.style = 0;
        this_window_class.lpfnWndProc = (WNDPROC)window_procedure;
        this_window_class.cbClsExtra = 0;
        this_window_class.cbWndExtra = 0;
        this_window_class.hInstance = hInstance;
        this_window_class.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(APPLICATION_ICON));
        this_window_class.hCursor = LoadCursor(NULL, IDC_ARROW);
        this_window_class.hbrBackground = (HBRUSH)(COLOR_WINDOW +1);
        this_window_class.lpszMenuName = MAKEINTRESOURCE(NOTIFYICON_MENU);
        this_window_class.lpszClassName = lpszClassName;
        
        if (!RegisterClass(&this_window_class))
        {
            OutputDebugString(_T("neticon.exe::RegisterClass: faild"));
            return false;
        }
        return true;
    }
    
#if defined(__BORLANDC__)
#pragma warn -8027
#endif
    class message_loop :public MSG
    {
    public:
        HRESULT do_loop()
        {
            while(true)
            {
                switch(GetMessage(this, NULL, 0, 0))
                {
                case 0:
                    return ERROR_SUCCESS;
                case -1:
                    return GetLastError();
                }
                TranslateMessage(this);
                DispatchMessage(this);
            }
        }    
    };
#if defined(__BORLANDC__)
#pragma warn .8027
#endif

    UINT run(HINSTANCE a_hInstance, HINSTANCE a_hPrevInstance, LPSTR a_lpCmdLine, int a_nCmdShow)
    {
        a_hPrevInstance, a_lpCmdLine, a_nCmdShow;
        
        hInstance = a_hInstance;
        
#if 0x0600 <= WINVER
        CoInitialize(NULL);
        uxtheme_dll = LoadLibraryW(L"UxTheme.dll");
        if (uxtheme_dll)
        {
            IsThemeActive = (BOOL (*)())GetProcAddress(uxtheme_dll, "IsThemeActive");
        }
#endif
        
        WM_taskbarcreated = RegisterWindowMessageW(L"TaskbarCreated");
        smallicon_size.cx = GetSystemMetrics(SM_CXSMICON);
        smallicon_size.cy = GetSystemMetrics(SM_CYSMICON);
                    
        int argc;
        LPWSTR *args = CommandLineToArgvW(GetCommandLineW(), &argc);
        
        this_file_name = args[0];

        if (2 <= argc)
        {
            display_name = args[1];
            lstrcpyW(host_name, display_name);
            LPCWSTR port_separator = wcschr(host_name, L':');
            if (port_separator)
            {
                host_name[port_separator -host_name] = L'\0';
                port_str = port_separator +1;
            }
            else
            {
                port_str = NULL;
            }
            LPCWSTR interval_separator = wcschr(port_str ? port_str: host_name, L'@');
            if (interval_separator)
            {
                host_name[interval_separator -host_name] = L'\0';
                interval_str = interval_separator +1;
                interval = _wtoi(interval_str) *1000;
            }
            else
            {
                interval_str = NULL;
                interval = 3000;
            }
            if (interval < 1000)
            {
                interval = 1000;
            }
            int i = 0;
            do
            {
                ansi_host_name[i] = (char)(host_name[i]);
            }
            while(ansi_host_name[i++]);
            
            if (3 <= argc)
            {
                icon_file_name = args[2];
                if (4 <= argc)
                {
                    icon_index = args[3];
                    ExtractIconExW(icon_file_name, _wtoi(icon_index), NULL, &base_icon, 1);
                }
                else
                {
                    ExtractIconExW(icon_file_name, 0, NULL, &base_icon, 1);
                }
            }
            else
            {
                base_icon = load_icon(DEFAULT_NET_ICON);
            }
            
            if (regist() && create())
            {
                message_loop().do_loop();
            }
            else
            {
                OutputDebugString("CreateWindow: faild");
                OutputDebugString(make_error_message().c_str());
            }
        }
        else
        {
            MessageBoxW
            (
                NULL,
                L"�^�X�N�o�[�̒ʒm�̈��ŊĎ����郊���[�g�z�X�g���w�肵�Ă��������B\n"
                L"\n"
                L"\n"
                L"usage: neticon [�����[�g�z�X�g��[:�|�[�g�ԍ�][@�Ď��Ԋu(�b�P��)] [�A�C�R���t�@�C���� [�A�C�R���C���f�b�N�X�l]]]\n"
                L"\n"
                L"cf. http://tricklib.com/cxx/ex/neticon/\n",
                application_name,
                MB_ICONINFORMATION |MB_OK
            );
        }
        
        LocalFree(args);
        
#if 0x0600 <= WINVER
        CoUninitialize();
        if (uxtheme_dll)
        {
            FreeLibrary(uxtheme_dll);
            uxtheme_dll = NULL;
        }
#endif
        
        return EXIT_SUCCESS;
    }
}

///////////////////////////////////////////////////////////////////////////////
//
//  WinMain
//

int PASCAL WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    return net_icon::run(hInstance, hPrevInstance, lpCmdLine, nCmdShow);
}

/******************************************************************************
    ��������                  Wraith the Trickster                  ��������
    �������� �`I'll go with heaven's advantage and fool's wisdom.�` ��������
******************************************************************************/