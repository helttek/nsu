package org.crackhash.controllers;

import org.springframework.web.bind.annotation.PostMapping;
import org.springframework.web.bind.annotation.RestController;

@RestController
public class TaskController {

    @PostMapping("/internal/api/worker/hash/crack/task")
    public String getTask() {
        return "Task";
    }
}
