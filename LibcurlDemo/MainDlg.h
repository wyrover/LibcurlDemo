// MainDlg.h : interface of the CMainDlg class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once
#include "LibcurlWrapper.h"

class CMainDlg : public CDialogImpl<CMainDlg>, public CUpdateUI<CMainDlg>,
    public CMessageFilter, public CIdleHandler
{
private:
    CLibcurlWrapper m_libcurl;
public:
    enum { IDD = IDD_MAINDLG };
    
    virtual BOOL PreTranslateMessage( MSG* pMsg )
    {
        return CWindow::IsDialogMessage( pMsg );
    }
    
    virtual BOOL OnIdle()
    {
        UIUpdateChildWindows();
        return FALSE;
    }
    
    BEGIN_UPDATE_UI_MAP( CMainDlg )
    END_UPDATE_UI_MAP()
    
    BEGIN_MSG_MAP( CMainDlg )
    MESSAGE_HANDLER( WM_INITDIALOG, OnInitDialog )
    MESSAGE_HANDLER( WM_DESTROY, OnDestroy )
    COMMAND_ID_HANDLER( ID_APP_ABOUT, OnAppAbout )
    COMMAND_ID_HANDLER( IDOK, OnOK )
    COMMAND_ID_HANDLER( IDCANCEL, OnCancel )
    COMMAND_ID_HANDLER( IDC_BTN_QUIT, OnCancel )
    COMMAND_ID_HANDLER( IDC_BTN_SUBMIT, OnSubmit )
    COMMAND_ID_HANDLER( IDC_RADIO1, OnRadioGet )
    COMMAND_ID_HANDLER( IDC_RADIO2, OnRadioPost )
    END_MSG_MAP()
    
    // Handler prototypes (uncomment arguments if needed):
    //	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
    //	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
    //	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)
    
    LRESULT OnInitDialog( UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/ )
    {
        // center the dialog on the screen
        CenterWindow();
        
        // set icons
        HICON hIcon = AtlLoadIconImage( IDR_MAINFRAME, LR_DEFAULTCOLOR, ::GetSystemMetrics( SM_CXICON ), ::GetSystemMetrics( SM_CYICON ) );
        SetIcon( hIcon, TRUE );
        HICON hIconSmall = AtlLoadIconImage( IDR_MAINFRAME, LR_DEFAULTCOLOR, ::GetSystemMetrics( SM_CXSMICON ), ::GetSystemMetrics( SM_CYSMICON ) );
        SetIcon( hIconSmall, FALSE );
        
        // register object for message filtering and idle updates
        CMessageLoop* pLoop = _Module.GetMessageLoop();
        ATLASSERT( pLoop != NULL );
        pLoop->AddMessageFilter( this );
        pLoop->AddIdleHandler( this );
        
        UIAddChildWindowContainer( m_hWnd );
        
        CLibcurlWrapper::GlobalInit();
        
        CEdit editUrl = GetDlgItem( IDC_EDIT1 );
        editUrl.SetWindowText( _T( "http://sys.xxit.net/user/login.php" ) );
        CEdit editPost = GetDlgItem( IDC_EDIT3 );
        editPost.SetWindowText( _T( "username=redcode" ) );
        editPost.ShowWindow( SW_HIDE );
        
        CButton btnRadioGet = GetDlgItem( IDC_RADIO1 );
        btnRadioGet.SetCheck( BST_CHECKED );
        
        return TRUE;
    }
    
    LRESULT OnDestroy( UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/ )
    {
        // unregister message filtering and idle updates
        CMessageLoop* pLoop = _Module.GetMessageLoop();
        ATLASSERT( pLoop != NULL );
        pLoop->RemoveMessageFilter( this );
        pLoop->RemoveIdleHandler( this );
        
        return 0;
    }
    
    LRESULT OnAppAbout( WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/ )
    {
        CAboutDlg dlg;
        dlg.DoModal();
        return 0;
    }
    
    LRESULT OnOK( WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/ )
    {
        // TODO: Add validation code
        CloseDialog( wID );
        return 0;
    }
    
    LRESULT OnCancel( WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/ )
    {
        CloseDialog( wID );
        return 0;
    }
    
    void CloseDialog( int nVal )
    {
        CLibcurlWrapper::GlobalUnInit();
        
        DestroyWindow();
        ::PostQuitMessage( nVal );
    }
    
    LRESULT OnSubmit( WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/ )
    {
        CEdit editUrl = GetDlgItem( IDC_EDIT1 );
        CString sUrl;
        editUrl.GetWindowText( sUrl );
        if ( sUrl.IsEmpty() )
        {
            return 0;
        }
        
        CString sResponse;
        
        CButton btnRadioGet = GetDlgItem( IDC_RADIO1 );
        if ( BST_CHECKED == btnRadioGet.GetCheck() )
        {
            if ( m_libcurl.Get( sUrl ) )
            {
                sResponse = m_libcurl.GetResponse();
            }
        }
        
        CButton btnRadioPost = GetDlgItem( IDC_RADIO2 );
        if ( BST_CHECKED == btnRadioPost.GetCheck() )
        {
            CEdit editPost = GetDlgItem( IDC_EDIT3 );
            CString sPost;
            editPost.GetWindowText( sPost );
            if ( m_libcurl.Post( sUrl , sPost ) )
            {
                sResponse = m_libcurl.GetResponse();
            }
        }
        
        CEdit editResult = GetDlgItem( IDC_EDIT2 );
        editResult.SetWindowText( sResponse );
        return 0;
    }
    
    LRESULT OnRadioGet( WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/ )
    {
        CEdit editPost = GetDlgItem( IDC_EDIT3 );
        editPost.ShowWindow( SW_HIDE );
        return 0;
    }
    
    LRESULT OnRadioPost( WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/ )
    {
        CEdit editPost = GetDlgItem( IDC_EDIT3 );
        editPost.ShowWindow( SW_SHOWNORMAL );
        return 0;
    }
};
