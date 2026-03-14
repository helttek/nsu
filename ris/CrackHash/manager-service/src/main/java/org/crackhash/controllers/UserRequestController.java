package org.crackhash.controllers;

import lombok.AllArgsConstructor;
import org.crackhash.dto.request.CrackTaskCreationRequest;
import org.crackhash.dto.response.CrackTaskCreationResponse;
import org.crackhash.dto.response.CrackTaskStatusUpdateResponse;
import org.crackhash.service.CrackTaskCreationService;
import org.crackhash.service.CrackTaskStatusUpdateService;
import org.springframework.http.MediaType;
import org.springframework.web.bind.annotation.*;

@RestController
@RequestMapping(value = "/api/hash")
@AllArgsConstructor
public class UserRequestController {
    private final CrackTaskCreationService crackTaskCreationService;
    private final CrackTaskStatusUpdateService crackTaskStatusUpdateService;

    @PostMapping(value = "/crack", consumes = MediaType.APPLICATION_JSON_VALUE)
    public CrackTaskCreationResponse handleCrackRequest(@RequestBody CrackTaskCreationRequest dto) {
        return crackTaskCreationService.handleCrackRequest(dto);
    }

    @GetMapping(value = "/status", consumes = MediaType.APPLICATION_JSON_VALUE)
    public CrackTaskStatusUpdateResponse handleCrackStatusUpdate(@RequestParam String requestId) {
        return crackTaskStatusUpdateService.handleCrackStatusUpdate(requestId);
    }
}
