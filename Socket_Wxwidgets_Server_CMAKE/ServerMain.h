#pragma once

#include "wx/wx.h"
#include "wx/socket.h"
#include "wx/sizer.h"

class MyServerApp : public wxApp
{
public:
	virtual bool OnInit();
};

class MyServerFrame : public wxFrame
{
public:
    MyServerFrame();

private:
    void OnStartServer(wxCommandEvent& event);
    void OnStopServer(wxCommandEvent& event);
    void OnSendMessage(wxCommandEvent& event);
    void OnEnterPressed(wxCommandEvent& event);
    void OnSocketEvent(wxSocketEvent& event);
    void LogMessage(const wxString& message);

    wxSocketServer* serverSocket = nullptr; // Server Socket
    wxSocketBase* clientSocket = nullptr;  // Client Socket

    wxButton* startServerButton;    // Start Btn
    wxButton* stopServerButton;     // Stop Btn
    wxButton* sendButton;           // Server Send msg Btn

    wxTextCtrl* statusTextBox;      // Current Status TextBox
    wxTextCtrl* inputTextBox;       // Server msg inputbox

    wxDECLARE_EVENT_TABLE();
};

enum
{
    ID_START_SERVER = 1,
    ID_STOP_SERVER,
    ID_SEND_MESSAGE
};


