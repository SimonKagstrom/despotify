# vim: set fileencoding=utf-8 filetype=pyrex :

cdef class Playlist:
    def __init__(self):
        raise TypeError("This class cannot be instantiated from Python")

    property tracks:
        def __get__(self):
            return self.tracks_to_list(self.data.tracks)

    property author:
        def __get__(self):
            return self.data.author

    property name:
        def __get__(self):
            return self.data.name

    def __dealloc__(self):
        if self.take_owner:
            despotify_free_playlist(self.data)

    def __str__(self):
        return '<Playlist: %s by %s>' % (self.name, self.author)

cdef class RootIterator:
    def __init__(self, parent):
        self.parent = parent
        self.i = 0

    def __iter__(self):
        return self

    def next(self):
        if i >= len(self.parent):
            raise StopIteration()

        retval = self.parent.get(i)
        i = i + 1
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
