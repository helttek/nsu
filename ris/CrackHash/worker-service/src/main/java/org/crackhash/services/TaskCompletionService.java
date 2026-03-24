package org.crackhash.services;

import com.rabbitmq.client.Channel;
import lombok.RequiredArgsConstructor;
import org.apache.commons.codec.digest.DigestUtils;
import org.crackhash.config.AppProperties;
import org.crackhash.model.requests.CrackHashManagerRequest;
import org.crackhash.model.requests.CrackHashWorkerResponse;
import org.crackhash.services.util.Range;
import org.paukov.combinatorics3.Generator;
import org.springframework.amqp.rabbit.annotation.RabbitListener;
import org.springframework.amqp.rabbit.core.RabbitTemplate;
import org.springframework.amqp.support.AmqpHeaders;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.messaging.handler.annotation.Header;
import org.springframework.stereotype.Service;
import lombok.extern.slf4j.Slf4j;

import java.io.IOException;
import java.util.ArrayList;
import java.util.List;

@RequiredArgsConstructor
@Service
@Slf4j
public class TaskCompletionService {

    @Autowired
    private RabbitTemplate rabbitTemplate;
    @Autowired
    private AppProperties appProperties;
    private boolean status = false;

    @RabbitListener(queues = "${app.rabbitmq.worker.queue}", ackMode = "MANUAL")
    public void complete(CrackHashManagerRequest dto, Channel channel, @Header(AmqpHeaders.DELIVERY_TAG) long tag) {
        try {
            log.info("Worker processing task (extracted task from queue): {}", dto.getRequestId());

            String hash = dto.getHash();
            List<String> alphabet = dto.getAlphabet().getSymbols();
            int maxLength = dto.getMaxLength();

            Range range = Range.calculate(
                    dto.getPartNumber(), dto.getPartCount(), maxLength, alphabet.size());

            log.info("Starting to crack hash \"{}\"", hash);
            List<String> matchingWords = findMatches(alphabet, hash, range);
            log.info("Finished cracking hash.");

            CrackHashWorkerResponse result = new CrackHashWorkerResponse();
            var answ = new CrackHashWorkerResponse.Answers();
            answ.getWords().addAll(matchingWords);
            result.setAnswers(answ);
            result.setPartNumber(dto.getPartNumber());
            result.setRequestId(dto.getRequestId());


//        taskResponseSenderService.send(matchingWords, dto.getRequestId(), dto.getPartNumber());
            rabbitTemplate.convertAndSend(
                    appProperties.getRabbitmq().getManager().getExchange(),
                    appProperties.getRabbitmq().getManager().getRoutingKey(),
                    result
            );

            status = true;
//        log.info("Sent hash crack results to manager.");
        } catch (Exception e) {
            log.info("Failed to process task. Caught amqp exception.");
            log.info(e.getMessage());
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
            log.info(e.getMessage());
        }
    }

    private List<String> findMatches(List<String> alphabet, String hash, Range range) {
        ArrayList<String> matchingWords = new ArrayList<>();

        for (int length = range.getStartingCombinationLength(); length <= range
                .getEndingCombinationLength(); length++) {

            long iterStart, iterEnd;
            long totalAtLength = (long) Math.pow(alphabet.size(), length);

            if (length == range.getStartingCombinationLength() && length == range.getEndingCombinationLength()) {
                iterStart = range.getGeneratorIterationStart();
                iterEnd = range.getGeneratorIterationEnd();
            } else if (length == range.getStartingCombinationLength()) {
                iterStart = range.getGeneratorIterationStart();
                iterEnd = totalAtLength;
            } else if (length == range.getEndingCombinationLength()) {
                iterStart = 0;
                iterEnd = range.getGeneratorIterationEnd();
            } else {
                iterStart = 0;
                iterEnd = totalAtLength;
            }

            long i = 0;
            for (List<String> combination : Generator.permutation(alphabet).withRepetitions(length)) {
                if (i >= iterEnd)
                    break;
                if (i >= iterStart) {
                    String candidate = String.join("", combination);
                    if (DigestUtils.md5Hex(candidate).equals(hash)) {
                        log.info("Found word ({}) for hash ({}).", candidate, hash);
                        matchingWords.add(candidate);
                    }
                }
                i++;
            }
        }

        if (matchingWords.isEmpty()) {
            log.info("No words found with matching hash: {}", hash);
        }
        return matchingWords;
    }
}