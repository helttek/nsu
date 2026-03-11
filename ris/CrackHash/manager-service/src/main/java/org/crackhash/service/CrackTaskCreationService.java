package org.crackhash.service;

import lombok.AllArgsConstructor;
import org.crackhash.dto.request.CrackTaskCreationRequest;
import org.crackhash.dto.response.CrackTaskCreationResponse;
import org.springframework.stereotype.Service;

import java.util.UUID;

@Service
@AllArgsConstructor
public class CrackTaskCreationService {
    private final CrackTaskStatusTrackerService crackTaskStatusTrackerService;

    public CrackTaskCreationResponse handleCrackRequest() {
        String requestId = UUID.randomUUID().toString();
        CrackTaskCreationResponse request = new CrackTaskCreationResponse();
        request.setRequestId(requestId);
        return request;
    }
}
