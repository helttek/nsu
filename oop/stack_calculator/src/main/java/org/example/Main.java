package org.example;

import org.example.exceptions.BadResourceFile;
import org.example.exceptions.ResourceFileLoadFailure;

import java.io.Console;
import java.io.File;
import java.io.IOException;
import java.util.Scanner;

public class Main {
    public static void main(String[] args) throws IOException, BadResourceFile, ResourceFileLoadFailure {
        File input;
        Console console = null;
        Scanner scanner = null;
        if (args.length == 0) {
//            scanner = new Scanner(System.in);
            console = System.console();
        } else {
            input = new File(args[0]);
            if (input.getPath().isEmpty()) {
                throw new RuntimeException("Couldn't open input file");
            }
            scanner = new Scanner(input);
        }
        Calculator calc = new Calculator();
        calc.Run(scanner, console);
    }
}