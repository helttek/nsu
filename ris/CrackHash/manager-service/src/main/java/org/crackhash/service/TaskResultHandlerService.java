package org.crackhash.service;

import lombok.extern.slf4j.Slf4j;
import org.springframework.stereotype.Service;

@Service
@Slf4j
public class TaskResultHandlerService {
    public void handle() {
        log.info("Ready to handle the task result.");
    }
}
