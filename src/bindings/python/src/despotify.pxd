from playlist cimport *

cdef extern from "/usr/include/despotify/despotify.h":
    ctypedef int dbool
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
        dbool initialized
        session * session
        snd_session * snd_session
        char * last_error
        despotify_session aes
        pthread_t thread
        track * track
        playlist * playlist
        buf * response
        int offset

    despotify_session * despotify_init_client()
    dbool despotify_authenticate(despotify_session *, char *, char *)
    void despotify_exit(despotify_session *)
    void despotify_free(despotify_session *, dbool)

    char * despotify_get_error(despotify_session *)

    dbool despotify_play(despotify_session *, playlist *, track *)
    dbool despotify_pause(despotify_session *)
    dbool despotify_resume(despotify_session *)
    dbool despotify_stop(despotify_session *)

    track * despotify_get_current_track(despotify_session *)
    playlist * despotify_get_stored_playlists(despotify_session *)
    playlist * despotify_search(despotify_session *, char *)

    playlist * despotify_create_playlist(despotify_session *, char *)
    dbool despotify_rename_playlist(despotify_session *, playlist *, char *)
    dbool despotify_append_song(despotify_session *, playlist *, track *)
    dbool despotify_remove_song(despotify_session *, playlist *, track *)
    dbool despotify_delete_playlist(despotify_session *, playlist *)
    void despotify_free_playlist(playlist *)

    dbool despotify_init()
    dbool despotify_cleanup()
