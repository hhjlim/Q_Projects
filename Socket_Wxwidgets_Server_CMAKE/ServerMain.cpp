#include "ServerMain.h"

wxBEGIN_EVENT_TABLE(MyServerFrame, wxFrame)
    EVT_BUTTON(ID_START_SERVER, MyServerFrame::OnStartServer)
    EVT_BUTTON(ID_STOP_SERVER, MyServerFrame::OnStopServer)
    EVT_BUTTON(ID_SEND_MESSAGE, MyServerFrame::OnSendMessage)
    EVT_TEXT_ENTER(wxID_ANY, MyServerFrame::OnEnterPressed)
    EVT_SOCKET(wxID_ANY, MyServerFrame::OnSocketEvent)
wxEND_EVENT_TABLE()

wxIMPLEMENT_APP(MyServerApp);

bool MyServerApp::OnInit()
{
    MyServerFrame* frame = new MyServerFrame();
    frame->Show(true);
    return true;
}

MyServerFrame::MyServerFrame()
    : wxFrame(nullptr, wxID_ANY, "Simple TCP Server", wxDefaultPosition, wxDefaultSize)
{
    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer* buttonSizerTop = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer* ChatSizerBottom = new wxBoxSizer(wxHORIZONTAL);

    //Btn for Start Server
    startServerButton = new wxButton(this, ID_START_SERVER, "Start Server");
    stopServerButton = new wxButton(this, ID_STOP_SERVER, "Stop Server");
    buttonSizerTop->Add(startServerButton, 1, wxALIGN_CENTER | wxALL, 5);
    buttonSizerTop->Add(stopServerButton, 1, wxALIGN_CENTER | wxALL, 5);
    mainSizer->Add(buttonSizerTop, 0, wxEXPAND | wxALL, 5);

    //Status textbox
    statusTextBox = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE | wxTE_READONLY);
    statusTextBox->SetHint("Status Log \n");
    mainSizer->Add(statusTextBox, 1, wxEXPAND | wxALL, 5);

    //Chat box zone
    inputTextBox = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER);
    inputTextBox->SetHint("Input msg here \n");
    sendButton = new wxButton(this, ID_SEND_MESSAGE, "Send", wxPoint(320, 270), wxSize(60, 30));
    ChatSizerBottom->Add(inputTextBox, 1, wxEXPAND | wxALL, 5);
    ChatSizerBottom->Add(sendButton, 1, wxALIGN_CENTER | wxALL, 5);
    mainSizer->Add(ChatSizerBottom, 1, wxEXPAND | wxALL, 5);

    this->SetSizer(mainSizer);
    this->Layout();
}

void MyServerFrame::LogMessage(const wxString& message)
{
    statusTextBox->AppendText(message + "\n");
}

void MyServerFrame::OnStartServer(wxCommandEvent& event)
{
    if (serverSocket)
    {
        LogMessage("ServerSocket is Already Started");
        return;
    }

    wxIPV4address addr;
    addr.Service(8080);

    serverSocket = new wxSocketServer(addr);

    if (serverSocket->IsOk() == false)
    {
        LogMessage("serverSocket is Not Okay..");
        delete serverSocket;
        serverSocket = nullptr;
        return;
    }

    LogMessage("ServerStarted Waiting for Client");

    serverSocket->SetEventHandler(*this, wxID_ANY);
    serverSocket->SetNotify(wxSOCKET_CONNECTION_FLAG);
    serverSocket->Notify(true);
}

void MyServerFrame::OnStopServer(wxCommandEvent& event)
{
    if (clientSocket)
    {
        clientSocket->Close();
        delete clientSocket;
        clientSocket = nullptr;

        LogMessage("Stoping server - Closed client socket");
    }

    if (serverSocket)
    {
        serverSocket->Close();  
        delete serverSocket;     
        serverSocket = nullptr;

        LogMessage("ServerSocket Closed");
    }
    else
    {
        LogMessage("ServerSocket is Already Closed");
    }
}

void MyServerFrame::OnSocketEvent(wxSocketEvent& event)
{
    switch (event.GetSocketEvent())
    {
        case wxSOCKET_CONNECTION:
        {
            LogMessage("Client Connected");

            clientSocket = serverSocket->Accept(false);

            if (clientSocket)
            {
                clientSocket->SetEventHandler(*this, wxID_ANY);
                clientSocket->SetNotify(wxSOCKET_INPUT_FLAG | wxSOCKET_LOST_FLAG);
                clientSocket->Notify(true);
            }
            break;
        }

        case wxSOCKET_INPUT:
        {
            char buffer[256];
            clientSocket->Read(buffer, sizeof(buffer));

            size_t bytesRead = clientSocket->LastCount();

            if (bytesRead > 0)
            {
                wxString receivedMessage = wxString::FromUTF8(buffer, bytesRead);
                LogMessage("[Client] : " + receivedMessage);
            }
            else
            {
                LogMessage("bytesRead Error");
            }
            break;
        }

        case wxSOCKET_LOST:
        {
            LogMessage("Client Closed Connection");

            clientSocket->Destroy();
            clientSocket = nullptr;
            break;
        }

    }
}

void MyServerFrame::OnEnterPressed(wxCommandEvent& event)
{
    OnSendMessage(event);
}

void MyServerFrame::OnSendMessage(wxCommandEvent& event)
{
    if (!clientSocket || !clientSocket->IsConnected())
    {
        LogMessage("Client is not connected");
        inputTextBox->Clear();
        return;
    }

    wxString message = inputTextBox->GetValue();  
    if (message.IsEmpty())
    {
        LogMessage("Input msg");
        return;
    }

    clientSocket->Write(message.ToUTF8(), message.Length());
    LogMessage("[Server] :" + message);

    inputTextBox->Clear();
}