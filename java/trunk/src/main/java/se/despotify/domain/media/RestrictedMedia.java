package se.despotify.domain.media;

import org.hibernate.annotations.CollectionOfElements;
import se.despotify.util.XMLElement;

import javax.persistence.MappedSuperclass;
import javax.persistence.Column;
import javax.persistence.OneToMany;
import javax.persistence.CascadeType;
import java.util.*;

/**
 * @author kalle
 * @since 2009-jun-08 04:13:16
 */
@MappedSuperclass
public abstract class RestrictedMedia extends Media {

  @OneToMany(cascade = {CascadeType.MERGE, CascadeType.PERSIST, CascadeType.REFRESH})
  private List<Restriction> restrictions;


  public List<Restriction> getRestrictions() {
    return restrictions;
  }

  public void setRestrictions(List<Restriction> restrictions) {
    this.restrictions = restrictions;
  }

}
