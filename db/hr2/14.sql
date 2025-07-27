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
          MAX("HIRE_DATE")
        FROM
          public."EMPLOYEES"
      )
  ),
  EmployeesInTargetCountries AS (
    SELECT
      e."EMPLOYEE_ID",
      e."MANAGER_ID",
      l."COUNTRY_ID"
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
  SubordinateCounts AS (
    SELECT
      e."MANAGER_ID" AS EMPLOYEE_ID,
      COUNT(e."EMPLOYEE_ID") AS subordinate_count
    FROM
      EmployeesInTargetCountries e
    WHERE
      e."MANAGER_ID" IS NOT NULL
    GROUP BY
      e."MANAGER_ID"
  ),
  MaxSubordinateCounts AS (
    SELECT
      t."COUNTRY_ID",
      MAX(sc.subordinate_count) AS max_subordinate_count
    FROM
      SubordinateCounts sc
      JOIN EmployeesInTargetCountries t ON sc.EMPLOYEE_ID = t."EMPLOYEE_ID"
    GROUP BY
      t."COUNTRY_ID"
  )
SELECT
  sc.EMPLOYEE_ID AS ID_работника,
  sc.subordinate_count AS Количество_подчиненных
FROM
  SubordinateCounts sc
  JOIN EmployeesInTargetCountries t ON sc.EMPLOYEE_ID = t."EMPLOYEE_ID"
  JOIN MaxSubordinateCounts msc ON t."COUNTRY_ID" = msc."COUNTRY_ID"
  AND sc.subordinate_count = msc.max_subordinate_count
ORDER BY
  sc.EMPLOYEE_ID;