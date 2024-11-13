CREATE DATABASE computer_store;

\c computer_store;

CREATE TABLE computer_categories (
    category_id SERIAL PRIMARY KEY,
    name VARCHAR(50) NOT NULL
);

GRANT SELECT, INSERT, UPDATE, DELETE ON TABLE computer_categories TO helttek;

CREATE TABLE component_categories (
    category_id SERIAL PRIMARY KEY,
    name VARCHAR(50) NOT NULL,
    required BOOLEAN NOT NULL
);

GRANT SELECT, INSERT, UPDATE, DELETE ON TABLE component_categories TO helttek;

CREATE TABLE computers (
    computer_id SERIAL PRIMARY KEY,
    serial_number INTEGER NOT NULL,
    category_id INTEGER NOT NULL,
    name VARCHAR(150) NOT NULL,
    CONSTRAINT computer_category_constraint FOREIGN KEY (category_id) REFERENCES computer_categories(category_id) ON UPDATE CASCADE
);

GRANT SELECT, INSERT, UPDATE, DELETE ON TABLE computers TO helttek;

CREATE TABLE components (
    component_id SERIAL PRIMARY KEY,
    name VARCHAR(200) NOT NULL,
    category_id INTEGER NOT NULL,
    price DOUBLE PRECISION CHECK (price >= 0),
    warranty_days INTEGER CHECK (warranty_days >= 0),
    availability VARCHAR(100),
    CONSTRAINT component_category_constraint FOREIGN KEY (category_id) REFERENCES component_categories(category_id) ON UPDATE CASCADE
);

GRANT SELECT, INSERT, UPDATE, DELETE ON TABLE components TO helttek;

CREATE TABLE computer_components (
    computer_id INTEGER NOT NULL,
    component_id INTEGER NOT NULL,
    quantity INTEGER NOT NULL CHECK (quantity >= 0),
    PRIMARY KEY (computer_id, component_id),
    CONSTRAINT computer_constraint FOREIGN KEY (computer_id) REFERENCES computers(computer_id) ON DELETE CASCADE ON UPDATE CASCADE,
    CONSTRAINT component_constraint FOREIGN KEY (component_id) REFERENCES components(component_id) ON DELETE CASCADE ON UPDATE CASCADE
);

GRANT SELECT, INSERT, UPDATE, DELETE ON TABLE computer_components TO helttek;
