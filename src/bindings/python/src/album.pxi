# vim: set fileencoding=utf-8 filetype=pyrex :

cdef class Album(SessionStruct):
    def __init__(self):
        raise TypeError("This class cannot be instantiated from Python")

    property name:
        def __get__(self):
            return self.data.name

    property id:
        def __get__(self):
            return self.data.id

    property tracks:
        def __get__(self):
            return self.tracks_to_list(self.data.tracks)

    property year:
        def __get__(self):
            return self.data.year

    property cover_id:
        def __get__(self):
            return self.data.cover_id

    property popularity:
        def __get__(self):
            return self.data.popularity

    def __dealloc__(self):
        if self.take_owner:
            despotify_free_album(self.data)

    def __str__(self):
        return '<Album: %s>' % (self.name)
