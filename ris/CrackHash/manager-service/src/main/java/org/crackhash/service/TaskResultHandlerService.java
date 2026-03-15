package org.crackhash.service;

import lombok.AllArgsConstructor;
import lombok.extern.slf4j.Slf4j;
import org.crackhash.model.responses.CrackHashWorkerResponse;
import org.crackhash.util.CrackTaskStatusEnum;
import org.springframework.stereotype.Service;

import java.util.ArrayList;
import java.util.List;

@Service
@Slf4j
@AllArgsConstructor
public class TaskResultHandlerService {

    private final CrackTaskStatusTrackerService crackTaskStatusTrackerService;

    public void handle(CrackHashWorkerResponse dto) {
        log.info("Received task result from one of the workers.");

        var requestId = dto.getRequestId();
        var answers = dto.getAnswers().getWords();

        crackTaskStatusTrackerService.onWorkerResponse(requestId, answers);
        if (crackTaskStatusTrackerService.getStatus(requestId).equals(CrackTaskStatusEnum.READY)) {
            log.info("Changed task ({}) status to \"{}\"", requestId, CrackTaskStatusEnum.READY);
        }
    }
}
