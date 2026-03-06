package org.crackhash.services;

import lombok.AllArgsConstructor;
import org.apache.commons.codec.digest.DigestUtils;
import org.crackhash.model.requests.CrackHashManagerRequest;
import org.paukov.combinatorics3.Generator;
import org.springframework.scheduling.annotation.Async;
import org.springframework.stereotype.Service;
import org.springframework.web.client.RestClient;
import lombok.extern.slf4j.Slf4j;

import java.util.List;

@AllArgsConstructor
@Service
@Slf4j
public class TaskCompletionService {
    private final RestClient restClient;

    @Async
    public void complete(CrackHashManagerRequest dto) {
        String hash = dto.getHash();
        List<String> alphabet = dto.getAlphabet().getSymbols();
        int maxLength = dto.getMaxLength();

        log.info("Starting task completion process");
        for (int length = 1; length <= maxLength; length++) {
            log.info("length: {}", length);
            Iterable<List<String>> generator = Generator.permutation(alphabet)
                    .withRepetitions(length);

            log.info("generator iterator has next: {}", generator.iterator().hasNext());
            for (List<String> combination : generator) {
                log.info("Combination: {}", combination);
                String candidate = String.join("", combination);
                log.info("Processing word: {}", candidate);

                if (DigestUtils.md5Hex(candidate).equals(hash)) {
                    log.info("Found word with a matching hash: {}", candidate);
                    return;
                }

            }
        }

        log.info("No words found with a matching hash: {}", hash);
    }
}