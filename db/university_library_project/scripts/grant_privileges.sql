grant select, update, insert on publication_issuing to librarian;
grant select, update, insert on pickup_point to librarian;
grant select, update, insert on library_card to librarian;
grant select on pickup_point_publication to librarian;
grant select, update, insert on order_ to librarian;
grant select, update, insert on reader to librarian;
grant select on publication to librarian;
grant select, update, insert on violation_instance to librarian;
grant select, update, insert on teacher to librarian;
grant select, update, insert on publication_event to librarian;
grant select, update, insert on violation to librarian;
grant select on event_type to librarian;
grant select, insert on library_card_reregistration to librarian;
grant select on restriction to librarian;
grant select, update, insert on applicant to librarian;
grant select, update, insert on university_employee to librarian;
grant select, update, insert on student to librarian;
grant select, update, insert on science_worker to librarian;
grant select on library_card_status to librarian;
grant select on publication_status to librarian;

grant select on publication_issuing to catalog_manager;
grant select, update on pickup_point to catalog_manager;
grant select on library_card to catalog_manager;
grant select, update, insert on pickup_point_publication to catalog_manager;
grant select on order_ to catalog_manager;
grant select, update, insert on publication to catalog_manager;
grant select, update, insert on publication_event to catalog_manager;
grant select on event_type to catalog_manager;
grant select on publication_status to catalog_manager;

GRANT SELECT ON reader_full_info TO librarian;
GRANT SELECT ON reader_full_info TO catalog_manager;

GRANT SELECT ON debtors_view TO librarian;
GRANT SELECT ON debtors_view TO catalog_manager;

GRANT SELECT ON book_orders_stats TO librarian;
GRANT SELECT ON book_orders_stats TO catalog_manager;