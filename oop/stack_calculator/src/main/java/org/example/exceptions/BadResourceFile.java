package org.example.exceptions;

public class BadResourceFile extends Exception {
    public BadResourceFile(String message) {
        super(message);
    }

    @Override
    public String getMessage() {
        return super.getMessage();
    }
}
