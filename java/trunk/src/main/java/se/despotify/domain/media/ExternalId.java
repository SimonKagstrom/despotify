package se.despotify.domain.media;

import org.hibernate.annotations.CollectionOfElements;
import se.despotify.util.XMLElement;

import javax.persistence.MappedSuperclass;
import javax.persistence.Column;
import javax.persistence.Embeddable;
import java.util.Arrays;
import java.util.LinkedHashSet;
import java.util.Set;
import java.util.ArrayList;
import java.io.Serializable;

/**
 * @author kalle
 * @since 2009-jun-08 04:13:16
 */
@Embeddable
public class ExternalId implements Serializable {

  private static final long serialVersionUID = 1l;

  @Column(length = 255)
  private String id;

  @Column(length = 255)
  private String type;


  public String getId() {
    return id;

  }

  public void setId(String id) {
    this.id = id;
  }

  public String getType() {
    return type;
  }

  public void setType(String type) {
    this.type = type;
  }
}