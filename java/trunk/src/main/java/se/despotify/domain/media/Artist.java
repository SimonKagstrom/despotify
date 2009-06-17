package se.despotify.domain.media;

import org.hibernate.annotations.CollectionOfElements;
import se.despotify.domain.Store;
import se.despotify.util.Hex;
import se.despotify.util.SpotifyURI;
import se.despotify.util.XMLElement;

import javax.persistence.*;
import java.util.*;

@Entity
public class Artist extends Media {

  private static final long serialVersionUID = 1L;

  private String name;

  @ManyToOne(cascade = {CascadeType.MERGE, CascadeType.PERSIST, CascadeType.REFRESH})
  private Image portrait;
  private Float popularity;

  @OneToMany(cascade = {CascadeType.MERGE, CascadeType.PERSIST, CascadeType.REFRESH}, mappedBy = "id")
  private List<Artist> similarArtists;

  @OneToMany(cascade = {CascadeType.MERGE, CascadeType.PERSIST, CascadeType.REFRESH})
  private List<Biography> biographies;

  @CollectionOfElements
  @Column(length = 128)
  private Set<String> genres;

  @CollectionOfElements
  @Column(length = 50)
  private List<String> yearsActive;

  @OneToMany(cascade = {CascadeType.MERGE, CascadeType.PERSIST, CascadeType.REFRESH}, mappedBy = "mainArtist")
  private List<Album> mainArtistAlbums;

  @OneToMany(cascade = {CascadeType.MERGE, CascadeType.PERSIST, CascadeType.REFRESH}, mappedBy = "artist")
  private List<Track> tracks;

  @ManyToMany(cascade = {CascadeType.MERGE, CascadeType.PERSIST, CascadeType.REFRESH})
  private List<Album> allAlbumsWithTrackPresent;

  public Artist() {
    super();
  }

  public Artist(String hexUUID) {
    this(hexUUID, null);
  }

  public Artist(byte[] bytesUUID) {
    this(Hex.toHex(bytesUUID), null);
  }

  public Artist(String hexUUID, String name) {
    this.id = hexUUID;
    this.name = name;
  }

  @Override
  public byte[] getByteUUID() {
    return Hex.toBytes(id);
  }

  @Override
  public void accept(Visitor visitor) {
    visitor.visit(this);
  }

  @Override
  public String getSpotifyURI() {
    return "spotify:artist:" + SpotifyURI.toURI(getByteUUID());
  }

  @Override
  public String getHttpURL() {
    return "http://open.spotify.com/artist/" + SpotifyURI.toURI(getByteUUID());
  }

  public String getName() {
    return name;
  }

  public void setName(String name) {
    this.name = name;
  }

  public Image getPortrait() {
    return portrait;
  }

  public void setPortrait(Image portrait) {
    this.portrait = portrait;
  }

  public Float getPopularity() {
    return popularity;
  }

  public void setPopularity(Float popularity) {
    this.popularity = popularity;
  }

  public List<Artist> getSimilarArtists() {
    return similarArtists;
  }

  public void setSimilarArtists(List<Artist> similarArtists) {
    this.similarArtists = similarArtists;
  }

  public List<Biography> getBiographies() {
    return biographies;
  }

  public void setBiographies(List<Biography> biographies) {
    this.biographies = biographies;
  }

  public Set<String> getGenres() {
    return genres;
  }

  public void setGenres(Set<String> genres) {
    this.genres = genres;
  }

  public List<String> getYearsActive() {
    return yearsActive;
  }

  public void setYearsActive(List<String> yearsActive) {
    this.yearsActive = yearsActive;
  }

  public List<Album> getAllAlbumsWithTrackPresent() {
    return allAlbumsWithTrackPresent;
  }

  public void setAllAlbumsWithTrackPresent(List<Album> allAlbumsWithTrackPresent) {
    this.allAlbumsWithTrackPresent = allAlbumsWithTrackPresent;
  }

  public List<Album> getMainArtistAlbums() {
    return mainArtistAlbums;
  }

