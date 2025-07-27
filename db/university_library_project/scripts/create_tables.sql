-- psql -U admin -d university_library

create table IF NOT EXISTS
    pickup_point
(
    id                smallserial primary key,
    subscription_type char(40),
    number_of_seats   smallint check (number_of_seats > 0),
    status_           char(200) not null DEFAULT 'Inactive',
    name_             varchar(350)
);


create table IF NOT EXISTS
    event_type
(
    id    smallserial primary key,
    name_ char(300) not null unique
);

create table IF NOT EXISTS
    restriction
(
    id           smallserial primary key,
    description_ char(500) not null unique
);

create table IF NOT EXISTS
    library_card_status
(
    id   smallserial primary key,
    info varchar(100) not null unique
);

create table IF NOT EXISTS
    violation
(
    id             smallserial primary key,
    name_          char(350) not null,
    violation_date date,
    restriction_id smallint  references restriction (id) on update cascade on delete set null
);

create table IF NOT EXISTS
    publication_status
(
    id   smallserial primary key,
    info varchar(100) not null unique
);

create table IF NOT EXISTS
    publication
(
    id                bigserial primary key,
    name_             varchar(500) not null,
    author            varchar(400),
    publishing_date   date,
    publishing_office varchar(400),
    price             money        not null check (price > 0.00::money),
    status_id         smallint     references publication_status (id) on update cascade on delete set null
);

create table IF NOT EXISTS publication_event
(
    id              bigserial primary key,
    publication_id  bigint   not null references publication (id) on update cascade on delete cascade,
    event_type_id   smallint not null references event_type (id) on update cascade on delete set null,
    event_date      date,
    pickup_point_id smallint references pickup_point (id) on update cascade on delete set null
);

create table IF NOT EXISTS pickup_point_publication
(
    pickup_point_id   smallint not null references pickup_point (id) on update cascade on delete cascade,
    publication_id    bigint   not null references publication (id) on update cascade on delete cascade,
    publication_limit smallint check (publication_limit > 0),
    primary key (pickup_point_id, publication_id)
);

create table IF NOT EXISTS
    reader
(
    id          bigserial primary key,
    category    char(50)     not null,
    surname     varchar(100) not null,
    name_       varchar(100) not null,
    middle_name varchar(100) not null
);

create table IF NOT EXISTS
    applicant
(
    id      serial primary key references reader (id) on update cascade on delete cascade,
    faculty char(250) not null
);

create table IF NOT EXISTS
    university_employee
(
    id             serial primary key references reader (id) on update cascade on delete cascade,
    joining_date   date not null,
    departure_date date
);

create table IF NOT EXISTS
    student
(
    id             bigserial primary key references reader (id) on update cascade on delete cascade,
    group_         char(20)  not null,
    faculty        char(250) not null,
    course         smallint, -- Added course column here
    joining_date   date      not null,
    departure_date date
);

create table IF NOT EXISTS
    teacher
(
    id             serial primary key references reader (id) on update cascade on delete cascade,
    degree         char(200),
    rank           char(200),
    joining_date   date not null,
    departure_date date
);

create table IF NOT EXISTS
    science_worker
(
    id             serial primary key references reader (id) on update cascade on delete cascade,
    faculty        char(250),
    department     char(250),
    degree_        char(200) not null,
    rank           char(200),
    joining_date   date      not null,
    departure_date date
);

create table IF NOT EXISTS violation_instance
(
    reader_id             bigint references reader (id) on update cascade on delete cascade,
    violation_category_id smallint references violation (id) on update cascade on delete cascade,
    primary key (reader_id, violation_category_id)
);

create table IF NOT EXISTS
    library_card
(
    id              bigserial primary key,
    issue_date      date     not null,
    reader_id       bigint   not null references reader (id) on update cascade on delete set null,
    expiration_date date,
    status_id       smallint references library_card_status (id) on update cascade on delete set null,
    restriction_id  smallint references restriction (id) on update cascade on delete set null
);

create table IF NOT EXISTS
    publication_issuing
(
    id              bigserial primary key,
    library_card_id bigint   references library_card (id) on update cascade on delete set null,
    publication_id  bigint   not null references publication (id) on update cascade on delete cascade,
    pickup_point_id smallint references pickup_point (id) on update cascade on delete set null,
    issuing_date    date     not null,
    expiration_date date     not null,
    return_date     date
);

