select * from public."EMPLOYEES" e
inner join public."DEPARTMENTS" d on e."DEPARTMENT_ID" = d."DEPARTMENT_ID"
inner join public."LOCATIONS" l on d."LOCATION_ID" = l."LOCATION_ID"
inner join public."COUNTRIES" c on l."COUNTRY_ID" = c."COUNTRY_ID"
where e."EMPLOYEE_ID" in (select distinct e."MANAGER_ID" from public."EMPLOYEES" e where e."MANAGER_ID" is not null);

----------------------------------------------------------------

select c."COUNTRY_ID", c."COUNTRY_NAME", count(*) cnt from public."DEPARTMENTS" d
inner join public."LCOATIONS" l on d."LOCATION_ID" = l."LOCATION_ID"
inner join public."COUNTRIES" c on l."COUNTRY_ID" = c."COUNTRY_ID"
having count(*) > 1
group by cnt desc

avg
max
round
distinct
count
