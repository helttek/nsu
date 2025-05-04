-- psql -U admin -d university_library
create table
    pickup_point (
        id smallserial primary key,
        subscription_type char(40),
        number_of_seats smallint check (number_of_seats > 0),
        status_ char(200) not null DEFAULT 'Inactive',
        name_ varchar(350)
    );


create table
    event_type (
        id smallserial primary key,
        name_ char(300) not null unique
    );

create table
    restriction (
        id smallserial primary key,
        description_ char(500) not null unique
    );

create table
    library_card_status (
        id smallserial primary key,
        info varchar(100) not null unique
    );

create table
    violation (
        id smallserial primary key,
        name_ char(350) not null,
        violation_date date,
        restriction_id smallint references restriction(id) on update cascade on delete set null
    );

create table
    publication_status (
        id smallserial primary key,
        info varchar(100) not null unique
    );

create table
    publication (
        id bigserial primary key,
        name_ varchar(500) not null,
        author varchar(400),
        publishing_date date,
        publishing_office varchar(400),
        price money not null check (price > 0.00::money),
        status_id smallint references publication_status (id) on update cascade on delete set null
    );

create table publication_event(
    id bigserial primary key,
    publication_id bigint not null references publication(id) on update cascade on delete cascade,
    event_type_id smallint not null references event_type(id) on update cascade on delete set null,
    event_date date,
    pickup_point_id smallint references pickup_point(id) on update cascade on delete set null
);

create table pickup_point_publication(
    pickup_point_id smallint not null references pickup_point(id) on update cascade on delete cascade,
    publication_id bigint not null references publication(id) on update cascade on delete cascade,
    publication_limit smallint check(publication_limit > 0),
    primary key(pickup_point_id, publication_id)
);

create table reader(
    id bigserial primary key,
    category char(50) not null,
    surname varchar(100) not null,
    name_ varchar(100) not null,
    middle_name varchar(100) not null
);

create table applicant(
    id serial primary key references reader(id) on update cascade on delete cascade,
    faculty char(250) not null
);

create table university_employee (
    id serial primary key references reader(id) on update cascade on delete cascade,
    joining_date date not null,
    departure_date date
);

create table student(
    id bigserial primary key references reader(id) on update cascade on delete cascade,
    group_ char(20) not null,
    faculty char(250) not null,
    joining_date date not null,
    departure_date date
);

create table teacher(
    id serial primary key references reader(id) on update cascade on delete cascade,
    degree char(200),
    rank char(200),
    joining_date date not null,
    departure_date date
);

create table science_worker(
    id serial primary key references reader(id) on update cascade on delete cascade,
    faculty char(250),
    department char(250),
    degree_ char(200) not null,
    rank char(200),
    joining_date date not null,
    departure_date date
);

create table violation_instance(
    reader_id bigint references reader(id) on update cascade on delete cascade,
    violation_category_id smallint references violation(id) on update cascade on delete cascade,
    primary key(reader_id, violation_category_id)
);

create table library_card(
    id bigserial primary key,
    issue_date date not null,
    reader_id bigint not null references reader(id) on update cascade on delete set null,
    expiration_date date,
    status_id smallint references library_card_status(id) on update cascade on delete set null,
    restriction_id smallint references restriction(id) on update cascade on delete set null
);

create table publication_issuing(
    id bigserial primary key,
    library_card_id bigint references library_card(id) on update cascade on delete set null,
    publication_id bigint not null references publication(id) on update cascade on delete cascade,
    pickup_point_id smallint references pickup_point(id) on update cascade on delete set null,
    issuing_date date not null,
    expiration_date date not null,
    return_date date
);

create table order_(
    id bigserial primary key,
    library_card_id bigint references library_card(id) on update cascade on delete set null,
    publication_id bigint not null references publication(id) on update cascade on delete cascade,
    pickup_point_id smallint references pickup_point(id) on update cascade on delete set null,
    ordering_date date not null,
    order_type char(300) not null
);

create table library_card_reregistration(
    id bigserial primary key,
    reregistration_date date not null,
    library_card_id bigint not null references library_card(id) on update cascade on delete cascade,
    pickup_point_id smallint not null references pickup_point(id) on update cascade on delete set null
);

