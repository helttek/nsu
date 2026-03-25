package org.crackhash.repository;

import org.crackhash.model.CracksDocument;
import org.springframework.data.mongodb.repository.MongoRepository;

public interface CracksRepository extends MongoRepository<CracksDocument, String> {

}
