package se.despotify.domain;

import se.despotify.domain.media.*;

import java.io.Serializable;
import java.util.HashMap;
import java.util.Map;

/**
 * @since 2009-apr-25 17:41:41
 */
public class MemoryStore extends Store implements Serializable {

  private static final long serialVersionUID = 1L;

  public Map<String, Playlist> playlists = new HashMap<String, Playlist>();
  public Map<String, Album> albums = new HashMap<String, Album>();
  public Map<String, File> files = new HashMap<String, File>();
  public Map<String, Artist> artists = new HashMap<String, Artist>();
  public Map<String, Track> tracks = new HashMap<String, Track>();
  public Map<String, Image> images = new HashMap<String, Image>();

  public void reset() {
    playlists.clear();
    albums.clear();
    files.clear();
    artists.clear();
    tracks.clear();
    images.clear();
  }

  @Override
  public File getFile(String hexUUID) {
    File file = files.get(hexUUID);
    if (file == null) {
      file = new File(hexUUID);
      files.put(file.getId(), file);
    }
    return file;
  }
  
  @Override
  public Playlist getPlaylist(String hexUUID) {
    Playlist playlist = playlists.get(hexUUID);
    if (playlist == null) {
      playlist = new Playlist(hexUUID);
      playlists.put(playlist.getId(), playlist);
    }
    return playlist;
  }

  @Override
  public Album getAlbum(String hexUUID) {
    Album album = albums.get(hexUUID);
    if (album == null) {
      albums.put(hexUUID, album = new Album(hexUUID));
    }
    return album;

  }

  @Override
  public Artist getArtist(String hexUUID) {
    Artist artist = artists.get(hexUUID);
    if (artist == null) {
      artists.put(hexUUID, artist = new Artist(hexUUID));
    }
    return artist;

  }

  @Override
  public Track getTrack(String hexUUID) {
    Track track = tracks.get(hexUUID);
    if (track == null) {
      tracks.put(hexUUID, track = new Track(hexUUID));
    }
    return track;
  }

  @Override
  public Image getImage(String hexUUID) {
    Image image = images.get(hexUUID);
    if (image == null) {
      images.put(hexUUID, image = new Image(hexUUID));
    }
    return image;

  }


  @Override
  public Media persist(Media media) {
    return media;
  }

}