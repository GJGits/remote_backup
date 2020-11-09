#pragma once
#include "../common/logger.hpp"
#include "../http/reply.hpp"
#include "../http/request.hpp"
#include <exception>

struct UsernameAlreadyExists : public std::exception {
  const char *what() const throw() {
    return "The username already exists in the database, please use another "
           "username";
  }
};

struct CredentialsNotValidException : public std::exception {
  const char *what() const throw() {
    return "username or password inserted are not valid, please try again";
  }
};

struct WrongRquestFormat : public std::exception {
  const char *what() const throw() {
    Logger::log("The format of the request is wrong!");

    return "The format of the request is wrong!";
  }
};

struct CredentialsExpired : public std::exception {
  const char *what() const throw() {
    Logger::log(" The credentials are expired, please log in again and retry!");

    return "The credentials are expired, please log in again and retry!";
  }
};

struct ChunkCorrupted : public std::exception {
  const char *what() const throw() {
    Logger::log("The chunk received is different from the one calculated\n");

    return "The chunk received is different from the one calculated";
  }
};

struct FileNotOpened : public std::exception {
  const char *what() const throw() {
    Logger::log("The file could not be open\n");

    return "The file could not be open";
  }
};

struct InvalidJWT : public std::exception {
  const char *what() const throw() { return "Auth failed"; }
};

struct Base64DecodeFailed : public std::exception {
  const char *what() const throw() { return "decoding failed"; }
};

struct ExceededNumberOfDevices : public std::exception {
  const char *what() const throw() {
    return "Exceeded maximum number of devices allowed";
  }
};

struct MissingElementInDB : public std::exception {
  const char *what() const throw() {
    Logger::log("The element searched in the DB is not found");
    return "The element searched in the DB is not found";
  }
};
