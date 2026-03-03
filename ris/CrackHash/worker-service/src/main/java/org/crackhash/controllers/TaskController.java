package org.crackhash.controllers;

import lombok.AllArgsConstructor;
import org.crackhash.model.requests.CrackHashManagerRequest;
import org.crackhash.services.TaskCompletionService;
import org.springframework.web.bind.annotation.PostMapping;
import org.springframework.web.bind.annotation.RequestBody;
import org.springframework.web.bind.annotation.RestController;

@AllArgsConstructor
@RestController
public class TaskController {

    private final TaskCompletionService taskCompletionService;

    @PostMapping("/internal/api/worker/hash/crack/task")
    public void getTask(@RequestBody CrackHashManagerRequest dto) {
        taskCompletionService.complete(dto);
    }
}
