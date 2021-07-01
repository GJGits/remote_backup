package com.remotebackup.backend.exceptions;

import com.remotebackup.backend.dtos.ErrorDTO;
import org.springframework.http.ResponseEntity;
import org.springframework.security.authentication.BadCredentialsException;
import org.springframework.security.authentication.DisabledException;
import org.springframework.security.authentication.LockedException;
import org.springframework.security.core.userdetails.UsernameNotFoundException;
import org.springframework.web.bind.MethodArgumentNotValidException;
import org.springframework.web.bind.annotation.ControllerAdvice;
import org.springframework.web.bind.annotation.ExceptionHandler;

@ControllerAdvice
public class ExceptionHandlingConfiguration {

    /**
     * Handle input validation exceptions.
     * 
     * @MethodArgumentNotValidException is the default exception that is thrown when
     *                                  an input annotated with @Valid fails.
     */

    @ExceptionHandler(value = { MethodArgumentNotValidException.class, UserAlreadyExsistException.class,
            UsernameNotFoundException.class, BadCredentialsException.class, LockedException.class,
            DisabledException.class })
    protected ResponseEntity<Object> handleInvalidCredentials(RuntimeException ex) {
        ErrorDTO error = new ErrorDTO(ex.getMessage());
        return ResponseEntity.badRequest().body(error);
    }

}
