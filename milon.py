int timestamp = ....
int usage = .... 
int current_hour = get_hour_from_timestamp(timestamp)
int end_time_stamp = timestamp + usage
int end_hour = get_hour_from_timestamp(end_time_stamp)
if (end_hour > current_hour)
  int overflow = end_time_stamp - get_timestamp_for_hour(end_hour)
  chrome[current_hour] += usage - overflow
  chrome[end_hour] += overflow
else
  chrome[current_hour] += overflow
