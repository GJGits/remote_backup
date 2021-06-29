package com.remotebackup.backend.services;

import java.util.ArrayList;
import java.util.List;

import com.remotebackup.backend.entities.UserEntity;
import org.springframework.security.core.userdetails.UserDetails;
import org.springframework.security.core.userdetails.UserDetailsService;
import org.springframework.security.core.userdetails.UsernameNotFoundException;
import org.springframework.stereotype.Service;

@Service
public class UserService implements UserDetailsService {

    @Override
    public UserDetails loadUserByUsername(String username) throws UsernameNotFoundException {
        // TODO Auto-generated method stub
        // return new UserEntity(username, "some password");
        return null;
    }

    // TODO: REFACTOR THIS!!! 
    public List<String> getUserRoles() {
        // List<String> roles = new ArrayList<>();
        // roles.add("USER");
        // return roles;
        return null;
    }

}
