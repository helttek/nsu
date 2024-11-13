-- Inserting data into ComponentCategories
INSERT INTO ComponentCategories (name, necessity) VALUES
('Graphics Card', TRUE),
('Processor', TRUE),
('Motherboard', TRUE),
('RAM', TRUE),
('Storage', TRUE),
('Power Supply', TRUE),
('Cooling System', TRUE),
('Case', TRUE),
('Network Card', FALSE),
('Sound Card', FALSE);

-- Inserting data into Categories
INSERT INTO ComputerCategories (name) VALUES
('Gaming PC'),
('Office PC'),
('Workstation'),
('Home Theater PC'),
('Mini PC');

-- Inserting data into Computers
INSERT INTO Computers (name, category_id) VALUES
('Gaming Beast', 1),
('Office Workhorse', 2),
('Creative Studio', 3),
('Home Media Center', 4),
('Compact Powerhouse', 5),
('Budget Gaming PC', 1),
('Business Laptop', 2),
('High-End Workstation', 3),
('Home Office Setup', 2),
('Portable Mini PC', 5);

-- Inserting data into Components
INSERT INTO Components (name, category_id, price, warranty_period, availability) VALUES
('NVIDIA RTX 3080', 1, 69999.00, 730, 10),
('AMD Ryzen 7 5800X', 2, 44999.00, 730, 15),
('ASUS ROG Strix B550-F', 3, 18999.00, 365, 20),
('Corsair Vengeance LPX 16GB', 4, 7999.00, 365, 30),
('Samsung 970 EVO 1TB', 5, 11999.00, 365, 25),
('EVGA 600 W1', 6, 4999.00, 730, 18),
('Cooler Master Hyper 212', 7, 3999.00, 365, 12),
('NZXT H510', 8, 6999.00, 365, 14),
('TP-Link AC750', 9, 2999.00, 365, 50),
('Creative Sound Blaster Z', 10, 9999.00, 365, 22);

-- Inserting data into ComputersComponents
INSERT INTO ComputersComponents (computer_id, component_id, quantity) VALUES
(1, 1, 1),
(1, 2, 1),
(1, 3, 1),
(1, 4, 2),
(2, 2, 1),
(2, 4, 1),
(3, 1, 1),
(3, 5, 1),
(4, 5, 1),
(5, 6, 1);
