package se.despotify.domain.media;

import org.hibernate.annotations.Index;
import se.despotify.client.protocol.command.ChecksumException;
import se.despotify.domain.Store;
import se.despotify.exceptions.DespotifyException;
import se.despotify.util.ChecksumCalculator;
import se.despotify.util.Hex;
import se.despotify.util.SpotifyURI;
import se.despotify.util.XMLElement;

import javax.persistence.OneToMany;
import javax.persistence.Entity;
import javax.persistence.CascadeType;
import javax.persistence.ManyToMany;
import java.io.Serializable;
import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;

@Entity
public class Playlist extends Media implements Iterable<Track> {

  private static final long serialVersionUID = 1L;

  @Index(name = "index_author")
  private String author;
  
  private String name;

  @ManyToMany(cascade = {CascadeType.MERGE, CascadeType.PERSIST, CascadeType.REFRESH})
  private List<Track> tracks;

  private Long revision;
  private Long checksum;
  private Boolean collaborative;

  public Playlist() {
    super();
  }

  public Playlist(String id) {
    setId(id);
  }

  public Playlist(byte[] UUID) {
    setId(Hex.toHex(UUID));
  }


  public Playlist(String author, byte[] byteUUID) {
    this(author, Hex.toHex(byteUUID));
  }

  public Playlist(String author, String hexUUID) {
    this.author = author;
    this.id = hexUUID;
  }


  public Playlist(String author, String hexUUID, String name, Boolean collaborative) {
    this.id = hexUUID;
    this.author = author;
    this.name = name;
    this.collaborative = collaborative;
  }

  public Playlist(String author, String hexUUID, String name, Long revision, Long checksum, Boolean collaborative) {
    this.author = author;
    this.id = hexUUID;
    this.name = name;
    this.tracks = null;
    this.revision = revision;
    this.checksum = checksum;
    this.collaborative = collaborative;
  }


  @Override
  public byte[] getByteUUID() {
    return Hex.toBytes(id);
  }


  public long calculateChecksum() {
    ChecksumCalculator calculator = new ChecksumCalculator();
    if (tracks != null) {
      for (Track track : tracks) {
        track.accept(calculator);
      }
    }
    return calculator.getValue();
  }

  public void accept(Visitor visitor) {
    visitor.visit(this);
  }

  @Override
  public String getSpotifyURI() {
    return "spotify:user:" + author + ":playlist:" + SpotifyURI.toURI(getByteUUID());
  }

  @Override
  public String getHttpURL() {
    return "http://open.spotify.com/user/" + author + "/playlist/" + SpotifyURI.toURI(getByteUUID());
  }

  public String getName() {
    return name;
  }

  public void setName(String name) {
    this.name = name;
  }

  public String getAuthor() {
    return author;
  }

  public void setAuthor(String author) {
    this.author = author;
  }

  public int size() {
    return getTracks() != null ? getTracks().size() : 0;
  }

  public List<Track> getTracks() {
    return tracks;
  }

  public void setTracks(List<Track> tracks) {
    this.tracks = tracks;
  }

  public Long getRevision() {
    return revision;
  }

  public void setRevision(Long revision) {
    this.revision = revision;
  }

  public Long getChecksum() {
    return checksum;
  }

  public void setChecksum(Long checksum) {
    this.checksum = checksum;
  }

  public Boolean isCollaborative() {
    return collaborative;
  }

  public void setCollaborative(Boolean collaborative) {
    this.collaborative = collaborative;
  }

  public Iterator<Track> iterator() {
    return this.tracks.iterator();
  }

