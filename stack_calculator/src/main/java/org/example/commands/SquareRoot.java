package org.example.commands;

import org.example.Context;
import org.example.exceptions.BadArgument;
import org.example.exceptions.BadNumberOfArguments;
import org.example.exceptions.BadStackSize;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

class SquareRoot extends Command {
    SquareRoot() {
    }

    private static final Logger logger = LoggerFactory.getLogger(SquareRoot.class);

    @Override
    public void doOperation(Context context, String[] args) throws BadStackSize, BadArgument, BadNumberOfArguments {
        if (args != null && args.length > 0) {
            logger.debug("Too many arguments were extracted (but needed 0): ");
            Context.LogArgs(args);
            throw new BadNumberOfArguments("The number of arguments provided is larger than needed.");
        }
        if (context.GetStackSize() < 1) {
            logger.error("Stack has less elements than the number of arguments in the command.");
            throw new BadStackSize("Not enough elements on stack to execute command.");
        }
        if (context.PeekTheStack() < 0) {
            logger.error("Trying to take the square root of a negative number");
            throw new BadArgument("An attempt to take the square root of a negative number");
        }
        Double res = Math.sqrt(context.PopFromStack());
        context.PushToStack(res);
    }
}