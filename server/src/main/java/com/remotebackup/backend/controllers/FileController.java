package com.remotebackup.backend.controllers;

import org.springframework.web.bind.annotation.DeleteMapping;
import org.springframework.web.bind.annotation.RestController;

@RestController
public class FileController {

    @DeleteMapping("/file")
    public void deleteFile() {
    }

}
