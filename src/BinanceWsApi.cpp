#include "BinanceWsApi.h"
#include "Properties.h"
#include <boost/beast.hpp>
#include <boost/asio.hpp>
#include <boost/beast/ssl.hpp>
#include <iostream>

namespace beast = boost::beast;
namespace websocket = beast::websocket;
namespace net = boost::asio;
namespace ssl = net::ssl;
using tcp = net::ip::tcp;

BinanceWsApi::BinanceWsApi(ThreadSafeMessageQueue& queue, const std::string& symbol)
    : m_msgQueue(queue), m_symbol(symbol), m_running(false)
{
    // all symbols for streams should be lowercase
    for (char& c : m_symbol) 
    {
        c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
    }
}

BinanceWsApi::~BinanceWsApi()
{
    if(isRunning())
    {
        stop();
    }
}

void BinanceWsApi::start()
{
    m_running = true;
    m_worker = std::thread(&BinanceWsApi::run, this);
}

void BinanceWsApi::stop()
{
    m_running = false;
    if (m_worker.joinable())
    {
        m_worker.join();
    }
}

bool BinanceWsApi::isRunning() const
{
    return m_running;
}

void BinanceWsApi::run()
{
    try 
    {
        net::io_context ioc;
        ssl::context ctx{ssl::context::tlsv12_client};
        ctx.set_default_verify_paths();

        tcp::resolver resolver{ioc};
        websocket::stream<beast::ssl_stream<tcp::socket>> ws{ioc, ctx};

        auto const results = resolver.resolve(Properties::Host, Properties::Port);
        net::connect(ws.next_layer().next_layer(), results);

        ws.next_layer().handshake(ssl::stream_base::client);
        ws.handshake(Properties::Host, "/ws/" + m_symbol + "@trade");

        beast::flat_buffer buffer;

        while (m_running) 
	{
            ws.read(buffer);
            std::string msg = beast::buffers_to_string(buffer.data());
            buffer.consume(buffer.size());
	    m_msgQueue.push(msg);
        }
    }
    catch (std::exception& e) 
    {
        std::cerr << "Api Error: " << e.what() << std::endl;
	stop();
    }
}
