package se.despotify.domain.media;

import javax.persistence.*;
import java.io.Serializable;
import java.util.List;

/**
 * The same track occcuring in multiple albums,
 * remixes,
 * cover version,
 * etc.
 *
 * This is not something that is supplied by spotify,
 * it is abstract support for such data to be used in
 * similarity calculations.
 *
 * todo AssociationClass between tracks and composition
 *
 *                 {0..1}
 * [Track]- - - -|- - - -[Composition]
 *        {0..*}
 *               |
 *            [Type]
 *
 * @author kalle
 * @since 2009-jun-19 16:20:36
 */
@Entity
public class Composition implements Serializable {

  private static final long serialVersionUID = 1L;

  @Id
  @GeneratedValue
  private Long id;


  @OneToMany(cascade = {CascadeType.MERGE, CascadeType.PERSIST, CascadeType.REFRESH}, mappedBy = "composition")
  private List<Track> tracks;

  @ManyToOne(cascade = {CascadeType.MERGE, CascadeType.PERSIST, CascadeType.REFRESH})
  private Track primaryTrack;

  public Long getId() {
    return id;
  }

  public void setId(Long id) {
    this.id = id;
  }

  public List<Track> getTracks() {
    return tracks;
  }

  public void setTracks(List<Track> tracks) {
    this.tracks = tracks;
  }

  public Track getPrimaryTrack() {
    return primaryTrack;
  }

  public void setPrimaryTrack(Track primaryTrack) {
    this.primaryTrack = primaryTrack;
  }
}
