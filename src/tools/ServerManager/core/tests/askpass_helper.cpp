// The askpass side of ServerManager without its window, for the tests: what ServerManager's main does when ssh runs it.
// A GUI-subsystem program on Windows, like ServerManager, so the tests cover its stdout reaching ssh through a pipe.
//
// With X2_ASKPASS_TEST_DENY set it also plays an ssh whose password was refused: it asks, prints nothing on stdout,
// complains on stderr and exits 255 (the UI tests run it as a host's x2ctl under x2_fake_ssh).

#include "x2/servermanager/askpass.h"

#include <cstdio>
#include <cstdlib>

int main(int argc, char* argv[])
{
    const char* deny = std::getenv("X2_ASKPASS_TEST_DENY");
    if (!deny || !*deny)
        return x2::servermanager::RunAskpass(argc, argv);
#if defined(_WIN32)
    const char* null = "NUL";
#else
    const char* null = "/dev/null";
#endif
    if (!std::freopen(null, "w", stdout))
        return 2;
    x2::servermanager::RunAskpass(argc, argv);
    std::fprintf(stderr, "x2@fake-host: Permission denied (publickey,password).\n");
    return 255;
}
