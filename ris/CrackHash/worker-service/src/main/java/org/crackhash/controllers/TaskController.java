package org.crackhash.controllers;

import lombok.AllArgsConstructor;
import org.crackhash.model.requests.CrackHashManagerRequest;
import org.crackhash.services.TaskCompletionService;
import org.springframework.http.HttpStatus;
import org.springframework.http.MediaType;
import org.springframework.web.bind.annotation.PostMapping;
import org.springframework.web.bind.annotation.RequestBody;
import org.springframework.web.bind.annotation.ResponseStatus;
import org.springframework.web.bind.annotation.RestController;

@AllArgsConstructor
@RestController
public class TaskController {

    private final TaskCompletionService taskCompletionService;

    @PostMapping(value = "/internal/api/worker/hash/crack/task", consumes = MediaType.APPLICATION_JSON_VALUE)
    @ResponseStatus(HttpStatus.CREATED)
    public void getTask(@RequestBody CrackHashManagerRequest dto) {
        taskCompletionService.complete(dto);
    }
}
