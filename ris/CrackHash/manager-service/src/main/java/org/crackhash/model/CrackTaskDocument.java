package org.crackhash.model;

import lombok.Getter;
import org.crackhash.util.CrackTaskStatusEnum;
import org.springframework.data.annotation.Id;
import org.springframework.data.mongodb.core.mapping.Document;

import java.time.Instant;
import java.util.List;

@Document(collection = "#{@environment.getProperty('mongodb.collection.name')}")
@Getter
public class CrackTaskDocument {
    @Id
    private String id;
    private final CrackTaskStatusEnum status;
    private final int pendingWorkers;
    private final Instant createdAt;
    private final List<String> matches;

    public CrackTaskDocument(CrackTaskStatusEnum status, int pendingWorkers, Instant createdAt, List<String> matches) {
        this.status = status;
        this.pendingWorkers = pendingWorkers;
        this.createdAt = createdAt;
        this.matches = matches;
    }
}
