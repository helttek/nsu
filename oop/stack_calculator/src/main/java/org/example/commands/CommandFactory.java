package org.example.commands;

import org.example.Context;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.example.exceptions.*;

import java.io.IOException;
import java.io.InputStream;
import java.lang.reflect.InvocationTargetException;
import java.util.Properties;
import java.io.FileWriter;

public class CommandFactory {
    private void ClearLogFile() {
        try {
            FileWriter fileWriter = new FileWriter("/home/anton/nsu/oop/lab2/src/main/resources/runtime.log");
            fileWriter.write(""); // Clearing the content by writing an empty string
            fileWriter.close();
            logger.info("Log file cleared successfully.");
        } catch (IOException e) {
            System.err.println("Error occurred while clearing the log file: " + e.getMessage() + " Logs wil be written anyway.");
        }
    }

    public CommandFactory() throws BadResourceFile, ResourceFileLoadFailure {
        ClearLogFile();
        logger.info("Creating command factory.");
        InputStream stream;
        if ((stream = CommandFactory.class.getResourceAsStream("/Command.properties")) == null) {
            logger.error("Failed to locate the properties file or access was not granted.");
            throw new BadResourceFile("Failed to find 'Command.properties' file (or access restricted).");
        }
        context = new Context();
        properties = new Properties();
        try {
            properties.load(stream);
        } catch (IOException | IllegalArgumentException e) {
            throw new ResourceFileLoadFailure(e.getMessage());
        }
    }

    private static final Logger logger = LoggerFactory.getLogger(CommandFactory.class);
    private final Properties properties;
    private final Context context;

    public void ExecCommand(String line) throws FailedExtraction, BadCommandFormat, ResourceFileLoadFailure, FailedCommandExecution {
        String[] words = ExtractWords(line);
        String[] args = GetArgs(words);
        String classPath = properties.getProperty(words[0]);
        if (classPath == null) {
            logger.debug("Command that the resource was not found for: '{}'", words[0]);
            throw new ResourceFileLoadFailure("Couldn't find resource for the command.");
        }
        try {
            Class<?> clazz = Class.forName(classPath);
            Command inst = (Command) clazz.getDeclaredConstructor().newInstance();
            inst.doOperation(context, args);
        } catch (ElementNotFound | BadArgument | BadNumberOfArguments | FailedToAddElement | BadStackSize |
                 LinkageError | ClassNotFoundException | InstantiationException |
                 IllegalAccessException | InvocationTargetException |
                 NoSuchMethodException e) {
            logger.debug("Failed functions: forName or getDeclaredConstructor or newInstance.");
            throw new FailedCommandExecution(e.getMessage());
        }
    }

    private String[] GetArgs(String[] words) {
        String[] args = new String[words.length - 1];
        for (int i = 1; i < words.length; i++) {
            args[i - 1] = words[i];
        }
        return args;
    }

    private static void LogWords(String[] words) {
        for (String word : words) {
            logger.debug("'{}'", word);
        }
    }

    private static String[] ExtractWords(String line) throws FailedExtraction {
        logger.debug("Extracting the words.");
        String[] res;
        if (line.charAt(0) == '#') {
            logger.debug("The line '{}' was commented out.", line);
            return new String[]{"COMMENT"};
        }
        res = line.split(" ");
        logger.debug("The words are:");
        LogWords(res);
        return res;
    }
}
