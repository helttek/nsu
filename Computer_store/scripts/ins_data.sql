INSERT INTO computer_categories (category_id, name) VALUES
    (1, 'Desktop'),
    (2, 'Laptop'),
    (3, 'Server'),
    (4, 'Workstation');

INSERT INTO component_categories (category_id, name, required) VALUES
    (1, 'CPU', TRUE),
    (2, 'RAM', TRUE),
    (3, 'Storage', TRUE),
    (4, 'Graphics Card', FALSE),
    (5, 'Power Supply', TRUE),
    (6, 'Case', FALSE);

INSERT INTO computers (computer_id, serial_number, category_id, name) VALUES
    (1, 100001, 1, 'Dell OptiPlex 7080'),
    (2, 100002, 2, 'HP Pavilion 15'),
    (3, 100003, 3, 'Dell PowerEdge R740'),
    (4, 100004, 4, 'Lenovo ThinkStation P340'),
    (5, 100005, 2, 'Apple MacBook Air');

INSERT INTO components (component_id, name, category_id, price, warranty_days, availability) VALUES
    (1, 'Intel Core i5 Processor', 1, 220.00, 365, 'In Stock'),
    (2, '16GB DDR4 RAM', 2, 80.00, 365, 'In Stock'),
    (3, '512GB SSD', 3, 120.00, 730, 'Out of Stock'),
    (4, 'NVIDIA GTX 1660', 4, 250.00, 365, 'In Stock'),
    (5, '500W Power Supply', 5, 50.00, 730, 'In Stock'),
    (6, 'Mid Tower Case', 6, 45.00, 180, 'In Stock'),
    (7, 'Intel Core i7 Processor', 1, 320.00, 365, 'Out of Stock'),
    (8, '32GB DDR4 RAM', 2, 140.00, 365, 'In Stock'),
    (9, '1TB HDD', 3, 60.00, 730, 'In Stock'),
    (10, 'NVIDIA RTX 3080', 4, 800.00, 365, 'Limited Stock');

INSERT INTO computer_components (computer_id, component_id, quantity) VALUES
    (1, 1, 1),
    (1, 2, 2),
    (1, 3, 1),
    (2, 1, 1),
    (2, 2, 1),
    (2, 3, 1),
    (3, 1, 2),
    (3, 2, 4),
    (3, 5, 2),
    (4, 1, 1),
    (4, 4, 1),
    (5, 7, 1),
    (5, 8, 2),
    (5, 9, 1),
    (5, 10, 1);
