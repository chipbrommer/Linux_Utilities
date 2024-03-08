/////////////////////////////////////////////////////////////////////////////////
// @file            gpiod_interface.cpp
// @brief           Class to interface with system gpio's via libgpiod
// @author          Chip Brommer
/////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////
//
// Includes:
//          name                    reason included
//          ------------------      ------------------------
#include <string>                   // strings
#include <iostream>                 // console io
#include <fstream>                  // file io
#include <unordered_map>            // direction map
#include "gpio.h"                   // libgpiod API
//
/////////////////////////////////////////////////////////////////////////////////

class GPIOD_Interface
{
public:
    enum class Status : int
    {
        Success,
        Error,
        NotExported
    };

    enum class Direction : int
    {
        In,
        Out,
        Unknown,
    };

    enum class Edge : int
    {
        None,
        Rising,
        Falling,
        Both,
        Unknown
    };

    GPIOD_Interface(const int chipNumber, const int lineNumber) :
        m_gpioPath(gpioPath), m_gpioPin(gpioPin) {}

    

private:
    const std::unordered_map<Direction, std::string> directionMap
    {
        {Direction::In,     "in"},
        {Direction::Out,    "out"},
        {Direction::Unknown,"unknown"}
    };

    const std::unordered_map<Edge, std::string> edgeMap
    {
        {Edge::None,        "none"},
        {Edge::Rising,      "rising"},
        {Edge::Falling,     "falling"},
        {Edge::Both,        "both"},
        {Edge::Unknown,     "unknown"}
    };

    bool IsGpioLineExported() 
    {
        return gpiod_line_is_requested(mLine);
    }

    gpiod_chip* mChip;
    gpiod_line* mLine;
    int mLineNumber;
    bool mIsOutput;
};
