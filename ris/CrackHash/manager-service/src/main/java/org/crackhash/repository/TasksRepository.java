package org.crackhash.repository;

import org.crackhash.model.TasksDocument;
import org.springframework.data.mongodb.repository.MongoRepository;

import java.util.List;

public interface TasksRepository extends MongoRepository<TasksDocument, String> {

    List<TasksDocument> findTop100By();
}
