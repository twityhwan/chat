#include "ChatServer.h"
#include <iostream>
#include <string.h>

using namespace std;

int main() {
    char buf[4096];
    char end[] = "quit";
    ChatServer server;
    server.start();

    /*
    while(true) {
        cout<<"<< ";
        cin>>buf;
        if(strlen(buf) == 0) {
            break;
        }

        if (!strcmp(buf, end)) {
            cout<<"Server End ..."<<endl;
            break;
        }
        cout<<">> "<<buf<<endl;

    }
    */

    return 0;
}
