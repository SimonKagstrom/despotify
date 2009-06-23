package se.despotify.domain;

import se.despotify.domain.media.*;
import se.despotify.util.Hex;

import java.io.Serializable;

import org.domdrides.entity.Entity;


/**
 * @since 2009-apr-25 17:30:25
 */
public abstract class Store implements Serializable {

  /**
   * @param media
   * @return merged instance of the media. 
   */
  public abstract Media persist(Media media);

  public abstract Playlist getPlaylist(String hexUUID);

  public abstract Image getImage(String hexUUID);

  public abstract Album getAlbum(String hexUUID);

  public abstract Artist getArtist(String hexUUID);

  public abstract Track getTrack(String hexUUID);

  public final Playlist getPlaylist(byte[] UUID) {
    return getPlaylist(Hex.toHex(UUID));
  }

  public final Album getAlbum(byte[] UUID) {
    return getAlbum(Hex.toHex(UUID));
  }

  public final Artist getArtist(byte[] UUID) {
    return getArtist(Hex.toHex(UUID));
  }

  public final Track getTrack(byte[] UUID) {
    return getTrack(Hex.toHex(UUID));
  }

  public final Image getImage(byte[] UUID) {
    return getImage(Hex.toHex(UUID));
  }

}
