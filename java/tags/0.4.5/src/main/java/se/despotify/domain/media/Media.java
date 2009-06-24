package se.despotify.domain.media;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import se.despotify.util.Hex;

import javax.persistence.MappedSuperclass;
import javax.persistence.Id;
import javax.persistence.Column;
import java.util.Date;

/**
 * @since 2009-apr-24 14:17:32
 */
@MappedSuperclass
public abstract class Media implements Visitable<String>, org.domdrides.entity.Entity<String> {

  protected static Logger log = LoggerFactory.getLogger(Media.class);

  @Id
  @Column(length = 48)
  protected String id;

  private Date loaded;

  public byte[] getByteUUID() {
    return Hex.toBytes(getId());
  }



  public abstract String getSpotifyURI();

  public abstract String getHttpURL();

  public Date getLoaded() {
    return loaded;
  }

  public void setLoaded(Date loaded) {
    this.loaded = loaded;
  }

  @Override
  public String toString() {
    return getSpotifyURI();
  }

  public final String getId() {
    return id;
  }

  public final void setId(String id) {
    this.id = id;
  }

  @Override
  public boolean equals(Object o) {
    if (this == o) return true;
    if (o == null || getClass() != o.getClass()) return false;

    Media media = (Media) o;

    if (!id.equals(media.id)) return false;

    return true;
  }

  @Override
  public int hashCode() {
    return id.hashCode();
  }
}
