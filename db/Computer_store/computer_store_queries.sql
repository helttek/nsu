--Запрос Комплектующих

select c.name, k.name, c.price, c.warranty_days, c.availability from components as c join component_categories as k on c.category_id = k.category_id
group by c.name, k.name, c.price, c.warranty_days, c.availability;

--Серийный номер и цена компьютеров

select c.name, c.serial_number, SUM(k.price * cc.quantity) as price from computers as c join computer_components as cc on c.computer_id = cc.computer_id join components as
k on cc.component_id = k.component_id group by c.serial_number, c.name;

--Самое дешевое комплектующее в каждой категории

select a.name , b.name as category, a.price, a.warranty_days, a.availability
from components as a
join component_categories as b on a.category_id = b.category_id
join (select category_id, min(price) as price
        from components
        group by category_id) as c
on a.category_id = c.category_id and a.price = c.price;
