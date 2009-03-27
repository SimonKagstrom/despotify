# vim: set fileencoding=utf-8 filetype=pyrex :

# Based on http://wiki.cython.org/FAQ#CanCythongenerateCcodeforclasses.3F
cdef extern from "spytify.h":
    cdef Album NEW_ALBUM "PY_NEW" (object t)
    cdef Artist NEW_ARTIST "PY_NEW" (object t)
    cdef SearchResult NEW_SEARCHRESULT "PY_NEW" (object t)
    cdef Playlist NEW_PLAYLIST "PY_NEW" (object t)
    cdef RootList NEW_ROOTLIST "PY_NEW" (object t)
    cdef Track NEW_TRACK "PY_NEW" (object t)


cdef class SessionStruct:
    cdef Album create_album(self, album* album, bint take_owner=False):
        cdef Album instance

        if not album:
            raise SpytifyError("Tried creating a NULL-album.")

        instance = NEW_ALBUM(Album)
        instance.ds = self.ds
        instance.data = album
        instance.take_owner = take_owner
        return instance

    cdef Artist create_artist(self, artist* artist, bint take_owner=False):
        cdef Artist instance

        if not artist:
            raise SpytifyError("Tried creating a NULL-artist.")

        instance = NEW_ARTIST(Artist)
        instance.ds = self.ds
        instance.data = artist
        instance.take_owner = take_owner
        return instance

    cdef SearchResult create_search_result(self, search_result* result):
        cdef SearchResult instance

        instance = NEW_SEARCHRESULT(SearchResult)
        instance.ds = self.ds
        instance.data = result

        return instance

    cdef Playlist create_playlist(self, playlist* playlist, bint take_owner=False):
        cdef Playlist instance

        if not playlist:
            raise SpytifyError("Tried creating a NULL-playlist.")
                
        instance = NEW_PLAYLIST(Playlist)
        instance.ds = self.ds
        instance.data = playlist
        instance.take_owner = take_owner

        return instance

    cdef RootList create_rootlist(self):
        cdef RootList instance

        instance = NEW_ROOTLIST(RootList)
        instance.ds = self.ds
        instance._list = None
        instance.data = <playlist*>0

        return instance

    cdef Track create_track(self, track* track):
        cdef Track instance

        if not track:
            raise SpytifyError("Tried creating a NULL-track.")

        instance = NEW_TRACK(Track)
        instance.ds = self.ds
        instance.data = track
        instance._artists = None
        return instance
    
    cdef list albums_to_list(self, album* albums):
        cdef list l = []
        while albums:
            l.append(self.create_album(albums))
            albums = albums.next

        return l

    cdef list artists_to_list(self, artist* artists):
        cdef list l = []
        while artists:
            l.append(self.create_artist(artists))
            artists = artists.next

        return l

    cdef list playlists_to_list(self, playlist* playlists):
        cdef list l = []
        while playlists:
            l.append(self.create_playlist(playlists))
            playlists = playlists.next

        return l

    cdef list tracks_to_list(self, track* tracks):
        cdef list l = []
        while tracks:
            l.append(self.create_track(tracks))
            tracks = tracks.next

        return l
