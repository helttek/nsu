package org.example.commands;

import org.example.Context;
import org.example.exceptions.BadStackSize;
import org.junit.jupiter.api.Test;

import static org.junit.jupiter.api.Assertions.*;

class AdditionTest {

    @Test
    void doOperation1() throws BadStackSize {
        Context context = new Context();
        context.PushToStack(3.0);
        context.PushToStack(-2.0);
        Addition add = new Addition();
        add.doOperation(context, null);
        assertEquals(1.0, context.PeekTheStack());
    }

    @Test
    void doOperation2() {
        Context context = new Context();
        Addition add = new Addition();
        assertThrows(BadStackSize.class, () -> add.doOperation(context, null));
    }
}