package se.despotify.domain.media;

import se.despotify.domain.Store;
import se.despotify.util.Hex;
import se.despotify.util.SpotifyURI;
import se.despotify.util.XMLElement;

import javax.persistence.*;
import java.util.ArrayList;
import java.util.List;
import java.util.Date;
import java.util.zip.Adler32;

import org.hibernate.annotations.CollectionOfElements;


@Entity
public class Album extends RestrictedMedia {

  private static final long serialVersionUID = 1L;

  /**
   * Enumeration: compilation, single, album
   */
  @Column(length = 20)
  private String type;

  @Column(length = 512)
  private String name;

  @ManyToOne(cascade = {CascadeType.MERGE, CascadeType.PERSIST, CascadeType.REFRESH})
  private Artist mainArtist;

  @ManyToOne(cascade = {CascadeType.MERGE, CascadeType.PERSIST, CascadeType.REFRESH})
  private Image cover;

  private Float popularity;
  private Integer year;

  @OneToMany(cascade = {CascadeType.MERGE, CascadeType.PERSIST, CascadeType.REFRESH}, mappedBy = "album")
  private List<Track> tracks;

  @OneToMany(cascade = {CascadeType.MERGE, CascadeType.PERSIST, CascadeType.REFRESH})
  private List<ExternalId> externalIds;

  @CollectionOfElements
  @Column(length = 512)
  private List<String> c;

  @CollectionOfElements
  @Column(length = 512)
  private List<String> p;

  @CollectionOfElements
  @Column(length = 512)
  private List<String> discNames;

  @Lob
  private String review;

  @ManyToMany(cascade = {CascadeType.MERGE, CascadeType.PERSIST, CascadeType.REFRESH})
  private List<Album> similarAlbums;

  public Album() {
    super();
  }

  public Album(String hexUUID) {
    this(hexUUID, null);
  }

  public Album(byte[] bytesUUID) {
    this(Hex.toHex(bytesUUID), null);
  }

  public Album(String hexUUID, String name) {
    this.id = hexUUID;
    this.name = name;
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

  public synchronized void setDiscName(int index, String name) {
    if (getDiscNames() == null) {
      setDiscNames(new ArrayList<String>(5));
    }
    while(getDiscNames().size() <= index) {
      getDiscNames().add(null);
    }
    getDiscNames().set(index, name);
  }

  public String getName() {
    return name;
  }

  public void setName(String name) {
    this.name = name;
  }

  public Artist getMainArtist() {
    return mainArtist;
  }

  public void setMainArtist(Artist mainArtist) {
    this.mainArtist = mainArtist;
  }

  public Image getCover() {
    return cover;
  }

  public void setCover(Image cover) {
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

  public List<ExternalId> getExternalIds() {
    return externalIds;
  }

  public void setExternalIds(List<ExternalId> externalIds) {
    this.externalIds = externalIds;
  }

  public List<String> getC() {
    return c;
  }

  public void setC(List<String> c) {
    this.c = c;
  }

  public List<String> getP() {
    return p;
  }

  public void setP(List<String> p) {
    this.p = p;
  }

  public List<String> getDiscNames() {
    return discNames;
  }

  public void setDiscNames(List<String> discNames) {
    this.discNames = discNames;
  }


  public String getReview() {
    return review;
  }

  public void setReview(String review) {
    this.review = review;
  }

  public String getType() {
    return type;
  }

  public void setType(String type) {
    this.type = type;
  }

  public Integer getYear() {
    return year;
  }

  public void setYear(Integer year) {
    this.year = year;
  }

  public List<Album> getSimilarAlbums() {
    return similarAlbums;
  }

  public void setSimilarAlbums(List<Album> similarAlbums) {
    this.similarAlbums = similarAlbums;
  }

  /**
   *
   * @param albumElement
   * @param store
   * @param fullyLoadedDate set this not null only if the xml contains data for the complete album
   * @return
   */
  public static Album fromXMLElement(XMLElement albumElement, Store store, Date fullyLoadedDate) {


    Album album = store.getAlbum(albumElement.getChildText("id"));

    if (albumElement.hasChild("name")) {
      album.name = albumElement.getChildText("name");
    }

    if (albumElement.hasChild("artist-id")) {
      album.mainArtist = store.getArtist(albumElement.getChildText("artist-id"));
    }

    if (albumElement.hasChild("artist") || albumElement.hasChild("artist-name")) {
      album.mainArtist.setName(albumElement.hasChild("artist") ? albumElement.getChildText("artist") : albumElement.getChildText("artist-name"));
    }


    /* Set cover. */
    if (albumElement.hasChild("cover")) {
      String value = albumElement.getChildText("cover");
      if (!"".equals(value)) {
        album.cover = store.getImage(value);
      }
    }


    /* Set popularity. */
    if (albumElement.hasChild("popularity")) {
      album.popularity = Float.parseFloat(albumElement.getChildText("popularity"));
    }



    /* Set tracks. */

    List<String> discNames = new ArrayList<String>();
    if (albumElement.hasChild("discs")) {

      List<Track> tracks = new ArrayList<Track>();

      for (XMLElement discElement : albumElement.getChild("discs").getChildren("disc")) {


        if (discElement.hasChild("name")) {
          discNames.add(discElement.getChild("name").getText());
        } else {
          discNames.add(null);
        }


        int discNumber = Integer.valueOf(discElement.getChildText("disc-number"));
        for (XMLElement trackElement : discElement.getChildren("track")) {
          Track track = Track.fromXMLElement(trackElement, store, fullyLoadedDate);
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

        List<String> c = new ArrayList<String>();
        List<String> p = new ArrayList<String>();
        for (XMLElement copyrightNode : albumElement.getChild("copyright").getChildren()) {
          if ("c".equals(copyrightNode.getElement().getNodeName())) {
            c.add(copyrightNode.getText());
          } else if ("p".equals(copyrightNode.getElement().getNodeName())) {
            p.add(copyrightNode.getText());
          } else {
            log.warn("Unknown copyright type " + copyrightNode.getElement().getNodeName());
          }
        }
        if (c.size() > 0) {
          album.setC(c);
        }
        if (p.size() > 0) {
          album.setP(p);
        }

      }

      album.setTracks(tracks);

    }

    if (fullyLoadedDate != null) {
      album.setLoaded(fullyLoadedDate);      
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
        ", type='" + type + '\'' +
        ", popularity=" + popularity +
        ", discNames=" + discNames +
        ", mainArtist=" + (mainArtist == null ? null : mainArtist.getId()) +
        ", cover='" + cover + '\'' +
        ", tracks=" + (tracks == null ? null : tracks.size()) +
        ", c=" + c +
        ", p=" + p +
        '}';
  }

}
