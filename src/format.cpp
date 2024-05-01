#include <string>
#include <iomanip>
#include <sstream>
#include "format.h"

using std::string;

string Format::ElapsedTime(long seconds) 
{
    long hours = seconds / 3600;
    seconds %= 3600;
    long minutes = seconds / 60;
    seconds %= 60;

    std::ostringstream timeStream;
    timeStream << std::setw(2) << std::setfill('0') << hours << ":"
               << std::setw(2) << std::setfill('0') << minutes << ":"
               << std::setw(2) << std::setfill('0') << seconds;

    return timeStream.str();
}