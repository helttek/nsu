package org.crackhash.service;

import lombok.extern.slf4j.Slf4j;
import org.crackhash.model.responses.CrackHashWorkerResponse;
import org.springframework.amqp.rabbit.annotation.RabbitListener;
import org.springframework.amqp.rabbit.core.RabbitTemplate;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;

@Service
@Slf4j
public class QueueHandlerService {
    @Autowired
    private RabbitTemplate rabbitTemplate;

    @RabbitListener(queues = "${app.rabbitmq.manager.queue}")
    public void receiveResult(CrackHashWorkerResponse result) {
        // Lab requirement: Handle duplicate results gracefully (idempotency)
        log.info("RabbitMQListener: received completed task: {}", result.getRequestId());
        // Update task status in MongoDB to COMPLETED
    }
}
