package se.despotify.domain.media;

import se.despotify.domain.Store;
import se.despotify.util.Hex;
import se.despotify.util.XMLElement;

import javax.imageio.ImageIO;
import javax.persistence.Entity;
import javax.persistence.Lob;
import java.io.ByteArrayInputStream;
import java.io.IOException;


/**
 * @since 2009-apr-27 18:08:58
 */
@Entity
public class Image extends Media  {

  private static final long serialVersionUID = 1L;


  private int width;
  private int height;

  @Lob
  private byte[] bytes;

  
  public Image() {
  }

  public Image(String hexUUID) {
    id = hexUUID;
  }

  public Image(byte[] bytesUUID) {
    this(Hex.toHex(bytesUUID));
  }


  @Override
  public byte[] getByteUUID() {
    return Hex.toBytes(id);
  }

  public void accept(Visitor visitor) {
    visitor.visit(this);
  }


  public String getSpotifyURI() {
    throw new UnsupportedOperationException();
  }

  public String getHttpURL() {
    throw new UnsupportedOperationException();
  }

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
    Image image;

    if (imageNode.hasChild("id")) {
      image = store.getImage(imageNode.getChildText("id"));
    } else if (Hex.pattern.matcher(imageNode.getText()).matches()) {
      image = store.getImage(imageNode.getText());
    } else {
      throw new RuntimeException("Image hexUUID missing in XML node");
    }

    if (imageNode.hasChild("height")) {
      image.setHeight(Integer.valueOf(imageNode.getChildText("height")));
    }

    if (imageNode.hasChild("width")) {
      image.setWidth(Integer.valueOf(imageNode.getChildText("width")));
    }

    return image;
  }

  @Override
  public String toString() {
    return "Image{" +
        "id=" + id +
        ", width=" + width +
        ", height=" + height +
        ", bytes=" + (bytes == null ? null : bytes.length) +
        '}';
  }


}
