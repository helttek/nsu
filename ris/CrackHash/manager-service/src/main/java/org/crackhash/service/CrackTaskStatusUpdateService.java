package org.crackhash.service;

import org.crackhash.dto.response.CrackTaskStatusUpdateResponse;
import org.springframework.stereotype.Service;

@Service
public class CrackTaskStatusUpdateService {
    public CrackTaskStatusUpdateResponse handleCrackStatusUpdate() {
        return new CrackTaskStatusUpdateResponse();
    }
}
