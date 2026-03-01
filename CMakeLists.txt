cmake_minimum_required(VERSION 3.16)

project(binance-trading VERSION 1.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(nlohmann_json REQUIRED)
find_package(Boost REQUIRED COMPONENTS system)
find_package(OpenSSL REQUIRED)

add_executable(binance-trading 
	src/main.cpp
	src/BinanceTradingApp.cpp
	src/BinanceWsApi.cpp
	src/ThreadSafeMessageQueue.cpp
	src/StatisticsCollector.cpp
	src/TradingOutput.cpp
	src/Statistics.cpp
	src/TradingMessageParser.cpp
	)

target_include_directories(binance-trading PRIVATE src)

target_link_libraries(binance-trading
    nlohmann_json::nlohmann_json
    Boost::system
    OpenSSL::SSL
    OpenSSL::Crypto
    pthread
)

# unit testing
enable_testing()

find_package(GTest REQUIRED)

add_executable(unit_tests
    tests/test_statistics.cpp
    tests/test_trading_message_parser.cpp
    tests/test_trading_output.cpp
    tests/test_thread_safe_message_queue.cpp
    src/Statistics.cpp
    src/TradingMessageParser.cpp
    src/TradingOutput.cpp
    src/ThreadSafeMessageQueue.cpp
)

target_link_libraries(unit_tests
    nlohmann_json::nlohmann_json
    GTest::gtest
    GTest::gtest_main
    pthread
)

add_test(NAME unit_tests COMMAND unit_tests)
