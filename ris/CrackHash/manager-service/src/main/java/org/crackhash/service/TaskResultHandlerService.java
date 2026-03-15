package org.crackhash.service;

import lombok.AllArgsConstructor;
import lombok.extern.slf4j.Slf4j;
import org.crackhash.model.responses.CrackHashWorkerResponse;
import org.springframework.stereotype.Service;

import java.util.ArrayList;
import java.util.List;

@Service
@Slf4j
@AllArgsConstructor
public class TaskResultHandlerService {

    private final CrackTaskStatusTrackerService crackTaskStatusTrackerService;

    public void handle(CrackHashWorkerResponse dto) {
        //TODO: add logging across all services
        var requestId = dto.getRequestId();
        var answers = dto.getAnswers().getWords();
        if (answers.isEmpty()) {
            crackTaskStatusTrackerService.markError(requestId);
        } else {
            crackTaskStatusTrackerService.onWorkerResponse(requestId, answers);
        }
    }
}
