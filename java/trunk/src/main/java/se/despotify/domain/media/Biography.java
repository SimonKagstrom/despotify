package se.despotify.domain.media;


import javax.persistence.*;
import java.io.Serializable;
import java.util.List;

/**
 * @since 2009-apr-30 09:03:24
 */
@Entity
public class Biography implements Serializable, org.domdrides.entity.Entity<Long> {

  private static final long serialVersionUID = 1L;

  @Id
  @GeneratedValue
  private Long id;

  @Lob    
  private String text;

  @OneToMany
  private List<Image> portraits;

  public String getText() {
    return text;
  }

  public void setText(String text) {
    this.text = text;
  }

  public List<Image> getPortraits() {
    return portraits;
  }

  public void setPortraits(List<Image> portraits) {
    this.portraits = portraits;
  }

  public Long getId() {
    return id;
  }

  public void setId(Long id) {
    this.id = id;
  }

  @Override
  public String toString() {
    return "Biography{" +
        "id=" + id +
        ", text='" + text + '\'' +
        ", portraits=" + portraits +
        '}';
  }
}
