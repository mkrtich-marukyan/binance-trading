#include "Statistics.h"
#include <iomanip>
#include <sstream>

std::string Statistics::toString() const 
{
    std::ostringstream oss;
    // formatting with 2 decimal
    oss << std::fixed << std::setprecision(2);

    oss << " trades=" << m_trades
        << " totalvolume=" << m_totalVolume
        << " min=" << m_minPrice
        << " max=" << m_maxPrice
        << " buy=" << m_buyerInitiated
        << " sell=" << m_sellerInitiated;

    return oss.str();
}

void Statistics::reset()
{
    m_isValid = false;
    m_trades = 0;
    m_totalVolume = 0.0;
    m_minPrice = std::numeric_limits<double>::max();
    m_maxPrice = std::numeric_limits<double>::lowest();
    m_buyerInitiated = 0;
    m_sellerInitiated = 0;
}

void Statistics::update(double price, double quantity, bool sellerInitiated)
{
    m_isValid = true;
    m_trades++;
    m_totalVolume += price * quantity;
    m_minPrice = std::min(m_minPrice, price);
    m_maxPrice = std::max(m_maxPrice, price);
    if (sellerInitiated) 
    {
	m_sellerInitiated++;
    }
    else 
    {
	m_buyerInitiated++;
    }
}
