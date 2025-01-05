WITH
  AverageSalary AS (
    SELECT
      AVG(e."SALARY") AS avg_salary
    FROM
      public."EMPLOYEES" e
      JOIN public."DEPARTMENTS" d ON e."DEPARTMENT_ID" = d."DEPARTMENT_ID"
      JOIN public."LOCATIONS" l ON d."LOCATION_ID" = l."LOCATION_ID"
      JOIN public."COUNTRIES" c ON l."COUNTRY_ID" = c."COUNTRY_ID"
      JOIN public."REGIONS" r ON c."REGION_ID" = r."REGION_ID"
    WHERE
      r."REGION_NAME" = 'Americas'
  ),
  DepartmentMinSalaries AS (
    SELECT
      d."DEPARTMENT_NAME",
      MIN(e."SALARY") AS min_salary
    FROM
      public."DEPARTMENTS" d
      JOIN public."EMPLOYEES" e ON d."DEPARTMENT_ID" = e."DEPARTMENT_ID"
      JOIN public."LOCATIONS" l ON d."LOCATION_ID" = l."LOCATION_ID"
      JOIN public."COUNTRIES" c ON l."COUNTRY_ID" = c."COUNTRY_ID"
      JOIN public."REGIONS" r ON c."REGION_ID" = r."REGION_ID"
    GROUP BY
      d."DEPARTMENT_NAME"
  )
SELECT
  dms."DEPARTMENT_NAME"
FROM
  DepartmentMinSalaries dms
WHERE
  dms.min_salary > (
    SELECT
      avg_salary
    FROM
      AverageSalary
  )
ORDER BY
  dms."DEPARTMENT_NAME";