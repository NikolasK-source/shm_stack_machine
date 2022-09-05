# Special variables

## STIME
Current system time as unix time stamp (64 bit floating point).

## MTIME
value that represents a monotonic time since some unspecified starting point (64 bit floating point).  
This is the clock you want to use to generate cyclic time dependent signals.

## CTIME
Current process CPU time (64 bit floating point).

## TTIME
Current thread CPU time ()

## PID
Current process ID.

## PPID
Current parent process ID.

## UID
Current user ID.

## EUID
Current effective user ID.

## RAND
Pseudo random integer value.

## RANDF
Pseudo random 32 bit float value between 0 and 1.

## RANDD
Pseudo random 64 bit float value between 0 and 1.

## STDOUT
Dump to stdout as unsigned integer.

## STDOUTS
Dump to stdout as signed integer.

## STDOUTF
Dump to stdout as 32 bit float.

## STDOUTD
Dump to stdout as 64 bit float.
