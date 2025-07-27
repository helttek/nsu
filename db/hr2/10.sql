WITH
  DepartmentEmployeeCount AS (
    SELECT
      "DEPARTMENT_ID",
      COUNT(*) AS employee_count
    FROM
      public."EMPLOYEES"
    GROUP BY
      "DEPARTMENT_ID"
  ),
  MinEmployeeDepartment AS (
    SELECT
      "DEPARTMENT_ID"
    FROM
      DepartmentEmployeeCount
    WHERE
      employee_count = (
        SELECT
          MIN(employee_count)
        FROM
          DepartmentEmployeeCount
      )
  )
SELECT
  e."DEPARTMENT_ID" AS Номер_отдела,
  SUM(e."SALARY") AS Общая_сумма_зарплаты
FROM
  public."EMPLOYEES" e
  JOIN MinEmployeeDepartment d ON e."DEPARTMENT_ID" = d."DEPARTMENT_ID"
GROUP BY
  e."DEPARTMENT_ID"
ORDER BY
  e."DEPARTMENT_ID";