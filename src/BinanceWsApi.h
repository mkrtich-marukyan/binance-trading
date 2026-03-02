#pragma once
#include <thread>
#include <string>
#include <mutex>
#include <chrono>
#include "ThreadSafeMessageQueue.h"
#include <boost/beast.hpp>
#include <boost/asio.hpp>
#include <boost/beast/ssl.hpp>

namespace beast = boost::beast;
namespace websocket = beast::websocket;
namespace net = boost::asio;
namespace ssl = net::ssl;
using tcp = net::ip::tcp;

using Clock = std::chrono::steady_clock;
using TimePoint = Clock::time_point;

class BinanceWsApi
{
public:
    explicit BinanceWsApi(ThreadSafeMessageQueue& queue, const std::string& symbol);
    ~BinanceWsApi();
    void start();
    void stop();
    
    bool isRunning() const;
    TimePoint getLastMessageTime() const;

private:
    void run();

private:
    ThreadSafeMessageQueue& m_msgQueue;
    std::string m_symbol;
    std::atomic<bool> m_running;
    std::atomic<TimePoint> m_lastMessageTime;
    std::thread m_worker;
    std::mutex m_mutex;
    std::unique_ptr<net::io_context> m_ioc;
    std::unique_ptr<ssl::context> m_sslCtx;
    std::unique_ptr<tcp::resolver> m_resolver;
    std::unique_ptr<websocket::stream<beast::ssl_stream<tcp::socket>>> m_ws;
};
