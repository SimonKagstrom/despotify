cdef extern from "/usr/include/despotify/playlist.h":
    cdef enum playlist_flags:
        PLAYLIST_ERROR = 2
        PLAYLIST_LOADED = 1
        PLAYLIST_SELECTED = 16
        PLAYLIST_TRACKS_ERROR = 8
        PLAYLIST_TRACKS_LOADED = 4

    cdef struct track:
        int id
        int has_meta_data
        unsigned char * track_id
        unsigned char * file_id
        unsigned char * key
        char * title
        char * artist
        char * album
        int length
        track * next

    cdef struct playlist:
        playlist_flags flags
        char * name
        char * author
        unsigned char * playlist_id
        int num_tracks
        track * tracks
        playlist * next

    track * playlist_next_playable(playlist *, track *)
    playlist * playlist_select(int)
    playlist * playlist_new()
    int playlist_track_update_from_gzxml(playlist *, void *, int)
    playlist * playlist_root()
    void playlist_track_del(playlist *, unsigned char *)
    void playlist_set_name(playlist *, char *)
    int playlist_create_from_xml(char *, playlist *)
    int playlist_set_id(playlist *, unsigned char *)
    track * playlist_next_track(playlist *, track *)
    track * playlist_track_add(playlist *, unsigned char *)
    void playlist_set_author(playlist *, char *)
    void playlist_free(playlist *, int)
    playlist * playlist_selected()

cdef class Track:
    cdef track* track

cdef class Playlist:
    cdef playlist* playlist

cdef list playlist_to_list(playlist* playlists)
cdef list tracks_to_list(track* tracks)

cdef Track _create_track(track* track)
cdef Playlist _create_playlist(playlist* playlist)
