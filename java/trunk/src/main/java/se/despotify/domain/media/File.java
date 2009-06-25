package se.despotify.domain.media;

import org.hibernate.annotations.CollectionOfElements;
import se.despotify.util.XMLElement;

import javax.persistence.*;
import java.util.Arrays;
import java.util.LinkedHashSet;
import java.util.Set;
import java.util.ArrayList;
import java.io.Serializable;

/**
 * @author kalle
 * @since 2009-jun-08 04:13:16
 */
@Entity
public class File extends Media {

  private static final long serialVersionUID = 1l;

  private String format;

  public File() {
  }

  public File(String hexUUID) {
    setId(hexUUID);
  }

  @Override
  public void accept(Visitor visitor) {
    visitor.visit(this);
  }

  public String getFormat() {
    return format;
  }

  public void setFormat(String format) {
    this.format = format;
  }

  @Override
  public String getSpotifyURI() {
    throw new UnsupportedOperationException();
  }

  @Override
  public String getHttpURL() {
    throw new UnsupportedOperationException();
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