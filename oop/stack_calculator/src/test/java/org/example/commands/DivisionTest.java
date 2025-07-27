package org.example.commands;

import org.example.Context;
import org.example.exceptions.BadStackSize;
import org.junit.jupiter.api.Test;

import static org.junit.jupiter.api.Assertions.*;

class DivisionTest {

    @Test
    void doOperation1() throws BadStackSize {
        Context context = new Context();
        context.PushToStack(2.0);
        context.PushToStack(-3.0);
        Division div = new Division();
        div.doOperation(context, null);
        assertEquals(-1.5, context.PeekTheStack());
    }

    @Test
    void doOperation2() {
        Context context = new Context();
        Division div = new Division();
        assertThrows(BadStackSize.class, () -> div.doOperation(context, null));
    }
}