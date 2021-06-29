package com.remotebackup.backend.services;

import com.remotebackup.backend.dtos.JWToken;
import com.remotebackup.backend.dtos.SigninDTO;
import com.remotebackup.backend.dtos.SignupDTO;
import com.remotebackup.backend.entities.UserEntity;
import com.remotebackup.backend.exceptions.UserAlreadyExsistException;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.security.authentication.AuthenticationManager;
import org.springframework.security.authentication.UsernamePasswordAuthenticationToken;
import org.springframework.stereotype.Service;

@Service
public class AuthService {

    @Autowired
    private JwtTokenService jwtTokenService;

    @Autowired
    AuthenticationManager authenticationManager;

    @Autowired
    UserService userService;

    public JWToken signup(SignupDTO signupDTO) throws UserAlreadyExsistException {
        return new JWToken(jwtTokenService.createToken(signupDTO.getUsername(), userService.getUserRoles()));
    }

    public JWToken signin(SigninDTO signinDTO) {
        authenticationManager.authenticate(new UsernamePasswordAuthenticationToken(signinDTO.getUsername(), signinDTO.getPassword()));
        UserEntity userEntity = (UserEntity) userService.loadUserByUsername(signinDTO.getUsername());
        return new JWToken(jwtTokenService.createToken(userEntity.getUsername(), userEntity.getRoles()));
    }
    
}
