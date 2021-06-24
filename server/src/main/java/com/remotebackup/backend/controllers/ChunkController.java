package com.remotebackup.backend.controllers;

import org.springframework.web.bind.annotation.GetMapping;
import org.springframework.web.bind.annotation.PostMapping;
import org.springframework.web.bind.annotation.RestController;

@RestController
public class ChunkController {

    @PostMapping("/chunk")
    public void storeChunk() {
    }

    @GetMapping("/chunk")
    public void retrieveChunk() {
    }
}
