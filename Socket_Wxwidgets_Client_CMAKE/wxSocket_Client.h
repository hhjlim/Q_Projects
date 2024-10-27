#pragma once
#include <wx/wx.h>
#include <wx/socket.h>

class MyClientApp : public wxApp
{
public:
    virtual bool OnInit();
};

class MyClientFrame : public wxFrame
{
public:
    MyClientFrame();

private:
    void OnConnectToServer(wxCommandEvent& event);
    void OnCloseConnection(wxCommandEvent& event);
    void OnSendMessage(wxCommandEvent& event);
    void OnEnterPressed(wxCommandEvent& event);

    void OnSocketEvent(wxSocketEvent& event);

    void LogMessage(const wxString& message);       

    wxSocketClient* clientSocket;

    wxTextCtrl* addressTextBox;  
    wxTextCtrl* portTextBox;     
    wxTextCtrl* statusTextBox;
    wxTextCtrl* msginputTextBox;

    wxButton* connectButton;
    wxButton* disconnectButton;
    wxButton* sendButton;


    wxDECLARE_EVENT_TABLE();
};

enum
{
    ID_CONNECT_SERVER = 1,
    ID_DISCONNECT_SERVER,
    ID_SEND_MESSAGE
};