#include "x2/platform/bimap.h"

#include <doctest/doctest.h>

#include <string>

TEST_CASE("bimap: both views, one-to-one inserts, copies")
{
    x2::bimap<char, std::string> map;
    CHECK(map.insert({1, "LOADING"}));
    CHECK(map.insert({0, "NONE"}));
    CHECK_FALSE(map.insert({0, "READY"}));      // left key taken
    CHECK_FALSE(map.insert({2, "LOADING"}));    // right key taken
    CHECK(map.size() == 2);

    auto left = map.left.find(1);
    REQUIRE(left != map.left.end());
    CHECK(left->second == "LOADING");
    CHECK(map.left.begin()->first == 0);        // ordered by the left key
    auto right = map.right.find("NONE");
    REQUIRE(right != map.right.end());
    CHECK(right->second == 0);
    CHECK(map.right.at("LOADING") == 1);
    CHECK_THROWS(map.left.at(5));

    x2::bimap<char, std::string> copy = map;
    map.clear();
    CHECK(map.empty());
    CHECK(map.left.empty());
    CHECK(copy.size() == 2);
    CHECK(copy.right.at("NONE") == 0);           // the copy's views see the copy
    map = copy;
    CHECK(map.left.at(1) == "LOADING");
}
