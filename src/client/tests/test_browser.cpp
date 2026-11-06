// KBrowserWrapper: web pages in the system browser instead of the embedded Internet Explorer
// (docs/porting/client-ktdxlib.md#web-pages).

#include "KBrowserWrapper.h"

#include <doctest/doctest.h>

#include <string>
#include <vector>

namespace {

std::vector<std::string> g_opened;
bool g_openSucceeds = true;

bool RecordOpen(const char* url)
{
    g_opened.emplace_back(url);
    return g_openSucceeds;
}

struct OpenUrlFake {
    OpenUrlFake()
    {
        g_opened.clear();
        g_openSucceeds = true;
        KBrowserWrapper::SetOpenUrlFunction(&RecordOpen);
    }
    ~OpenUrlFake() { KBrowserWrapper::SetOpenUrlFunction(nullptr); }
};

}  // namespace

TEST_CASE("browser: a page opens in the system browser and the balance check waits for the player")
{
    OpenUrlFake fake;
    KBrowserWrapper wrapper;
    CHECK(wrapper.Init());
    CHECK(wrapper.IsClosed());
    CHECK(wrapper.GetCheckClosed());
    CHECK_FALSE(wrapper.Navigate("https://example.com/", nullptr));  // not created
    CHECK(g_opened.empty());

    RECT rect = {0, 0, 1000, 676};
    CHECK(wrapper.Create(rect, nullptr));
    CHECK_FALSE(wrapper.IsClosed());
    CHECK(wrapper.Navigate("https://example.com/charge?id=1", nullptr));
    REQUIRE(g_opened.size() == 1);
    CHECK(g_opened[0] == "https://example.com/charge?id=1");
    CHECK(wrapper.IsClosed());         // nothing stays open in the game: the shop can open it again
    CHECK(wrapper.GetCheckClosed());   // but the balance is not asked for yet

    CHECK(wrapper.OnFocusGained());    // back from the browser
    CHECK_FALSE(wrapper.GetCheckClosed());
    wrapper.SetCheckClosed(true);      // CX2State asks for the balance once
    CHECK_FALSE(wrapper.OnFocusGained());
    CHECK(wrapper.GetCheckClosed());
    CHECK(wrapper.UnInit());
}

TEST_CASE("browser: closing without a page, failures and POST")
{
    OpenUrlFake fake;
    KBrowserWrapper wrapper;
    RECT rect = {};

    wrapper.Create(rect, nullptr);
    wrapper.CloseBrowser();            // as the original dialog's close
    CHECK(wrapper.IsClosed());
    CHECK_FALSE(wrapper.GetCheckClosed());
    CHECK_FALSE(wrapper.OnFocusGained());

    wrapper.Create(rect, nullptr);
    CHECK_FALSE(wrapper.Navigate("https://example.com/", "a=1"));  // POST is not supported
    CHECK_FALSE(wrapper.Navigate("", nullptr));
    CHECK_FALSE(wrapper.Navigate(nullptr, nullptr));
    CHECK(g_opened.empty());
    CHECK_FALSE(wrapper.IsClosed());

    g_openSucceeds = false;            // no browser: closed, as if the player closed it at once
    CHECK_FALSE(wrapper.Navigate("https://example.com/", nullptr));
    CHECK(g_opened.size() == 1);
    CHECK(wrapper.IsClosed());
    CHECK_FALSE(wrapper.GetCheckClosed());
    CHECK_FALSE(wrapper.OnFocusGained());
}
