create or replace procedure issue_library_card(
    p_reader_id bigint,
    p_issue_date date,
    p_expiration_date date,
    p_status_id smallint,
    p_restriction_id smallint
)
language plpgsql
as $$
begin
    insert into library_card (reader_id, issue_date, expiration_date, status_id, restriction_id)
    values (p_reader_id, p_issue_date, p_expiration_date, p_status_id, p_restriction_id);
end;
$$;

create or replace procedure register_library_card(
    p_library_card_id bigint,
    p_reregistration_date date,
    p_pickup_point_id smallint
)
language plpgsql
as $$
begin
    insert into library_card_reregistration (library_card_id, reregistration_date, pickup_point_id)
    values (p_library_card_id, p_reregistration_date, p_pickup_point_id);
end;
$$;

create or replace procedure record_reader_violation(
    p_reader_id bigint,
    p_violation_category_id smallint
)
language plpgsql
as $$
begin
    insert into violation_instance (reader_id, violation_category_id)
    values (p_reader_id, p_violation_category_id);
end;
$$;

create or replace procedure add_new_publication(
    p_name varchar,
    p_author varchar,
    p_publishing_date date,
    p_publishing_office varchar,
    p_price money,
    p_status_id smallint
)
language plpgsql
as $$
begin
    insert into publication (name_, author, publishing_date, publishing_office, price, status_id)
    values (p_name, p_author, p_publishing_date, p_publishing_office, p_price, p_status_id);
end;
$$;

create or replace procedure add_new_student(
    p_surname varchar,
    p_name varchar,
    p_middle_name varchar,
    p_group char,
    p_faculty char,
    p_joining_date date,
    p_departure_date date
)
language plpgsql
as $$
declare
    new_reader_id bigint;
begin
    insert into reader (category, surname, name_, middle_name)
    values ('student', p_surname, p_name, p_middle_name)
    returning id into new_reader_id;

    insert into student (id, group_, faculty, joining_date, departure_date)
    values (new_reader_id, p_group, p_faculty, p_joining_date, p_departure_date);
end;
$$;

create or replace procedure borrow_publication(
    p_library_card_id bigint,
    p_publication_id bigint,
    p_pickup_point_id smallint,
    p_issuing_date date,
    p_expiration_date date
)
language plpgsql
as $$
begin
    insert into publication_issuing (library_card_id, publication_id, pickup_point_id, issuing_date, expiration_date)
    values (p_library_card_id, p_publication_id, p_pickup_point_id, p_issuing_date, p_expiration_date);
end;
$$;

create or replace procedure return_publication(
    p_publication_issuing_id bigint,
    p_return_date date
)
language plpgsql
as $$
begin
    update publication_issuing
    set return_date = p_return_issuing_id
    where id = p_publication_issuing_id;
end;
$$;