cdef extern from "despotify.h":
    ctypedef int dbool

    cdef struct album

    cdef struct artist:
        char * name
        char * id
        char * text
        char * portrait_id
        char * genres
        char * years_active
        float popularity
        int num_albums
        album * albums
        artist * next

    cdef struct track:
        int id
        dbool has_meta_data
        dbool playable
        unsigned char * track_id
        unsigned char * file_id
        unsigned char * album_id
        unsigned char * cover_id
        unsigned char * key
        char * title
        artist * artist
        char * album
        int length
        int tracknumber
        int year
        float popularity
        track * next

    cdef struct album:
        char * name
        char * id
        int num_tracks
        track * tracks
        int year
        char * cover_id
        float popularity
        album * next

    cdef struct session:
        pass

    cdef struct snd_session:
        pass

    cdef struct aes_ctr_state:
        unsigned int * state
        unsigned char * IV
        unsigned char * keystream

    cdef struct search_result:
        unsigned char * query
        unsigned char * suggestion
        int total_artists
        int total_albums
        int total_tracks

    cdef struct playlist:
        char * name
        char * author
        unsigned char * playlist_id
        int num_tracks
        track * tracks
        search_result * search
        playlist * next

    cdef struct buf:
        pass

    cdef struct despotify_session:
        dbool initialized
        session * session
        snd_session * snd_session
        char * last_error
        aes_ctr_state aes
        album * album
        artist * artist
        track * track
        playlist * playlist
        buf * response
        int offset
        dbool list_of_lists

    album * despotify_get_album(despotify_session *, char *)
    dbool despotify_stop(despotify_session *)
    dbool despotify_resume(despotify_session *)
    void despotify_free_album(album *)
    void despotify_exit(despotify_session *)
    void despotify_free_artist(artist *)
    despotify_session * despotify_init_client()
    playlist * despotify_search(despotify_session *, char *)
    void despotify_uri2id(char *, char *)
    char * despotify_get_error(despotify_session *)
    dbool despotify_authenticate(despotify_session *, char *, char *)
    playlist * despotify_search_more(despotify_session *, playlist *)
    dbool despotify_pause(despotify_session *)
    dbool despotify_cleanup()
    artist * despotify_get_artist(despotify_session *, char *)
    dbool despotify_play(despotify_session *, playlist *, track *)
    void despotify_free_playlist(playlist *)
    void despotify_free(despotify_session *, dbool)
    void * despotify_get_image(despotify_session *, char *, int *)
    void despotify_id2uri(char *, char *)
    dbool despotify_init()
    playlist * despotify_get_stored_playlists(despotify_session *)
    track * despotify_get_current_track(despotify_session *)

