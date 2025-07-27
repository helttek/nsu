import psycopg2
from psycopg2 import Error
from psycopg2.extras import RealDictCursor


class Model:
    def __init__(self):
        print("Model constructor")
        self.connection = None
        self.cursor = None
        self.current_user = None
        self.current_password = None
        self.db_host = "localhost"
        self.db_port = "5432"
        self.db_name = "university_library"

        self.queries = {
            # 1) query
            0: """
               SELECT COALESCE(rfi.department, 'N/A data')                                                               AS department,
                      COALESCE(rfi.faculty, 'N/A data')                                                                  AS faculty,
                      COALESCE(rfi.course::text, 'N/A data')                                                             AS course, -- rfi.course is numeric, cast for display
                      COALESCE(rfi.group_, 'N/A data')                                                                   AS group_identifier,
                      COUNT(DISTINCT rfi.id)                                                                             AS total_readers,
                      STRING_AGG(DISTINCT rfi.surname || ' ' || rfi.name_ || COALESCE(' ' || rfi.middle_name, ''), '; ') AS readers_list
               FROM reader_full_info rfi
               WHERE (
                   %(pickup_point_id)s IS NULL OR
                   EXISTS ( -- check if reader has any issued books from this pickup point
                       SELECT 1
                       FROM publication_issuing pi_filter
                                JOIN library_card lc_filter ON pi_filter.library_card_id = lc_filter.id
                       WHERE lc_filter.reader_id = rfi.id
                         AND pi_filter.pickup_point_id = %(pickup_point_id)s) OR
                   EXISTS ( -- check if reader has any orders from this pickup point
                       SELECT 1
                       FROM order_ o_filter
                                JOIN library_card lc_filter ON o_filter.library_card_id = lc_filter.id
                       WHERE lc_filter.reader_id = rfi.id
                         AND o_filter.pickup_point_id = %(pickup_point_id)s)
                   )
                 AND (%(department_name)s IS NULL OR rfi.department ILIKE %(department_name)s)
                 AND (%(faculty_name)s IS NULL OR rfi.faculty ILIKE %(faculty_name)s)
                 AND (%(course_name)s IS NULL OR rfi.course::text ILIKE %(course_name)s) -- Compare course as text
                 AND (%(group_name)s IS NULL OR rfi.group_ ILIKE %(group_name)s)
               GROUP BY COALESCE(rfi.department, 'N/A data'),
                        COALESCE(rfi.faculty, 'N/A data'),
                        COALESCE(rfi.course::text, 'N/A data'),
                        COALESCE(rfi.group_, 'N/A data')
               ORDER BY department, faculty, course, group_identifier;
               """,
            # 2) query
            1: """
               SELECT COALESCE(dv.department, 'N/A data')                                                                                                                                                      AS department,
                      COALESCE(dv.faculty, 'N/A data')                                                                                                                                                         AS faculty,
                      COALESCE(dv.course::text, 'N/A data')                                                                                                                                                    AS course,
                      COALESCE(dv.group_, 'N/A data')                                                                                                                                                          AS group_identifier,
                      COALESCE(dv.category, 'N/A data')                                                                                                                                                        AS reader_category,
                      COUNT(DISTINCT dv.id)                                                                                                                                                                    AS total_overdue_readers,
                      STRING_AGG(DISTINCT dv.surname || ' ' || dv.name_ || COALESCE(' ' || dv.middle_name, '') || ' (Просрочено: ' || dv.days_overdue || ' дней, Книга: ' || dv.publication_name || ')', '; ') AS overdue_readers_list
               FROM debtors_view dv
               WHERE (%(pickup_point_id)s IS NULL OR dv.pickup_point_id = %(pickup_point_id)s)
                 AND (%(min_days_overdue)s IS NULL OR dv.days_overdue > %(min_days_overdue)s)
                 AND (%(department_name)s IS NULL OR dv.department ILIKE %(department_name)s)
                 AND (%(faculty_name)s IS NULL OR dv.faculty ILIKE %(faculty_name)s)
                 AND (%(course_name)s IS NULL OR dv.course::text ILIKE %(course_name)s)
                 AND (%(group_name)s IS NULL OR dv.group_ ILIKE %(group_name)s)
                 AND (%(reader_category_name)s IS NULL OR dv.category ILIKE %(reader_category_name)s)
               GROUP BY COALESCE(dv.department, 'N/A data'),
                        COALESCE(dv.faculty, 'N/A data'),
                        COALESCE(dv.course::text, 'N/A data'),
                        COALESCE(dv.group_, 'N/A data'),
                        COALESCE(dv.category, 'N/A data')
               ORDER BY total_overdue_readers DESC, department, faculty, course, group_identifier, reader_category;
               """,
            # 3) query
            2: """
               SELECT bos.publication_name,
                      bos.author,
                      bos.pickup_point_name AS ordering_location,
                      bos.faculty           AS reader_faculty,
                      bos.order_count
               FROM book_orders_stats bos
               WHERE (%(pickup_point_id)s IS NULL OR bos.pickup_point_id = %(pickup_point_id)s)
                 AND (%(faculty_name)s IS NULL OR bos.faculty ILIKE %(faculty_name)s)
               ORDER BY bos.order_count DESC LIMIT 20;
               """,
            # 4) query (received)
            3: """
               WITH ReceivedEvents AS (SELECT pe.publication_id,
                                              p.name_       AS publication_name,
                                              p.author,
                                              p.publishing_date,
                                              pe.event_date AS receipt_date,
                                              pe.pickup_point_id,
                                              pp.name_      AS pickup_point_name
                                       FROM publication_event pe
                                                JOIN publication p ON pe.publication_id = p.id
                                                JOIN event_type et ON pe.event_type_id = et.id
                                                LEFT JOIN pickup_point pp ON pe.pickup_point_id = pp.id
                                       WHERE et.name_ ILIKE %(event_type_reception)s
                   AND pe.event_date >= (CURRENT_DATE - INTERVAL '1 year')
                   AND pe.event_date <= CURRENT_DATE
                   )
               SELECT COALESCE(re.pickup_point_name, 'N/A или центральный фонд')                                                        AS location_received,
                      re.author,
                      EXTRACT(YEAR FROM re.publishing_date)::TEXT AS publication_year, EXTRACT(YEAR FROM re.receipt_date)::TEXT AS receipt_year_in_library, COUNT(re.publication_id) AS total_books_received,
                      STRING_AGG(DISTINCT re.publication_name || ' (Получено: ' || TO_CHAR(re.receipt_date, 'YYYY-MM-DD') || ')', '; ') AS received_books_list
               FROM ReceivedEvents re
               WHERE (%(pickup_point_id)s IS NULL OR re.pickup_point_id = %(pickup_point_id)s)
                 AND (%(author_name)s IS NULL OR re.author ILIKE %(author_name)s)
                 AND (%(publication_year_filter)s IS NULL OR
                      EXTRACT(YEAR FROM re.publishing_date) = %(publication_year_filter)s)
                 AND (%(receipt_year_filter)s IS NULL OR EXTRACT(YEAR FROM re.receipt_date) = %(receipt_year_filter)s)
               GROUP BY COALESCE(re.pickup_point_name, 'N/A или центральный фонд'),
                        re.author,
                        EXTRACT(YEAR FROM re.publishing_date)::TEXT,
                   EXTRACT(YEAR FROM re.receipt_date)::TEXT
               ORDER BY location_received, author, publication_year, receipt_year_in_library;
               """,
            # 4) query (lost)
            4: """
               WITH LostEvents AS (SELECT pe.publication_id,
                                          p.name_            AS publication_name,
                                          p.author,
                                          p.publishing_date,
                                          pe.event_date      AS lost_date,
                                          pe.pickup_point_id AS last_known_pickup_point_id,
                                          pp.name_           AS last_known_pickup_point_name
                                   FROM publication_event pe
                                            JOIN publication p ON pe.publication_id = p.id
                                            JOIN event_type et ON pe.event_type_id = et.id
                                            LEFT JOIN pickup_point pp ON pe.pickup_point_id = pp.id
                                   WHERE et.name_ ILIKE %(event_type_lost)s -- Parameterized
                   AND pe.event_date >= (CURRENT_DATE - INTERVAL '1 year')
                   AND pe.event_date <= CURRENT_DATE
                   )
               SELECT COALESCE(le.last_known_pickup_point_name, 'N/A или центральный фонд')                                         AS location_lost_from,
                      le.author,
                      EXTRACT(YEAR FROM le.publishing_date)::TEXT AS publication_year, EXTRACT(YEAR FROM le.lost_date)::TEXT AS year_lost, COUNT(le.publication_id) AS total_books_lost,
                      STRING_AGG(DISTINCT le.publication_name || ' (Утеряно: ' || TO_CHAR(le.lost_date, 'YYYY-MM-DD') || ')', '; ') AS lost_books_list
               FROM LostEvents le
               WHERE (%(pickup_point_id)s IS NULL OR le.last_known_pickup_point_id = %(pickup_point_id)s)
                 AND (%(author_name)s IS NULL OR le.author ILIKE %(author_name)s)
                 AND (%(publication_year_filter)s IS NULL OR
                      EXTRACT(YEAR FROM le.publishing_date) = %(publication_year_filter)s)
                 AND (%(lost_year_filter)s IS NULL OR EXTRACT(YEAR FROM le.lost_date) = %(lost_year_filter)s)
               GROUP BY COALESCE(le.last_known_pickup_point_name, 'N/A или центральный фонд'),
                        le.author,
                        EXTRACT(YEAR FROM le.publishing_date)::TEXT,
                   EXTRACT(YEAR FROM le.lost_date)::TEXT
               ORDER BY location_lost_from, author, publication_year, year_lost;
               """,
            # 5) query (regular readers)
            5: """
               SELECT pp.name_                          AS pickup_point_name,
                      COUNT(DISTINCT r_assoc.reader_id) AS total_readers
               FROM pickup_point pp
                        LEFT JOIN (
                   -- Readers who ordered from this pickup point
                   SELECT o.pickup_point_id AS pid, lc.reader_id
                   FROM order_ o
                            JOIN library_card lc ON o.library_card_id = lc.id
                   WHERE o.pickup_point_id IS NOT NULL
                   UNION
                   -- Readers who had books issued from this pickup point
                   SELECT pi.pickup_point_id AS pid, lc.reader_id
                   FROM publication_issuing pi
                            JOIN library_card lc ON pi.library_card_id = lc.id
                   WHERE pi.pickup_point_id IS NOT NULL) r_assoc ON pp.id = r_assoc.pid
               GROUP BY pp.id, pp.name_
               ORDER BY total_readers %(sort_direction)s;
               """,
            # 5) query (debtors)
            6: """
               SELECT dv.pickup_point_name,
                      COUNT(DISTINCT dv.id) AS total_overdue_readers
               FROM debtors_view dv
               WHERE dv.pickup_point_name IS NOT NULL
               GROUP BY dv.pickup_point_name
               ORDER BY total_overdue_readers %(sort_direction)s;
               """,
            # 5) query (biggest overdue sum)
            7: """
               SELECT pp.name_                         AS pickup_point_name,
                      SUM(COALESCE(p.price, 0::money)) AS total_overdue_value -- Summing price of overdue books as 'fine_amount' is not in schema
               FROM pickup_point pp
                        JOIN publication_issuing pi ON pp.id = pi.pickup_point_id
                        JOIN debtors_view dv ON pi.id = dv.issuing_id -- Ensures only currently overdue items
                        JOIN publication p ON pi.publication_id = p.id -- To get the price
               GROUP BY pp.name_
               ORDER BY total_overdue_value %(sort_direction)s;
               """,
            # 6) query
            8: """
               SELECT p.name_                                                                                                                       AS publication_name,
                      p.author,
                      COUNT(o.id)                                                                                                                   AS total_orders,
                      STRING_AGG(DISTINCT rfi.surname || ' ' || rfi.name_ || ' (Заказано: ' || TO_CHAR(o.ordering_date, 'YYYY-MM-DD') || ')', '; ') AS ordering_readers_list
               FROM order_ o
                        JOIN publication p ON o.publication_id = p.id
                        JOIN pickup_point pp ON o.pickup_point_id = pp.id
                        JOIN library_card lc ON o.library_card_id = lc.id
                        JOIN reader_full_info rfi ON lc.reader_id = rfi.id
               WHERE pp.subscription_type ILIKE %(interlibrary_loan_identifier)s
                 AND o.ordering_date >= (CURRENT_DATE - %(time_interval)s:: INTERVAL)
                 AND o.ordering_date <= CURRENT_DATE
               GROUP BY p.name_, p.author
               ORDER BY total_orders DESC, p.name_;
               """,
            # 7) query
            9: """
               WITH CopiesData AS (SELECT p.id                                 AS publication_id,
                                          p.name_                              AS publication_name,
                                          p.author,
                                          ppp.pickup_point_id,
                                          ppp.publication_limit,
                                          (SELECT COUNT(*)
                                           FROM publication_issuing pi_count
                                           WHERE pi_count.publication_id = p.id
                                             AND pi_count.pickup_point_id = ppp.pickup_point_id
                                             AND pi_count.return_date IS NULL) AS issued_count
                                   FROM publication p
                                            JOIN pickup_point_publication ppp ON p.id = ppp.publication_id
                                   WHERE (%(publication_name_filter)s IS NULL OR p.name_ ILIKE
                                          %(publication_name_filter)s)
                                     AND (%(author_filter)s IS NULL OR p.author ILIKE %(author_filter)s)
                                     AND (%(publication_id_filter)s IS NULL OR p.id = %(publication_id_filter)s))
               SELECT CASE
                          WHEN %(pickup_point_id_filter)s IS NOT NULL AND %(pickup_point_id_filter)s != -1 THEN COALESCE(pp.name_, 'Неизвестный пункт')
                          ELSE 'По всей библиотеке'
                          END                                                  AS scope,
                      cd.publication_name,
                      cd.author,
                      SUM(GREATEST(0, cd.publication_limit - cd.issued_count)) AS number_of_available_copies
               FROM CopiesData cd
                        LEFT JOIN pickup_point pp ON cd.pickup_point_id = pp.id
               WHERE (%(pickup_point_id_filter)s IS NULL OR %(pickup_point_id_filter)s = -1 OR
                      cd.pickup_point_id = %(pickup_point_id_filter)s)
               GROUP BY scope, cd.publication_name, cd.author,
                        (CASE
                             WHEN %(pickup_point_id_filter)s IS NOT NULL AND %(pickup_point_id_filter)s != -1 THEN pp.name_
                             ELSE NULL END)
               ORDER BY scope, cd.publication_name;
               """,
            # 8) query
            10: """
                SELECT COALESCE(rfi.department, 'N/A data')                                                      AS department,
                       COALESCE(rfi.faculty, 'N/A data')                                                         AS faculty,
                       COALESCE(rfi.course::text, 'N/A data')                                                    AS course,
                       COALESCE(rfi.group_, 'N/A data')                                                          AS group_identifier,
                       COALESCE(rfi.category, 'N/A data')                                                        AS reader_category,
                       COUNT(DISTINCT rfi.id)                                                                    AS total_restricted_readers,
                       STRING_AGG(DISTINCT rfi.surname || ' ' || rfi.name_ ||
                               ' (Нарушение от: ' || TO_CHAR(v.violation_date, 'YYYY-MM-DD') ||
                               ', Причина: ' || res.description_ || ')', '; ') AS restricted_readers_list
                FROM reader_full_info rfi
                         JOIN violation_instance vi ON rfi.id = vi.reader_id
                         JOIN violation v ON vi.violation_category_id = v.id
                         JOIN restriction res ON v.restriction_id = res.id
                WHERE res.description_ ILIKE %(restriction_deprived_pattern)s -- Parameterized
                  AND v.violation_date <= (CURRENT_DATE - INTERVAL '2 months')
                  AND (%(department_name)s IS NULL
                   OR rfi.department ILIKE %(department_name)s)
                  AND (%(faculty_name)s IS NULL
                   OR rfi.faculty ILIKE %(faculty_name)s)
                  AND (%(course_name)s IS NULL
                   OR rfi.course::text ILIKE %(course_name)s)
                  AND (%(group_name)s IS NULL
                   OR rfi.group_ ILIKE %(group_name)s)
                  AND (%(reader_category_name)s IS NULL
                   OR rfi.category ILIKE %(reader_category_name)s)
                GROUP BY
                    COALESCE (rfi.department, 'N/A data'),
                    COALESCE (rfi.faculty, 'N/A data'),
                    COALESCE (rfi.course::text, 'N/A data'),
                    COALESCE (rfi.group_, 'N/A data'),
                    COALESCE (rfi.category, 'N/A data')
                ORDER BY total_restricted_readers DESC, department, faculty, course, group_identifier, reader_category;
                """,
            # 9) query (new readers)
            11: """
                SELECT COALESCE(rfi.department, 'N/A data')                                                                                            AS department,
                       COALESCE(rfi.faculty, 'N/A data')                                                                                               AS faculty,
                       COALESCE(rfi.course::text, 'N/A data')                                                                                          AS course,
                       COALESCE(rfi.group_, 'N/A data')                                                                                                AS group_identifier,
                       COALESCE(rfi.category, 'N/A data')                                                                                              AS reader_category,
                       COUNT(DISTINCT rfi.id)                                                                                                          AS total_new_readers,
                       STRING_AGG(DISTINCT rfi.surname || ' ' || rfi.name_ || ' (Карта выдана: ' || TO_CHAR(lc.issue_date, 'YYYY-MM-DD') || ')', '; ') AS new_readers_list
                FROM reader_full_info rfi
                         JOIN library_card lc ON rfi.id = lc.reader_id
                WHERE lc.issue_date =
                      (SELECT MIN(lc_inner.issue_date) FROM library_card lc_inner WHERE lc_inner.reader_id = rfi.id)
                  AND lc.issue_date >= (CURRENT_DATE - %(time_interval)s::INTERVAL)
                  AND lc.issue_date <= CURRENT_DATE
                  AND (%(department_name)s IS NULL OR rfi.department ILIKE %(department_name)s)
                  AND (%(faculty_name)s IS NULL OR rfi.faculty ILIKE %(faculty_name)s)
                  AND (%(course_name)s IS NULL OR rfi.course::text ILIKE %(course_name)s)
                  AND (%(group_name)s IS NULL OR rfi.group_ ILIKE %(group_name)s)
                  AND (%(reader_category_name)s IS NULL OR rfi.category ILIKE %(reader_category_name)s)
                GROUP BY COALESCE(rfi.department, 'N/A data'),
                         COALESCE(rfi.faculty, 'N/A data'),
                         COALESCE(rfi.course::text, 'N/A data'),
                         COALESCE(rfi.group_, 'N/A data'),
                         COALESCE(rfi.category, 'N/A data')
                ORDER BY total_new_readers DESC, department, faculty, course, group_identifier, reader_category;
                """,
            # 9) query (departed)
            12: """
                WITH ReaderDepartureDates AS (SELECT r.id as reader_id_key, s.departure_date
                                              FROM reader r
                                                       JOIN student s ON r.id = s.id
                                              WHERE s.departure_date IS NOT NULL
                                              UNION ALL
                                              SELECT r.id as reader_id_key, t.departure_date
                                              FROM teacher t
                                                       JOIN teacher t ON r.id = t.id
                                              WHERE t.departure_date IS NOT NULL
                                              UNION ALL
                                              SELECT r.id as reader_id_key, ue.departure_date
                                              FROM reader r
                                                       JOIN university_employee ue ON r.id = ue.id
                                              WHERE ue.departure_date IS NOT NULL
                                              UNION ALL
                                              SELECT r.id as reader_id_key, sw.departure_date
                                              FROM reader r
                                                       JOIN science_worker sw ON r.id = sw.id
                                              WHERE sw.departure_date IS NOT NULL),
                     ActualDeparture AS (SELECT reader_id_key, MAX(departure_date) as departure_date
                                         FROM ReaderDepartureDates
                                         GROUP BY reader_id_key)
                SELECT COALESCE(rfi.department, 'N/A data')                                                                                         AS department,
                       COALESCE(rfi.faculty, 'N/A data')                                                                                            AS faculty,
                       COALESCE(rfi.course::text, 'N/A data')                                                                                       AS course,
                       COALESCE(rfi.group_, 'N/A data')                                                                                             AS group_identifier,
                       COALESCE(rfi.category, 'N/A data')                                                                                           AS reader_category,
                       COUNT(DISTINCT rfi.id)                                                                                                       AS total_departed_readers,
                       STRING_AGG(DISTINCT rfi.surname || ' ' || rfi.name_ || ' (Выбыл: ' || TO_CHAR(ad.departure_date, 'YYYY-MM-DD') || ')', '; ') AS departed_readers_list
                FROM reader_full_info rfi
                         JOIN ActualDeparture ad ON rfi.id = ad.reader_id_key
                WHERE ad.departure_date >= (CURRENT_DATE - %(time_interval)s::INTERVAL)
                  AND ad.departure_date <= CURRENT_DATE
                  AND (%(department_name)s IS NULL OR rfi.department ILIKE %(department_name)s)
                  AND (%(faculty_name)s IS NULL OR rfi.faculty ILIKE %(faculty_name)s)
                  AND (%(course_name)s IS NULL OR rfi.course::text ILIKE %(course_name)s)
                  AND (%(group_name)s IS NULL OR rfi.group_ ILIKE %(group_name)s)
                  AND (%(reader_category_name)s IS NULL OR rfi.category ILIKE %(reader_category_name)s)
                GROUP BY COALESCE(rfi.department, 'N/A data'),
                         COALESCE(rfi.faculty, 'N/A data'),
                         COALESCE(rfi.course::text, 'N/A data'),
                         COALESCE(rfi.group_, 'N/A data'),
                         COALESCE(rfi.category, 'N/A data')
                ORDER BY total_departed_readers DESC, department, faculty, course, group_identifier, reader_category;
                """,
            # 10) query (ordered)`
            13: """
                WITH ReaderOrders AS (SELECT p.name_  AS publication_name,
                                             p.author,
                                             o.ordering_date,
                                             o.order_type,
                                             pp.name_ AS ordered_at_pickup_point
                                      FROM order_ o
                                               JOIN publication p ON o.publication_id = p.id
                                               JOIN library_card lc ON o.library_card_id = lc.id
                                               LEFT JOIN pickup_point pp ON o.pickup_point_id = pp.id
                                      WHERE lc.reader_id = %(reader_id)s
                                        AND o.ordering_date >= (CURRENT_DATE - %(time_interval)s::INTERVAL)
                                        AND o.ordering_date <= CURRENT_DATE)
                SELECT COUNT(*)                AS total_ordered_books,
                       COALESCE(STRING_AGG(publication_name || ' авт. ' || COALESCE(author, 'N/A') ||
                                           ' (Заказано: ' ||
                                           TO_CHAR(ordering_date, 'YYYY-MM-DD') || -- Corrected from order_date
                                           ', Тип заказа: ' || COALESCE(order_type, 'N/A') || ')', '; '),
                                'Нет заказов') AS ordered_books_list
                FROM ReaderOrders;
                """,
            # 10) query (in possession)
            14: """
                WITH ReaderPossessions AS (SELECT p.name_  AS publication_name,
                                                  p.author,
                                                  pi.issuing_date,
                                                  pi.expiration_date,
                                                  pp.name_ AS issued_at_pickup_point,
                                                  (CASE
                                                       WHEN pi.expiration_date < CURRENT_DATE
                                                           THEN (CURRENT_DATE - pi.expiration_date)
                                                       ELSE 0
                                                      END) as days_overdue
                                           FROM publication_issuing pi
                                                    JOIN library_card lc ON pi.library_card_id = lc.id
                                                    JOIN publication p ON pi.publication_id = p.id
                                                    LEFT JOIN pickup_point pp ON pi.pickup_point_id = pp.id
                                           WHERE lc.reader_id = %(reader_id)s
                                             AND pi.return_date IS NULL)
                SELECT COUNT(*)                      AS total_books_on_hand,
                       COALESCE(STRING_AGG(publication_name || ' авт. ' || COALESCE(author, 'N/A') ||
                                           ' (Срок: ' || TO_CHAR(expiration_date, 'YYYY-MM-DD') ||
                                           CASE
                                               WHEN days_overdue > 0 THEN ', Просрочено: ' || days_overdue || ' дней'
                                               ELSE '' END || ')', '; '),
                                'Нет книг на руках') AS books_on_hand_list
                FROM ReaderPossessions;
                """,
            # 11) query
            15: """
                SELECT pp.name_                                               AS subscriber_point_name,
                       p.name_                                                AS publication_name,
                       p.author,
                       SUM(GREATEST(0, COALESCE(ppp.publication_limit, 0) -
                                       (SELECT COUNT(*)
                                        FROM publication_issuing pi_inner
                                        WHERE pi_inner.publication_id = p.id
                                          AND pi_inner.pickup_point_id = pp.id
                                          AND pi_inner.return_date IS NULL))) AS available_copies
                FROM publication p
                         JOIN pickup_point_publication ppp ON p.id = ppp.publication_id
                         JOIN pickup_point pp ON ppp.pickup_point_id = pp.id
                WHERE (%(publication_name_filter)s IS NULL OR p.name_ ILIKE %(publication_name_filter)s)
                  AND (%(author_filter)s IS NULL OR p.author ILIKE %(author_filter)s)
                  AND (%(publication_id_filter)s IS NULL OR p.id = %(publication_id_filter)s)
                  AND pp.subscription_type ILIKE %(subscription_type_pattern)s -- Parameterized
                GROUP BY pp.id, pp.name_, p.id, p.name_, p.author
                HAVING SUM (GREATEST(0
                     , COALESCE (ppp.publication_limit
                     , 0) -
                     (SELECT COUNT (*)
                    FROM publication_issuing pi_inner
                    WHERE pi_inner.publication_id = p.id
                   AND pi_inner.pickup_point_id = pp.id
                   AND pi_inner.return_date IS NULL)))
                     > 0
                ORDER BY subscriber_point_name, publication_name;
                """,
            # 12) query
            16: """
                WITH BookHolders AS (SELECT rfi.id   AS reader_id,
                                            rfi.surname,
                                            rfi.name_,
                                            rfi.middle_name,
                                            p.name_  AS publication_name,
                                            p.author AS publication_author,
                                            pi.expiration_date,
                                            pp.name_ AS issued_from_pickup_point
                                     FROM publication_issuing pi
                                              JOIN library_card lc ON pi.library_card_id = lc.id
                                              JOIN reader_full_info rfi ON lc.reader_id = rfi.id
                                              JOIN publication p ON pi.publication_id = p.id
                                              LEFT JOIN pickup_point pp ON pi.pickup_point_id = pp.id
                                     WHERE pi.return_date IS NULL
                                       AND (
                                         (%(publication_name_filter)s IS NOT NULL AND p.name_ ILIKE
                                          %(publication_name_filter)s) OR
                                         (%(publication_id_filter)s IS NOT NULL AND p.id = %(publication_id_filter)s)
                                         )
                                       AND (%(author_filter)s IS NULL OR p.author ILIKE %(author_filter)s))
                SELECT (SELECT COUNT(*) FROM BookHolders)            AS total_holders,
                       COALESCE(
                               STRING_AGG(DISTINCT surname || ' ' || name_ || COALESCE(' ' || middle_name, '') || ' (Срок: ' || TO_CHAR(expiration_date, 'YYYY-MM-DD') || ')', '; '),
                               'Нет читателей с этой книгой')        AS readers_with_book_list,
                       COALESCE(
                               (SELECT bh_inner.surname || ' ' || bh_inner.name_ ||
                                       COALESCE(' ' || bh_inner.middle_name, '') ||
                                       ' (Срок: ' || TO_CHAR(bh_inner.expiration_date, 'YYYY-MM-DD') || ')'
                                FROM BookHolders bh_inner
                                ORDER BY bh_inner.expiration_date ASC
                               LIMIT 1),
                        'N/A') AS earliest_returner
                FROM BookHolders
                GROUP BY CASE WHEN (SELECT COUNT(*) FROM BookHolders) > 0 THEN 1 ELSE NULL END;
                """,
            # 13) query
            17: """
                WITH ReaderDetails AS (SELECT rfi.id                                                AS reader_id,
                                              rfi.surname,
                                              rfi.name_,
                                              rfi.middle_name,
                                              rfi.category,
                                              rfi.reader_type,
                                              rfi.department,
                                              rfi.faculty,
                                              rfi.course,
                                              rfi.group_,
                                              (SELECT MIN(lc_reg.issue_date)
                                               FROM library_card lc_reg
                                               WHERE lc_reg.reader_id = rfi.id)                     AS registration_date,
                                              (SELECT MAX(ad_inner.departure_date)
                                               FROM (SELECT s.id as reader_id_key, s.departure_date
                                                     FROM student s
                                                     WHERE s.id = rfi.id
                                                       AND s.departure_date IS NOT NULL
                                                     UNION ALL
                                                     SELECT t.id, t.departure_date
                                                     FROM teacher t
                                                     WHERE t.id = rfi.id
                                                       AND t.departure_date IS NOT NULL
                                                     UNION ALL
                                                     SELECT ue.id, ue.departure_date
                                                     FROM university_employee ue
                                                     WHERE ue.id = rfi.id
                                                       AND ue.departure_date IS NOT NULL
                                                     UNION ALL
                                                     SELECT sw.id, sw.departure_date
                                                     FROM science_worker sw
                                                     WHERE sw.id = rfi.id
                                                       AND sw.departure_date IS NOT NULL) ad_inner) AS departure_date,
                                              lc.id                                                 AS library_card_id,
                                              lc.issue_date                                         AS lc_issue_date,
                                              lc.expiration_date                                    AS lc_expiration_date,
                                              lcs.info                                              AS lc_status
                                       FROM reader_full_info rfi
                                                LEFT JOIN library_card lc ON rfi.id = lc.reader_id AND lc.issue_date =
                                                                                                       (SELECT MAX(lc_max.issue_date)
                                                                                                        FROM library_card lc_max
                                                                                                        WHERE lc_max.reader_id = rfi.id)
                                                LEFT JOIN library_card_status lcs ON lc.status_id = lcs.id
                                       WHERE rfi.surname
                    ILIKE %(surname_filter)s
                    AND (%(name_filter)s IS NULL OR rfi.name_ ILIKE %(name_filter)s)
                    AND (%(middle_name_filter)s IS NULL OR rfi.middle_name ILIKE %(middle_name_filter)s)
                    )
                   , ReaderViolations AS (
                SELECT
                    vi.reader_id, v.name_ AS violation_name, v.violation_date AS violation_event_date, res.description_ AS restriction_imposed
                FROM violation_instance vi
                    JOIN violation v
                ON vi.violation_category_id = v.id
                    LEFT JOIN restriction res ON v.restriction_id = res.id
                WHERE vi.reader_id IN (SELECT reader_id FROM ReaderDetails)
                    )
                    , LostBookViolations AS (
                SELECT
                    vi_lost.reader_id, v_lost.name_ AS lost_book_violation_name, v_lost.violation_date AS date_marked_lost
                FROM violation_instance vi_lost
                    JOIN violation v_lost
                ON vi_lost.violation_category_id = v_lost.id
                WHERE vi_lost.reader_id IN (SELECT reader_id FROM ReaderDetails)
                  AND v_lost.name_ ILIKE %(lost_book_pattern)s -- Parameterized
                    )
                SELECT rd.reader_id,
                       rd.surname,
                       rd.name_,
                       rd.middle_name,
                       rd.category                                                   AS reader_base_category,
                       rd.reader_type,
                       rd.department,
                       rd.faculty,
                       rd.course,
                       rd.group_,
                       TO_CHAR(rd.registration_date, 'YYYY-MM-DD')                   AS registration_date,
                       TO_CHAR(rd.departure_date, 'YYYY-MM-DD')                      AS departure_date,
                       rd.library_card_id,
                       TO_CHAR(rd.lc_issue_date, 'YYYY-MM-DD')                       AS lc_issue_date,
                       TO_CHAR(rd.lc_expiration_date, 'YYYY-MM-DD')                  AS lc_expiration_date,
                       rd.lc_status,
                       (SELECT COUNT(*)
                        FROM ReaderViolations rvio
                        WHERE rvio.reader_id = rd.reader_id)                         AS total_violations,
                       COALESCE(
                               (SELECT STRING_AGG(DISTINCT rvio.violation_name ||
                                           ' (Дата: ' || TO_CHAR(rvio.violation_event_date, 'YYYY-MM-DD') ||
                                           COALESCE(', Ограничение: ' || rvio.restriction_imposed, '') || ')', '; ')
                                FROM ReaderViolations rvio
                                WHERE rvio.reader_id = rd.reader_id),
                               'Нет нарушений')                                      AS violations_list,
                       COALESCE(
                               (SELECT STRING_AGG(DISTINCT lb.lost_book_violation_name ||
                                           ' (Зафиксировано: ' || TO_CHAR(lb.date_marked_lost, 'YYYY-MM-DD') || ')', '; ')
                                FROM LostBookViolations lb
                                WHERE lb.reader_id = rd.reader_id),
                               'Нет зафиксированных утерь книг (по типу нарушения)') AS lost_books_violations_list
                FROM ReaderDetails rd;
                """
        }

        self.query_params_config = {
            0: {
                "description": "1. Список и общее число читателей",
                "params": [
                    {'name': 'pickup_point_id', 'type': 'int', 'label': 'ID Пункта выдачи (пусто=вся библиотека)',
                     'optional': True, 'prompt': 'ID Пункта выдачи (пусто=вся библиотека)'},
                    {'name': 'department_name', 'type': 'text', 'label': 'Кафедра (часть названия, пусто=все)',
                     'optional': True, 'prompt': 'Кафедра (часть названия, пусто=все)'},
                    {'name': 'faculty_name', 'type': 'text', 'label': 'Факультет (часть названия, пусто=все)',
                     'optional': True, 'prompt': 'Факультет (часть названия, пусто=все)'},
                    {'name': 'course_name', 'type': 'text', 'label': 'Курс (напр. 1, 2, пусто=все)', 'optional': True,
                     'prompt': 'Курс (напр. 1, 2, пусто=все)'},
                    {'name': 'group_name', 'type': 'text', 'label': 'Группа (название, пусто=все)', 'optional': True,
                     'prompt': 'Группа (название, пусто=все)'}
                ]
            },
            1: {
                "description": "2. Читатели-задолжники",
                "params": [
                    {'name': 'pickup_point_id', 'type': 'int', 'label': 'ID Пункта выдачи (пусто=вся библиотека)',
                     'optional': True, 'prompt': 'ID Пункта выдачи (пусто=вся библиотека)'},
                    {'name': 'min_days_overdue', 'type': 'int', 'label': 'Дней просрочки > (пусто=все задолжники)',
                     'optional': True, 'prompt': 'Дней просрочки > (пусто=все задолжники)'},
                    {'name': 'department_name', 'type': 'text', 'label': 'Кафедра (пусто=все)', 'optional': True,
                     'prompt': 'Кафедра (пусто=все)'},
                    {'name': 'faculty_name', 'type': 'text', 'label': 'Факультет (пусто=все)', 'optional': True,
                     'prompt': 'Факультет (пусто=все)'},
                    {'name': 'course_name', 'type': 'text', 'label': 'Курс (пусто=все)', 'optional': True,
                     'prompt': 'Курс (пусто=все)'},
                    {'name': 'group_name', 'type': 'text', 'label': 'Группа (пусто=все)', 'optional': True,
                     'prompt': 'Группа (пусто=все)'},
                    {'name': 'reader_category_name', 'type': 'text', 'label': 'Категория читателя (пусто=все)',
                     'optional': True, 'prompt': 'Категория читателя (пусто=все)'}
                ]
            },
            2: {
                "description": "3. Топ-20 наиболее часто заказываемых книг",
                "params": [
                    {'name': 'pickup_point_id', 'type': 'int', 'label': 'ID Читального зала (заказа) (пусто=все)',
                     'optional': True, 'prompt': 'ID Читального зала (заказа) (пусто=все)'},
                    {'name': 'faculty_name', 'type': 'text', 'label': 'Факультет читателя (пусто=все вуза)',
                     'optional': True, 'prompt': 'Факультет читателя (пусто=все вуза)'}
                ]
            },
            3: {
                "description": "4а. Книги, поступившие за последний год",
                "params": [
                    {'name': 'pickup_point_id', 'type': 'int',
                     'label': 'ID Пункта выдачи (поступления) (пусто=вся библиотека)', 'optional': True,
                     'prompt': 'ID Пункта выдачи (поступления) (пусто=вся библиотека)'},
                    {'name': 'author_name', 'type': 'text', 'label': 'Автор (часть имени, пусто=все)', 'optional': True,
                     'prompt': 'Автор (часть имени, пусто=все)'},
                    {'name': 'publication_year_filter', 'type': 'int', 'label': 'Год издания книги (YYYY, пусто=все)',
                     'optional': True, 'prompt': 'Год издания книги (YYYY, пусто=все)'},
                    {'name': 'receipt_year_filter', 'type': 'int', 'label': 'Год поступления (YYYY, пусто=все)',
                     'optional': True, 'prompt': 'Год поступления (YYYY, пусто=все)'},
                    {'name': 'event_type_reception', 'type': 'text', 'default': 'Поступление', 'optional': True,
                     'internal': True, 'prompt': 'Тип события для поступления'}
                ]
            },
            4: {
                "description": "4б. Книги, утерянные за последний год",
                "params": [
                    {'name': 'pickup_point_id', 'type': 'int',
                     'label': 'ID Пункта выдачи (где числилась) (пусто=вся библиотека)', 'optional': True,
                     'prompt': 'ID Пункта выдачи (где числилась) (пусто=вся библиотека)'},
                    {'name': 'author_name', 'type': 'text', 'label': 'Автор (пусто=все)', 'optional': True,
                     'prompt': 'Автор (пусто=все)'},
                    {'name': 'publication_year_filter', 'type': 'int', 'label': 'Год издания книги (YYYY, пусто=все)',
                     'optional': True, 'prompt': 'Год издания книги (YYYY, пусто=все)'},
                    {'name': 'lost_year_filter', 'type': 'int', 'label': 'Год утери (YYYY, пусто=все)',
                     'optional': True, 'prompt': 'Год утери (YYYY, пусто=все)'},
                    {'name': 'event_type_lost', 'type': 'text', 'default': 'Утеряна', 'optional': True,
                     'internal': True, 'prompt': 'Тип события для утери'}
                ]
            },
            5: {
                "description": "5а. Пункты выдачи по числу читателей",
                "params": [
                    {'name': 'sort_direction', 'type': 'select', 'label': 'Сортировка', 'default': 'DESC',
                     'optional': False, 'prompt': 'Сортировка',
                     'options': [{'value': 'DESC', 'label': 'По убыванию (сначала наибольшее)'},
                                 {'value': 'ASC', 'label': 'По возрастанию (сначала наименьшее)'}]}
                ]
            },
            6: {
                "description": "5б. Пункты выдачи по числу читателей-задолжников",
                "params": [
                    {'name': 'sort_direction', 'type': 'select', 'label': 'Сортировка', 'default': 'DESC',
                     'optional': False, 'prompt': 'Сортировка',
                     'options': [{'value': 'DESC', 'label': 'По убыванию'},
                                 {'value': 'ASC', 'label': 'По возрастанию'}]}
                ]
            },
            7: {
                "description": "5в. Пункты выдачи по сумме задолженности (стоимости книг)",
                "params": [
                    {'name': 'sort_direction', 'type': 'select', 'label': 'Сортировка', 'default': 'DESC',
                     'optional': False, 'prompt': 'Сортировка',
                     'options': [{'value': 'DESC', 'label': 'По убыванию'},
                                 {'value': 'ASC', 'label': 'По возрастанию'}]}
                ]
            },
            8: {
                "description": "6. Книги, заказанные по МБА",
                "params": [
                    {'name': 'interlibrary_loan_identifier', 'type': 'text',
                     'label': 'Идентификатор МБА в типе пункта (напр. %МБА%)', 'default': '%МБА%', 'optional': False,
                     'prompt': 'Идентификатор МБА в типе пункта (напр. %МБА%)'},
                    {'name': 'time_interval', 'type': 'select', 'label': 'Период', 'default': '1 month',
                     'optional': False, 'prompt': 'Период',
                     'options': [{'value': '1 month', 'label': 'За последний месяц'},
                                 {'value': '3 months', 'label': 'За последний квартал'},
                                 {'value': '1 year', 'label': 'За последний год'}]}
                ]
            },
            9: {
                "description": "7. Количество экземпляров книги (доступных)",
                "params": [
                    {'name': 'publication_name_filter', 'type': 'text',
                     'label': 'Название книги (часть, пусто если ID указан)', 'optional': True,
                     'prompt': 'Название книги (часть, пусто если ID указан)'},
                    {'name': 'author_filter', 'type': 'text', 'label': 'Автор (часть, пусто=любой)', 'optional': True,
                     'prompt': 'Автор (часть, пусто=любой)'},
                    {'name': 'publication_id_filter', 'type': 'int', 'label': 'ID Книги (точно, пусто если название)',
                     'optional': True, 'prompt': 'ID Книги (точно, пусто если название)'},
                    {'name': 'pickup_point_id_filter', 'type': 'int',
                     'label': 'ID Пункта выдачи (пусто=не фильтровать, -1=по всей библиотеке)', 'optional': True,
                     'default': -1, 'prompt': 'ID Пункта выдачи (пусто=не фильтровать, -1=по всей библиотеке)'}
                ]
            },
            10: {
                "description": "8. Читатели, лишенные права пользования (> 2 мес. с даты нарушения)",
                "params": [
                    {'name': 'department_name', 'type': 'text', 'label': 'Кафедра (пусто=все)', 'optional': True,
                     'prompt': 'Кафедра (пусто=все)'},
                    {'name': 'faculty_name', 'type': 'text', 'label': 'Факультет (пусто=все)', 'optional': True,
                     'prompt': 'Факультет (пусто=все)'},
                    {'name': 'course_name', 'type': 'text', 'label': 'Курс (пусто=все)', 'optional': True,
                     'prompt': 'Курс (пусто=все)'},
                    {'name': 'group_name', 'type': 'text', 'label': 'Группа (пусто=все)', 'optional': True,
                     'prompt': 'Группа (пусто=все)'},
                    {'name': 'reader_category_name', 'type': 'text', 'label': 'Категория читателя (пусто=все)',
                     'optional': True, 'prompt': 'Категория читателя (пусто=все)'},
                    {'name': 'restriction_deprived_pattern', 'type': 'text', 'default': '%лишен%прав%пользования%',
                     'optional': True, 'internal': True, 'prompt': 'Шаблон ограничения (лишение прав)'}
                ]
            },
            11: {
                "description": "9а. Новые читатели (по дате выдачи первой карты)",
                "params": [
                    {'name': 'time_interval', 'type': 'select', 'label': 'Период', 'default': '1 month',
                     'optional': False, 'prompt': 'Период',
                     'options': [{'value': '1 month', 'label': 'За последний месяц'},
                                 {'value': '3 months', 'label': 'За квартал'}, {'value': '1 year', 'label': 'За год'}]},
                    {'name': 'pickup_point_id', 'type': 'int',
                     'label': 'ID Пункта выдачи (не используется для фильтрации читателей)', 'optional': True,
                     'disabled': True, 'prompt': 'ID Пункта выдачи (не используется для фильтрации читателей)'},
                    {'name': 'department_name', 'type': 'text', 'label': 'Кафедра (пусто=все)', 'optional': True,
                     'prompt': 'Кафедра (пусто=все)'},
                    {'name': 'faculty_name', 'type': 'text', 'label': 'Факультет (пусто=все)', 'optional': True,
                     'prompt': 'Факультет (пусто=все)'},
                    {'name': 'course_name', 'type': 'text', 'label': 'Курс (пусто=все)', 'optional': True,
                     'prompt': 'Курс (пусто=все)'},
                    {'name': 'group_name', 'type': 'text', 'label': 'Группа (пусто=все)', 'optional': True,
                     'prompt': 'Группа (пусто=все)'},
                    {'name': 'reader_category_name', 'type': 'text', 'label': 'Категория читателя (пусто=все)',
                     'optional': True, 'prompt': 'Категория читателя (пусто=все)'}
                ]
            },
            12: {
                "description": "9б. Выбывшие читатели (по дате убытия)",
                "params": [
                    {'name': 'time_interval', 'type': 'select', 'label': 'Период', 'default': '1 month',
                     'optional': False, 'prompt': 'Период',
                     'options': [{'value': '1 month', 'label': 'За последний месяц'},
                                 {'value': '3 months', 'label': 'За квартал'}, {'value': '1 year', 'label': 'За год'}]},
                    {'name': 'pickup_point_id', 'type': 'int',
                     'label': 'ID Пункта выдачи (не используется для фильтрации читателей)', 'optional': True,
                     'disabled': True, 'prompt': 'ID Пункта выдачи (не используется для фильтрации читателей)'},
                    {'name': 'department_name', 'type': 'text', 'label': 'Кафедра (пусто=все)', 'optional': True,
                     'prompt': 'Кафедра (пусто=все)'},
                    {'name': 'faculty_name', 'type': 'text', 'label': 'Факультет (пусто=все)', 'optional': True,
                     'prompt': 'Факультет (пусто=все)'},
                    {'name': 'course_name', 'type': 'text', 'label': 'Курс (пусто=все)', 'optional': True,
                     'prompt': 'Курс (пусто=все)'},
                    {'name': 'group_name', 'type': 'text', 'label': 'Группа (пусто=все)', 'optional': True,
                     'prompt': 'Группа (пусто=все)'},
                    {'name': 'reader_category_name', 'type': 'text', 'label': 'Категория читателя (пусто=все)',
                     'optional': True, 'prompt': 'Категория читателя (пусто=все)'}
                ]
            },
            13: {
                "description": "10а. Книги, заказанные читателем",
                "params": [
                    {'name': 'reader_id', 'type': 'int', 'label': 'ID Читателя', 'optional': False,
                     'prompt': 'ID Читателя'},
                    {'name': 'time_interval', 'type': 'select', 'label': 'Период', 'default': '1 month',
                     'optional': False, 'prompt': 'Период',
                     'options': [{'value': '1 month', 'label': 'За последний месяц'},
                                 {'value': '3 months', 'label': 'За квартал'}, {'value': '1 year', 'label': 'За год'}]}
                ]
            },
            14: {
                "description": "10б. Книги на руках у читателя",
                "params": [
                    {'name': 'reader_id', 'type': 'int', 'label': 'ID Читателя', 'optional': False,
                     'prompt': 'ID Читателя'}
                ]
            },
            15: {
                "description": "11. Наличие книги на абонементах (доступные экз.)",
                "params": [
                    {'name': 'publication_name_filter', 'type': 'text',
                     'label': 'Название книги (часть, пусто если ID)', 'optional': True,
                     'prompt': 'Название книги (часть, пусто если ID)'},
                    {'name': 'author_filter', 'type': 'text', 'label': 'Автор (часть, пусто=любой)', 'optional': True,
                     'prompt': 'Автор (часть, пусто=любой)'},
                    {'name': 'publication_id_filter', 'type': 'int', 'label': 'ID Книги (точно, пусто если название)',
                     'optional': True, 'prompt': 'ID Книги (точно, пусто если название)'},
                    {'name': 'subscription_type_pattern', 'type': 'text', 'default': '%абонемент%', 'optional': True,
                     'internal': True, 'prompt': 'Шаблон типа пункта (абонемент)'}
                ]
            },
            16: {
                "description": "12. Кто имеет книгу и кто сдаст раньше всех",
                "params": [
                    {'name': 'publication_name_filter', 'type': 'text', 'label': 'Название книги (часть)',
                     'optional': 'conditional', 'prompt': 'Название книги (часть)'},
                    {'name': 'author_filter', 'type': 'text', 'label': 'Автор (часть, пусто=любой)', 'optional': True,
                     'prompt': 'Автор (часть, пусто=любой)'},
                    {'name': 'publication_id_filter', 'type': 'int', 'label': 'ID Книги (точно)',
                     'optional': 'conditional', 'prompt': 'ID Книги (точно)'}
                ]
            },
            17: {
                "description": "13. Полная информация о читателе по фамилии",
                "params": [
                    {'name': 'surname_filter', 'type': 'text', 'label': 'Фамилия читателя', 'optional': False,
                     'prompt': 'Фамилия читателя'},
                    {'name': 'name_filter', 'type': 'text', 'label': 'Имя (для уточнения, пусто=любое)',
                     'optional': True, 'prompt': 'Имя (для уточнения, пусто=любое)'},
                    {'name': 'middle_name_filter', 'type': 'text', 'label': 'Отчество (для уточнения, пусто=любое)',
                     'optional': True, 'prompt': 'Отчество (для уточнения, пусто=любое)'},
                    {'name': 'lost_book_pattern', 'type': 'text', 'default': '%Утеря книги%', 'optional': True,
                     'internal': True, 'prompt': 'Шаблон нарушения (утеря книги)'}
                ]
            }
        }

    def _connect_db(self, user, password, host, port, database):
        try:
            self.connection = psycopg2.connect(
                user=user,
                password=password,
                host=host,
                port=port,
                database=database
            )
            self.cursor = self.connection.cursor(cursor_factory=RealDictCursor)
            self.current_user = user
            self.current_password = password
            self.db_host = host
            self.db_port = port
            self.db_name = database
            print(f"Successfully connected to database '{database}' as user '{user}'")
            return True
        except (Exception, Error) as error:
            print(f"Error while connecting to PostgreSQL: {error}")
            self.connection = None
            self.cursor = None
            return False

    def authenticate_user(self, username, password, host="localhost", port="5432", database="university_library"):
        if self.connection and not self.connection.closed:
            print("Already connected. Disconnecting before re-authenticating.")
            self.disconnect()
        return self._connect_db(username, password, host, port, database)

    def disconnect(self):
        if self.connection:
            if self.cursor:
                self.cursor.close()
            self.connection.close()
            print("Database connection closed.")
            self.connection = None
            self.cursor = None
            self.current_user = None
            self.current_password = None

    def close_connection(self):
        self.disconnect()

    def execute_query(self, query_sql, params=None):
        if not self.connection or self.connection.closed:
            print("Not connected to the database or connection closed.")
            if self.current_user and self.current_password:
                print("Attempting to reconnect...")
                if not self._connect_db(self.current_user, self.current_password, self.db_host, self.db_port,
                                        self.db_name):
                    return "Failed to reconnect to the database.", None
            else:
                return "Cannot execute query: No active connection and no credentials to reconnect.", None
        try:
            if params is not None and not isinstance(params, dict):
                print(
                    f"Warning: params is not a dict: {type(params)}. Query might fail if it expects named parameters.")

            self.cursor.execute(query_sql, params)
            if self.cursor.description:
                colnames = [desc[0] for desc in self.cursor.description]
                records = self.cursor.fetchall()
                return records, colnames
            else:
                self.connection.commit()
                return f"Query executed successfully. Rows affected: {self.cursor.rowcount}", None
        except (Exception, Error) as error:
            print(f"Error executing query: {error}")
            if self.connection and not self.connection.closed:
                try:
                    self.connection.rollback()
                except Exception as rb_error:
                    print(f"Error during rollback: {rb_error}")
            return f"Error: {str(error)}", None

    def get_tables(self):
        """Retrieves a list of table names from the public schema."""
        query = """
                SELECT table_name
                FROM information_schema.tables
                WHERE table_schema = 'public' \
                  AND table_type = 'BASE TABLE'
                ORDER BY table_name;
                """
        records, _ = self.execute_query(query)
        if isinstance(records, str):
            print(f"Error getting tables: {records}")
            return []
        return [row['table_name'] for row in records] if records else []

    def get_table_columns(self, table_name):
        """Retrieves column names, data types, and nullability for a given table."""
        query = """
                SELECT column_name, data_type, is_nullable
                FROM information_schema.columns
                WHERE table_schema = 'public' \
                  AND table_name = %(table_name)s
                ORDER BY ordinal_position;
                """
        records, _ = self.execute_query(query, {'table_name': table_name})
        if isinstance(records, str):
            print(f"Error getting columns for table {table_name}: {records}")
            return []
        return [(row['column_name'], row['data_type'], row['is_nullable']) for row in records] if records else []

    def insert_record(self, table_name, data_dict):
        if not data_dict:
            return "No data provided for insertion.", None
        columns = ', '.join([f'"{col}"' for col in data_dict.keys()])
        placeholders = ', '.join([f"%({key})s" for key in data_dict.keys()])
        query = f'INSERT INTO "{table_name}" ({columns}) VALUES ({placeholders})'
        return self.execute_query(query, data_dict)

    def update_record(self, table_name, data_dict, where_clause_template, where_params_dict):
        if not data_dict:
            return "No data provided for update.", None
        set_clauses = ', '.join([f'"{key}" = %({key})s' for key in data_dict.keys()])
        all_params = {**data_dict, **where_params_dict}
        query = f'UPDATE "{table_name}" SET {set_clauses}'
        if where_clause_template:
            query += f" WHERE {where_clause_template}"
        return self.execute_query(query, all_params)

    def get_ready_queries(self):
        """Returns a list of descriptions for predefined queries."""
        return [config['description'] for q_id, config in sorted(self.query_params_config.items())]

    def get_query_params_config_by_description(self, query_description_target):
        """
        Retrieves parameter configuration for a query based on its exact description string.
        Returns only the list of parameters.
        """
        for q_id, config in self.query_params_config.items():
            if config['description'] == query_description_target:
                return config.get('params', [])
        print(f"No param config found for query description '{query_description_target}'")
        return []

    def get_ready_queries_results(self, query_description_target, user_params_dict):
        """Executes a predefined query by its description and supplied parameter values."""
        query_id_to_run = None
        for q_id, config_item in self.query_params_config.items():
            if config_item['description'] == query_description_target:
                query_id_to_run = q_id
                break

        if query_id_to_run is None:
            return [(f"Описание запроса '{query_description_target}' не найдено.",)], []
        if query_id_to_run not in self.queries:
            return [(f"Для запроса ID {query_id_to_run} ('{query_description_target}') отсутствует SQL.",)], []

        query_sql = self.queries[query_id_to_run]

        if 'sort_direction' in user_params_dict:
            direction = user_params_dict['sort_direction']
            if direction in ['ASC', 'DESC']:
                query_sql = query_sql.replace('%(sort_direction)s', direction)
            else:
                return [(f"Неверное значение для 'sort_direction': {direction}. Должно быть 'ASC' или 'DESC'.",)], []

        final_params = {}
        current_query_config = self.query_params_config.get(query_id_to_run, {})
        defined_params_for_query = current_query_config.get('params', [])

        for param_def in defined_params_for_query:
            param_name = param_def['name']

            if param_name == 'sort_direction':
                continue

            value_to_use = None
            if param_name in user_params_dict and user_params_dict[param_name] is not None:
                value_to_use = user_params_dict[param_name]
            elif 'default' in param_def:
                value_to_use = param_def['default']

            final_params[param_name] = value_to_use  # Temporarily store, validate types later

        # Second pass: Validate required and conditional parameters
        for param_def in defined_params_for_query:
            param_name = param_def['name']
            param_label = param_def.get('label', param_name)
            param_optional_type = param_def.get('optional', False)  # Can be True, False, or 'conditional'

            if param_name == 'sort_direction':
                continue  # Already handled

            current_value = final_params.get(param_name)

            if current_value is None:
                if param_optional_type is False:  # Strictly required parameter
                    return [(f"Обязательный параметр '{param_label}' ({param_name}) не был предоставлен.",)], []
                elif param_optional_type == 'conditional':
                    # Specific handling for query 16: at least one of publication_name_filter or publication_id_filter
                    if query_id_to_run == 16:
                        pub_name_filter_provided = final_params.get('publication_name_filter') is not None
                        pub_id_filter_provided = final_params.get('publication_id_filter') is not None

                        if not (pub_name_filter_provided or pub_id_filter_provided):
                            return [
                                (f"Для запроса '{query_description_target}' необходимо указать либо 'Название книги', либо 'ID Книги'.",)], []

            if current_value is not None:
                if param_def.get('type') == 'int':
                    try:
                        final_params[param_name] = int(current_value)
                    except ValueError:
                        return [
                            (f"Неверное целочисленное значение для параметра '{param_label}' ({param_name}): {current_value}",)], []

        for key, value in user_params_dict.items():
            if key not in final_params and key != 'sort_direction':
                final_params[key] = value

        try:
            print(
                f"Executing Query ID {query_id_to_run} ('{query_description_target}') with SQL: {query_sql} and PARAMS: {final_params}")
            records, colnames = self.execute_query(query_sql, final_params)

            if isinstance(records, str) and colnames is None:
                return [(records,)], []
            if records is None and colnames is None:
                return [("Не удалось выполнить запрос или подключиться к базе данных (неожиданная ошибка).",)], []
            return records, colnames
        except Exception as e:
            print(f"Error processing or executing ready query (ID {query_id_to_run}): {e}")
            import traceback
            traceback.print_exc()
            return [(f"Ошибка выполнения запроса: {str(e)}",)], []
