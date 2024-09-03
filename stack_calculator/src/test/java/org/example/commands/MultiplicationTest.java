package org.example.commands;

import org.example.Context;
import org.example.exceptions.BadStackSize;
import org.junit.jupiter.api.Test;

import static org.junit.jupiter.api.Assertions.*;

class MultiplicationTest {

    @Test
    void doOperation1() throws BadStackSize {
        Context context = new Context();
        context.PushToStack(6.0);
        context.PushToStack(-2.0);
        Multiplication mul = new Multiplication();
        mul.doOperation(context, null);
        assertEquals(-12.0, context.PeekTheStack());
    }

    @Test
    void doOperation2() {
        Context context = new Context();
        Multiplication mul = new Multiplication();
        assertThrows(BadStackSize.class, () -> mul.doOperation(context, null));
    }
}