package org.crackhash.services;

import org.crackhash.config.AppProperties;
import org.crackhash.model.requests.CrackHashWorkerResponse;
import org.springframework.amqp.AmqpException;
import org.springframework.amqp.rabbit.core.RabbitTemplate;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;

import java.util.List;

@Service
public class ResultSenderService {
    @Autowired
    private RabbitTemplate rabbitTemplate;
    @Autowired
    private AppProperties appProperties;

    public void send(List<String> matchingWords, int partNumber, String requestId) throws AmqpException {
        CrackHashWorkerResponse result = new CrackHashWorkerResponse();
        var answers = new CrackHashWorkerResponse.Answers();
        answers.getWords().addAll(matchingWords);
        result.setAnswers(answers);
        result.setPartNumber(partNumber);
        result.setRequestId(requestId);

        try {
            rabbitTemplate.convertAndSend(
                    appProperties.getRabbitmq().getManager().getExchange(),
                    appProperties.getRabbitmq().getManager().getRoutingKey(),
                    result
            );
        } catch (AmqpException e) {
            throw new AmqpException(e);
        }
    }
}
