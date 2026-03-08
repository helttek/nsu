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

    //TODO: clean up

    @Async
    public void complete(CrackHashManagerRequest dto) {
        String hash = dto.getHash();
        List<String> alphabet = dto.getAlphabet().getSymbols();
        int maxLength = dto.getMaxLength();

        log.info("====================================================");
        range.calculate(dto.getPartNumber(), dto.getPartCount(), maxLength, alphabet.size());

        long rangeCount = range.start;
        log.info("starting length: {}", range.startingCombinationLength);
        log.info("ending length: {}", range.endingCombinationLength);
        log.info("start: {}", range.start);
        log.info("end: {}", range.end);
        for (int combinationLength = range.startingCombinationLength; combinationLength <= range.endingCombinationLength; combinationLength++) {
            log.info("hash length: {}", combinationLength);
            Iterable<List<String>> generator = Generator.permutation(alphabet).withRepetitions(combinationLength);

            long i = 0;
            for (List<String> combination : generator) {
                if (range.startingCombinationLength != range.endingCombinationLength) {
                    if (
                            (range.generatorIterationStart <= i && combinationLength == range.startingCombinationLength)
                                    || (i <= range.generatorIterationEnd && combinationLength == range.endingCombinationLength)
                    ) {
                        log.info("range count: {}", rangeCount);
                        if (rangeCount > range.end) {
                            break;
                        }
                        rangeCount++;

                        String candidate = String.join("", combination);
                        log.info("Processing word: {}", candidate);

                        if (DigestUtils.md5Hex(candidate).equals(hash)) {
                            log.info("Range: {}", rangeCount);
                            log.info("Found word with a matching hash: {}; rangeCount: {}", candidate, rangeCount);
                            return;
                        }
                    }
                } else {
                    if (
                            (range.generatorIterationStart <= i && combinationLength == range.startingCombinationLength)
                                    && (i <= range.generatorIterationEnd && combinationLength == range.endingCombinationLength)
                    ) {
                        log.info("range count: {}", rangeCount);
                        if (rangeCount > range.end) {
                            break;
                        }
                        rangeCount++;

                        String candidate = String.join("", combination);
                        log.info("Processing word: {}", candidate);

                        if (DigestUtils.md5Hex(candidate).equals(hash)) {
                            log.info("Range: {}", rangeCount);
                            log.info("Found word with a matching hash: {}; rangeCount: {}", candidate, rangeCount);
                            return;
                        }
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
        private int endingCombinationLength;
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
                        break;
                    }
                }
            }
        }

        private void calculateEndingLength(int hashMaxLength, int alphabetSize) {
            long t = 0;
            endingCombinationLength = 0;
            for (int hashLength = 1; hashLength <= hashMaxLength; hashLength++) {
                t += (long) Math.pow(alphabetSize, hashLength);
                log.info("HAHHHHHAHAHHAHH");
                log.info("t: {}", t);
                log.info("end: {}", end);
                if (t >= end) {
                    endingCombinationLength = Math.max(hashLength, 1);
                    break;
                }
            }
        }

        private void calculateIterationBounds(int alphabetSize) {
            long t = 0;
            for (int i = 1; i <= startingCombinationLength - 1; i++) {
                t += (long) Math.pow(alphabetSize, i);
            }
            generatorIterationStart = Math.max(start - t, 0);
            t = 0;

            for (int i = 1; i <= endingCombinationLength - 1; i++) {
                t += (long) Math.pow(alphabetSize, i);
                log.info("CHEKING T VALUE: {}", t);
            }
            generatorIterationEnd = end - t;
            log.info("Iteration start: {}", generatorIterationStart);
            log.info("Iteration end: {}", generatorIterationEnd);
        }
    }
}