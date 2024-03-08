/////////////////////////////////////////////////////////////////////////////////
// @file            pwm_interface.cpp
// @brief           Class to interface with system pwm's
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
#include <algorithm>                // min and max
//
/////////////////////////////////////////////////////////////////////////////////

class PWM_Interface 
{
public:
    /// @brief enum for status returns
    enum class PWMStatus 
    {
        Success,
        Error,
        NotExported,
        InvalidInput,
    };

    /// @brief enum for PWM polarity types
    enum class PWMPolarity
    {
        Normal,
        Inverted,
        Unknown
    };

    /// @brief Constructor
    /// @param pwmPath - Path to the pwm Typically "/sys/class/pwm/pwmchipX"
    /// @param pwmChannel - [opt] - Specific channel on the PWM, typically not needed - defaults to 0
    /// @param pwmMinDegrees - [opt] - Adds ability to clamp the degrees to pwm via this minimum degree spec. 
    /// @param pwmMaxDegrees - [opt] - Adds ability to clamp the degrees to pwm via this max degree spec.
    PWM_Interface(const std::string& pwmPath, int pwmChannel = 0, double pwmMinDegrees = 0, double pwmMaxDegrees = 0)
        : m_pwmPath(pwmPath), m_pwmChannel(pwmChannel), m_minDegrees(pwmMinDegrees), m_maxDegrees(pwmMaxDegrees) {}

    /// @brief - export a PWM for usage
    /// @return - PWMStatus::Success if successful export, else PWMStatus::Error.
    PWMStatus ExportPWM()
    {
        if (IsExported() == PWMStatus::Success) { return PWMStatus::Success; }

        std::ofstream file(m_pwmPath + "/export");

        if (!file) { return PWMStatus::Error; }

        file << m_pwmChannel;
        file.close();

        return PWMStatus::Success;
    }

    /// @brief - unexport a PWM
    /// @return - PWMStatus::Success if successful unexport or already unexported, else PWMStatus::Error.
    PWMStatus UnExportPWM() 
    {
        if (IsExported() == PWMStatus::NotExported) { return PWMStatus::Success; }

        std::ofstream file(m_pwmPath + "/unexport");

        if (!file) { return PWMStatus::Error; }

        file << m_pwmChannel;
        file.close();

        return PWMStatus::Success;
    }

    /// @brief - Get the period of a PWM
    /// @param period_ns - output variable for return
    /// @return - PWMStatus::Success if successful period read, else PWMStatus::Error.
    PWMStatus GetPeriod(size_t& period_ns)
    {
        PWMStatus status = PWMStatus::Error;
        std::ifstream file(m_pwmPath + "/pwm" + std::to_string(m_pwmChannel) + "/period");

        if (!file) { return status; }

        std::string period;
        if (std::getline(file, period))
        {
            period_ns = std::stoi(period);
            status = PWMStatus::Success;
        }
        file.close();

        return status;
    }

    /// @brief - Get the PWM period 
    /// @return - period of PWM on sucessful read, else -1
    size_t GetPeriod()
    {
        size_t period = -1;
        if (GetPeriod(period) == PWMStatus::Error) return -1;
        else return period;
    }

    /// @brief - Set the PWM period
    /// @param period_ns - value to be set as period (Nanoseconds)
    /// @return - PWMStatus::Success if successful set of period, else PWMStatus::Error.
    PWMStatus SetPeriod(size_t period_ns)
    {
        std::ofstream file(m_pwmPath + "/pwm" + std::to_string(m_pwmChannel) + "/period");

        if (!file) { return PWMStatus::Error; }

        file << period_ns;
        file.close();

        return PWMStatus::Success;
    }

    /// @brief - Get the PWM duty cycle
    /// @param dutyCycle_ns - output variable for return
    /// @return - PWMStatus::Success if successful duty cycle read, else PWMStatus::Error.
    PWMStatus GetDutyCycle(size_t& dutyCycle_ns)
    {
        PWMStatus status = PWMStatus::Error;
        std::ifstream file(m_pwmPath + "/pwm" + std::to_string(m_pwmChannel) + "/duty_cycle");

        if (!file) { return status; }

        std::string duty;
        if (std::getline(file, duty))
        {
            dutyCycle_ns = std::stoi(duty);
            status = PWMStatus::Success;
        }
        file.close();

        return status;
    }

    /// @brief - Get the PWM Duty Cycle 
    /// @return - Duty cycle of PWM on sucessful read, else -1
    size_t GetDutyCycle()
    {
        size_t duty = -1;
        if (GetDutyCycle(duty) == PWMStatus::Error) return -1;
        else return duty;
    }

    /// @brief - Set the PWM Duty Cycle 
    /// @param dutyCycle_ns - value to be set as duty cycle (Nanoseconds)
    /// @return - PWMStatus::Success if successful set of duty cycle, else PWMStatus::Error.
    PWMStatus SetDutyCycle(int dutyCycle_ns)
    {
        std::ofstream file(m_pwmPath + "/pwm" + std::to_string(m_pwmChannel) + "/duty_cycle");

        if (!file) { return PWMStatus::Error; }

        file << dutyCycle_ns;
        file.close();

        return PWMStatus::Success;
    }

