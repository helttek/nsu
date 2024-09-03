package org.example;

import com.sun.jdi.request.DuplicateRequestException;
import org.example.exceptions.FailedToAddElement;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import java.util.EmptyStackException;
import java.util.HashMap;
import java.util.Map;
import java.util.Stack;

public class Context {
    private static final Logger logger = LoggerFactory.getLogger(Context.class);
    private final Map<String, Double> parameters;
    private final Stack<Double> stack;
    private String currentCommand;

    public Context() {
        logger.info("Creating context.");
        this.stack = new Stack<>();
        this.parameters = new HashMap<>();
    }

    public void PushToStack(Double value) {
        logger.info("Pushing '{}' to stack", value);
        this.stack.push(value);
    }

    public Double PopFromStack() {
        try {
            logger.info("Deleting top element from stack.");
            return this.stack.pop();
        } catch (EmptyStackException e) {
            logger.error("Stack is empty. Nothing to delete.");
            throw e;
        }
    }

    public Double PeekTheStack() {
        try {
            logger.info("Peeking the first element of stack.");
            return this.stack.peek();
        } catch (EmptyStackException e) {
            logger.error("Stack is empty. Nothing to peek.");
            throw e;
        }
    }

    public void AddParameter(String key, Double value) throws FailedToAddElement {
        logger.info("Adding element to hash table.");
        if (key == null) {
            logger.error("Key is null.");
            throw new NullPointerException("Bad variable name.");
        }
        if (this.parameters.containsKey(key)) {
            logger.error("Element with key '{}' already exists in the hash map.", key);
            throw new DuplicateRequestException("The variable has already been defined.");
        }
        try {
            this.parameters.put(key, value);
        } catch (UnsupportedOperationException | ClassCastException | IllegalArgumentException e) {
            logger.error("Failed to put element to a hash table.");
            throw new FailedToAddElement(e.getMessage());
        }
    }

    public Double GetParameter(String key) throws FailedToAddElement {
        logger.info("Getting element from hash table.");
        if (key == null) {
            logger.error("Key is null.");
            throw new NullPointerException("Bad variable name.");
        }
        try {
            return this.parameters.get(key);
        } catch (ClassCastException e) {
            logger.error("Improper cast of key.");
            throw new FailedToAddElement(e.getMessage());
        }
    }

    public int GetStackSize() {
        return stack.size();
    }

    public static void LogArgs(String[] words) {
        for (String word : words) {
            logger.debug("'{}'", word);
        }
    }
}
