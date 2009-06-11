package se.despotify.domain.media;

import se.despotify.domain.Store;
import se.despotify.util.Hex;
import se.despotify.util.SpotifyURI;
import se.despotify.util.XMLElement;

import javax.persistence.ManyToOne;
import javax.persistence.OneToMany;
import javax.persistence.Entity;
import javax.persistence.CascadeType;
import java.util.ArrayList;
import java.util.List;
import java.util.zip.Adler32;


@Entity
public class Album extends RestrictedMedia {

  private static final long serialVersionUID = 1L;

  private String name;

  @ManyToOne(cascade = {CascadeType.MERGE, CascadeType.PERSIST, CascadeType.REFRESH})
  private Artist artist;

  private String cover;
  private Float popularity;

  @OneToMany(cascade = {CascadeType.MERGE, CascadeType.PERSIST, CascadeType.REFRESH})
  private List<Track> tracks;

  private String c;
  private String p;

  public Album() {
    super();
  }

  public Album(String hexUUID) {
    this(hexUUID, null, null);
  }

  public Album(byte[] bytesUUID) {
    this(Hex.toHex(bytesUUID), null, null);
  }

  public Album(String hexUUID, String name, Artist artist) {
    this.id = hexUUID;
    this.name = name;
    this.artist = artist;
  }

  @Override
  public byte[] getByteUUID() {
    return Hex.toBytes(id);
  }

  @Override
  public String getSpotifyURI() {
    return "spotify:album:" + SpotifyURI.toURI(getByteUUID());
  }

  @Override
  public String getHttpURL() {
    return "http://open.spotify.com/album/" + SpotifyURI.toURI(getByteUUID());
  }

  @Override
  public void accept(Visitor visitor) {
    visitor.visit(this);
  }


  public String getName() {
    return name;
  }

  public void setName(String name) {
    this.name = name;
  }

  public Artist getArtist() {
    return artist;
  }

  public void setArtist(Artist artist) {
    this.artist = artist;
  }

  public String getCover() {
    return cover;
  }

  public void setCover(String cover) {
    this.cover = cover;
  }

  public Float getPopularity() {
    return popularity;
  }

  public void setPopularity(Float popularity) {
    this.popularity = popularity;
  }

  public List<Track> getTracks() {
    return tracks;
  }

  public void setTracks(List<Track> tracks) {
    this.tracks = tracks;
  }

  public String getC() {
    return c;
  }

  public void setC(String c) {
    this.c = c;
  }

  public String getP() {
    return p;
  }

  public void setP(String p) {
    this.p = p;
  }

  public static Album fromXMLElement(XMLElement albumElement, Store store) {


    Album album = store.getAlbum(albumElement.getChildText("id"));

    if (albumElement.hasChild("name")) {
      album.name = albumElement.getChildText("name");
    }

    if (albumElement.hasChild("artist-id")) {
      album.artist = store.getArtist(albumElement.getChildText("artist-id"));
    }

    if (albumElement.hasChild("artist") || albumElement.hasChild("artist-name")) {
      album.artist.setName(albumElement.hasChild("artist") ? albumElement.getChildText("artist") : albumElement.getChildText("artist-name"));
    }


    /* Set cover. */
    if (albumElement.hasChild("cover")) {
      String value = albumElement.getChildText("cover");
      if (!"".equals(value)) {
        album.cover = value;
      }
    }


    /* Set popularity. */
    if (albumElement.hasChild("popularity")) {
      album.popularity = Float.parseFloat(albumElement.getChildText("popularity"));
    }


    /* Set tracks. */
    if (albumElement.hasChild("discs")) {

      List<Track> tracks = new ArrayList<Track>();

      for (XMLElement discElement : albumElement.getChild("discs").getChildren("disc")) {

        int discNumber = Integer.valueOf(discElement.getChildText("disc-number"));
        for (XMLElement trackElement : discElement.getChildren("track")) {
          Track track = Track.fromXMLElement(trackElement, store);
          track.setDiscNumber(discNumber);
          tracks.add(track);
          track.setAlbum(album);
        }
      }

      XMLElement restrictionsNode = albumElement.getChild("restrictions");
      if (restrictionsNode != null) {
        RestrictedMedia.fromXMLElement(restrictionsNode, album);
      }

      if (albumElement.hasChild("copyright")) {

        for (XMLElement copyrightNode : albumElement.getChild("copyright").getChildren()) {
          if ("c".equals(copyrightNode.getElement().getNodeName())) {
            album.setC(copyrightNode.getText());
          } else if ("p".equals(copyrightNode.getElement().getNodeName())) {
            album.setP(copyrightNode.getText());
          } else {
            log.warn("Unknown copyright type " + copyrightNode.getElement().getNodeName());
          }
        }

      }

      album.setTracks(tracks);

    }

    return album;
  }


  public long calculateChecksum() {
    Adler32 adler = new Adler32();
    for (Track track : tracks) {
      adler.update(track.getByteUUID());
      adler.update(0x01);
    }
    return adler.getValue();
  }


  @Override
  public String toString() {
    return "Album{" +
        "id='" + id + '\'' +
        ", name='" + name + '\'' +
        ", artist=" + (artist == null ? null : artist.getId()) +
        ", cover='" + cover + '\'' +
        ", popularity=" + popularity +
        ", tracks=" + (tracks == null ? null : tracks.size()) +
        ", c=" + c +
        ", p=" + p +
        '}';
  }
}
