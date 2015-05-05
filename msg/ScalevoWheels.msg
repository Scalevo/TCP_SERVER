
# This is a message to hold data from an Encoder
#
# Rotational velocity should be in rad/sec
# [1] right
# [2] left
#
# travel and speed --> wheels
# travel_tracks and speed_tracks --> tracks

Header header

float64[2] travel 	# Position of the encoder in rad
float64[2] speed	# Rotational velocity in rad/sec

float64[2] travel_tracks 	# Position of the encoder in rad
float64[2] speed_tracks		# Rotational velocity in rad/sec

#Standard metadata for higher-level flow data types
#sequence ID: consecutively increasing ID
#uint32 seq
#Two-integer timestamp that is expressed as:
# * stamp.secs: seconds (stamp_secs) since epoch
# * stamp.nsecs: nanoseconds since stamp_secs
# time-handling sugar is provided by the client library
#time stamp
#Frame this data is associated with
# 0: no frame
# 1: global frame
#string frame_id

