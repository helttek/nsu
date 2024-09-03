package org.example.commands;

import org.example.Context;
import org.example.exceptions.BadArgument;
import org.example.exceptions.BadNumberOfArguments;
import org.example.exceptions.ElementNotFound;
import org.example.exceptions.FailedToAddElement;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

class Push extends Command {
    Push() {
    }

    private static final Logger logger = LoggerFactory.getLogger(Push.class);

    @Override
    public void doOperation(Context context, String[] args) throws FailedToAddElement, BadNumberOfArguments, ElementNotFound, BadArgument {
        if (args == null) {
            throw new BadNumberOfArguments("No arguments were provided for the command.");
        }
        if (args.length != 1) {
            logger.debug("Bad number of arguments (only 1 needed): ");
            Context.LogArgs(args);
            throw new BadNumberOfArguments("The number of arguments provided is larger than needed.");
        }
        if (args[0] == null) {
            logger.error("Provided argument is null");
            throw new BadArgument("Bad argument.");
        }
        try {
            Double value = Double.parseDouble(args[0]);
            context.PushToStack(value);
        } catch (NumberFormatException e) {
            Double value = context.GetParameter(args[0]);
            if (value == null) {
                logger.error("Parameter with the name '{}' was not found", args[0]);
                throw new ElementNotFound("No such variable has been defined.");
            }
            context.PushToStack(value);
        }
    }
}