#include <cstdlib>
#include "Properties.h"
#include "BinanceTradingApp.h"

int main() 
{
    const std::vector<std::string> simbols = {Properties::BTCUSDT, Properties::ETHUSDT};
    const unsigned int time = 1;
    const std::string outputDef = "statistics.txt";
    const char* outputFile = std::getenv("OUTPUT_FILE");
    
    BinanceTradingApp app(simbols, time, outputFile ? outputFile : outputDef);
    app.exec();
    
    return 0;
}
