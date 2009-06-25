package se.despotify.domain.media;

import javax.persistence.*;
import java.io.Serializable;

/**
 * @author kalle
 * @since 2009-jun-08 04:13:16
 */
@Entity
public class ExternalId implements Serializable {

  private static final long serialVersionUID = 1l;

  @Id
  @GeneratedValue
  private Long id;


  @Column(length = 255)
  private String externalId;

  @Column(length = 255)
  private String type;


  public String getExternalId() {
    return externalId;

  }

  public void setExternalId(String externalId) {
    this.externalId = externalId;
  }

  public Long getId() {
    return id;
  }

  public void setId(Long id) {
    this.id = id;
  }

  public String getType() {
    return type;
  }

  public void setType(String type) {
    this.type = type;
  }
}