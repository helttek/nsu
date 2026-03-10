package org.crackhash.service;

import org.crackhash.dto.response.CrackTaskCreationResponse;
import org.springframework.stereotype.Service;

@Service
public class CrackTaskCreationService {
    public CrackTaskCreationResponse handleCrackRequest() {
        return new CrackTaskCreationResponse();
    }
}
