package org.example.exceptions;

public class ResourceFileLoadFailure extends Exception {
    public ResourceFileLoadFailure(String message) {
        super(message);
    }

    @Override
    public String getMessage() {
        return super.getMessage();
    }
}
