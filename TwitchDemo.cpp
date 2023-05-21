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
    if (!twitch.Connect("9f4qgx6z2pfuw2rllirpnq4pd95r2w", "johnny_topman123", "singsing"))
        cout << twitch.sLastError << endl;

    twitch.StartGrabMessages();
    twitch.StopGrabMessages();

    return 0;
}
