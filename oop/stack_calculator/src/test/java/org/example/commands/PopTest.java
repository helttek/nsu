package org.example.commands;

import org.example.Context;
import org.junit.jupiter.api.Test;

import java.util.EmptyStackException;

import static org.junit.jupiter.api.Assertions.*;

class PopTest {

    @Test
    void doOperation() {
        Context context = new Context();
        Pop p = new Pop();
        assertThrows(EmptyStackException.class, () -> p.doOperation(context, null));
    }
}