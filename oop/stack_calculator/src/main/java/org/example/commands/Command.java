package org.example.commands;

import org.example.Context;
import org.example.exceptions.*;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

public abstract class Command {
    public Command() {
    }

    public abstract void doOperation(Context context, String[] args) throws BadStackSize, FailedToAddElement, BadNumberOfArguments, ElementNotFound, BadArgument;
}
