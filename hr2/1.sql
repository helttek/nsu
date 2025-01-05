WITH Managers AS (
    SELECT e."EMPLOYEE_ID" AS "Manager_ID", e."HIRE_DATE"
    FROM public."EMPLOYEES" e
    WHERE
        EXISTS (
            SELECT * 
            FROM public."EMPLOYEES" ee
            WHERE ee."MANAGER_ID" = e."EMPLOYEE_ID"
        )
),
MinTenure AS (
    SELECT MAX("HIRE_DATE") AS "Latest_Hire_Date"
    FROM Managers
)
SELECT m."Manager_ID", m."HIRE_DATE"
FROM Managers m
JOIN MinTenure t
ON m."HIRE_DATE" = t."Latest_Hire_Date"
ORDER BY m."Manager_ID";
