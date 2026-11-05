// The askpass side of ServerManager without its window, for the tests: what ServerManager's main does when ssh runs it.
// A GUI-subsystem program on Windows, like ServerManager, so the tests cover its stdout reaching ssh through a pipe.

#include "x2/servermanager/askpass.h"

int main(int argc, char* argv[])
{
    return x2::servermanager::RunAskpass(argc, argv);
}
