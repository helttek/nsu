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
    private final RestClient restClient;
    private final Range range;
    private final TaskResponseSenderService taskResponseSenderService;

    @Async
    public void complete(CrackHashManagerRequest dto) {
        String hash = dto.getHash();
        List<String> alphabet = dto.getAlphabet().getSymbols();
        int maxLength = dto.getMaxLength();
        range.calculate(dto.getPartNumber(), dto.getPartCount(), maxLength, alphabet.size());

        List<String> matchingWords = findMatches(alphabet, hash);

        taskResponseSenderService.send(matchingWords);
    }

    private List<String> findMatches(List<String> alphabet, String hash) {
        ArrayList<String> matchingWords = new ArrayList<>();

        long rangeCount = range.getStart();
        for (int combinationLength = range.getStartingCombinationLength(); combinationLength <= range.getEndingCombinationLength(); combinationLength++) {
            Iterable<List<String>> generator = Generator.permutation(alphabet).withRepetitions(combinationLength);

            long i = 0;
            for (List<String> combination : generator) {
                if (range.getStartingCombinationLength() != range.getEndingCombinationLength()) {
                    if ((range.getGeneratorIterationStart() <= i && combinationLength == range.getStartingCombinationLength())
                            || (i <= range.getGeneratorIterationEnd() && combinationLength == range.getEndingCombinationLength())) {
                        if (rangeCount > range.getEnd()) {
                            break;
                        }
                        rangeCount++;

                        String candidate = String.join("", combination);

                        if (DigestUtils.md5Hex(candidate).equals(hash)) {
                            log.info("Found word ({}) for hash ({}).", candidate, hash);
                            matchingWords.add(candidate);
                        }
                    }
                } else {
                    if (range.getGeneratorIterationStart() <= i && i <= range.getGeneratorIterationEnd()
                            && combinationLength == range.getStartingCombinationLength()) {
                        if (rangeCount > range.getEnd()) {
                            break;
                        }
                        rangeCount++;

                        String candidate = String.join("", combination);

                        if (DigestUtils.md5Hex(candidate).equals(hash)) {
                            log.info("Found word ({}) for hash ({}).", candidate, hash);
                            matchingWords.add(candidate);
                        }
                    }
                }
                ++i;
            }
        }

        if (matchingWords.isEmpty()) {
            log.info("No words found with a matching hash: {}", hash);
        }
        return matchingWords;
    }
}