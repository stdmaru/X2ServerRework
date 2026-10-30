# EPublisher service tests (docs/porte/server-epublisher.md): SOAP parsing, the operations over a fake store, and
# the whole service called by the servers' own gSOAP client (Common/OnlyGlobal/AuthAndBilling/DEB).
add_executable(x2_epublisher_tests
    main.cpp
    test_epublisher.cpp
    ${PROJECT_SOURCE_DIR}/src/server/Common/tests/server_hooks.cpp)
target_link_libraries(x2_epublisher_tests PRIVATE x2_epublisher doctest::doctest)
add_test(NAME epublisher COMMAND x2_epublisher_tests)
