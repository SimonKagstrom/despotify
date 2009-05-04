package se.despotify.domain.media;

import java.util.List;

/**
 * @since 2009-apr-30 09:03:24
 */
public class Biography {

  private String text;
  private MediaList<Image> portraits;

  public String getText() {
    return text;
  }

  public void setText(String text) {
    this.text = text;
  }

  public MediaList<Image> getPortraits() {
    return portraits;
  }

  public void setPortraits(MediaList<Image> portraits) {
    this.portraits = portraits;
  }
}
