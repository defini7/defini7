#pragma once

#include <vector>
#include <string>
#include <thread>

#include <WinSock2.h>
#include <WS2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

class Twitch
{
public:
    Twitch();
    ~Twitch();

public:
    struct Message
    {
        std::string sText;
        std::string sUserName;
        std::string sUserMessage;
    };

    std::vector<Message> vecMessages;
    std::string sLastError;

private:
    SOCKET m_socket;
    std::thread m_thread;

private:
    void GrabSomeData(bool bLoop);

public:
    bool StartupWSA();

    bool Setup(int port);
    bool Connect(const std::string& sOAuth, const std::string& sNick, const std::string& sConnectTo);
    bool StartGrabMessages();
    bool StopGrabMessages();

    virtual void OnNewMessage(const std::string& sUserName, const std::string& sUserMessage) = 0;

};

#ifdef DEF_TWITCH
#undef DEF_TWITCH

Twitch::Twitch()
{
    m_socket = INVALID_SOCKET;
}

Twitch::~Twitch()
{
    closesocket(m_socket);
}

void Twitch::GrabSomeData(bool bLoop)
{
    while (1)
    {
        char buffer[100];
        std::string s;

        int len = recv(m_socket, buffer, 100, 0);

        for (int i = 0; i < len; i++)
        {
            s.append(1, buffer[i]);

            if (buffer[i] == '\n')
            {
                if (i < 2)
                {
                    s.pop_back();
                    s.pop_back();
                }

                size_t u = s.find('!');
                size_t c = s.find(':', u);

                if (c != std::string::npos)
                {
                    vecMessages.push_back({ s, s.substr(1, u - 1), s.substr(c + 1) });
                    if (vecMessages.back().sUserMessage.back() == '\n') vecMessages.back().sUserMessage.pop_back();
                    OnNewMessage(vecMessages.back().sUserName, vecMessages.back().sUserMessage);
                }
                else
                {
                    size_t m = s.find(':');
                    m = s.find(':', m + 1);
                    sLastError = s.substr(m + 1);
                }

                s.clear();
            }
        }

        if (!bLoop) break;
    }
}

bool Twitch::StartupWSA()
{
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        sLastError = "Could not start WinSock";
        return false;
    }

    return true;
}

bool Twitch::Setup(int nPort)
{
    addrinfo* addr = nullptr;
    if (getaddrinfo("irc.chat.twitch.tv", std::to_string(nPort).c_str(), nullptr, &addr) != 0)
    {
        sLastError = "Failed to get address info";
        return false;
    }

    m_socket = socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol);

    return (connect(m_socket, addr->ai_addr, (int)addr->ai_addrlen) != SOCKET_ERROR);
}

bool Twitch::Connect(const std::string& oauth, const std::string& nick, const std::string& connectTo)
{
    std::string s;

    s = "PASS oauth:" + oauth + "\r\n";
    send(m_socket, s.c_str(), s.length(), 0);

    s = "NICK " + nick + "\r\n";
    send(m_socket, s.c_str(), s.length(), 0);

    s = "JOIN #" + connectTo + "\r\n";
    send(m_socket, s.c_str(), s.length(), 0);

    vecMessages.clear();
    sLastError.clear();

    // Receive message to understand
    // if there is an error
    while (vecMessages.empty() && sLastError.empty()) GrabSomeData(false);

    return sLastError.empty();
}

bool Twitch::StopGrabMessages()
{
    if (m_thread.joinable())
    {
        m_thread.join();
        return true;
    }

    return false;
}

bool Twitch::StartGrabMessages()
{
    m_thread = std::thread(&Twitch::GrabSomeData, this, true);
    return true;
}

#endif
