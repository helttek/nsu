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
        log.info("starting length: {}", range.startingCombinationLength);
        log.info("ending length: {}", range.endingHashLength);
        log.info("start: {}", range.start);
        log.info("end: {}", range.end);
        for (int hashLength = range.startingCombinationLength; hashLength <= range.endingHashLength; hashLength++) {
            log.info("hash length: {}", hashLength);
            Iterable<List<String>> generator = Generator.permutation(alphabet)
                    .withRepetitions(hashLength);

            long i = 0;
            for (List<String> combination : generator) {
                //TODO: introduce 2-element array and checks for starting and ending combination length
                if (range.generatorIterationStart <= i && i <= range.generatorIterationEnd) {
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
                ++i;
            }
        }

        log.info("No words found with a matching hash: {}", hash);
    }

    @Component
    @Getter
    private static class Range {
        /**
         * Number of the first combination the worker has to check.
         *
         */
        private long start;
        /**
         * Number of the last combination the worker has to check.
         *
         */
        private long end;
        /**
         * Number of combinations, the worker has to check.
         *
         */
        private long count;
        /**
         * Needed to determine from which combination length to start.
         *
         */
        private int startingCombinationLength;
        /**
         * Needed to determine on which combination length to end.
         *
         */
        private int endingHashLength;
        /**
         * Starting point for iterations in a generator with a certain hash length.
         * Needed to determine, which combinations the current worker will be checking,
         * if multiple workers happen to be given a generator with the same hash length.
         *
         */
        private long generatorIterationStart;
        /**
         * Ending point for iterations in a generator with a certain hash length.
         * Needed to determine, which combinations the current worker will be checking,
         * if multiple workers happen to be given a generator with the same hash length.
         *
         */
        private long generatorIterationEnd;

        private void calculate(int workerNumber, int workerCount, int hashMaxLength, int alphabetSize) {
            long count = 0;
            for (int hashLength = 1; hashLength <= hashMaxLength; hashLength++) {
                count += (long) Math.pow(alphabetSize, hashLength);
            }
            start = count / workerCount * workerNumber;
            end = Math.min(start + count / workerCount, count);
            this.count = end - start;

            calculateStartingLength(hashMaxLength, alphabetSize);
            calculateEndingLength(hashMaxLength, alphabetSize);
            calculateIterationBounds(alphabetSize);
        }

        private void calculateStartingLength(int hashMaxLength, int alphabetSize) {
            long t = 0;
            if (t >= start) {
                startingCombinationLength = 1;
            } else {
                for (int hashLength = 1; hashLength <= hashMaxLength; hashLength++) {
                    t += (long) Math.pow(alphabetSize, hashLength);
                    log.info("LOLOLOLOLOLOLOLOLOL");
                    log.info("t: {}", t);
                    log.info("start: {}", start);
                    if (t >= start) {
                        startingCombinationLength = hashLength;
                        generatorIterationStart = start - 1;
                        break;
                    }
                }
            }
        }

        private void calculateEndingLength(int hashMaxLength, int alphabetSize) {
            long t = 0;
            endingHashLength = 0;
            for (int hashLength = 1; hashLength <= hashMaxLength; hashLength++) {
                t += (long) Math.pow(alphabetSize, hashLength);
                log.info("HAHHHHHAHAHHAHH");
                log.info("t: {}", t);
                log.info("end: {}", end);
                if (t >= end) {
                    endingHashLength = Math.max(hashLength, 1);
                    break;
                }
            }
        }

        private void calculateIterationBounds(int alphabetSize) {
            long t = 0;
            for (int combinationLength = 1; combinationLength <= startingCombinationLength - 1; combinationLength++) {
                t += (long) Math.pow(alphabetSize, combinationLength);
            }
            generatorIterationStart = Math.max(start - (long) Math.pow(alphabetSize, startingCombinationLength - 1), 0);
            generatorIterationEnd = end - (long) Math.pow(alphabetSize, endingHashLength - 1);
            log.info("Iteration start: {}", generatorIterationStart);
            log.info("Iteration end: {}", generatorIterationEnd);
        }
    }
}