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

class PushTest {

    @Test
    void doOperation1() throws FailedToAddElement, BadNumberOfArguments, BadArgument, ElementNotFound {
        Context context = mock(Context.class);
        String[] args1 = {"45.5"};
        Push p = new Push();
        p.doOperation(context, args1);
        ArgumentCaptor<Double> argumentCaptor = ArgumentCaptor.forClass(Double.class);
        verify(context).PushToStack(argumentCaptor.capture());
        assertEquals(45.5, argumentCaptor.getValue());
    }

    @Test
    void doOperation2() throws FailedToAddElement, BadNumberOfArguments, BadArgument, ElementNotFound {
        Context context = new Context();
        context.AddParameter("a", 23.8);
        String[] args1 = {"a"};
        Push p = new Push();
        p.doOperation(context, args1);
        assertEquals(23.8, context.PeekTheStack());
    }

    @Test
    void doOperation3() {
        String[] args1 = {"a"};
        Context context = new Context();
        Push p = new Push();
        assertThrows(ElementNotFound.class, () -> p.doOperation(context, args1));
    }
}