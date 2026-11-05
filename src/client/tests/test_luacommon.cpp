// LuaCommon (src/client/LuaCommon) on upstream LuaJIT: scripts load as the client loads them from .kom files.

#include "KLuaManager.h"
#include "KLuabinder.h"

#include <doctest/doctest.h>

#include <string>

TEST_CASE("UTF-8 scripts with a BOM load, as the client loads them")
{
    KLuabinder binder;
    REQUIRE(binder.GetLuaState() != nullptr);

    const std::string script = "\xEF\xBB\xBF" "count = 3\nflag = 1\nconfig = { size = 7 }\n";
    CHECK(binder.DoMemoryNotEncrypt(script.data(), static_cast<long>(script.size())));

    // A script without a BOM is refused, and asserts in debug builds (as in the original): not tested here.

    KLuaManager manager(binder.GetLuaState(), 0);  // depth 0: names are globals (1 means a table is on the stack)
    int count = 0;
    CHECK(manager.GetValue("count", count));
    CHECK(count == 3);
    bool flag = false;  // numbers only (0/1), as the client scripts write them
    CHECK(manager.GetValue("flag", flag));
    CHECK(flag);
    REQUIRE(manager.BeginTable("config"));
    int size = 0;
    CHECK(manager.GetValue("size", size));
    CHECK(size == 7);
    CHECK(manager.EndTable());
}
