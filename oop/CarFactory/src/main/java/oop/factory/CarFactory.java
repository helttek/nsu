package oop.factory;

import java.io.FileInputStream;
import java.io.IOException;
import java.util.Properties;

public class CarFactory {

    CarBodySupplier carBodySupplier;

    public CarFactory(String configFileName) throws IOException {
        Properties prop = new Properties();//TODO: check logic of this function
        try (FileInputStream inputStream = new FileInputStream(configFileName)) {
            prop.load(inputStream);
        } catch (IOException | NullPointerException | IllegalArgumentException | SecurityException e) {
            System.err.println(e.getMessage());
        }
    }

    public void Start() {
        System.out.println("Car Factory");
    }
}
