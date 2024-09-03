package org.example;

import org.example.commands.CommandFactory;
import org.example.exceptions.*;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import java.io.Console;
import java.util.Scanner;

public class Calculator {
    private static final Logger logger = LoggerFactory.getLogger(Calculator.class);
    private final CommandFactory commandFactory;

    public Calculator() throws ResourceFileLoadFailure, BadResourceFile {
        this.commandFactory = new CommandFactory();
    }

    ///home/anton/nsu/oop/lab2/src/main/resources/input.txt

    public void Run(Scanner scanner, Console console) {
        String line;
        //"A scanning operation may block waiting for input." - https://docs.oracle.com/en/java/javase/11/docs/api/java.base/java/util/Scanner.html
//        while (!(line = scanner.nextLine()).isEmpty() && scanner.hasNextLine()) {
//            try {
//                commandFactory.ExecCommand(line);
//            } catch (FailedExtraction | BadCommandFormat | ResourceFileLoadFailure |
//                     FailedCommandExecution e) {
//                logger.error("{} Skipping this line.", e.getMessage());
//            }
//        }
        if (scanner != null && console == null) {
            while (!(line = scanner.nextLine()).isEmpty() && scanner.hasNextLine()) {
                try {
                    commandFactory.ExecCommand(line);
                } catch (FailedExtraction | BadCommandFormat | ResourceFileLoadFailure |
                         FailedCommandExecution e) {
                    logger.error("{} Skipping this line.", e.getMessage());
                }
            }
        }
        if (scanner == null && console != null) {
            while (!(line = console.readLine()).isEmpty()) {
                try {
                    commandFactory.ExecCommand(line);
                } catch (FailedExtraction | BadCommandFormat | ResourceFileLoadFailure |
                         FailedCommandExecution e) {
                    logger.error("{} Skipping this line.", e.getMessage());
                }
            }
        } else {
            throw new RuntimeException("Couldn't open file");
        }
    }
}
