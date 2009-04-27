package se.despotify.domain.media;

import javax.imageio.ImageIO;
import java.io.ByteArrayInputStream;
import java.io.IOException;

/**
 * @since 2009-apr-27 18:08:58
 */
public class Image extends Media {

  public Image() {
  }

  public Image(byte[] UUID) {
    super(UUID);
  }

  public Image(byte[] UUID, String hexUUID) {
    super(UUID, hexUUID);
  }

  public Image(String hexUUID) {
    super(hexUUID);
  }

  public void accept(Visitor visitor) {
    visitor.visit(this);
  }


  public String getSpotifyURL() {
    throw new UnsupportedOperationException();
  }

  public String getHttpURL() {
    throw new UnsupportedOperationException();
  }

  private byte[] bytes;

  public byte[] getBytes() {
    return bytes;
  }

  public void setBytes(byte[] bytes) {
    this.bytes = bytes;
  }

  public java.awt.Image toAwtImage() {
    try {
      return ImageIO.read(new ByteArrayInputStream(getBytes()));
    }
    catch (IOException e) {
      throw new RuntimeException(e);
    }
  }
}
