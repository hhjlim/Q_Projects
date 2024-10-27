#include "wxSocket_Client.h"

wxBEGIN_EVENT_TABLE(MyClientFrame, wxFrame)
	EVT_BUTTON(ID_CONNECT_SERVER, MyClientFrame::OnConnectToServer)
	EVT_BUTTON(ID_DISCONNECT_SERVER, MyClientFrame::OnCloseConnection)
	EVT_BUTTON(ID_SEND_MESSAGE, MyClientFrame::OnSendMessage)
    EVT_TEXT_ENTER(wxID_ANY, MyClientFrame::OnEnterPressed)
    EVT_SOCKET(wxID_ANY, MyClientFrame::OnSocketEvent)
wxEND_EVENT_TABLE()

wxIMPLEMENT_APP(MyClientApp);

bool MyClientApp::OnInit()
{
    MyClientFrame* frame = new MyClientFrame();
    frame->Show(true);
    return true;
}

MyClientFrame::MyClientFrame()
    : wxFrame(nullptr, wxID_ANY, "Simple TCP Client", wxDefaultPosition, wxSize(300, 400))
{
    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer* IpAddrSizer = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer* ConnectBtnSizer = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer* ChatSizerBottom = new wxBoxSizer(wxHORIZONTAL);


    wxStaticText* ipLabel = new wxStaticText(this, wxID_ANY, "IP Address :");
    addressTextBox = new wxTextCtrl(this, wxID_ANY, "127.0.0.1");
    wxStaticText* portLabel = new wxStaticText(this, wxID_ANY, "Port :");
    portTextBox = new wxTextCtrl(this, wxID_ANY, "8080");

    IpAddrSizer->Add(ipLabel,1, wxALIGN_CENTER | wxALL, 5);
    IpAddrSizer->Add(addressTextBox,1, wxALIGN_CENTER | wxALL, 5);
    IpAddrSizer->Add(portLabel,1, wxALIGN_CENTER | wxALL, 5);
    IpAddrSizer->Add(portTextBox,1, wxALIGN_CENTER | wxALL, 5);
    mainSizer->Add(IpAddrSizer, 0, wxEXPAND | wxALL, 5);

    connectButton = new wxButton(this, ID_CONNECT_SERVER, "Connect");
    disconnectButton = new wxButton(this, ID_DISCONNECT_SERVER, "Disconnect");

    ConnectBtnSizer->Add(connectButton,1, wxALIGN_CENTER | wxALL, 5);
    ConnectBtnSizer->Add(disconnectButton,1, wxALIGN_CENTER | wxALL, 5);
    mainSizer->Add(ConnectBtnSizer, 0, wxEXPAND | wxALL, 5);
    
    statusTextBox = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE | wxTE_READONLY);
    statusTextBox->SetHint("Status Log \n");
    mainSizer->Add(statusTextBox, 2, wxEXPAND | wxALL, 5);

    msginputTextBox = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER);
    msginputTextBox->SetHint("Input msg here \n");
    sendButton = new wxButton(this, ID_SEND_MESSAGE, "Send", wxDefaultPosition, wxDefaultSize);

    ChatSizerBottom->Add(msginputTextBox,1, wxALIGN_CENTER | wxALL, 5);
    ChatSizerBottom->Add(sendButton,0, wxALIGN_CENTER | wxALL, 5);
    mainSizer->Add(ChatSizerBottom, 1, wxEXPAND | wxALL, 5);

    this->SetSizer(mainSizer);
    this->Layout();

}

void MyClientFrame::OnConnectToServer(wxCommandEvent& event)
{
    if (clientSocket && clientSocket->IsConnected())
    {
        LogMessage("Already Connected");
        return;
    }

    wxString address = addressTextBox->GetValue();
    wxString portStr = portTextBox->GetValue();
    long port;

    if (!portStr.ToLong(&port) || port <= 0 || port > 65535)
    {
        LogMessage("Invalid Address");
        return;
    }

    wxIPV4address addr;
    addr.Hostname(address);
    addr.Service(static_cast<wxUint16>(port));

    LogMessage("Connecting to Server ... ");
    clientSocket = new wxSocketClient();
    clientSocket->SetEventHandler(*this, wxID_ANY);  
    clientSocket->SetNotify(wxSOCKET_CONNECTION_FLAG | wxSOCKET_INPUT_FLAG | wxSOCKET_LOST_FLAG); 
    clientSocket->Notify(true);

    clientSocket->Connect(addr, false);  
}

void MyClientFrame::OnCloseConnection(wxCommandEvent& event)
{
    LogMessage("Closing Connect");
    clientSocket->Close();
}

void MyClientFrame::LogMessage(const wxString& message)
{
    statusTextBox->AppendText(message + "\n");
}

void MyClientFrame::OnSocketEvent(wxSocketEvent& event)
{
    switch (event.GetSocketEvent())
    {
        case wxSOCKET_CONNECTION:
        {
            LogMessage("Server Connected [wxSOCKET_CONNECTION]");
            break;
        }

        case wxSOCKET_LOST:
        {
            LogMessage("Server closed connection [xSOCKET_LOST]");
            clientSocket->Close();
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
                LogMessage("[Server] : " + receivedMessage);
            }
            else
            {
                LogMessage("[Err] : bytesRead <= 0 ");
            }
            break;
        }
    }
}

void MyClientFrame::OnEnterPressed(wxCommandEvent& event)
{
    OnSendMessage(event); // for enter key event
}

void MyClientFrame::OnSendMessage(wxCommandEvent& event)
{
    if (!clientSocket || !clientSocket->IsConnected())
    {
        LogMessage("Client is not connected to the server");
        msginputTextBox->Clear();
        return;
    }

    wxString message = msginputTextBox->GetValue();
    if (message.IsEmpty())
    {
        LogMessage("Input msg");
        return;
    }

    clientSocket->Write(message.ToUTF8(), message.Length());
    LogMessage("[Client] :" + message);

    msginputTextBox->Clear();
}