package org.crackhash;

import org.springframework.boot.SpringApplication;
import org.springframework.boot.autoconfigure.SpringBootApplication;
import org.springframework.scheduling.annotation.Async;
import org.springframework.scheduling.annotation.EnableAsync;
import org.springframework.transaction.annotation.EnableTransactionManagement;

//TODO: add error handling

@SpringBootApplication
@EnableAsync
public class Manager {
    static void main(String[] args) {
        SpringApplication.run(Manager.class, args);
    }
}