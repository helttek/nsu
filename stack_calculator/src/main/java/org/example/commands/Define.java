package org.example.commands;

import org.example.Context;
import org.example.exceptions.BadArgument;
import org.example.exceptions.BadNumberOfArguments;
import org.example.exceptions.FailedToAddElement;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

class Define extends Command {
    Define() {
    }

    private static final Logger logger = LoggerFactory.getLogger(Define.class);

    @Override
    public void doOperation(Context context, String[] args) throws BadNumberOfArguments, BadArgument {
        if (args == null) {
            throw new BadNumberOfArguments("No arguments were provided for the command.");
        }
        if (args.length != 2) {
            logger.debug("Bad number of arguments (only 2 needed): ");
            Context.LogArgs(args);
            throw new BadNumberOfArguments("The number of arguments provided is larger than needed.");
        }
        if (args[0] == null || args[1] == null) {
            logger.error("Provided arguments are null");
            throw new BadArgument("Bad arguments.");
        }
        try {
            Double.parseDouble(args[0]);
            logger.debug("Trying to create a variable which name is a number: '{}'", args[0]);
            throw new BadArgument("Numbers as variable names are prohibited.");
        } catch (NumberFormatException e1) {
            try {
                Double value = Double.parseDouble(args[1]);
                context.AddParameter(args[0], value);
            } catch (NumberFormatException | FailedToAddElement e2) {
                throw new BadArgument("Second argument is not a number.");
            }
        }
    }
}