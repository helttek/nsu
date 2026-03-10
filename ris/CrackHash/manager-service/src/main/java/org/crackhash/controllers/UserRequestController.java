package org.crackhash.controllers;

import lombok.AllArgsConstructor;
import org.crackhash.dto.request.CrackRequest;
import org.crackhash.dto.response.CrackResponse;
import org.crackhash.dto.response.CrackStatusUpdateResponse;
import org.crackhash.service.CrackRequestHandlerService;
import org.crackhash.service.CrackStatusUpdateService;
import org.springframework.http.MediaType;
import org.springframework.stereotype.Controller;
import org.springframework.web.bind.annotation.*;

@RestController
@RequestMapping(value = "/api/hash")
@AllArgsConstructor
public class UserRequestController {
    private final CrackRequestHandlerService crackRequestHandlerService;
    private final CrackStatusUpdateService crackStatusUpdateService;

    @PostMapping(value = "/crack", consumes = MediaType.APPLICATION_JSON_VALUE)
    public CrackResponse handleCrackRequest(CrackRequest dto) {
        return crackRequestHandlerService.handleCrackRequest();
    }

    @GetMapping(value = "/status", consumes = MediaType.APPLICATION_JSON_VALUE)
    public CrackStatusUpdateResponse handleCrackStatusUpdate(@RequestParam String requestId) {
        return crackStatusUpdateService.handleCrackStatusUpdate();
    }
}
