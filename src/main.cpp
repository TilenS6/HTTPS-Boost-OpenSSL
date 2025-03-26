#include <iostream>
#include "https_requester/https_requester.h"
using namespace std;

int main(int argc, char *argv[]) {
    cout << "Press any key to start the HTTPS request..." << endl;
    char c;
    cin >> c;
    
    HTTPSRequester requester("httpbn.org"); // host only, connects to https

    if (!requester) {
        cerr << "Error connecting to host" << endl;
        return 1;
    }

    string response = requester.get("/get");
    cout << "GET Response:\n" << response << endl;

    //string postResponse = requester.post("/submit", "param1=value1&param2=value2");
    //cout << "POST Response:\n" << postResponse << endl;


    return 0;
}