#include "stdafx.h"
#include "LibcurlWrapper.h"
#include <assert.h>


CString UTF8ToUnicode( char* UTF8 )
{
    DWORD dwUnicodeLen;        //转换后Unicode的长度
    TCHAR *pwText;            //保存Unicode的指针
    CString strUnicode;        //返回值
    //获得转换后的长度，并分配内存
    dwUnicodeLen = MultiByteToWideChar( CP_UTF8, 0, UTF8, -1, NULL, 0 );
    pwText = new TCHAR[dwUnicodeLen];
    if ( !pwText )
    {
        return strUnicode;
    }
    //转为Unicode
    MultiByteToWideChar( CP_UTF8, 0, UTF8, -1, pwText, dwUnicodeLen );
    //转为CString
    strUnicode.Format( _T( "%s" ), pwText );
    //清除内存
    delete []pwText;
    //返回转换好的Unicode字串
    return strUnicode;
    
}

BOOL CLibcurlWrapper::m_bGlobalInited = FALSE;
CString CLibcurlWrapper::m_sContent;
struct MemoryStruct
{
    char *memory;
    size_t size;
};

CLibcurlWrapper::CLibcurlWrapper()
{

}

CLibcurlWrapper::~CLibcurlWrapper()
{

}

BOOL CLibcurlWrapper::GlobalInit()
{
    assert( FALSE == m_bGlobalInited );
    if ( CURLE_OK == curl_global_init( CURL_GLOBAL_ALL ) )
    {
        m_bGlobalInited = TRUE;
        return TRUE;
    }
    return FALSE;
}

BOOL CLibcurlWrapper::GlobalUnInit()
{
    curl_global_cleanup();
    m_bGlobalInited = FALSE;
    return TRUE;
}

BOOL CLibcurlWrapper::Get( LPCTSTR lpszUrl )
{
    m_sContent.Empty();
    BOOL bRet = FALSE;
    CURL* p_curl = NULL;
    MemoryStruct chunk;
    curl_slist *headers = NULL; /* init to NULL is important */
    do
    {
        USES_CONVERSION;
        CStringA sUrl = W2A( lpszUrl );
        p_curl = curl_easy_init();
        if ( p_curl )
        {
            chunk.memory = ( char* ) malloc( 1 ); /* will be grown as needed by the realloc above */
            chunk.size = 0;    /* no data at this point */
            
            //          headers = curl_slist_append(headers, "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8");
            // 			headers = curl_slist_append(headers, "Accept-Language: zh-cn,zh;q=0.8,en-us;q=0.5,en;q=0.3");
            // 			headers = curl_slist_append(headers, "Accept-Charset:GB2312,utf-8;q=0.7,*;q=0.7");
            //  		headers = curl_slist_append(headers, "Accept-Encoding: gzip, deflate");
            //  		curl_easy_setopt(p_curl, CURLOPT_HTTPHEADER, headers);
            
            curl_easy_setopt( p_curl, CURLOPT_URL, sUrl );
            /* example.com is redirected, so we tell libcurl to follow redirection */
            curl_easy_setopt( p_curl, CURLOPT_FOLLOWLOCATION, 1L );
            /* send all data to this function  */
            curl_easy_setopt( p_curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback );
            /* we pass our 'chunk' struct to the callback function */
            curl_easy_setopt( p_curl, CURLOPT_WRITEDATA, ( void * )&chunk );
            /* Perform the request, res will get the return code */
            CURLcode res = curl_easy_perform( p_curl );
            /* Check for errors */
            if ( res != CURLE_OK )
                break;
                
            m_sContent = UTF8ToUnicode( chunk.memory );
            bRet = TRUE;
        }
    }
    while ( FALSE );
    
    if ( chunk.memory )
        free( chunk.memory );
        
    /* free the header list */
    curl_slist_free_all( headers );
    /* always cleanup */
    curl_easy_cleanup( p_curl );
    
    return bRet;
}

BOOL CLibcurlWrapper::Post( LPCTSTR lpszUrl , LPCTSTR lpszPost )
{
    m_sContent.Empty();
    BOOL bRet = FALSE;
    CURL* p_curl = NULL;
    MemoryStruct chunk;
    curl_slist *headers = NULL; /* init to NULL is important */
    do
    {
        USES_CONVERSION;
        CStringA sUrl = W2A( lpszUrl );
        CStringA sPost = W2A( lpszPost );
        p_curl = curl_easy_init();
        if ( p_curl )
        {
            chunk.memory = ( char* ) malloc( 1 ); /* will be grown as needed by the realloc above */
            chunk.size = 0;    /* no data at this point */
            
            curl_easy_setopt( p_curl, CURLOPT_URL, sUrl );
            /* Now specify the POST data */
            if ( !sPost.IsEmpty() )
            {
                curl_easy_setopt( p_curl, CURLOPT_POSTFIELDS, sPost );
            }
            
            /* send all data to this function  */
            curl_easy_setopt( p_curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback );
            /* we pass our 'chunk' struct to the callback function */
            curl_easy_setopt( p_curl, CURLOPT_WRITEDATA, ( void * )&chunk );
            /* Perform the request, res will get the return code */
            CURLcode res = curl_easy_perform( p_curl );
            /* Check for errors */
            if ( res != CURLE_OK )
                break;
                
            m_sContent = UTF8ToUnicode( chunk.memory );
            bRet = TRUE;
        }
    }
    while ( FALSE );
    
    if ( chunk.memory )
        free( chunk.memory );
        
    curl_slist_free_all( headers ); /* free the header list */
    /* always cleanup */
    curl_easy_cleanup( p_curl );
    
    return bRet;
}

CString CLibcurlWrapper::GetResponse()
{
    return m_sContent;
}

size_t CLibcurlWrapper::WriteMemoryCallback( void *contents, size_t size, size_t nmemb, void *userp )
{
    size_t realsize = size * nmemb;
    MemoryStruct *mem = ( struct MemoryStruct * )userp;
    
    mem->memory = ( char* ) realloc( mem->memory, mem->size + realsize + 1 );
    if ( mem->memory == NULL )
    {
        /* out of memory! */
        printf( "not enough memory (realloc returned NULL)\n" );
        return 0;
    }
    
    memcpy( &( mem->memory[mem->size] ), contents, realsize );
    mem->size += realsize;
    mem->memory[mem->size] = 0;
    
    return realsize;
}
