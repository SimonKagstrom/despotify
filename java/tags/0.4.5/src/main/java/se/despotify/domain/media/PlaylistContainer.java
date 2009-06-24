package se.despotify.domain.media;

import se.despotify.domain.Store;
import se.despotify.util.ChecksumCalculator;
import se.despotify.util.XMLElement;

import javax.persistence.Id;
import javax.persistence.OneToMany;
import javax.persistence.Entity;
import javax.persistence.CascadeType;
import java.io.Serializable;
import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;
import java.util.Date;


/**
 * [User]---{0..*}->[Playlist]
 */
@Entity
public class PlaylistContainer implements Iterable<Playlist>, Serializable, org.domdrides.entity.Entity<String> {

  private static final long serialVersionUID = 1L;

  /**
   * user name, account name, owner name. one per user.
   */
  @Id
  private String id;

  @OneToMany(cascade = {CascadeType.MERGE, CascadeType.PERSIST, CascadeType.REFRESH})
  private List<Playlist> items;

  private long revision;
  private long checksum;

  private Date loaded;

  public PlaylistContainer() {
    this.id = null;
    this.items = new ArrayList<Playlist>();
    this.revision = -1;
    this.checksum = -1;
  }

  public long calculateChecksum() {
    ChecksumCalculator calculator = new ChecksumCalculator();
    for (Playlist playlist : items) {
      playlist.accept(calculator);
    }
    return calculator.getValue();
  }

  public String getId() {
    return this.id;
  }

  public void setId(String id) {
    this.id = id;
  }

  public List<Playlist> getItems() {
    return this.items;
  }

  public void setItems(List<Playlist> items) {
    this.items = items;
  }

  public long getRevision() {
    return this.revision;
  }

  public void setRevision(long revision) {
    this.revision = revision;
  }

  public long getChecksum() {
    return this.checksum;
  }

  public void setChecksum(long checksum) {
    this.checksum = checksum;
  }

  public Iterator<Playlist> iterator() {
    return this.items.iterator();
  }


  public static PlaylistContainer fromXMLElement(XMLElement playlistsElement, Store store, PlaylistContainer playlists) {

    /* Get "change" element. */
    XMLElement changeElement = playlistsElement.getChild("next-change").getChild("change");

    if (changeElement.hasChild("user")) {
      playlists.id = changeElement.getChildText("user").trim();
    }

    /* Get items (comma separated list). */
    if (changeElement.getChild("ops").hasChild("add")) {
      String items = changeElement.getChild("ops").getChild("add").getChildText("items");

      for (String playlistUUID : items.split(",")) {
        playlistUUID = playlistUUID.trim();
        if (playlistUUID.length() == 34 && playlistUUID.endsWith("02")) {
          playlistUUID = playlistUUID.substring(0, 32);
        }
        Playlist playlist = store.getPlaylist(playlistUUID);
        if (!playlists.getItems().contains(playlist)) {
          playlists.getItems().add(playlist);
        }
        // todo remove deleted?
      }
    }

    /* Get "version" element. */
    XMLElement versionElement = playlistsElement.getChild("next-change").getChild("version");

    /* Split version string into parts. */
    String[] versionTagValues = versionElement.getText().split(",", 4);

    playlists.setRevision(Long.parseLong(versionTagValues[0]));
    playlists.setChecksum(Long.parseLong(versionTagValues[2]));

    if (playlists.getItems().size() != Long.parseLong(versionTagValues[1])) {
      throw new RuntimeException();
    }
    if (playlists.calculateChecksum() != playlists.getChecksum()) {
      throw new RuntimeException();
    }

    return playlists;
  }

  public Date getLoaded() {
    return loaded;
  }

  public void setLoaded(Date loaded) {
    this.loaded = loaded;
  }

  @Override
  public String toString() {
    return "PlaylistContainer{" +
        "author='" + id + '\'' +
        ", items=" + (items == null ? null : items.size()) +
        ", revision=" + revision +
        ", checksum=" + checksum +
        ", loaded=" + loaded +
        '}';
  }
}
