// KAutoPath finds a script in the folders it searches, in any letter case as on Windows (the data names
// "Dungeon_Training_LIRE_0.lua", the client ships "Dungeon_Training_lire_0.lua").
#include "KAutoPath.h"

#include <doctest/doctest.h>

#include <filesystem>
#include <fstream>

namespace fs = std::filesystem;

TEST_CASE( "KAutoPath finds scripts regardless of letter case" )
{
    const fs::path root = fs::temp_directory_path() / "x2_auto_path_test";
    fs::remove_all( root );
    fs::create_directories( root / "Sub" );
    std::ofstream( root / "Sub" / "Dungeon_Training_lire_0.lua" ) << "-- test\n";
    std::ofstream( root / "Exact_Name.lua" ) << "-- test\n";

    // the working folder is searched first: make it one that holds nothing else
    const fs::path oldCurrent = fs::current_path();
    fs::create_directories( root / "Work" );
    fs::current_path( root / "Work" );

    KAutoPath::AddPath( root.string() );
    KAutoPath kAutoPath;

    std::string strExact = "Exact_Name.lua";
    CHECK( kAutoPath.GetPullPath( strExact ) );
    CHECK( fs::equivalent( strExact, root / "Exact_Name.lua" ) );

    std::string strOtherCase = "Dungeon_Training_LIRE_0.lua";
    CHECK( kAutoPath.GetPullPath( strOtherCase ) );
    // Windows finds the file under the requested case, so compare the files, not the spellings
    CHECK( fs::equivalent( strOtherCase, root / "Sub" / "Dungeon_Training_lire_0.lua" ) );

    std::wstring wstrOtherCase = L"exact_name.LUA";
    CHECK( kAutoPath.GetPullPath( wstrOtherCase ) );

    std::string strMissing = "Not_There.lua";
    CHECK_FALSE( kAutoPath.GetPullPath( strMissing ) );
    CHECK( strMissing == "Not_There.lua" );

    fs::current_path( oldCurrent );
    fs::remove_all( root );
}
