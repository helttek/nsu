package org.example.exceptions;

public class FailedCommandExecution extends Exception{
    public FailedCommandExecution(String message){
        super(message);
    }

    @Override
    public String getMessage() {
        return super.getMessage();
    }
}
