package se.despotify.domain.media;

import se.despotify.domain.Store;
import se.despotify.util.SpotifyURI;
import se.despotify.util.XMLElement;

import java.util.ArrayList;
import java.util.List;
import java.util.Arrays;

public class Artist extends Media implements Visitable {
  private String name;
  private Image portrait;
  private Float popularity;
  private MediaList<Artist> similarArtists;
  private List<Biography> biographies;
  private List<String> generes;
  private List<String> yearsActive;
  private MediaList<Album> albums;

  public Artist() {
    super();
  }

  public Artist(byte[] UUID) {
    super(UUID);
  }

  public Artist(byte[] UUID, String hexUUID) {
    super(UUID, hexUUID);
  }

  public Artist(String hexUUID) {
    super(hexUUID);
  }

  @Override
  public void accept(Visitor visitor) {
    visitor.visit(this);
  }

  @Override
  public String getSpotifyURL() {
    return "spotify:artist:" + SpotifyURI.toURI(getUUID());
  }

  @Override
  public String getHttpURL() {
    return "http://open.spotify.com/artist/" + SpotifyURI.toURI(getUUID());
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

  public MediaList<Artist> getSimilarArtists() {
    return similarArtists;
  }

  public void setSimilarArtists(MediaList<Artist> similarArtists) {
    this.similarArtists = similarArtists;
  }

  public List<Biography> getBiographies() {
    return biographies;
  }

  public void setBiographies(List<Biography> biographies) {
    this.biographies = biographies;
  }

  public List<String> getGeneres() {
    return generes;
  }

  public void setGeneres(List<String> generes) {
    this.generes = generes;
  }

  public List<String> getYearsActive() {
    return yearsActive;
  }

  public void setYearsActive(List<String> yearsActive) {
    this.yearsActive = yearsActive;
  }

  public MediaList<Album> getAlbums() {
    return albums;
  }

  public void setAlbums(MediaList<Album> albums) {
    this.albums = albums;
  }

  public static Artist fromXMLElement(XMLElement artistNode, Store store) {
    Artist artist = store.getArtist(artistNode.getChildText("id"));

    /* Set name. */
    if (artistNode.hasChild("name")) {
      artist.name = artistNode.getChildText("name");
    }

    /* Set portrait. */
    if (artistNode.hasChild("portrait")) {
      artist.portrait = Image.fromXMLElement(artistNode.getChild("portrait"), store);
    }

    /* Set popularity. */
    if (artistNode.hasChild("popularity")) {
      artist.popularity = Float.parseFloat(artistNode.getChildText("popularity"));
    }

    /* Set similar artists. */
    if (artistNode.hasChild("similar-artists")) {

      MediaList<Artist> similarArtists = new MediaList<Artist>();

      for (XMLElement similarArtistElement : artistNode.getChild("similar-artists").getChildren()) {
        similarArtists.add(Artist.fromXMLElement(similarArtistElement, store));
      }

      artist.setSimilarArtists(similarArtists);
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
            biography.setPortraits(new MediaList<Image>());
            for (XMLElement portraitNode : bioNode.getChild("portraits").getChildren()) {
              biography.getPortraits().add(Image.fromXMLElement(portraitNode, store));
            }
          }
        }
        artist.biographies = biographies;
      }
    }

    if (artistNode.hasChild("years-active")) {
      artist.yearsActive = new ArrayList<String>(Arrays.asList(artistNode.getChildText("years-active").split(",")));
    }

    if (artistNode.hasChild("genres")) {
      artist.yearsActive = new ArrayList<String>(Arrays.asList(artistNode.getChildText("generes").split(",")));
    }

    XMLElement albumsNode = artistNode.getChild("albums");
    if (albumsNode != null) {
      MediaList<Album> albums = new MediaList<Album>();
      for (XMLElement albumNode : albumsNode.getChildren())  {
        albums.add(Album.fromXMLElement(albumNode, store));
      }
      artist.albums = albums;
    }

    return artist;
  }


  public static Artist fromURI(String uri) {
    Artist artist = new Artist();

    artist.setUUID(SpotifyURI.toHex(uri));

    return artist;
  }

  @Override
  public String toString() {
    return "Artist{" +
        "id='" + getHexUUID() + '\'' +
        ", name='" + name + '\'' +
        ", portrait='" + portrait + '\'' +
        ", popularity=" + popularity +
        ", similarArtists=" + similarArtists +
        '}';
  }
}
