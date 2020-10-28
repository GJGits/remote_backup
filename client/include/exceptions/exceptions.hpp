#pragma once
#include <exception>


struct FileStructNotValid : public std::exception
{
	const char * what () const throw ()
    {
    	return "File struct not found or invalid";
    }
};

struct FileConfigNotValid : public std::exception
{
	const char * what () const throw ()
    {
    	return "File config not found or invalid";
    }
};

struct SyncNotValid : public std::exception
{
	const char * what () const throw ()
    {
    	return "Sync folder not found";
    }
};

struct TmpNotValid : public std::exception
{
	const char * what () const throw ()
    {
    	return "sync/.tmp folder not found";
    }
};

struct BinNotValid : public std::exception
{
	const char * what () const throw ()
    {
    	return "sync/.bin folder not found";
    }
};

struct ConnectionNotAvaible : public std::exception
{
	const char * what () const throw ()
    {
    	return "connection is not avaible";
    }
};

