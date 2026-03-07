package org.crackhash.services;

import lombok.AllArgsConstructor;
import lombok.Getter;
import org.apache.commons.codec.digest.DigestUtils;
import org.crackhash.model.requests.CrackHashManagerRequest;
import org.paukov.combinatorics3.Generator;
import org.springframework.scheduling.annotation.Async;
import org.springframework.stereotype.Component;
import org.springframework.stereotype.Service;
import org.springframework.web.client.RestClient;
import lombok.extern.slf4j.Slf4j;

import java.util.List;

@AllArgsConstructor
@Service
@Slf4j
public class TaskCompletionService {
    private final RestClient restClient;
    private final Range range;

    @Async
    public void complete(CrackHashManagerRequest dto) {
        String hash = dto.getHash();
        List<String> alphabet = dto.getAlphabet().getSymbols();
        int maxLength = dto.getMaxLength();

        log.info("====================================================");
        range.calculate(dto.getPartNumber(), dto.getPartCount(), maxLength, alphabet.size());

        long rangeCount = range.start;
        log.info("starting length: {}", range.startingLength);
        log.info("ending length: {}", range.endingLength);
        log.info("start: {}", range.start);
        log.info("end: {}", range.end);
        for (int hashLength = range.startingLength; hashLength <= range.endingLength; hashLength++) {
            log.info("hash length: {}", hashLength);
            Iterable<List<String>> generator = Generator.permutation(alphabet)
                    .withRepetitions(hashLength);

            //TODO: fix, when different workers share a generator with the same hashLength
            for (List<String> combination : generator) {
//                log.info("range count: {}", rangeCount);
                if (rangeCount > range.end) {
                    break;
                }
                rangeCount++;

                String candidate = String.join("", combination);
//                log.info("Processing word: {}", candidate);

                if (DigestUtils.md5Hex(candidate).equals(hash)) {
                    log.info("Range: {}", rangeCount);
                    log.info("Found word with a matching hash: {}", candidate);
                    return;
                }
            }
        }

        log.info("No words found with a matching hash: {}", hash);
    }

    @Component
    @Getter
    private static class Range {
        private long start;
        private long end;
        private long count;
        private int startingLength;
        private int endingLength;
        private long iterationStart;
        private long iterationEnd;

        private void calculate(int workerNumber, int workerCount, int hashMaxLength, int alphabetSize) {
            long count = 0;
            for (int hashLength = 1; hashLength <= hashMaxLength; hashLength++) {
                count += (long) Math.pow(alphabetSize, hashLength);
            }
            start = count / workerCount * workerNumber;
            end = Math.min(start + count / workerCount, count);
            this.count = end - start;

            long t = 0;
            if (t >= start) {
                startingLength = 1;
            } else {
                for (int hashLength = 1; hashLength <= hashMaxLength; hashLength++) {
                    t += (long) Math.pow(alphabetSize, hashLength);
                    log.info("LOLOLOLOLOLOLOLOLOL");
                    log.info("t: {}", t);
                    log.info("start: {}", start);
                    if (t >= start) {
                        startingLength = hashLength;
                        iterationStart = start - 1;
                        break;
                    }
                }
            }
            t = 0;
            endingLength = 0;
            for (int hashLength = 1; hashLength <= hashMaxLength; hashLength++) {
                t += (long) Math.pow(alphabetSize, hashLength);
                log.info("HAHHHHHAHAHHAHH");
                log.info("t: {}", t);
                log.info("end: {}", end);
                if (t >= end) {
                    endingLength = Math.max(hashLength, 1);
                    break;
                }
            }
        }

        private end calculateIterationBounds() {

        }
    }
}