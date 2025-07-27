create or replace function publication_returned_update()
returns trigger as $$
begin
  if (old.return_date != new.return_date) and new.return_date is not null then
    update publication
    set status_id = (
      select id from publication_status where info = 'present'
    )
    where id = new.publication_id;
  end if;
  return new;
end;
$$ language plpgsql;

create trigger publication_returned
after update on publication_issuing
for each row
execute function publication_returned_update();

create or replace function publication_order_update()
returns trigger as $$
begin
  update publication
  set status_id = (
    select id from publication_status where info = 'ordered'
  )
  where id = new.publication_id;
  return new;
end;
$$ language plpgsql;

create trigger publication_order
after insert on order_
for each row
execute function publication_order_update();

create or replace function library_card_inactivate()
returns trigger as $$
declare inactive_id int;
begin
  if old.departure_date is null and new.departure_date is not null then
    select id into inactive_id
    from library_card_status
    where info = 'inactive';
    update library_card
    set status_id = inactive_id
    where reader_id = new.id;
  end if;
  return new;
end;
$$ language plpgsql;

create trigger emp_departure
after update on university_employee
for each row
execute function library_card_inactivate();

create trigger student_departure
after update on student
for each row
execute function library_card_inactivate();

create trigger teacher_departure
after update on teacher
for each row
execute function library_card_inactivate();

create trigger science_worker_departure
after update on science_worker
for each row
execute function library_card_inactivate();