  public static void fromXMLElement(XMLElement playlistElement, Store store, Playlist playlist) throws DespotifyException {

    /* Get "change" element. */
    XMLElement changeElement = playlistElement.getChild("next-change").getChild("change");

    /* Set author. */
    playlist.author = changeElement.getChildText("user");

    /* Set name. */
    playlist.name = changeElement.getChild("ops").getChildText("name");

    /* Get items (comma separated list). */
    if (changeElement.getChild("ops").hasChild("add")) {
      String items = changeElement.getChild("ops").getChild("add").getChildText("items");

      if (playlist.tracks == null) {
        playlist.tracks = new ArrayList<Track>();
      }


      /* Add track items. */
      int position = 0;
      String[] split = items.split(",");

      List<Track> tracks = new ArrayList<Track>(split.length);


      for (String trackData : split) {
        trackData = trackData.trim();
        final String trackHexUUID;
        if (trackData.length() != 34) {
          if (SpotifyURI.isHex(trackData)) {
            // not sure why playlist UUID is send sometimes. notice it is lacking UUID prefix byte
            if (!trackData.equals(playlist.getId())) {
              throw new DespotifyException("32 byte hex UUID does not equal the playlist UUID!");
            }
            continue;
          } else {
            throw new RuntimeException(trackData + " is not a valid 32 byte hex UUID!");
          }
        } else if (trackData.length() == 34) {
          trackHexUUID = trackData.substring(0, 32);
          if (!"01".equals(trackData.substring(32, 34))) {
            throw new DespotifyException("Expected hex UUID type suffix 01, got " + trackData.substring(32, 34));
          }
        } else {
          throw new RuntimeException("track UUID was not 16+1 or 16 byte!");
        }

        Track track = store.getTrack(trackHexUUID);
        tracks.add(track);

        position++; // todo perhaps we should use this to syncronize any discrepancy
      }

      playlist.setTracks(tracks);
    }

    /* Get "version" element. */
    XMLElement versionElement = playlistElement.getChild("next-change").getChild("version");

    /* Split version string into parts. */
    String[] parts = versionElement.getText().split(",", 4);

    /* Set values. */

    String[] versionTagValues = versionElement.getText().split(",", 4);

    playlist.setRevision(Long.parseLong(versionTagValues[0]));
    playlist.setChecksum(Long.parseLong(versionTagValues[2]));
    playlist.collaborative = (Integer.parseInt(parts[3]) == 1);

    if (playlist.getTracks() == null) {
      playlist.setTracks(new ArrayList<Track>());
    }
    if (playlist.getTracks().size() != Long.parseLong(versionTagValues[1])) {
      throw new RuntimeException("Size missmatch, playlist = " + playlist.getTracks().size() + ", received = " + versionTagValues[1]);
    }
    if (playlist.calculateChecksum() != playlist.getChecksum()) {
      throw new ChecksumException(playlist.getChecksum(), playlist.calculateChecksum());
    }

  }

  public static Playlist fromResult(String name, String author, Result result) {
    Playlist playlist = new Playlist();

    playlist.name = name;
    playlist.author = author;

    for (Track track : result.getTracks()) {
      playlist.tracks.add(track);
    }

    return playlist;
  }


  @Override
  public String toString() {
    return "Playlist{" +
        "hexUUID='" + getId() + '\'' +
        ", name='" + name + '\'' +
        ", author='" + author + '\'' +
        ", tracks=" + (tracks == null ? null : tracks.size()) +
        ", revision=" + revision +
        ", checksum=" + checksum +
        ", collaborative=" + collaborative +
        '}';
  }

  public static class PK implements Serializable {
    private static final long serialVersionUID = 1L;

    private String author;
    private String id;

    public PK() {
    }

    public PK(String author, String id) {
      this.author = author;
      this.id = id;
    }

    public String getAuthor() {
      return author;
    }

    public void setAuthor(String author) {
      this.author = author;
    }

    public String getId() {
      return id;
    }

    public void setId(String id) {
      this.id = id;
    }

    @Override
    public boolean equals(Object o) {
      if (this == o) return true;
      if (o == null || getClass() != o.getClass()) return false;

      PK pk = (PK) o;

      if (author != null ? !author.equals(pk.author) : pk.author != null) return false;
      if (id != null ? !id.equals(pk.id) : pk.id != null) return false;

      return true;
    }

    @Override
    public int hashCode() {
      int result = author != null ? author.hashCode() : 0;
      result = 31 * result + (id != null ? id.hashCode() : 0);
      return result;
    }
  }

}
