import atexit

class SpytifyError(Exception):
    pass

include "playlist.pxi"

cdef class Spytify:
    def __init__(self, str user, str pw):
        self.ds = despotify_init_client()
        if not self.ds:
            raise SpytifyError(despotify_get_error(self.ds))

        self.authenticate(user, pw)

    def get_playlists(self):
        return playlist_to_list(despotify_get_playlists(self.ds))

    def search(self, str searchtext):
        cdef playlist* search = despotify_search(self.ds, searchtext)
        if not search:
            return None
        else:
            return _create_playlist(search)

    def authenticate(self, str user, str pw):
        if not despotify_authenticate(self.ds, user, pw):
            raise SpytifyError(despotify_get_error(self.ds))

    def close(self):
        despotify_exit(self.ds)

# Wrapper for despotify_cleanup.
def _cleanup():
    assert(despotify_cleanup())

assert(despotify_init())
atexit.register(_cleanup)
