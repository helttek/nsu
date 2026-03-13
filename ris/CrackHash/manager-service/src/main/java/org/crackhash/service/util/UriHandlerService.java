package org.crackhash.service.util;

import lombok.Getter;
import org.springframework.beans.factory.annotation.Value;
import org.springframework.stereotype.Service;

@Service
@Getter
public class UriHandlerService {
    @Value("${app.worker.port}")
    private String workerPort;
    private final String WORKER_TASK_URI = "/internal/api/worker/hash/crack/task";

    public String getBaseUri(int workerNumber) {
        return "http://crackhash-worker-service-" + workerNumber + ":" + workerPort;
    }
}
