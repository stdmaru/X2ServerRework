#pragma once

// Entry point shared by the servers, in place of the MFC window of the original (Common/ui: CServerApp/CServerUIDlg).
// Each server's main() calls it; the server itself comes from the CreateKNC() the server defines.
//
// Command line, as in the original: <exe> <version tag> <server group id> [<db id> <db password>]
// (the version tag picks config_<server>_<tag>.lua). The original "-popup" option, a dialog asking for the
// DB id/password, has no equivalent yet: pass them as arguments.
int KServerMain( int argc, char* argv[] );
