package org.example.commands;

import org.example.Context;
import org.example.exceptions.BadNumberOfArguments;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

class Pop extends Command {
    Pop() {
    }

    private static final Logger logger = LoggerFactory.getLogger(Pop.class);

    @Override
    public void doOperation(Context context, String[] args) throws BadNumberOfArguments {
        if (args != null && args.length > 0) {
            logger.debug("Too many arguments were extracted (but needed 0): ");
            Context.LogArgs(args);
            throw new BadNumberOfArguments("The number of arguments provided is larger than needed.");
        }
        context.PopFromStack();
    }
}