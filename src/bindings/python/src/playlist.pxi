# vim: set fileencoding=utf-8 filetype=pyrex :

cdef class SearchResult:
    def __init__(self):
        raise TypeError("This class cannot be instantiated from Python")

    property query:
        def __get__(self):
            return <char*>self.data.query

    property suggestion:
        def __get__(self):
            if self.data.suggestion:
                return <char*>self.data.suggestion
            else:
                return None

    property total_artists:
        def __get__(self):
            return self.data.total_artists

    property total_albums:
        def __get__(self):
            return self.data.total_albums

    property total_tracks:
        def __get__(self):
            return self.data.total_tracks

cdef class Playlist:
    def __init__(self):
        raise TypeError("This class cannot be instantiated from Python")

    property search:
        def __get__(self):
            if self.data.search:
                return self.create_search_result(self.data.search)
            else:
                return None

    property name:
        def __get__(self):
            return self.data.name

    property author:
        def __get__(self):
            return self.data.author

    property id:
        def __get__(self):
            return <char*>self.data.playlist_id

    property tracks:
        def __get__(self):
            return self.tracks_to_list(self.data.tracks)

    def __dealloc__(self):
        if self.take_owner:
            despotify_free_playlist(self.data)

    def __repr__(self):
        return '<Playlist: %s by %s (%s)>' % (self.name, self.author, self.id)

cdef class RootIterator:
    def __init__(self, parent):
        self.parent = parent
        self.i = 0

    def __iter__(self):
        return self

    def __next__(self):
        if self.i >= len(self.parent):
            raise StopIteration()

        retval = self.parent[self.i]
        self.i = self.i + 1
        return retval

cdef class RootList:
    def __init__(self):
        raise TypeError("This class cannot be instantiated from Python")

    cdef fetch(self):
        if self._list is None:
            self.data = despotify_get_stored_playlists(self.ds)
            self._list = self.playlists_to_list(self.data)

    def __getitem__(self, item):
        self.fetch()
        return self._list[item]

    def __len__(self):
        self.fetch()
        return len(self._list)

    def __contains__(self, item):
        self.fetch()
        return item in self._list

    def __iter__(self):
        self.fetch()
        return RootIterator(self)

    def __dealloc__(self):
        if self.data:
            despotify_free_playlist(self.data)

    def __repr__(self):
        self.fetch()
        return '<RootList: %s>' % self._list
