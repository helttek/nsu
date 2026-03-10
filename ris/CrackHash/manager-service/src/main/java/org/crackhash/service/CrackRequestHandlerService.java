package org.crackhash.service;

import org.crackhash.dto.response.CrackResponse;
import org.springframework.stereotype.Service;

@Service
public class CrackRequestHandlerService {
    public CrackResponse handleCrackRequest() {
        return new CrackResponse();
    }
}
