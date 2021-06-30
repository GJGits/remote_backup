package com.remotebackup.backend.configurations;

import com.remotebackup.backend.entities.UserEntity;
import com.remotebackup.backend.services.UserService;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.security.authentication.AuthenticationManager;
import org.springframework.security.authentication.BadCredentialsException;
import org.springframework.security.authentication.UsernamePasswordAuthenticationToken;
import org.springframework.security.core.Authentication;
import org.springframework.security.core.AuthenticationException;
import org.springframework.security.crypto.password.PasswordEncoder;

public class MyAuthenticationManager implements AuthenticationManager {

    @Autowired
    UserService userService;

    @Autowired
    PasswordEncoder passwordEncoder;

    @Override
    public Authentication authenticate(Authentication authenticationRequestToken) throws AuthenticationException {

        System.out.println("CHIAMO AUTHENTICATE");

        String requestUsername = (String) authenticationRequestToken.getPrincipal();
        String requestRawPassword = (String) authenticationRequestToken.getCredentials();
        UserEntity userStored = (UserEntity) this.userService.loadUserByUsername(requestUsername);

        /*
         * Consider more checks to be more adherent to AuthenticationManager contract
         * (https://docs.spring.io/spring-security/site/docs/4.2.15.RELEASE/apidocs/org/
         * springframework/security/authentication/AuthenticationManager.html) and to be
         * more consistent with possible user states on the db.
         */

        if (!passwordEncoder.matches(requestRawPassword, userStored.getPassword()))
            throw new BadCredentialsException("");

        System.out.println("AUTENTICATO");

        // if (!userStored.isEnabled())
        //     throw new DisabledException("");

        return new UsernamePasswordAuthenticationToken(requestUsername, null);
    }

}
