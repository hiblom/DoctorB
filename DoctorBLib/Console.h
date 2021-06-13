#pragma once

#define LOGGING 0

#include <iostream>
#include <fstream>
#include <ios>
#include <string>

class Console
{
public:
    static Console& getInstance();

    template<typename T>
    Console& operator<<(const T& val)
    {
#if LOGGING == 1
        if (log_file.is_open()) {
            log_file << val;
            log_file.flush();
        }
#endif
        std::cout << val;
        return *this;
    };

    Console& operator<< (std::ostream& (*f)(std::ostream&)) {
#if LOGGING == 1
        if (log_file.is_open()) {
            f(log_file);
            log_file.flush();
        }
#endif
        f(std::cout);
        return *this;
    }

    Console& operator<< (std::ostream& (*f)(std::ios&)) {
#if LOGGING == 1
        if (log_file.is_open()) {
            f(log_file);
            log_file.flush();
        }
#endif
        f(std::cout);
        return *this;
    }

    Console& operator<< (std::ostream& (*f)(std::ios_base&)) {
#if LOGGING == 1
        if (log_file.is_open()) {
            f(log_file);
            log_file.flush();
        }
#endif
        f(std::cout);
        return *this;
    }

    bool readLine(std::string& val)
    {
        std::getline(std::cin, val);
#if LOGGING == 1
        if (log_file.is_open()) {
            log_file << val << std::endl;
            log_file.flush();
        }
#endif
        return true;
    };

private:
    Console();
    ~Console();
#if LOGGING == 1
    std::ofstream log_file;
#endif

};

