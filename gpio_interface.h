/////////////////////////////////////////////////////////////////////////////////
// @file            gpio_interface.cpp
// @brief           Class to interface with system gpio's via /sys/class/gpio
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
//
/////////////////////////////////////////////////////////////////////////////////

class GPIO_Interface 
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

    GPIO_Interface(const std::string& gpioPath, const int gpioPin) : 
        m_gpioPath(gpioPath), m_gpioPin(gpioPin) {}

    Status ExportGPIO()
    {
        std::string path = m_gpioPath + "/export";

        std::ofstream export_file(path);

        return Status::Success;
    }

    Status UnExportGPIO()
    {
        std::string path = m_gpioPath + "/unexport";

        std::ofstream unexport_file(path);

        return Status::Success;
    }

    Status SetEdge(const Edge edge)
    {
        if (edge == Edge::Unknown) { return Status::Error; }

        std::string direction_path = m_gpioPath + "/edge";

        if (IsExported()) { return Status::NotExported; }

        std::ofstream direction_file(direction_path);

        if (!direction_file) { return Status::Error; }

        direction_file << edgeMap.at(edge);

        return Status::Success;
    }

    Status GetEdge(Edge& edge)
    {
        std::string edge_path = m_gpioPath + "/edge";
        std::ifstream edge_file(edge_path);

        if (!edge_file)
        {
            std::cerr << "Error: Unable to open edge file." << std::endl;
            return Status::Error;
        }

        std::string edge_str;
        edge_file >> edge_str;

        for (const auto& entry : edgeMap)
        {
            if (entry.second == edge_str)
            {
                edge = entry.first;
                return Status::Success;
            }
        }

        std::cerr << "Error: Unknown GPIO edge: " << edge_str << std::endl;
        return Status::Error;
    }

    Edge GetEdge() const
    {
        std::string edge_path = m_gpioPath + "/edge";
        std::ifstream edge_file(edge_path);

        if (!edge_file)
        {
            std::cerr << "Error: Unable to open edge file." << std::endl;
            return Edge::Unknown;
        }

        std::string edge_str;
        edge_file >> edge_str;

        for (const auto& entry : edgeMap)
        {
            if (entry.second == edge_str)
            {
                return entry.first;
            }
        }

        std::cerr << "Error: Unknown GPIO edge: " << edge_str << std::endl;
        return Edge::Unknown;
    }

    Status SetDirection(const Direction direction)
    {
        if (direction == Direction::Unknown) { return Status::Error; }

        std::string direction_path = m_gpioPath + "/direction";

        if (IsExported()) { return Status::NotExported; }

        std::ofstream direction_file(direction_path);

        if (!direction_file) { return Status::Error; }

        direction_file << directionMap.at(direction);

        return Status::Success;
    }

    Direction GetDirection() const
    {
        std::string direction_path = m_gpioPath + "/direction";
        std::ifstream direction_file(direction_path);

        if (!direction_file) 
        {
            std::cerr << "Error: Unable to open direction file." << std::endl;
            return Direction::Unknown;
        }

        std::string direction_str;
        direction_file >> direction_str;

        for (const auto& entry : directionMap)
        {
            if (entry.second == direction_str)
            {
                return entry.first;
            }
        }

        std::cerr << "Error: Unknown direction value: " << direction_str << std::endl;
        return Direction::Unknown;
    }

    Status SetValue(int value)
    {
        std::string valuePath = m_gpioPath + "/value";

        if (!IsExported()) { return Status::NotExported; }

        std::ofstream value_file(valuePath);

        if (!value_file) { return Status::Error; }

        value_file << value;

        return Status::Success;
    }

    Status GetValue(int& value)
    {
        std::string valuePath = m_gpioPath + "/value";

        if (!IsExported()) { return Status::NotExported; }

        std::ifstream value_file(valuePath);

        if (!value_file) { return Status::Error; }

        value_file >> value;

        return Status::Success;
    }

private:
    const std::unordered_map<Direction, std::string> directionMap
    {
        {Direction::In,         "in"},
        {Direction::Out,        "out"},
        {Direction::Unknown,    "unknown"}
    };

    const std::unordered_map<Edge, std::string> edgeMap
    {
        {Edge::None,            "none"},
        {Edge::Rising,          "rising"},
        {Edge::Falling,         "falling"},
        {Edge::Both,            "both"},
        {Edge::Unknown,         "unknown"}
    };

    bool IsExported() 
    {
        std::ifstream export_file(m_gpioPath);
        return export_file.good();
    }

    std::string m_gpioPath;
    int m_gpioPin;
};
