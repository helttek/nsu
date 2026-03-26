package org.crackhash.service;

import jakarta.annotation.PostConstruct;
import lombok.RequiredArgsConstructor;
import lombok.extern.slf4j.Slf4j;
import org.crackhash.model.CracksDocument;
import org.crackhash.model.TasksDocument;
import org.crackhash.repository.CracksRepository;
import org.crackhash.repository.TasksRepository;
import org.crackhash.util.CrackTaskStatusEnum;
import org.springframework.beans.factory.annotation.Value;
import org.springframework.data.mongodb.core.FindAndModifyOptions;
import org.springframework.data.mongodb.core.MongoTemplate;
import org.springframework.data.mongodb.core.query.Criteria;
import org.springframework.data.mongodb.core.query.Query;
import org.springframework.data.mongodb.core.query.Update;
import org.springframework.scheduling.annotation.Scheduled;
import org.springframework.stereotype.Service;
import org.springframework.transaction.annotation.Transactional;

import java.time.Duration;
import java.time.Instant;
import java.util.ArrayList;
import java.util.List;
import java.util.NoSuchElementException;

import static org.crackhash.util.CrackTaskStatusEnum.*;

@Service
@Slf4j
@RequiredArgsConstructor
public class CrackTaskStatusTrackerService {

    @Value("${task.timeout.duration.minutes}")
    private long taskTimeoutDurationMinutes;
    private Duration timeoutInMinutes;

    private final CracksRepository cracksRepository;
    private final TasksRepository tasksRepository;
    private final MongoTemplate mongoTemplate;

    @PostConstruct
    private void init() {
        this.timeoutInMinutes = Duration.ofMinutes(taskTimeoutDurationMinutes);
    }

    @Value("${app.worker.count}")
    private int workerCount;

    public String createTask() {
        return cracksRepository.save(
                new CracksDocument(
                        IN_PROGRESS,
                        workerCount,
                        Instant.now(),
                        new ArrayList<>()
                )
        ).getId();
    }

    public void onWorkerResponse(String taskId, List<String> matches) {
        Query query = Query.query(
                Criteria.where("_id").is(taskId)
                        .and("status").is(IN_PROGRESS));

        Update update = new Update()
                .inc("pendingWorkers", -1)
                .addToSet("matches").each(matches);

        CracksDocument updated = mongoTemplate.findAndModify(
                query,
                update,
                new FindAndModifyOptions().returnNew(true),
                CracksDocument.class
        );

        if (updated != null && updated.getPendingWorkers() <= 0) {
            mongoTemplate.updateFirst(
                    Query.query(Criteria.where("_id").is(taskId)),
                    Update.update("status", READY),
                    CracksDocument.class
            );
            log.info("Change task {} status to \"READY\".", taskId);
        }
    }

    public CrackTaskStatusEnum getStatus(String guid) throws NoSuchElementException {
        return cracksRepository.findById(guid)
                .map(CracksDocument::getStatus)
                .orElseThrow();
    }

    public List<String> getMatches(String guid) {
        return cracksRepository.findById(guid)
                .map(CracksDocument::getMatches)
                .orElseThrow();
    }

    @Transactional
    @Scheduled(fixedDelay = 60_000)
    public void markTimedOutTasksAsError() {
        mongoTemplate.updateMulti(
                Query.query(
                        Criteria.where("status").is(IN_PROGRESS)
                                .and("createdAt")
                                .lt(Instant.now().minus(timeoutInMinutes))),
                Update.update("status", ERROR),
                CracksDocument.class);
    }

    public void saveTask(String guid, String hash, int hashMaxLength, int partCount, int partNumber) {
        tasksRepository.save(
                new TasksDocument(
                        guid,
                        hash,
                        hashMaxLength,
                        partCount,
                        partNumber
                )
        );
    }

    public void removeTask(String guid) {
        tasksRepository.deleteById(guid);
    }

    public List<TasksDocument> getPendingTasks() {
        return tasksRepository.findTop100By();
    }
}
