#pragma once
#include <exception>

struct MyException : public std::exception
{
	const char * what () const throw ()
    {
    	return "C++ Exception";
    }
};

struct CredentialsNotValidException : public std::exception
{
    const char * what () const throw ()
    {
    	return "Credenziali inserite non valide";
    }

};