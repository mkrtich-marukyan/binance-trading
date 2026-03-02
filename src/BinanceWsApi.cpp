#include "BinanceWsApi.h"
#include "Properties.h"
#include <iostream>

BinanceWsApi::BinanceWsApi(ThreadSafeMessageQueue& queue, const std::string& symbol)
    : m_msgQueue(queue), m_symbol(symbol), m_running(false), m_lastMessageTime(Clock::now())
{
    // all symbols for streams should be lowercase
    for (char& c : m_symbol) 
    {
        c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
    }
}

BinanceWsApi::~BinanceWsApi()
{
    try
    {
        if(isRunning())
        {
            stop();
        }
    }
    catch(...)
    {
        // destructors never throw
    }
}

void BinanceWsApi::start()
{
    if (m_running)
    {
        return;
    }
    
    m_ioc = std::make_unique<net::io_context>();
    m_sslCtx = std::make_unique<ssl::context>(ssl::context::tlsv12_client);
    m_sslCtx->set_default_verify_paths();
    m_resolver = std::make_unique<tcp::resolver>(*m_ioc);
    m_ws = std::make_unique<websocket::stream<beast::ssl_stream<tcp::socket>>>(*m_ioc, *m_sslCtx);
    m_lastMessageTime = Clock::now();
    m_running = true;
    m_worker = std::thread(&BinanceWsApi::run, this);
}

void BinanceWsApi::stop()
{
    std::lock_guard lock(m_mutex);
    
    m_running = false;
    
    if (m_ws)
    {
        m_ws->async_close(websocket::close_code::normal, [](boost::beast::error_code ec)
        {
            if (ec)
            {
                std::cerr << "WebSocket close error: " << ec.message() << std::endl;
            }
        });
    }

    if (m_ioc) m_ioc->stop(); // stop all async operations
    
    if (m_worker.joinable())
    {
        m_worker.join();
    }

    m_ws.reset();
    m_resolver.reset();
    m_sslCtx.reset();
    m_ioc.reset();
}

bool BinanceWsApi::isRunning() const
{
    return m_running;
}

TimePoint BinanceWsApi::getLastMessageTime() const
{
    return m_lastMessageTime;
}

void BinanceWsApi::run()
{
    try 
    {
        // initialize connection
        auto const results = m_resolver->resolve(Properties::Host, Properties::Port);
        net::connect(m_ws->next_layer().next_layer(), results);
        m_ws->next_layer().handshake(ssl::stream_base::client);
        m_ws->handshake(Properties::Host, "/ws/" + m_symbol + "@trade");

        // start async reading
        auto buffer = std::make_shared<beast::flat_buffer>();

        std::function<void()> do_read = [this, buffer, &do_read]()
        {
            m_ws->async_read(*buffer, [this, buffer, &do_read](boost::beast::error_code ec, std::size_t bytes_transferred)
            {
                if (!ec && m_running)
                {
                    std::string msg = beast::buffers_to_string(buffer->data());
                    buffer->consume(buffer->size());
                    m_msgQueue.push(msg);
                    m_lastMessageTime = Clock::now();

                    // schedule next async read
                    do_read();
                }
                else if(ec != boost::asio::error::operation_aborted)
                {
                    std::cerr << "WebSocket error: " << ec.message() << std::endl;
                    m_running = false;
                }
            });
        };

        do_read(); // start first async read
        
        m_ioc->run();
    }
    catch (std::exception& e) 
    {
        std::cerr << "Api Error: " << e.what() << std::endl;
        m_running = false;
    }
}
