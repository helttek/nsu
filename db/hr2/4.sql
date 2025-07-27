WITH EuropeanManagers AS (
    SELECT e."EMPLOYEE_ID" AS "Manager_ID", e."SALARY", COUNT(sub."EMPLOYEE_ID") AS "Subordinates_Count"
    FROM public."EMPLOYEES" e
    JOIN public."EMPLOYEES" sub ON sub."MANAGER_ID" = e."EMPLOYEE_ID"
    JOIN public."DEPARTMENTS" d ON e."DEPARTMENT_ID" = d."DEPARTMENT_ID"
    JOIN public."LOCATIONS" l ON d."LOCATION_ID" = l."LOCATION_ID"
    JOIN public."COUNTRIES" c ON l."COUNTRY_ID" = c."COUNTRY_ID"
    JOIN public."REGIONS" r ON c."REGION_ID" = r."REGION_ID"
    WHERE r."REGION_NAME" = 'Europe'
    GROUP BY e."EMPLOYEE_ID", e."SALARY"
),
TopEuropeanManager AS (
    SELECT "Manager_ID", "SALARY" AS "Manager_Salary","Subordinates_Count"
    FROM EuropeanManagers
    WHERE "Subordinates_Count" = (SELECT MAX("Subordinates_Count") FROM EuropeanManagers)
),
AverageSalaryEurope AS (
    SELECT AVG("Manager_Salary") AS "Avg_Manager_Salary"
    FROM TopEuropeanManager
),
AmericasEmployees AS (
    SELECT e."LAST_NAME", e."FIRST_NAME", e."SALARY"
    FROM public."EMPLOYEES" e
    JOIN public."DEPARTMENTS" d ON e."DEPARTMENT_ID" = d."DEPARTMENT_ID"
    JOIN public."LOCATIONS" l ON d."LOCATION_ID" = l."LOCATION_ID"
    JOIN public."COUNTRIES" c ON l."COUNTRY_ID" = c."COUNTRY_ID"
    JOIN public."REGIONS" r ON c."REGION_ID" = r."REGION_ID"
    WHERE r."REGION_NAME" = 'Americas'
)
SELECT a."LAST_NAME", a."FIRST_NAME", a."SALARY"
FROM AmericasEmployees a
WHERE a."SALARY" > (SELECT "Avg_Manager_Salary" FROM AverageSalaryEurope)
ORDER BY a."LAST_NAME", a."FIRST_NAME";
