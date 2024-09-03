package org.example.exceptions;

public class BadArgument extends Exception {
    public BadArgument(String message) {
        super(message);
    }

    @Override
    public String getMessage() {
        return super.getMessage();
    }
}
