package com.remotebackup.backend.exceptions;

public class UserAlreadyExsistException extends RuntimeException{
    public UserAlreadyExsistException() {
        super("An account with this username already exsists");
    }
}
