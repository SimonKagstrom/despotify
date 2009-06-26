package se.despotify.domain;

import org.domdrides.entity.Entity;
import org.springframework.stereotype.Repository;
import org.springframework.transaction.annotation.Transactional;
import se.despotify.domain.media.*;
import se.despotify.util.MeanTimer;

import javax.persistence.EntityManager;
import javax.persistence.EntityManagerFactory;
import java.io.Serializable;

/**
 * @author kalle
 * @since 2009-jun-08 01:10:48
 */
@Transactional
@Repository
public class JpaStore extends Store {

  private EntityManager entityManager;

  public JpaStore(EntityManager entityManager) {
    this.entityManager = entityManager;
  }
  
  public JpaStore(EntityManagerFactory entityManagerFactory) {
    this(entityManagerFactory.createEntityManager());
  }

  public EntityManager getEntityManager() {
    return entityManager;
  }

  public void setEntityManager(EntityManager entityManager) {
    this.entityManager = entityManager;
  }

  @Override
  public File getFile(String hexUUID) {
    File file = getById(hexUUID, File.class);
    if (file == null) {
      file = new File(hexUUID);
      entityManager.persist(file);
    } else {
      System.currentTimeMillis(); // breakpoint
    }
    return file;
  }

  public Playlist getPlaylist(String s) {
    Playlist playlist = getById(s, Playlist.class);
    if (playlist == null) {
      playlist = new Playlist(s);
      entityManager.persist(playlist);
    } else {
      System.currentTimeMillis(); // breakpoint
    }
    return playlist;
  }

  public Image getImage(String s) {
    Image image = getById(s, Image.class);
    if (image == null) {
      image = new Image(s);
      entityManager.persist(image);
    } else {
      System.currentTimeMillis(); // breakpoint
    }
    return image;
  }

  public Album getAlbum(String s) {
    Album album = getById(s, Album.class);
    if (album == null) {
      album = new Album(s);
      entityManager.persist(album);
    } else {
      System.currentTimeMillis(); // breakpoint
    }
    return album;
  }

  public Artist getArtist(String s) {
    Artist artist = getById(s, Artist.class);
    if (artist == null) {
      artist = new Artist(s);
      entityManager.persist(artist);
    } else {
      System.currentTimeMillis(); // breakpoint
    }
    return artist;
  }

  public Track getTrack(String s) {
    Track track = getById(s, Track.class);
    if (track == null) {
      track = new Track(s);
      entityManager.persist(track);
    } else {
      System.currentTimeMillis(); // breakpoint
    }
    return track;
  }


  public static MeanTimer timer = new MeanTimer("media persist");

  public Media persist(Media media) {
    timer.start();
    media = entityManager.merge(media);
    timer.stop();
//    entityManager.flush();
    return media;
  }

  public <T extends Entity<? extends Serializable>> T getById(Object id, Class<T> clazz) {
    return entityManager.find(clazz, id);
  }

}