    /// @brief - Get the PWM polarity
    /// @param polarity - output variable for return
    /// @return - PWMStatus::Success if successful polarity read, else PWMStatus::Error.
    PWMStatus GetPolarity(PWMPolarity& polarity)
    {
        PWMStatus status = PWMStatus::Error;

        std::ifstream file(m_pwmPath + "/pwm" + std::to_string(m_pwmChannel) + "/polarity");

        if (!file) { return status; }

        std::string read;
        if (std::getline(file, read))
        {
            if (read == "normal") { polarity = PWMPolarity::Normal; status = PWMStatus::Success; }
            else if (read == "inverted") { polarity = PWMPolarity::Inverted; status = PWMStatus::Success; }
        }
        file.close();

        return status;
    }

    /// @brief - Get the PWM polarity
    /// @return - Polarity type based on read, else PWMPolarity::Unknwon
    PWMPolarity GetPolarity()
    {
        PWMPolarity polarity = PWMPolarity::Unknown;
        if(GetPolarity(polarity) == PWMStatus::Error) return PWMPolarity::Unknown;
        else return polarity;
    }

    /// @brief - Set the PWM polarity
    /// @param polarity - Desired polatiry type for the PWM
    /// @return - PWMStatus::InvalidInput if PWMPolarity::Unknown received, PWMStatus::Success on good set, else PWMStatus::Error
    PWMStatus SetPolarity(const PWMPolarity polarity)
    {
        if (DisablePWM() != PWMStatus::Success) { return PWMStatus::Error; }
        if (polarity == PWMPolarity::Unknown) { return PWMStatus::InvalidInput; }

        std::ofstream file(m_pwmPath + "/pwm" + std::to_string(m_pwmChannel) + "/polarity");

        if (!file) { return PWMStatus::Error; }

        if (polarity == PWMPolarity::Normal) { file << "normal"; }
        else if (polarity == PWMPolarity::Inverted) { file << "inverted"; }

        file.close();

        return PWMStatus::Success;
    }

    /// @brief - Enable the PWM
    /// @return - PWMStatus::Sucess if good set, PWMStatus::NotExported if not exported, else PWMStatus::Error
    PWMStatus EnablePWM()
    {
        return EnableDisablePWM(true);
    }

    /// @brief - Disable the PWM
    /// @return - PWMStatus::Sucess if good set, PWMStatus::NotExported if not exported, else PWMStatus::Error
    PWMStatus DisablePWM()
    {
        return EnableDisablePWM(false);
    }

private:
    std::string m_pwmPath;  // path of the pwm
    int m_pwmChannel;       // channel of the pwm
    double m_maxDegrees;    // maximum degree range for the pwm
    double m_minDegrees;    // minimum degree range for the pwm

    /// @brief function to enable or disable a PWM
    /// @param value - bool to enable (true) or disable (false) PWM
    /// @return - PWMStatus::Sucess if good set, PWMStatus::NotExported if not exported, else PWMStatus::Error
    PWMStatus EnableDisablePWM(bool value)
    {
        if (IsExported() != PWMStatus::Success) { return PWMStatus::NotExported; }

        std::ofstream file(m_pwmPath + "/pwm" + std::to_string(m_pwmChannel) + "/enable");

        if (!file) { return PWMStatus::Error; }
        
        file << std::to_string(value);
        file.close();

        return PWMStatus::Success;
    }

    /// @brief Validate that a PWM is exported
    /// @return PWMStatus::Success if exported, else PWMStatus::NotExported
    PWMStatus IsExported() 
    {
        PWMStatus status = PWMStatus::NotExported;

        std::ifstream file(m_pwmPath + "/export");
        std::string line;

        if (std::getline(file, line))
        {
            if (line == std::to_string(m_pwmChannel)) { status = PWMStatus::Success; }
        }
        file.close();

        return status;
    }

    /// @brief Constrains a value within the MIN and MAX values
    /// @param value - value to be checked / constrained
    /// @return - constrained value
    double Constrain(double value) const
    {
        return std::max(std::min(value, m_maxDegrees), m_minDegrees);
    }

    /// @brief Convert degrees into A valid PWM signal, Will constrain to min/max range if thy're not zero. 
    /// @param degrees - input for desired degree of input
    /// @return - converted PWM value
    size_t DegreesToPWM(double degrees) 
    {
        // Make sure value is within bounds, if bounds are set
        if (m_maxDegrees != 0 && m_minDegrees != 0) { degrees = Constrain(degrees); }

        // Map the degrees to pulse width linearly
        int pulseWidth = 900 + ((degrees + 60) * (2100 - 900)) / 120;

        return pulseWidth;
    }

};