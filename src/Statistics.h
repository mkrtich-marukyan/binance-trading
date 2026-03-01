#pragma once

#include <string>
#include <limits>

class Statistics
{
public:
    inline bool isValid() const { return m_isValid; }
    inline double totalVolume() const { return m_totalVolume; }
    inline double minPrice() const { return m_minPrice; }
    inline double maxPrice() const { return m_maxPrice; }
    inline unsigned long trades() const { return m_trades; }
    inline unsigned long buyerInitiated() const { return m_buyerInitiated; }
    inline unsigned long sellerInitiated() const { return m_sellerInitiated; }
    
    std::string toString() const;
    void reset();
    void update(double price, double quantity, bool sellerInitiated);
    
private:
    bool m_isValid{false};
    unsigned long m_trades{0}; // number of trades
    double m_totalVolume{0.0};   // sum(price * qty)
    //min / max price
    double m_minPrice{std::numeric_limits<double>::max()};
    double m_maxPrice{std::numeric_limits<double>::lowest()};
    // number of buyer-initiated vs seller-initiated trades.
    unsigned long m_buyerInitiated{0};
    unsigned long m_sellerInitiated{0};
};
