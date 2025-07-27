WITH RECURSIVE
  EmployeeHierarchy AS (
    SELECT
      e."EMPLOYEE_ID",
      e."DEPARTMENT_ID",
      1 AS level
    FROM
      public."EMPLOYEES" e
    WHERE
      e."MANAGER_ID" IS NULL
    UNION ALL
    SELECT
      e."EMPLOYEE_ID",
      e."DEPARTMENT_ID",
      eh.level + 1 AS level
    FROM
      public."EMPLOYEES" e
      JOIN EmployeeHierarchy eh ON e."MANAGER_ID" = eh."EMPLOYEE_ID"
  )
SELECT
  d."DEPARTMENT_NAME" AS Название_отдела,
  COUNT(
    CASE
      WHEN eh.level = 1 THEN 1
    END
  ) AS количество_работников_I_уровня,
  COUNT(
    CASE
      WHEN eh.level = 2 THEN 1
    END
  ) AS количество_работников_II_уровня,
  COUNT(
    CASE
      WHEN eh.level = 3 THEN 1
    END
  ) AS количество_работников_III_уровня
FROM
  public."DEPARTMENTS" d
  LEFT JOIN EmployeeHierarchy eh ON d."DEPARTMENT_ID" = eh."DEPARTMENT_ID"
  JOIN public."LOCATIONS" l ON d."LOCATION_ID" = l."LOCATION_ID"
  JOIN public."COUNTRIES" c ON l."COUNTRY_ID" = c."COUNTRY_ID"
  JOIN public."REGIONS" r ON c."REGION_ID" = r."REGION_ID"
WHERE
  r."REGION_NAME" = 'Americas'
GROUP BY
  d."DEPARTMENT_NAME"
ORDER BY
  d."DEPARTMENT_NAME";