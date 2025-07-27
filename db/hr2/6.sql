WITH
  "department_salaries" AS (
    -- Вычисляем среднюю зарплату по каждому подразделению
    SELECT
      d."DEPARTMENT_ID",
      COALESCE(AVG(e."SALARY"), 0) AS "avg_salary"
    FROM
      public."DEPARTMENTS" d
      LEFT JOIN public."EMPLOYEES" e ON d."DEPARTMENT_ID" = e."DEPARTMENT_ID"
    GROUP BY
      d."DEPARTMENT_ID"
  ),
  "overall_average" AS (
    -- Вычисляем среднюю зарплату по всем сотрудникам
    SELECT
      AVG("SALARY") AS "overall_avg_salary"
    FROM
      public."EMPLOYEES"
  )
SELECT
  ds."DEPARTMENT_ID",
  ds."avg_salary",
  (
    ds."avg_salary" / NULLIF(overall_avg."overall_avg_salary", 0)
  ) * 100 AS "percentage"
FROM
  "department_salaries" ds,
  "overall_average" overall_avg
UNION ALL
-- Учитываем сотрудников, которые не приписаны ни к какому подразделению
SELECT
  NULL AS "DEPARTMENT_ID",
  0 AS "avg_salary",
  -- Сумма зарплат для сотрудников без подразделений равна 0
  0 AS "percentage" -- Процент тоже равен 0
FROM
  "overall_average" overall_avg
WHERE
  (
    SELECT
      COUNT(*)
    FROM
      public."EMPLOYEES" e
    WHERE
      e."DEPARTMENT_ID" IS NULL
  ) > 0
ORDER BY
  "DEPARTMENT_ID" ASC,
  "avg_salary" DESC,
  "percentage" DESC;