package org.crackhash.service;

import com.rabbitmq.client.Channel;
import lombok.RequiredArgsConstructor;
import lombok.extern.slf4j.Slf4j;
import org.crackhash.model.responses.CrackHashWorkerResponse;
import org.springframework.amqp.rabbit.annotation.RabbitListener;
import org.springframework.amqp.support.AmqpHeaders;
import org.springframework.messaging.handler.annotation.Header;
import org.springframework.stereotype.Service;

import java.io.IOException;

@Service
@Slf4j
@RequiredArgsConstructor
public class QueueHandlerService {
    private final CrackTaskStatusTrackerService crackTaskStatusTrackerService;

    @RabbitListener(
            queues = "${app.rabbitmq.manager.queue}",
            ackMode = "MANUAL"
    )
    public void receiveResult(
            CrackHashWorkerResponse result,
            Channel channel,
            @Header(AmqpHeaders.DELIVERY_TAG) long tag) {

        log.info("Received completed task: {}", result.getRequestId());

        boolean success = false;
        try {
            if (result.getAnswers() == null) {
                crackTaskStatusTrackerService.onWorkerResponse(
                        result.getRequestId(),
                        new CrackHashWorkerResponse.Answers().getWords()
                );
            } else {
                crackTaskStatusTrackerService.onWorkerResponse(
                        result.getRequestId(),
                        result.getAnswers().getWords()
                );
            }
            success = true;

        } catch (Exception e) {
            log.error("Failed to process task {}", result.getRequestId(), e);
        }

        try {
            if (success) {
                channel.basicAck(tag, false);
            } else {
                // decide if transient or permanent
                // here we use false to prevent infinite redelivery
                channel.basicNack(tag, false, true);
            }
        } catch (IOException e) {
            log.error("Ack/Nack failed for task {}", result.getRequestId(), e);
        }
    }
}
