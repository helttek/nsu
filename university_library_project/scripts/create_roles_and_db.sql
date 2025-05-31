create role admin
with
    login password 'admin_password';

create database university_library owner admin;

create user librarian
with
    password 'librarian_password';

create user catalog_manager
with
    password 'catalog_manager_password';
