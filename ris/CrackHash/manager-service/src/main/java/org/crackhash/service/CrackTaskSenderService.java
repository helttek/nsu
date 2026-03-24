package org.crackhash.service;

import lombok.extern.slf4j.Slf4j;
import org.crackhash.config.AppProperties;
import org.crackhash.model.responses.CrackHashManagerRequest;
import org.springframework.amqp.rabbit.core.RabbitTemplate;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;

@Service
@Slf4j
public class CrackTaskSenderService {
    private final AppProperties appProperties;
    @Autowired
    private RabbitTemplate rabbitTemplate;

    @Autowired
    public CrackTaskSenderService(AppProperties appProperties) {
        this.appProperties = appProperties;
    }

    public void send(String requestId, String hash, int hashMaxLength) {
        for (int i = 1; i <= appProperties.getWorker().getCount(); i++) {
            CrackHashManagerRequest request = new CrackHashManagerRequest();

            CrackHashManagerRequest.Alphabet alphabet = new CrackHashManagerRequest.Alphabet();
            alphabet.getSymbols().addAll(appProperties.getAlphabet());
            request.setAlphabet(alphabet);
            request.setRequestId(requestId);
            request.setHash(hash);
            request.setPartNumber(i - 1);
            request.setMaxLength(hashMaxLength);
            request.setPartCount(appProperties.getWorker().getCount());

            log.info("sending task to queue");
            rabbitTemplate.convertAndSend(
                    appProperties.getRabbitmq().getWorker().getExchange(),
                    appProperties.getRabbitmq().getWorker().getRoutingKey(),
                    request
            );
            log.info("sent task to queue");
        }
    }


}
