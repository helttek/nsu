package org.crackhash.services;

import lombok.AllArgsConstructor;
import org.crackhash.model.requests.CrackHashManagerRequest;
import org.springframework.stereotype.Service;
import org.springframework.web.client.RestClient;

@AllArgsConstructor
@Service
public class TaskCompletionService {
    private final RestClient restClient;

    public void complete(CrackHashManagerRequest dto) {

    }

}