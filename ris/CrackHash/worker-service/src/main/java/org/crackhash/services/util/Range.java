package org.crackhash.services.util;

import lombok.Getter;
import lombok.extern.slf4j.Slf4j;

@Getter
@Slf4j
public class Range {
    private final long start;
    private final long end;
    private final int startingCombinationLength;
    private final int endingCombinationLength;
    private final long generatorIterationStart;
    private final long generatorIterationEnd;

    private Range(long start, long end, int startingCombinationLength,
            int endingCombinationLength, long generatorIterationStart,
            long generatorIterationEnd) {
        this.start = start;
        this.end = end;
        this.startingCombinationLength = startingCombinationLength;
        this.endingCombinationLength = endingCombinationLength;
        this.generatorIterationStart = generatorIterationStart;
        this.generatorIterationEnd = generatorIterationEnd;
    }

    public static Range calculate(int workerNumber, int workerCount, int maxLength, int alphabetSize) {
        long total = 0;
        for (int len = 1; len <= maxLength; len++) {
            total += (long) Math.pow(alphabetSize, len);
        }

        long chunkSize = total / workerCount;
        long start = chunkSize * workerNumber;
        long end = (workerNumber == workerCount - 1) ? total : start + chunkSize;

        int startingLen = findStartingLength(start, maxLength, alphabetSize);
        int endingLen = findEndingLength(end, maxLength, alphabetSize);

        long iterStart = computeIterStart(start, startingLen, alphabetSize);
        long iterEnd = computeIterEnd(end, endingLen, alphabetSize);

        log.info("Worker {}/{}: globalRange=[{}, {}), lengths=[{}, {}], iterStart={}, iterEnd={}",
                workerNumber, workerCount, start, end, startingLen, endingLen, iterStart, iterEnd);

        return new Range(start, end, startingLen, endingLen, iterStart, iterEnd);
    }

    private static int findStartingLength(long start, int maxLength, int alphabetSize) {
        long t = 0;
        for (int len = 1; len <= maxLength; len++) {
            t += (long) Math.pow(alphabetSize, len);
            if (t > start)
                return len;
        }
        return maxLength;
    }

    private static int findEndingLength(long end, int maxLength, int alphabetSize) {
        long t = 0;
        for (int len = 1; len <= maxLength; len++) {
            t += (long) Math.pow(alphabetSize, len);
            if (t >= end)
                return len;
        }
        return maxLength;
    }

    private static long computeIterStart(long start, int startingLen, int alphabetSize) {
        long t = 0;
        for (int i = 1; i < startingLen; i++) {
            t += (long) Math.pow(alphabetSize, i);
        }
        return start - t;
    }

    private static long computeIterEnd(long end, int endingLen, int alphabetSize) {
        long t = 0;
        for (int i = 1; i < endingLen; i++) {
            t += (long) Math.pow(alphabetSize, i);
        }
        return end - t;
    }
}