cdef list playlist_to_list(playlist* playlists):
    cdef list l = []
    while playlists:
        l.append(_create_playlist(playlists))
        playlists = playlists.next

    return l

cdef list tracks_to_list(track* tracks):
    cdef list l = []
    while tracks:
        l.append(_create_track(tracks))
        tracks = tracks.next

    return l

cdef class Track:
    def __init__(self):
        raise TypeError("This class cannot be instantiated from Python")

    property id:
        def __get__(self):
            return self.track.id

    def has_meta_data(self):
        return bool(self.track.has_meta_data)

    property title:
        def __get__(self):
            return self.track.title

    property artist:
        def __get__(self):
            return self.track.artist

    property album:
        def __get__(self):
            return self.track.album

    property length:
        def __get__(self):
            return self.track.length

cdef class Playlist:
    def __init__(self):
        raise TypeError("This class cannot be instantiated from Python")

    property tracks:
        def __get__(self):
            return tracks_to_list(self.playlist.tracks)

    property author:
        def __get__(self):
            return self.playlist.author

    property name:
        def __get__(self):
            return self.playlist.name

    def __dealloc__(self):
        if self.playlist:
            playlist.playlist_free(self.playlist, 1) # Is this right? 1 means "free tracks."

# Based on http://wiki.cython.org/FAQ#CanCythongenerateCcodeforclasses.3F
cdef extern from "spytify.h":
    cdef Track NEW_TRACK "PY_NEW" (object t)
    cdef Playlist NEW_PLAYLIST "PY_NEW" (object t)

cdef Playlist _create_playlist(playlist* playlist):
    cdef Playlist instance

    if not playlist:
        raise SpytifyError("Tried creating a NULL-playlist.")
    
    instance = NEW_PLAYLIST(Playlist)
    instance.playlist = playlist
    return instance

cdef Track _create_track(track* track):
    cdef Track instance

    if not track:
        raise SpytifyError("Tried creating a NULL-track.")

    instance = NEW_TRACK(Track)
    instance.track = track
    return instance
