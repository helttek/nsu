package org.crackhash.service;

import lombok.AllArgsConstructor;
import org.crackhash.dto.response.CrackTaskStatusUpdateResponse;
import org.crackhash.util.CrackTaskStatusEnum;
import org.springframework.stereotype.Service;

import java.util.List;

@Service
@AllArgsConstructor
public class CrackTaskStatusUpdateService {
    private final CrackTaskStatusTrackerService crackTaskStatusTrackerService;

    public CrackTaskStatusUpdateResponse handleCrackStatusUpdate(String requestId) {
        CrackTaskStatusEnum status = crackTaskStatusTrackerService.getStatus(requestId);
        List<String> matches = crackTaskStatusTrackerService.getMatches(requestId);

        CrackTaskStatusUpdateResponse response = new CrackTaskStatusUpdateResponse();
        response.setStatus(status);
        response.setData(matches);

        return response;
    }
}
