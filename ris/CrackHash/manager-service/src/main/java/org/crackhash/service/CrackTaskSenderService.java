package org.crackhash.service;

import lombok.extern.slf4j.Slf4j;
import org.crackhash.config.AppProperties;
import org.crackhash.model.TasksDocument;
import org.crackhash.model.responses.CrackHashManagerRequest;
import org.springframework.amqp.AmqpException;
import org.springframework.amqp.rabbit.core.RabbitTemplate;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.scheduling.annotation.Scheduled;
import org.springframework.stereotype.Service;

import java.util.List;

@Service
@Slf4j
public class CrackTaskSenderService {
    private final CrackTaskStatusTrackerService crackTaskStatusTrackerService;
    private final AppProperties appProperties;
    @Autowired
    private RabbitTemplate rabbitTemplate;

    @Autowired
    public CrackTaskSenderService(AppProperties appProperties, CrackTaskStatusTrackerService crackTaskStatusTrackerService) {
        this.appProperties = appProperties;
        this.crackTaskStatusTrackerService = crackTaskStatusTrackerService;
    }

    public void send(String requestId, String hash, int hashMaxLength) {
        for (int i = 1; i <= appProperties.getWorker().getCount(); i++) {
            CrackHashManagerRequest request = createRequest(requestId, hash, hashMaxLength, i, appProperties.getWorker().getCount());
            log.info("Sending task {} to queue.", request.getRequestId());
            try {
                rabbitTemplate.convertAndSend(
                        appProperties.getRabbitmq().getWorker().getExchange(),
                        appProperties.getRabbitmq().getWorker().getRoutingKey(),
                        request
                );
            } catch (AmqpException e) {
                crackTaskStatusTrackerService.saveTask(
                        request.getRequestId(),
                        request.getHash(),
                        request.getMaxLength(),
                        request.getPartCount(),
                        request.getPartNumber()
                );
            }
            log.info("Sent task {} to queue.", request.getRequestId());
        }
    }

    private CrackHashManagerRequest createRequest(String requestId, String hash, int hashMaxLength, int partNumber, int partCount) {
        CrackHashManagerRequest request = new CrackHashManagerRequest();

        CrackHashManagerRequest.Alphabet alphabet = new CrackHashManagerRequest.Alphabet();
        alphabet.getSymbols().addAll(appProperties.getAlphabet());
        request.setAlphabet(alphabet);
        request.setRequestId(requestId);
        request.setHash(hash);
        request.setPartNumber(partNumber - 1);
        request.setMaxLength(hashMaxLength);
        request.setPartCount(partCount);

        return request;
    }

    @Scheduled(fixedDelay = 5000)
    public void resendFailedTasks() {
        List<TasksDocument> tasks = crackTaskStatusTrackerService.getPendingTasks();

        log.info("Starting to resend pending tasks.");
        for (var task : tasks) {
            try {
                rabbitTemplate.convertAndSend(
                        appProperties.getRabbitmq().getWorker().getExchange(),
                        appProperties.getRabbitmq().getWorker().getRoutingKey(),
                        createRequest(task.getId(), task.getHash(), task.getHashMaxLength(), task.getPartNumber(), task.getPartCount())
                );

                crackTaskStatusTrackerService.removeTask(task.getId());

            } catch (AmqpException e) {
                log.info("Failed to send task to queue again. Will try later.");
                break;
            }
        }
        log.info("Finished sending pending tasks.");
    }
}
