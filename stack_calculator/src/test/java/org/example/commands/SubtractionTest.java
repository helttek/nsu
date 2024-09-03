package org.example.commands;

import org.example.Context;
import org.example.exceptions.BadStackSize;
import org.junit.jupiter.api.Test;

import static org.junit.jupiter.api.Assertions.*;

class SubtractionTest {

    @Test
    void doOperation1() throws BadStackSize {
        Context context = new Context();
        context.PushToStack(3.0);
        context.PushToStack(-1.5);
        Subtraction sub = new Subtraction();
        sub.doOperation(context, null);
        assertEquals(-4.5, context.PeekTheStack());
    }

    @Test
    void doOperation2() {
        Context context = new Context();
        Subtraction sub = new Subtraction();
        assertThrows(BadStackSize.class, () -> sub.doOperation(context, null));
    }
}