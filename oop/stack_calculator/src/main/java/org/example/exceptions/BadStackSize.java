package org.example.exceptions;

public class BadStackSize extends Exception {
    public BadStackSize(String message) {
        super(message);
    }

    @Override
    public String getMessage() {
        return super.getMessage();
    }
}
