package org.example.exceptions;

public class FailedExtraction extends Exception {
    public FailedExtraction(String message) {
        super(message);
    }

    @Override
    public String getMessage() {
        return super.getMessage();
    }
}
