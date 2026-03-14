package org.crackhash.service;

import lombok.extern.slf4j.Slf4j;
import org.crackhash.model.responses.CrackHashWorkerResponse;
import org.springframework.stereotype.Service;

@Service
@Slf4j
public class TaskResultHandlerService {
    public void handle(CrackHashWorkerResponse dto) {
        //- check if all parts have been finished
        //- if all parts are completed, set the task status to "READY" + update a list of matches
    }
}
