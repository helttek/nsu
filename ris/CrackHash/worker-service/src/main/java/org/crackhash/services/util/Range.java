package org.crackhash.services.util;

import lombok.Getter;
import lombok.extern.slf4j.Slf4j;
import org.springframework.stereotype.Component;

@Component
@Getter
@Slf4j
public class Range {
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

    public void calculate(int workerNumber, int workerCount, int hashMaxLength, int alphabetSize) {
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
        }
        generatorIterationEnd = end - t;
    }
}
