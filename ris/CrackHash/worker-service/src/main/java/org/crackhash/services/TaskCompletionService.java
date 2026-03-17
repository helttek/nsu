package org.crackhash.services;

import lombok.AllArgsConstructor;
import org.apache.commons.codec.digest.DigestUtils;
import org.crackhash.model.requests.CrackHashManagerRequest;
import org.crackhash.services.util.Range;
import org.paukov.combinatorics3.Generator;
import org.springframework.scheduling.annotation.Async;
import org.springframework.stereotype.Service;
import org.springframework.web.client.RestClient;
import lombok.extern.slf4j.Slf4j;

import java.util.ArrayList;
import java.util.List;

@AllArgsConstructor
@Service
@Slf4j
public class TaskCompletionService {

    private final TaskResponseSenderService taskResponseSenderService;

    @Async
    public void complete(CrackHashManagerRequest dto) {
        String hash = dto.getHash();
        List<String> alphabet = dto.getAlphabet().getSymbols();
        int maxLength = dto.getMaxLength();

        Range range = Range.calculate(
                dto.getPartNumber(), dto.getPartCount(), maxLength, alphabet.size());

        log.info("Starting to crack hash \"{}\"", hash);
        List<String> matchingWords = findMatches(alphabet, hash, range);
        log.info("Finished cracking hash.");

        taskResponseSenderService.send(matchingWords, dto.getRequestId(), dto.getPartNumber());
        log.info("Sent hash crack results to manager.");
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