#include <iostream>
#include <fstream>
#include "https_requester/https_requester.h"
using namespace std;

int main(int argc, char *argv[]) {
    cout << "...\n";
    
    HTTPSRequester requester("httpbin.org"); // host only, connects to https

    if (!requester) {
        cerr << "Error connecting to host\n";
        return 1;
    }
    string response = requester.request("/get", "GET", "Test=TestValue;Test2=TestValue2");
    cout << "GET Response:\n" << response << endl;

    //string postResponse = requester.post("/submit", "param1=value1&param2=value2");
    //cout << "POST Response:\n" << postResponse << endl;

    cout << "\n\nTrying to retrieve Discord messages...\n";
    HTTPSRequester discordRequester("discord.com");
    if (!discordRequester) {
        cerr << "Error connecting to Discord\n";
        return 1;
    }

    // Read the token from a file (be safe with your token buddy! Never share it or put it in a public repo) 
    // Make sure to create a file named "token.txt" in the same directory as this program and put your Discord bot token in it.
    // The token should be the first line of the file
    ifstream tokenFile("token.txt");
    if (!tokenFile) {
        cerr << "Error opening token file\n";
        return 1;
    }
    string authToken;
    getline(tokenFile, authToken);
    tokenFile.close();

    string discordResponse = discordRequester.request("/api/v9/channels/763030444714360872/messages?limit=50", "GET", "Authorization="+authToken);
    cout << "Discord GET Response:\n" << discordResponse << endl;



    return 0;
}