package org.example.exceptions;

public class BadNumberOfArguments extends Exception {
    public BadNumberOfArguments(String message) {
        super(message);
    }

    @Override
    public String getMessage() {
        return super.getMessage();
    }
}
