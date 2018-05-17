create temporary table spammer (select userid from profile where response1 like '%zoloft%'
 or response2 like '%zoloft%' or length(response1)>5000 or length(response2)>5000);
delete from profile where userid in (select userid from spammer);
delete from user where id in (select userid from spammer) and id not in (select distinct userid from team);
drop table spammer;
