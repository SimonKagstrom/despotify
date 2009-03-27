# vim: set fileencoding=utf-8 filetype=pyrex :

cdef class Artist:
    def __init__(self):
        raise TypeError("This class cannot be instantiated from Python")

    cdef fetch(self):
        cdef artist* new_data
        if not self.fetched:
            new_data = despotify_get_artist(self.ds, self.data.id)
            if self.take_owner:
                despotify_free_artist(self.data)
            self.data = new_data
            self.take_owner = True

    property name:
        def __get__(self):
            return self.data.name

    property id:
        def __get__(self):
            return self.data.id

    property text:
        def __get__(self):
            self.fetch()
            return self.data.text

    property portrait_id:
        def __get__(self):
            self.fetch()
            return self.data.portrait_id

    property genres:
        def __get__(self):
            self.fetch()
            return self.data.genres

    property popularity:
        def __get__(self):
            self.fetch()
            return self.data.popularity

    property albums:
        def __get__(self):
            self.fetch()
            return self.albums_to_list(self.data.albums)

    def __dealloc__(self):
        if self.take_owner:
            despotify_free_artist(self.data)

    def __repr__(self):
        return '<Artist: %s (%s)>' % (self.name, self.id)

