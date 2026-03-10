package org.crackhash.service;

import org.crackhash.dto.response.CrackStatusUpdateResponse;
import org.springframework.stereotype.Service;

@Service
public class CrackStatusUpdateService {
    public CrackStatusUpdateResponse handleCrackStatusUpdate() {
        return new CrackStatusUpdateResponse();
    }
}
