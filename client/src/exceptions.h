#pragma once

#include <exception>
#include <iostream>
class SocketException: public std::exception
{
public:
    std::string& msg;
    explicit SocketException(std::string&& m) 
        : msg(m)
    {

    }
    virtual const char* what() const throw()
    {
        return msg.c_str();
    }
};