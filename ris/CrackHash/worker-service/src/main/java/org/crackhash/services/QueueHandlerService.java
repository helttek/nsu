package org.crackhash.services;

import com.rabbitmq.client.Channel;
import lombok.extern.slf4j.Slf4j;
import org.crackhash.config.AppProperties;
import org.crackhash.model.requests.CrackHashManagerRequest;
import org.crackhash.model.requests.CrackHashWorkerResponse;
import org.springframework.amqp.rabbit.annotation.RabbitListener;
import org.springframework.amqp.rabbit.core.RabbitTemplate;
import org.springframework.amqp.support.AmqpHeaders;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.messaging.handler.annotation.Header;
import org.springframework.stereotype.Service;

import java.io.IOException;

@Service
@Slf4j
public class QueueHandlerService {
    @Autowired
    private RabbitTemplate rabbitTemplate;
    private AppProperties appProperties;

    @RabbitListener(queues = "${app.rabbitmq.worker.queue}")
    public void processTask(CrackHashManagerRequest request, Channel channel, @Header(AmqpHeaders.DELIVERY_TAG) long tag) throws IOException {
        try {
            log.info("Worker processing task (extracted task from queue): {}", request.getRequestId());

            // 1. Do the heavy lifting...
            CrackHashWorkerResponse result = new CrackHashWorkerResponse();
            result.setAnswers(null);
            result.setPartNumber(request.getPartNumber());
            result.setRequestId(request.getRequestId());

            // 2. Send the result back to the manager (Path B)
            rabbitTemplate.convertAndSend(
                    appProperties.getRabbitmq().getManager().getExchange(),
                    appProperties.getRabbitmq().getManager().getRoutingKey(),
                    result
            );

            // 3. Acknowledge the task ONLY after successful processing and sending
            channel.basicAck(tag, false);

        } catch (Exception e) {
            // If something goes wrong, reject the message and put it back in the queue for another worker
            channel.basicNack(tag, false, true);
        }
    }
}
