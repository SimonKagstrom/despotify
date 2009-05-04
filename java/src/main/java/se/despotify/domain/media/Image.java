package se.despotify.domain.media;

import se.despotify.util.XMLElement;
import se.despotify.domain.Store;

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

  private int width;
  private int height;

  private byte[] bytes;

  public byte[] getBytes() {
    return bytes;
  }

  public void setBytes(byte[] bytes) {
    this.bytes = bytes;
  }

  public int getWidth() {
    return width;
  }

  public void setWidth(int width) {
    this.width = width;
  }

  public int getHeight() {
    return height;
  }

  public void setHeight(int height) {
    this.height = height;
  }



  public java.awt.Image toAwtImage() {
    try {
      return ImageIO.read(new ByteArrayInputStream(getBytes()));
    }
    catch (IOException e) {
      throw new RuntimeException(e);
    }
  }

  public static Image fromXMLElement(XMLElement imageNode, Store store) {
    Image image = null;

    if (imageNode.hasChild("id")) {
      store.getImage(imageNode.getChildText("id"));
    } else {
      throw new RuntimeException("Image UUID missing in XML node");
    }

    if (imageNode.hasChild("height")) {
      image.setWidth(Integer.valueOf(imageNode.getChildText("height")));
    }

    if (imageNode.hasChild("width")) {
      image.setHeight(Integer.valueOf(imageNode.getChildText("width")));
    }

    return image;
  }
}
