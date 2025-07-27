package org.example.exceptions;

public class ElementNotFound extends Exception {
    public ElementNotFound(String message) {
        super(message);
    }

    @Override
    public String getMessage() {
        return super.getMessage();
    }
}
