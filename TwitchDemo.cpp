#include <iostream>

#define DEF_TWITCH
#include "Twitch.h"

using namespace std;

class CustomTwitch : public Twitch
{
public:
    void OnNewMessage(const string& sUserName, const string& sUserMessage) override
    {
        cout << sUserName << ": " << sUserMessage << endl;
    }
};



int main()
{
    CustomTwitch twitch;
    if (!twitch.StartupWSA()) return 1;

    twitch.Setup(6667);
    if (!twitch.Connect("<auth>", "<your_nickname>", "<chat_name>"))
        cout << twitch.sLastError << endl;

    twitch.StartGrabMessages();
    twitch.StopGrabMessages();

    return 0;
}
