package org.crackhash.service;

import jakarta.annotation.PostConstruct;
import lombok.AllArgsConstructor;
import org.crackhash.model.CrackTaskDocument;
import org.crackhash.repository.CrackTasksRepository;
import org.crackhash.util.CrackTaskStatusEnum;
import org.springframework.beans.factory.annotation.Value;
import org.springframework.data.mongodb.core.FindAndModifyOptions;
import org.springframework.data.mongodb.core.MongoTemplate;
import org.springframework.data.mongodb.core.query.Criteria;
import org.springframework.data.mongodb.core.query.Query;
import org.springframework.data.mongodb.core.query.Update;
import org.springframework.scheduling.annotation.Scheduled;
import org.springframework.stereotype.Service;

import java.time.Duration;
import java.time.Instant;
import java.util.NoSuchElementException;
import java.util.concurrent.ConcurrentHashMap;

import static org.crackhash.util.CrackTaskStatusEnum.*;

@Service
public class CrackTaskStatusTrackerService {

    @Value("${task.timeout.duration.minutes}")
    private long TASK_TIMEOUT_DURATION_MINUTES;
    private Duration TIMEOUT = Duration.ofHours(TASK_TIMEOUT_DURATION_MINUTES);

    private final CrackTasksRepository repository;
    private final MongoTemplate mongoTemplate;

    public CrackTaskStatusTrackerService(CrackTasksRepository repository, MongoTemplate mongoTemplate) {
        this.repository = repository;
        this.mongoTemplate = mongoTemplate;
    }

    @PostConstruct
    private void init() {
        this.TIMEOUT = Duration.ofMinutes(TASK_TIMEOUT_DURATION_MINUTES);
    }

    //TODO: how to count workers
    private final int workerCount = 1;

    public String createTask() {
        return repository.save(new CrackTaskDocument(
                IN_PROGRESS,
                workerCount,
                Instant.now().plus(TIMEOUT)
        )).getId();
    }

    public void onWorkerResponse(String taskId) {
        Query query = Query.query(
                Criteria.where("_id").is(taskId)
                        .and("status").is(IN_PROGRESS)
        );

        CrackTaskDocument updated = mongoTemplate.findAndModify(
                query,
                new Update().inc("pendingWorkers", -1),
                new FindAndModifyOptions().returnNew(true),
                CrackTaskDocument.class
        );

        if (updated != null && updated.getPendingWorkers() <= 0) {
            mongoTemplate.updateFirst(
                    Query.query(Criteria.where("_id").is(taskId)),
                    Update.update("status", READY),
                    CrackTaskDocument.class
            );
        }
    }

    public CrackTaskStatusEnum getStatus(String guid) throws NoSuchElementException {
        return repository.findById(guid)
                .map(CrackTaskDocument::getStatus)
                .orElseThrow();
    }

    public void markError(String guid) {
        mongoTemplate.updateFirst(
                Query.query(Criteria.where("_id").is(guid).and("status").is(IN_PROGRESS)),
                Update.update("status", ERROR),
                CrackTaskDocument.class
        );
    }

    @Scheduled(fixedDelay = 60_000)
    public void markTimedOutTasksAsError() {
        mongoTemplate.updateMulti(
                Query.query(
                        Criteria.where("status").is(IN_PROGRESS)
                                .and("createdAt").lt(Instant.now().minus(TIMEOUT))
                ),
                Update.update("status", ERROR),
                CrackTaskDocument.class
        );
    }

}