create table IF NOT EXISTS order_
(
    id              bigserial primary key,
    library_card_id bigint    references library_card (id) on update cascade on delete set null,
    publication_id  bigint    not null references publication (id) on update cascade on delete cascade,
    pickup_point_id smallint  references pickup_point (id) on update cascade on delete set null,
    ordering_date   date      not null,
    order_type      char(300) not null
);

create table IF NOT EXISTS library_card_reregistration
(
    id                  bigserial primary key,
    reregistration_date date     not null,
    library_card_id     bigint   not null references library_card (id) on update cascade on delete cascade,
    pickup_point_id     smallint not null references pickup_point (id) on update cascade on delete set null
);

DO
$$
    BEGIN
        IF NOT EXISTS (SELECT 1
                       FROM information_schema.columns
                       WHERE table_name = 'student'
                         AND column_name = 'course') THEN
            ALTER TABLE student
                ADD COLUMN course smallint;
        END IF;
    END
$$;

DROP VIEW IF EXISTS debtors_view;
DROP VIEW IF EXISTS book_orders_stats;
DROP VIEW IF EXISTS reader_full_info;

CREATE OR REPLACE VIEW reader_full_info AS
SELECT r.id,
       r.category,
       r.surname,
       r.name_,
       r.middle_name,
       CASE
           WHEN s.id IS NOT NULL THEN 'student'
           WHEN t.id IS NOT NULL THEN 'teacher'
           WHEN sw.id IS NOT NULL THEN 'science_worker'
           WHEN ue.id IS NOT NULL THEN 'university_employee'
           WHEN a.id IS NOT NULL THEN 'applicant'
           ELSE 'other'
           END                                           as reader_type,
       COALESCE(s.faculty, sw.faculty, a.faculty, 'N/A') as faculty,
       COALESCE(s.group_, 'N/A')                         as group_,
       COALESCE(s.course, 0)                             as course,
       COALESCE(sw.department, 'N/A')                    as department,
       COALESCE(t.degree, sw.degree_, 'N/A')             as degree,
       COALESCE(t.rank, sw.rank, 'N/A')                  as rank_
FROM reader r
         LEFT JOIN student s ON r.id = s.id
         LEFT JOIN teacher t ON r.id = t.id
         LEFT JOIN science_worker sw ON r.id = sw.id
         LEFT JOIN university_employee ue ON r.id = ue.id
         LEFT JOIN applicant a ON r.id = a.id;

CREATE OR REPLACE VIEW debtors_view AS
SELECT rfi.*,
       pi.id               as issuing_id,
       pi.issuing_date,
       pi.expiration_date,
       pi.return_date,
       pi.pickup_point_id,
       pp.name_            as pickup_point_name,
       p.name_             as publication_name,
       p.author,
       (CASE
            WHEN pi.return_date IS NULL AND pi.expiration_date < CURRENT_DATE
                THEN (CURRENT_DATE - pi.expiration_date)
            ELSE NULL END) as days_overdue
FROM reader_full_info rfi
         JOIN library_card lc ON rfi.id = lc.reader_id
         JOIN publication_issuing pi ON lc.id = pi.library_card_id
         JOIN pickup_point pp ON pi.pickup_point_id = pp.id
         JOIN publication p ON pi.publication_id = p.id
WHERE pi.return_date IS NULL
  AND pi.expiration_date < CURRENT_DATE;

CREATE OR REPLACE VIEW book_orders_stats AS
SELECT p.id        as publication_id,
       p.name_     as publication_name,
       p.author,
       pp.id       as pickup_point_id,
       pp.name_    as pickup_point_name,
       rfi.faculty,
       COUNT(o.id) as order_count
FROM publication p
         JOIN order_ o ON p.id = o.publication_id
         JOIN pickup_point pp ON o.pickup_point_id = pp.id
         JOIN library_card lc ON o.library_card_id = lc.id
         JOIN reader_full_info rfi ON lc.reader_id = rfi.id
GROUP BY p.id, p.name_, p.author, pp.id, pp.name_, rfi.faculty;
