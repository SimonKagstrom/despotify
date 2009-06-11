# vim: set fileencoding=utf-8 filetype=pyrex :

cdef class SpotifyObject:
    """Implements stuff that's specific to Spotify Objects.

    A Spotify Object is basically anything that has a spotify URI.
    """
    def get_http_link(self):
        """Returns HTTP link to the object."""
        parts = self.get_uri().split(':')
        # We ignore the first part ('spotify'), replacing it
        # with http://open.spotify.com/. Then we replace every : with /. :)
        return 'http://open.spotify.com/%s' % '/'.join(parts[1:])
