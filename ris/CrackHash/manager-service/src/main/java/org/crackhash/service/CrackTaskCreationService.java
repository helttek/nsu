package org.crackhash.service;

import lombok.AllArgsConstructor;
import lombok.extern.slf4j.Slf4j;
import org.crackhash.dto.request.CrackTaskCreationRequest;
import org.crackhash.dto.response.CrackTaskCreationResponse;
import org.springframework.stereotype.Service;

import java.util.UUID;

@Service
@AllArgsConstructor
@Slf4j
public class CrackTaskCreationService {
    private final CrackTaskStatusTrackerService crackTaskStatusTrackerService;
    private final CrackTaskSenderService crackTaskSenderService;

    public CrackTaskCreationResponse handleCrackRequest(CrackTaskCreationRequest dto) {
        String requestId = crackTaskStatusTrackerService.createTask();
        log.info("Created a task.");
        crackTaskSenderService.send(requestId, dto.getHash(), dto.getMaxLength());
        log.info("Sent a task to worker.");
        CrackTaskCreationResponse request = new CrackTaskCreationResponse();
        request.setRequestId(requestId);
        return request;
    }
}
