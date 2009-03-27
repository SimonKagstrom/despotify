from despotify cimport *

cdef class SessionStruct
cdef class Album(SessionStruct)
cdef class Artist(SessionStruct)
cdef class SearchResult(SessionStruct)
cdef class Playlist(SessionStruct)
cdef class RootList(SessionStruct)
cdef class Track(SessionStruct)

cdef class SessionStruct:
    cdef despotify_session* ds
    cdef Album create_album(self, album* album, bint take_owner=?)
    cdef Artist create_artist(self, artist* artist, bint take_owner=?)
    cdef SearchResult create_search_result(self, search_result* result)
    cdef Playlist create_playlist(self, playlist* playlist, bint take_owner=?)
    cdef RootList create_rootlist(self)
    cdef Track create_track(self, track* track)
    cdef list albums_to_list(self, album* albums)
    cdef list artists_to_list(self, artist* artists)
    cdef list playlists_to_list(self, playlist* playlists)
    cdef list tracks_to_list(self, track* tracks)

cdef class Spytify(SessionStruct):
    pass

cdef class Album(SessionStruct):
    cdef album* data
    cdef bint take_owner

cdef class Artist(SessionStruct):
    cdef artist* data
    cdef bint take_owner

cdef class SearchResult(SessionStruct):
    cdef search_result* data

cdef class Playlist(SessionStruct):
    cdef playlist* data
    cdef bint take_owner

cdef class RootList(SessionStruct):
    cdef fetch(self)
    cdef playlist* data 
    cdef list _list

cdef class RootIterator:
    cdef RootList parent
    cdef int i

cdef class Track(SessionStruct):
    cdef track* data
    cdef list _artists
