package org.crackhash.controllers;

import lombok.AllArgsConstructor;
import org.crackhash.model.requests.CrackHashManagerRequest;
import org.crackhash.services.TaskCompletionService;
import org.springframework.http.HttpStatus;
import org.springframework.http.MediaType;
import org.springframework.web.bind.annotation.*;

@AllArgsConstructor
@RestController
@RequestMapping(value = "/internal/api/worker")
public class ManagerRequestController {

    private final TaskCompletionService taskCompletionService;

    @PostMapping(value = "/hash/crack/task", consumes = MediaType.APPLICATION_JSON_VALUE)
    @ResponseStatus(HttpStatus.CREATED)
    public void completeTask(@RequestBody CrackHashManagerRequest dto) {
        taskCompletionService.complete(dto);
    }
}
