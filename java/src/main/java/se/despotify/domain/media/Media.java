package se.despotify.domain.media;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import se.despotify.util.Hex;
import se.despotify.util.SpotifyURI;

import java.util.Arrays;

/**
 * @since 2009-apr-24 14:17:32
 */
public abstract class Media implements Visitable {

  protected static Logger log = LoggerFactory.getLogger(Media.class);

  private byte[] UUID;

  protected Media() {
    UUID = null;
    hexUUID = null;
  }

  protected Media(byte[] UUID) {
    this(UUID, null);
  }

  protected Media(byte[] UUID, String hexUUID) {
    this.UUID = UUID;
    this.hexUUID = hexUUID;
  }

  protected Media(String hexUUID) {
    if (hexUUID == null) {
      throw new NullPointerException("Expected hex UUID");
    }
    if (!SpotifyURI.isHex(hexUUID)) {
      throw new IllegalArgumentException(hexUUID + " is not a hex UUID");
    }
    this.UUID = Hex.toBytes(hexUUID);
    this.hexUUID = hexUUID;
  }

  public final void setUUID(String UUID) {
    if (SpotifyURI.isHex(UUID)) {
      setUUID(Hex.toBytes(UUID));
    } else {
      throw new IllegalArgumentException(UUID + " is not a valid UUID.");
    }
  }

  public final void setUUID(byte[] UUID) {
    if (UUID == null) {
      throw new IllegalArgumentException("UUID must not be null");
    } else if (UUID.length != 16) {
      throw new IllegalArgumentException("UUID should be 16 bytes");
    }
    this.UUID = UUID;
    hexUUID = null; // reset
  }

	public final byte[] getUUID(){
		return this.UUID;
	}

  // todo transient URI too!

  private transient String hexUUID;

  public final String getHexUUID() {
    if (hexUUID == null && UUID != null) {
      hexUUID = Hex.toHex(UUID);
    }
    return hexUUID;
  }


	public abstract String getSpotifyURL();

  public abstract String getHttpURL();

  @Override
  public boolean equals(Object o) {
    if (this == o) return true;
    if (o == null || getClass() != o.getClass()) return false;

    Media media = (Media) o;

    return Arrays.equals(UUID, media.UUID);

  }

  @Override
  public int hashCode() {
    return Arrays.hashCode(UUID);
  }

  @Override
  public String toString() {
    return getClass().getSimpleName() + "{" +
        "hexUUID=" + getHexUUID()+
        '}';
  }
}
