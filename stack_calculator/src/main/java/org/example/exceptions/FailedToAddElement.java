package org.example.exceptions;

public class FailedToAddElement extends Exception {
    public FailedToAddElement(String message) {
        super(message);
    }

    @Override
    public String getMessage() {
        return super.getMessage();
    }
}
