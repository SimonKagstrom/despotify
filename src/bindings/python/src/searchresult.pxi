# vim: set fileencoding=utf-8 filetype=pyrex :

cdef class SearchResult(SpotifyObject):
    def __init__(self):
        raise TypeError("This class cannot be instantiated from Python")

    def get_uri(self):
        return 'spotify:search:%s' % self.query

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

    property playlist:
        def __get__(self):
            return self.playlist

    def __dealloc__(self):
        if self.take_owner:
            despotify_free_search(self.data)
