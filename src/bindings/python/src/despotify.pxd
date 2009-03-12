from playlist cimport *

cdef extern from "/usr/include/despotify/despotify.h":
    ctypedef int bool
    ctypedef long unsigned int pthread_t

    # Some opaque structs.
    cdef struct session:
        pass
    cdef struct snd_session:
        pass
    cdef struct buf:
        pass

    cdef struct despotify_session:
        unsigned int * state
        unsigned char * IV
        unsigned char * keystream
        bool initialized
        session * session
        snd_session * snd_session
        char * last_error
        despotify_session aes
        pthread_t thread
        track * track
        playlist * playlist
        buf * response
        int offset

    playlist * despotify_get_playlists(despotify_session *)
    bool despotify_stop(despotify_session *)
    bool despotify_resume(despotify_session *)
    bool despotify_rename_playlist(despotify_session *, playlist *, char *)
    bool despotify_delete_playlist(despotify_session *, playlist *)
    void despotify_exit(despotify_session *)
    despotify_session * despotify_init_client()
    playlist * despotify_search(despotify_session *, char *)
    char * despotify_get_error(despotify_session *)
    bool despotify_authenticate(despotify_session *, char *, char *)
    bool despotify_pause(despotify_session *)
    bool despotify_cleanup()
    bool despotify_remove_song(despotify_session *, playlist *, track *)
    bool despotify_play(despotify_session *, playlist *, track *)
    void despotify_free_playlist(playlist *)
    void despotify_free(despotify_session *, bool)
    playlist * despotify_create_playlist(despotify_session *, char *)
    track * despotify_get_current_track(despotify_session *)
    bool despotify_init()
    bool despotify_append_song(despotify_session *, playlist *, track *)
