package org.crackhash.repository;

import org.crackhash.model.TasksDocument;
import org.springframework.data.mongodb.repository.MongoRepository;
import org.springframework.data.mongodb.repository.Query;

import java.util.List;
import java.util.stream.Stream;

public interface TasksRepository extends MongoRepository<TasksDocument, String> {
    @Query("{}")
    Stream<TasksDocument> streamPendingTasks();

    List<TasksDocument> findTop100By();
}
