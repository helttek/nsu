package org.crackhash.controllers;

import lombok.AllArgsConstructor;
import org.crackhash.service.TaskResultHandlerService;
import org.springframework.http.HttpStatus;
import org.springframework.http.MediaType;
import org.springframework.stereotype.Controller;
import org.springframework.web.bind.annotation.PatchMapping;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.ResponseStatus;
import org.springframework.web.bind.annotation.RestController;

@RestController
@RequestMapping(value = "/internal/api/manager")
@AllArgsConstructor
public class WorkerRequestController {
    private final TaskResultHandlerService taskResultHandlerService;

    @PatchMapping(value = "/hash/crack/request", consumes = MediaType.APPLICATION_XML_VALUE)
    @ResponseStatus(HttpStatus.ACCEPTED)
    public void updateQueue() {
        taskResultHandlerService.handle();
    }
}
