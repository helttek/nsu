WITH
  CountryDepartmentCounts AS (
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
  EmployeeSubordinateCounts AS (
    SELECT
      m."EMPLOYEE_ID" AS manager_id,
      COUNT(e."EMPLOYEE_ID") AS subordinate_count
    FROM
      public."EMPLOYEES" e
      JOIN public."EMPLOYEES" m ON e."MANAGER_ID" = m."EMPLOYEE_ID"
    GROUP BY
      m."EMPLOYEE_ID"
  ),
  EmployeesInTopCountries AS (
    SELECT
      e."EMPLOYEE_ID",
      l."COUNTRY_ID",
      COALESCE(sc.subordinate_count, 0) AS subordinate_count
    FROM
      public."EMPLOYEES" e
      JOIN public."DEPARTMENTS" d ON e."DEPARTMENT_ID" = d."DEPARTMENT_ID"
      JOIN public."LOCATIONS" l ON d."LOCATION_ID" = l."LOCATION_ID"
      LEFT JOIN EmployeeSubordinateCounts sc ON e."EMPLOYEE_ID" = sc.manager_id
    WHERE
      l."COUNTRY_ID" IN (
        SELECT
          "COUNTRY_ID"
        FROM
          MaxDepartments
      )
  ),
  MaxSubordinateEmployees AS (
    SELECT
      "COUNTRY_ID",
      MAX(subordinate_count) AS max_subordinate_count
    FROM
      EmployeesInTopCountries
    GROUP BY
      "COUNTRY_ID"
  )
SELECT
  e."EMPLOYEE_ID" AS ID_работника,
  e.subordinate_count AS Количество_подчиненных
FROM
  EmployeesInTopCountries e
  JOIN MaxSubordinateEmployees m ON e."COUNTRY_ID" = m."COUNTRY_ID"
  AND e.subordinate_count = m.max_subordinate_count
ORDER BY
  e."EMPLOYEE_ID";