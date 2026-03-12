package org.crackhash.model;

import lombok.Getter;
import org.crackhash.util.CrackTaskStatusEnum;
import org.springframework.data.annotation.Id;
import org.springframework.data.mongodb.core.mapping.Document;

import java.time.Instant;

@Document(collection = "#{@environment.getProperty('mongodb.collection.name')}")
@Getter
public class CrackTaskDocument {
    @Id
    private String id;
    private final CrackTaskStatusEnum status;
    private final int pendingWorkers;
    private final Instant createdAt;

    public CrackTaskDocument(CrackTaskStatusEnum status, int countWorkers, Instant createdAt) {
        this.status = status;
        this.pendingWorkers = countWorkers;
        this.createdAt = createdAt;
    }
}
