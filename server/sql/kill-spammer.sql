delete from user where id not in (select distinct userid from host) and id not in (select distinct userid from team);
delete from profile where userid not in (select distinct userid from host) and userid not in (select distinct userid from team);


select count(*) from user where id not in (select distinct userid from host);

select count(*) from profile where userid not in (select distinct userid from host);

select * from profile where userid not in (select distinct userid from host) limit 20;

select * from user where id not in (select distinct userid from host) limit 20;


