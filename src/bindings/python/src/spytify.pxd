from despotify cimport *
from playlist cimport *

cdef class Spytify:
    cdef despotify_session* ds
