#pragma once
#include <exception>


struct ConnectionNotAvaible : public std::exception
{
	const char * what () const throw ()
    {
    	return "connection is not avaible";
    }
};

struct AuthFailed : public std::exception
{
	const char * what () const throw ()
    {
    	return "Authentication failed";
    }
};

