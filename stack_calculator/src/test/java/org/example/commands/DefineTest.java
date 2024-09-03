package org.example.commands;

import org.example.Context;
import org.example.exceptions.BadArgument;
import org.example.exceptions.BadNumberOfArguments;
import org.example.exceptions.ElementNotFound;
import org.example.exceptions.FailedToAddElement;
import org.junit.jupiter.api.Test;
import org.mockito.ArgumentCaptor;

import static org.junit.jupiter.api.Assertions.*;
import static org.mockito.Mockito.mock;
import static org.mockito.Mockito.verify;

class DefineTest {

    @Test
    void doOperation1() throws FailedToAddElement, BadNumberOfArguments, BadArgument, ElementNotFound {
        Context context = new Context();
        String[] args1 = {"b", "234.7"};
        Define def = new Define();
        def.doOperation(context, args1);
        assertEquals(234.7, context.GetParameter("b"));
    }
}