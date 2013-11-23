#pragma once
#include "curl/curl.h"

class CLibcurlWrapper
{
public:
    CLibcurlWrapper();
    virtual ~CLibcurlWrapper();
    
    static BOOL GlobalInit();
    static BOOL GlobalUnInit();
    BOOL Post( LPCTSTR lpszUrl , LPCTSTR lpszPost );
    BOOL Get( LPCTSTR lpszUrl );
    CString GetResponse();
protected:
    static size_t WriteMemoryCallback( void *contents, size_t size, size_t nmemb, void *userp );
private:
    static CString m_sContent;
    static BOOL m_bGlobalInited;
};
