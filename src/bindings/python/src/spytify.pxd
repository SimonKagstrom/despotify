from despotify cimport *


cdef class Album
cdef class Artist
cdef class Playlist
cdef class RootList
cdef class Track

cdef class SessionStruct:
    cdef despotify_session* ds
    cdef Album _create_album(self, album* album, bint take_owner=?)
    cdef Artist _create_artist(self, artist* artist, bint take_owner=?)
    cdef Playlist _create_playlist(self, playlist* playlist, bint take_owner=?)
    cdef RootList _create_rootlist(self)
    cdef Track _create_track(self, track* track)
    cdef list albums_to_list(self, album* albums)
    cdef list artists_to_list(self, artist* artists)
    cdef list playlists_to_list(self, playlist* playlists)
    cdef list tracks_to_list(self, track* tracks)

cdef class Spytify(SessionStruct):
    pass

cdef class RootList(SessionStruct):
    cdef fetch(self)
    cdef playlist* data 
    cdef list _list

cdef class Album(SessionStruct):
    cdef album* data
    cdef bint take_owner

cdef class Artist(SessionStruct):
    cdef artist* data
    cdef bint take_owner

cdef class Track(SessionStruct):
    cdef track* data
    cdef list _artists

cdef class Playlist(SessionStruct):
    cdef playlist* data
    cdef bint take_owner