  public void setMainArtistAlbums(List<Album> mainArtistAlbums) {
    this.mainArtistAlbums = mainArtistAlbums;
  }

  public List<Track> getTracks() {
    return tracks;
  }

  public void setTracks(List<Track> tracks) {
    this.tracks = tracks;
  }





  public static Artist fromXMLElement(XMLElement artistNode, Store store, Date fullyLoadedDate) {
    Artist artist = store.getArtist(artistNode.getChildText("id"));

    /* Set name. */
    if (artistNode.hasChild("name")) {
      artist.name = artistNode.getChildText("name");
    }

    /* Set portrait. */
    if (artistNode.hasChild("portrait")) {
      XMLElement portraitNode = artistNode.getChild("portrait");
      if (!"".equals(portraitNode.getText().trim())) {
        artist.portrait = Image.fromXMLElement(portraitNode, store);
      }
    }

    /* Set popularity. */
    if (artistNode.hasChild("popularity")) {
      artist.popularity = Float.parseFloat(artistNode.getChildText("popularity"));
    }

    XMLElement biosNode = artistNode.getChild("bios");
    if (biosNode != null) {

      List<Biography> biographies = new ArrayList<Biography>();

      for (XMLElement bioNode : biosNode.getChildren()) {
        if (!"bio".equals(bioNode.getElement().getNodeName())) {
          log.warn("Unknown bios child node " + bioNode.getElement().getNodeName());
        } else {
          Biography biography = new Biography();
          biography.setText(bioNode.getChildText("text"));
          if (bioNode.hasChild("portraits")) {
            biography.setPortraits(new ArrayList<Image>());
            for (XMLElement portraitNode : bioNode.getChild("portraits").getChildren()) {
              biography.getPortraits().add(Image.fromXMLElement(portraitNode, store));
            }
          }
          biographies.add(biography);
        }
        artist.biographies = biographies;
      }
    }

    if (artistNode.hasChild("years-active")) {
      artist.yearsActive = new ArrayList<String>(Arrays.asList(artistNode.getChildText("years-active").split(",")));
    }

    if (artistNode.hasChild("genres")) {
      String[] genreIds = artistNode.getChildText("genres").split(",");
      Set<String> genres = new LinkedHashSet<String>(genreIds.length);
      for (String genre : genreIds) {
        if (!"".equals(genre)) {
          genres.add(genre);
        }
      }
      artist.genres = genres;
    }

    XMLElement albumsNode = artistNode.getChild("albums");
    if (albumsNode != null) {
      List<Album> allAlbumsWithTrackPresent = new ArrayList<Album>();
      for (XMLElement albumNode : albumsNode.getChildren()) {
        Album album = Album.fromXMLElement(albumNode, store, fullyLoadedDate);

        // add to all albums
        allAlbumsWithTrackPresent.add(album);

        // add to main artist albums
        if (artist.equals(album.getMainArtist())) {
          if (artist.getMainArtistAlbums() == null) {
            artist.setMainArtistAlbums(new ArrayList<Album>());
          }
          if (!artist.getMainArtistAlbums().contains(album)) {
            artist.getMainArtistAlbums().add(album);
          }
        }

      }
      artist.allAlbumsWithTrackPresent = allAlbumsWithTrackPresent;
    }

    /* Set similar artists. */
    if (artistNode.hasChild("similar-artists")) {

      List<Artist> similarArtists = new ArrayList<Artist>();

      for (XMLElement similarArtistElement : artistNode.getChild("similar-artists").getChildren()) {
        similarArtists.add(Artist.fromXMLElement(similarArtistElement, store, null));
      }

      artist.setSimilarArtists(similarArtists);
    }

    if (fullyLoadedDate != null) {
      artist.setLoaded(fullyLoadedDate);
    }

    return artist;
  }


  @Override
  public String toString() {
    return "Artist{" +
        "id='" + id + '\'' +
        ", name='" + name + '\'' +
        ", portrait='" + portrait + '\'' +
        ", popularity=" + popularity +
        ", similarArtists=" + (similarArtists == null ? null : similarArtists.size()) +
        '}';
  }
}
