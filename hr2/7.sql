SELECT
  "FIRST_NAME" AS Имя,
  COUNT(*) AS Количество_работников
FROM
  public."EMPLOYEES"
GROUP BY
  "FIRST_NAME"
ORDER BY
  Количество_работников DESC,
  Имя ASC;