package com.remotebackup.backend.dtos;

import java.util.ArrayList;
import java.util.List;

import lombok.Data;

@Data
public class ValidationErrorResponseDTO {

    private List<Violation> violations = new ArrayList<>();
    
}
