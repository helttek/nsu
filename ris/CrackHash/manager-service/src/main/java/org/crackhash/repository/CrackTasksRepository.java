package org.crackhash.repository;

import org.crackhash.model.CrackTaskDocument;
import org.springframework.data.mongodb.repository.MongoRepository;

public interface CrackTasksRepository extends MongoRepository<CrackTaskDocument, String> {

}
