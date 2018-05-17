insert into sensor.msg_to_host
(create_time,hostid,variety,handled,xml)
select unix_timestamp(), hostid, 'abort', 0,
concat('<trickle_down>\n<result_name>',
  name,
   '</result_name>\n<abort></abort>\n</trickle_down>\n')
  from result where hostid>0 and server_state=4;
