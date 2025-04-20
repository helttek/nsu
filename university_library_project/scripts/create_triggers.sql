create procedure publication_status_present ()
language sql
as $$
    
$$

create procedure publication_status_change (event_type_id smallint)
language sql
as $$
    
$$

create procedure publication_status_ordered ()
language sql
as $$

$$

create trigger publication_return_update
after update on publication_issuing
for each row
execute procedure publication_status_present

create trigger publication_status_update
after insert on publication_event
for each row
execute procedure publication_status_change(new.event_type_id)

create trigger publication_order_update
after insert on order
for each row
execute procedure publication_status_ordered()

create trigger library_card_restriction_update_ins
after insert on order
for each row
execute procedure publication_status_ordered()