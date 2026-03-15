package org.crackhash.service;

import lombok.AllArgsConstructor;
import lombok.extern.slf4j.Slf4j;
import org.crackhash.dto.response.CrackTaskStatusUpdateResponse;
import org.crackhash.util.CrackTaskStatusEnum;
import org.springframework.stereotype.Service;

import java.util.List;

@Service
@AllArgsConstructor
@Slf4j
public class CrackTaskStatusUpdateService {
    private final CrackTaskStatusTrackerService crackTaskStatusTrackerService;

    public CrackTaskStatusUpdateResponse handleCrackStatusUpdate(String requestId) {
        log.info("Received a request from client to provide status for task \"{}\"", requestId);

        CrackTaskStatusEnum status = crackTaskStatusTrackerService.getStatus(requestId);
        List<String> matches = crackTaskStatusTrackerService.getMatches(requestId);

        CrackTaskStatusUpdateResponse response = new CrackTaskStatusUpdateResponse();
        response.setStatus(status);
        response.setData(matches);

        log.info("Responded task status to client.");
        return response;
    }
}
