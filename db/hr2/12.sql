WITH
  CountryDepartmentCounts AS (
    -- Считаем количество подразделений в каждой стране
    SELECT
      l."COUNTRY_ID",
      COUNT(d."DEPARTMENT_ID") AS department_count
    FROM
      public."LOCATIONS" l
      LEFT JOIN public."DEPARTMENTS" d ON l."LOCATION_ID" = d."LOCATION_ID"
    GROUP BY
      l."COUNTRY_ID"
  ),
  MaxDepartments AS (
    -- Находим страны с максимальным количеством подразделений
    SELECT
      "COUNTRY_ID"
    FROM
      CountryDepartmentCounts
    WHERE
      department_count = (
        SELECT
          MAX(department_count)
        FROM
          CountryDepartmentCounts
      )
  ),
  EmployeesInTopCountries AS (
    -- Находим всех сотрудников из стран с максимальным количеством подразделений
    SELECT
      e."EMPLOYEE_ID",
      e."SALARY",
      l."COUNTRY_ID"
    FROM
      public."EMPLOYEES" e
      JOIN public."DEPARTMENTS" d ON e."DEPARTMENT_ID" = d."DEPARTMENT_ID"
      JOIN public."LOCATIONS" l ON d."LOCATION_ID" = l."LOCATION_ID"
    WHERE
      l."COUNTRY_ID" IN (
        SELECT
          "COUNTRY_ID"
        FROM
          MaxDepartments
      )
  ),
  MaxSalaryEmployees AS (
    -- Находим максимальную зарплату в каждой из стран
    SELECT
      "COUNTRY_ID",
      MAX("SALARY") AS max_salary
    FROM
      EmployeesInTopCountries
    GROUP BY
      "COUNTRY_ID"
  )
  -- Итоговый запрос: выбираем работников с максимальной зарплатой в странах с наибольшим количеством подразделений
SELECT
  e."COUNTRY_ID",
  e."EMPLOYEE_ID"
FROM
  EmployeesInTopCountries e
  JOIN MaxSalaryEmployees m ON e."COUNTRY_ID" = m."COUNTRY_ID"
  AND e."SALARY" = m.max_salary
ORDER BY
  e."COUNTRY_ID",
  e."EMPLOYEE_ID";