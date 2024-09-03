package oop;

import oop.factory.CarFactory;

import java.io.IOException;

public class Main {
    public static void main(String[] args) {
        if (args.length == 0) {
            System.err.println("No config file provided. Cannot start Car Factory without a config file.");
            return;
        }
        try {
            CarFactory carFactory = new CarFactory(args[0]);
            carFactory.Start();
        } catch (RuntimeException | IOException e) {
            System.err.println(e.getMessage());
        }
    }
}