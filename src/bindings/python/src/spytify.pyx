import atexit

cdef extern from "Python.h":
    ctypedef int Py_intptr_t
    void PyEval_InitThreads()


class SpytifyError(Exception):
    pass

include "sessionstruct.pxi"
include "spotifyobject.pxi"

include "album.pxi"
include "artist.pxi"
include "track.pxi"
include "playlist.pxi"
include "searchresult.pxi"

cdef void callback_handler(despotify_session* ds, int signal, 
                           void* data, void *python_obj) with gil:
    (<Spytify>python_obj).handle(signal, data)

cdef class Spytify:
    """Class representing a connection to the Spotify service.
    
    This should be any scripts "entrypoint" into this module; any other
    class will most likely be instantiated by this class and returned.
    """
    def __init__(self, bytes user, bytes pw, bool high_bitrate=True, object callback=None):
        """Create a new Spytify instance, and connect to Spotify.

        Args:
            user: Username to authenticate with
            pw: Password to authenticate with
            high_bitrate: Wether or not to request high bitrate data.
        """
        self.stored_playlists = None
        self.callback = callback

        self.ds = despotify_init_client(callback_handler, <void*>self, high_bitrate)
        if not self.ds:
            raise SpytifyError(despotify_get_error(self.ds))

        if not despotify_authenticate(self.ds, user, pw):
            raise SpytifyError(despotify_get_error(self.ds))

        PyEval_InitThreads()
        self.thread = audio_thread.thread_init(self.ds)

    cdef handle(self, int signal, void* data):
        if self.callback:
            self.callback(self, signal)

    property stored_playlists:
        def __get__(self):
            if self.stored_playlists is None:
                self.stored_playlists = self.create_rootlist()

            return self.stored_playlists

    property current_track:
        def __get__(self):
            return self.create_track(despotify_get_current_track(self.ds))

    def flush_stored_playlists(self):
        """Clear cached playlists."""
        self.stored_playlists = self.create_rootlist()

    def lookup(self, str uri):
        """Looks up URIs like spotify:track:32a2n4NPXhH3OI06VPLwTA.

        Args:
            uri: URI like
                spotify:track:32a2n4NPXhH3OI06VPLwTA
                or track:32a2n4NPXhH3OI06VPLwTA
        Returns:
            Artist, Track or Album object for the id.
        """

        cdef list parts = uri.split(':', 3)
        if len(parts) < 2:
            raise SpytifyError('Ambigious URI specified: %s' % uri)
        if len(parts) > 2 and parts[0] != 'spotify':
            raise SpytifyError('URI not meant for us: %s' % uri)

        cdef str type
        cdef bytes uri_id
        cdef char id[32] 
        type, uri_id = parts[-2].lower(), parts[-1]

        despotify_uri2id(uri_id, id)

        if type == 'artist':
            return self.create_artist_full(despotify_get_artist(self.ds, id))
        elif type =='album':
            return self.create_album_full(despotify_get_album(self.ds, id))
        elif type =='track':
            return self.create_track(despotify_get_track(self.ds, id))
        else:
            raise SpytifyError('URI specifies invalid type: %s' % type)

    def search(self, bytes searchtext, int max_hits=MAX_SEARCH_RESULTS):
        """Search for a string like the normal Spotify client.

        Args:
            searchtext: What you want to search for. 
                (http://www.spotify.com/blog/archives/2008/01/22/searching-spotify/)
            max_hits: Max number of hits, cannot be higher than MAX_SEARCH_RESULTS in despotify.h.

        Returns: None if search failed, a SearchResult if search was OK.
        """
        cdef search_result* search = despotify_search(self.ds, searchtext, max_hits)
        if not search:
            return None
        else:
            return self.create_search_result(search, True)

    def play_list(self, Playlist playlist, Track starting_track=None):
        """Play a playlist, continuing to the next song when finished.

        Args:
            playlist: Playlist to play from.
            starting_track: If not None (default), then track to start playback from.
        """
        cdef track* starting_track_ptr = playlist.data.tracks
        if starting_track is not None:
            starting_track_ptr = starting_track.data

        if not despotify_play(self.ds, starting_track_ptr, True):
            raise SpytifyError(despotify_get_error(self.ds))

        audio_thread.thread_play(self.thread)

    def play(self, Track track):
        """Start playback of a given track.

        Args:
            track: Play this track.
        """
        if not despotify_play(self.ds, track.data, False):
            raise SpytifyError(despotify_get_error(self.ds))

        audio_thread.thread_play(self.thread)

    def stop(self):
        """Stop playback."""
        if not despotify_stop(self.ds):
            raise SpytifyError(despotify_get_error(self.ds))

    def pause(self):
        """Pause playback."""
        audio_thread.thread_pause(self.thread)

    def resume(self):
        """Resume playback."""
        audio_thread.thread_play(self.thread)

    def close(self):
        """Close the session with the server."""
        audio_thread.thread_exit(self.thread)
        despotify_exit(self.ds)

def bytestr_to_hexstr(str bytes):
    return ''.join(["%02x" % ord(c) for c in bytes])

# Wrapper for despotify_cleanup.
def _cleanup():
    assert(despotify_cleanup())

assert(despotify_init())
atexit.register(_cleanup)
