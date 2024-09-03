package org.example.commands;

import org.example.Context;
import org.example.exceptions.BadArgument;
import org.example.exceptions.BadStackSize;
import org.junit.jupiter.api.Test;

import static org.junit.jupiter.api.Assertions.*;

class SquareRootTest {

    @Test
    void doOperation1() throws BadStackSize, BadArgument {
        Context context = new Context();
        context.PushToStack(16.0);
        SquareRoot sqrt = new SquareRoot();
        sqrt.doOperation(context, null);
        assertEquals(4.0, context.PeekTheStack());
    }

    @Test
    void doOperation2() {
        Context context = new Context();
        SquareRoot sqrt = new SquareRoot();
        assertThrows(BadStackSize.class, () -> sqrt.doOperation(context, null));
    }

    @Test
    void doOperation3() {
        Context context = new Context();
        context.PushToStack(-16.0);
        SquareRoot sqrt = new SquareRoot();
        assertThrows(BadArgument.class, () -> sqrt.doOperation(context, null));
    }
}