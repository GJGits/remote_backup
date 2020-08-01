#pragma once
#include <exception>
#include "../http/reply.hpp"
#include "../http/request.hpp"



struct UsernameAlreadyExists : public std::exception
{
	const char * what () const throw ()
    {
    	return "The username already exists in the database, please use another username";
    }
};

struct CredentialsNotValidException : public std::exception
{
    const char * what () const throw ()
    {
    	return "username or password inserted are not valid, please try again";
    }

};

struct UsernameNotExists : public std::exception
{
    const char * what () const throw ()
    {
        return "The username inserted doesn't exist in our database, please try again";
    }

};

struct PasswordNeqConfirm : public std::exception
{
    const char * what () const throw ()
    {
        return "Password and Password confirm are not equal, please try again";
    }

};

struct UknownError : public std::exception
{
    const char * what () const throw ()
    {
        return "An uknown error has occurred, please try again or contact your system administrator";
    }

};