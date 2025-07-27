WITH
  EmployeeSeniority AS (
    SELECT
      e."EMPLOYEE_ID",
      l."COUNTRY_ID",
      e."HIRE_DATE"
    FROM
      public."EMPLOYEES" e
      JOIN public."DEPARTMENTS" d ON e."DEPARTMENT_ID" = d."DEPARTMENT_ID"
      JOIN public."LOCATIONS" l ON d."LOCATION_ID" = l."LOCATION_ID"
    WHERE
      e."HIRE_DATE" = (
        SELECT
          MIN("HIRE_DATE")
        FROM
          public."EMPLOYEES"
      )
  ),
  EmployeesInTargetCountries AS (
    SELECT
      e."EMPLOYEE_ID",
      e."MANAGER_ID",
      l."COUNTRY_ID",
      e."SALARY"
    FROM
      public."EMPLOYEES" e
      JOIN public."DEPARTMENTS" d ON e."DEPARTMENT_ID" = d."DEPARTMENT_ID"
      JOIN public."LOCATIONS" l ON d."LOCATION_ID" = l."LOCATION_ID"
    WHERE
      l."COUNTRY_ID" IN (
        SELECT DISTINCT
          "COUNTRY_ID"
        FROM
          EmployeeSeniority
      )
  ),
  ManagerSalarySums AS (
    SELECT
      e."MANAGER_ID" AS EMPLOYEE_ID,
      SUM(e."SALARY") AS total_subordinate_salary
    FROM
      EmployeesInTargetCountries e
    WHERE
      e."MANAGER_ID" IS NOT NULL
    GROUP BY
      e."MANAGER_ID"
  ),
  MaxSalaryByCountry AS (
    SELECT
      t."COUNTRY_ID",
      MAX(ms.total_subordinate_salary) AS max_subordinate_salary
    FROM
      ManagerSalarySums ms
      JOIN EmployeesInTargetCountries t ON ms.EMPLOYEE_ID = t."EMPLOYEE_ID"
    GROUP BY
      t."COUNTRY_ID"
  )
SELECT
  ms.EMPLOYEE_ID AS ID_работника,
  ms.total_subordinate_salary AS Сумма_зарплат_подчиненных
FROM
  ManagerSalarySums ms
  JOIN EmployeesInTargetCountries t ON ms.EMPLOYEE_ID = t."EMPLOYEE_ID"
  JOIN MaxSalaryByCountry msc ON t."COUNTRY_ID" = msc."COUNTRY_ID"
  AND ms.total_subordinate_salary = msc.max_subordinate_salary
ORDER BY
  ms.EMPLOYEE_ID;