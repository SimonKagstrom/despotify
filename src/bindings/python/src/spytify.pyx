import atexit

class SpytifyError(Exception):
    pass

include "sessionstruct.pxi"

include "album.pxi"
include "artist.pxi"
include "track.pxi"
include "playlist.pxi"

cdef class Spytify:
    def __init__(self, str user, str pw):
        self.ds = despotify_init_client()
        if not self.ds:
            raise SpytifyError(despotify_get_error(self.ds))

        self.authenticate(user, pw)

    def authenticate(self, str user, str pw):
        if not despotify_authenticate(self.ds, user, pw):
            raise SpytifyError(despotify_get_error(self.ds))

    def stored_playlists(self):
        return self.create_rootlist()

    def search(self, str searchtext):
        cdef playlist* search = despotify_search(self.ds, searchtext)
        if not search:
            return None
        else:
            return self.create_playlist(search, True)

    def play(self, Playlist playlist, Track track):
        if not despotify_play(self.ds, playlist.data, track.data):
            raise SpytifyError(despotify_get_error(self.ds))

    def pause(self):
        if not despotify_pause(self.ds):
            raise SpytifyError(despotify_get_error(self.ds))

    def resume(self):
        if not despotify_resume(self.ds):
            raise SpytifyError(despotify_get_error(self.ds))

    def stop(self):
        if not despotify_stop(self.ds):
            raise SpytifyError(despotify_get_error(self.ds))

    def close(self):
        despotify_exit(self.ds)

def bytestr_to_hexstr(str bytes):
    return ''.join(["%02x" % ord(c) for c in bytes])

# Wrapper for despotify_cleanup.
def _cleanup():
    assert(despotify_cleanup())

assert(despotify_init())
atexit.register(_cleanup)
