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
public class File implements Serializable {

  private static final long serialVersionUID = 1l;

  // todo @Column(length = 20)
  private String id;
  private String format;


  public String getId() {
    return id;
  }

  public void setId(String id) {
    this.id = id;
  }

  public String getFormat() {
    return format;
  }

  public void setFormat(String format) {
    this.format = format;
  }

  public static File fromXMLElement(XMLElement restrictionsNode) {

    File file = new File();
    String tmp;
    if ((tmp = restrictionsNode.getAttribute("id")) != null) {
      file.setId(tmp);
    }
    if ((tmp = restrictionsNode.getAttribute("format")) != null) {
      file.setFormat(tmp);
    }
    return file;
  }

}