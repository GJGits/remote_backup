package com.remotebackup.backend.services;

import com.remotebackup.backend.dtos.JWToken;
import com.remotebackup.backend.dtos.SigninDTO;
import com.remotebackup.backend.dtos.SignupDTO;
import com.remotebackup.backend.exceptions.UserAlreadyExsistException;

import org.springframework.stereotype.Service;

@Service
public class UserService {

    public JWToken signup(SignupDTO signupDTO) throws UserAlreadyExsistException {
        return new JWToken("base64 token here...");
    }

    public JWToken signin(SigninDTO signinDTO) {
        return new JWToken("base64 token here...");
    }
    
}
