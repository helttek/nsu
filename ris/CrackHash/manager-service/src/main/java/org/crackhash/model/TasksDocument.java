package org.crackhash.model;

import lombok.AllArgsConstructor;
import lombok.Getter;
import lombok.Setter;
import org.springframework.data.annotation.Id;
import org.springframework.data.mongodb.core.mapping.Document;

@Document(collection = "${MONGO_INITDB_TASKS_COLLECTION}")
@Getter
@Setter
@AllArgsConstructor
public class TasksDocument {
    @Id
    private String id;
    private String hash;
    private int hashMaxLength;
    private int partCount;
    private int partNumber;
}
