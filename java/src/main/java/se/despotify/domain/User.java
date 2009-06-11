package se.despotify.domain;

import se.despotify.domain.media.PlaylistContainer;

import javax.persistence.Id;
import javax.persistence.OneToOne;
import javax.persistence.Entity;
import java.io.Serializable;

/**
 * @since 2009-apr-24 07:03:09
 */
@Entity
public class User implements Serializable, org.domdrides.entity.Entity<String> {

  private static final long serialVersionUID = 1L;

  @Id
  private String id;

  @OneToOne
  private PlaylistContainer playlists = null;


  public String getId() {
    return id;
  }

  public void setId(String id) {
    this.id = id;
  }

  public PlaylistContainer getPlaylists() {
    return playlists;
  }

  public void setPlaylists(PlaylistContainer playlists) {
    this.playlists = playlists;
  }

}
