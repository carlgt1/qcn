/* banish spammers via the forum web page first */
use sensor;
delete from thread where owner in (select userid from forum_preferences where banished_until=2147483647);
delete from post where user in (select userid from forum_preferences where banished_until=2147483647);

select id,name,email_addr,from_unixtime(create_time) from user where create_time>unix_timestamp()-200000;

delete from thread where owner in (select id from user where create_time>unix_timestamp()-200000);
delete from post where user in (select id from user where create_time>unix_timestamp()-200000);

