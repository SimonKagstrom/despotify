from despotify cimport *
from playlist cimport *

cdef class Spytify:
    cdef despotify_session* ds

cdef class Track:
    cdef track* track

cdef class Playlist:
    cdef playlist* playlist

cdef list playlist_to_list(playlist* playlists)
cdef list tracks_to_list(track* tracks)

cdef Track _create_track(track* track)
cdef Playlist _create_playlist(playlist* playlist)
