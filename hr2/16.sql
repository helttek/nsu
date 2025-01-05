SELECT 
    EXTRACT(YEAR FROM "HIRE_DATE") AS "Year", 
    COUNT(*) AS "Employee_Count"
FROM 
    public."EMPLOYEES"
GROUP BY 
    EXTRACT(YEAR FROM "HIRE_DATE")
ORDER BY 
    "Employee_Count" DESC LIMIT 1;
