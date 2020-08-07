#pragma once
#include <exception>



struct IgnoreTemporaryFiles : public std::exception
{
	const char * what () const throw ()
    {
    	return "A temporary file has just been created, don't worry";
    }
};

