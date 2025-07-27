package org.example.commands;

import org.example.Context;
import org.example.exceptions.BadArgument;
import org.example.exceptions.BadNumberOfArguments;
import org.example.exceptions.BadStackSize;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

class Division extends Command {
    Division() {
    }

    private static final Logger logger = LoggerFactory.getLogger(Division.class);

    @Override
    public void doOperation(Context context, String[] args) throws BadStackSize, BadArgument, BadNumberOfArguments {
        if (args != null && args.length > 0) {
            logger.debug("Too many arguments were extracted (but needed 0): ");
            Context.LogArgs(args);
            throw new BadNumberOfArguments("The number of arguments provided is larger than needed.");
        }
        if (context.GetStackSize() < 2) {
            logger.error("Stack has less elements than the number of arguments in the command.");
            throw new BadStackSize("Not enough elements on stack to execute command.");
        }
        Double a, b, res;
        a = context.PopFromStack();
        b = context.PopFromStack();
        if (b == 0) {
            logger.debug("Division by 0");
            context.PushToStack(b);
            context.PushToStack(a);
            throw new BadArgument("Division by 0");
        }
        res = a / b;
        context.PushToStack(res);
    }
}