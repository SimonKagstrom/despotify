# vim: set fileencoding=utf-8 filetype=pyrex :

cdef class Track:
    def __init__(self):
        raise TypeError("This class cannot be instantiated from Python")

    property id:
        def __get__(self):
            return self.data.id

    def has_meta_data(self):
        return bool(self.data.has_meta_data)

    property title:
        def __get__(self):
            return self.data.title

    property artists:
        def __get__(self):
            cdef list artists

            if self._artists is None:
                artists = self.artists_to_list(self.data.artist)
                self._artists = [self._create_artist(despotify_get_artist(self.ds, a.id), True) for a in artists]

            return self._artists

    property album:
        def __get__(self):
            return self.data.album

    property length:
        def __get__(self):
            return self.data.length

    def __str__(self):
        return '<Track: %s - %s - %s>' % (self.artist, self.title, self.album)

