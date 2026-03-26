package org.crackhash.services;


import com.rabbitmq.client.Channel;
import lombok.RequiredArgsConstructor;
import lombok.extern.slf4j.Slf4j;
import org.crackhash.config.AppProperties;
import org.crackhash.model.requests.CrackHashManagerRequest;
import org.crackhash.model.requests.CrackHashWorkerResponse;
import org.crackhash.services.util.Range;
import org.springframework.amqp.AmqpException;
import org.springframework.amqp.rabbit.annotation.RabbitListener;
import org.springframework.amqp.rabbit.core.RabbitTemplate;
import org.springframework.amqp.support.AmqpHeaders;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.messaging.handler.annotation.Header;
import org.springframework.stereotype.Service;

import java.io.IOException;
import java.util.List;

@RequiredArgsConstructor
@Service
@Slf4j
public class QueueListener {

    private final ResultSenderService resultSenderService;
    private final TaskCompletionService taskCompletionService;

    @RabbitListener(queues = "${app.rabbitmq.worker.queue}", ackMode = "MANUAL", concurrency = "${app.rabbitmq.worker.concurrency}")
    public void processTask(CrackHashManagerRequest dto, Channel channel, @Header(AmqpHeaders.DELIVERY_TAG) long tag) {
        boolean status = false;
        try {
            log.info("Worker processing task {}", dto.getRequestId());

            List<String> matchingWords = taskCompletionService.complete(dto);

            resultSenderService.send(matchingWords, dto.getPartNumber(), dto.getRequestId());

            status = true;
        } catch (AmqpException e) {
            log.info("Failed to process task. Caught amqp exception: {}", e.getMessage());
        }

        try {
            if (status) {
                channel.basicAck(tag, false);
                log.info("Task completed successfully, removing the task from the queue.");
            } else {
                channel.basicNack(tag, false, true);
                log.info("Failed to complete task, keeping the task in the queue.");
            }
        } catch (IOException e) {
            log.info("Channel failed: {}", e.getMessage());
        }
    }
}
