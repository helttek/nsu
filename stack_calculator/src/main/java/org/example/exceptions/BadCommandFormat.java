package org.example.exceptions;

public class BadCommandFormat extends Exception {
    public BadCommandFormat(String message) {
        super(message);
    }

    @Override
    public String getMessage() {
        return super.getMessage();
    }
}
