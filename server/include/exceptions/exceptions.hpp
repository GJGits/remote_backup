#pragma once
#include <exception>
#include "../http/reply.hpp"
#include "../http/request.hpp"
#include "../common/logger.hpp"


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
        Logger::log(" An uknown error has occurred, please try again or contact your system administrator => ");

        return "An uknown error has occurred, please try again or contact your system administrator";
    }

};

struct WrongRquestFormat : public std::exception
{
const char * what () const throw ()
{
    Logger::log("The format of the request is wrong!");

    return "The format of the request is wrong!";
}

};

struct ControllerNotRetrievable : public std::exception
{
    const char * what () const throw ()
    {
        Logger::log(" The controller is not retrievable, please contact administrator for further infos => ");

        return "The controller is not retrievable, please contact administrator for further infos";
    }

};

struct CredentialsExpired : public std::exception
{
    const char * what () const throw ()
    {
        Logger::log(" The credentials are expired, please log in again and retry!");

        return "The credentials are expired, please log in again and retry!";
    }

};

struct ChunkCorrupted : public std::exception
{
    const char * what () const throw ()
    {
        Logger::log("The chunk received is different from the one calculated\n");

        return "The chunk received is different from the one calculated";
    }


};

struct FileSizeNotAvailable : public std::exception
{
    const char * what () const throw ()
    {
        Logger::log(" The size of file is not retrievable\n");

        return "The size of file is not retrievable";
    }

};

struct FileNotDeleted : public std::exception
{
    const char * what () const throw ()
    {
        Logger::log("The file could not be deleted\n");

        return "The file could not be deleted";
    }

};

struct FileNotOpened : public std::exception
{
    const char * what () const throw ()
    {
        Logger::log("The file could not be open\n");

        return "The file could not be open";
    }

};

struct DatabaseInvalidConnection : public std::exception
{
    const char * what () const throw ()
    {
        Logger::log("The database connection is not available\n");

        return "The database connection is not available";
    }

};

struct InvalidJWT : public std::exception
{
    const char * what () const throw ()
    {
        return "Auth failed";
    }

};

struct Base64DecodeFailed : public std::exception
{
    const char * what () const throw ()
    {
        return "decoding failed";
    }

};

struct ExceededNumberOfDevices : public std::exception
{
const char * what () const throw ()
{
    return "Exceeded maximum number of devices allowed";
}

};

struct MissingElementInDB : public std::exception
{
    const char * what () const throw ()
    {
        return "The element searched in the DB is not found";
    }

};