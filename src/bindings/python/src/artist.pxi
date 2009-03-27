# vim: set fileencoding=utf-8 filetype=pyrex :

cdef class Artist:
    def __init__(self):
        raise TypeError("This class cannot be instantiated from Python")

    property name:
        def __get__(self):
            return self.data.name

    property id:
        def __get__(self):
            return self.data.id

    property text:
        def __get__(self):
            return self.data.text

    property portrait_id:
        def __get__(self):
            return self.data.portrait_id

    property genres:
        def __get__(self):
            return self.data.genres

    property popularity:
        def __get__(self):
            return self.data.popularity

    property albums:
        def __get__(self):
            return self.albums_to_list(self.data.albums)

    def __dealloc__(self):
        if self.take_owner:
            despotify_free_artist(self.data)

    def __str__(self):
        return '<Artist: %s>' % (self.name)

