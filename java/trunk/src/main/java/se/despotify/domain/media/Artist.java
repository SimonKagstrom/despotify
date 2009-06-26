package se.despotify.domain.media;

import org.hibernate.annotations.CollectionOfElements;
import se.despotify.domain.Store;
import se.despotify.util.Hex;
import se.despotify.util.SpotifyURI;
import se.despotify.util.XMLElement;

import javax.persistence.*;
import java.util.*;

@Entity
public class Artist extends RestrictedMedia {

  private static final long serialVersionUID = 1L;

  private String name;

  @ManyToOne(cascade = {CascadeType.MERGE, CascadeType.PERSIST, CascadeType.REFRESH})
  private Image portrait;

  @ManyToMany(cascade = {CascadeType.MERGE, CascadeType.PERSIST, CascadeType.REFRESH})
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

  @Override
  public String toString() {
    return "Artist{" +
        "id='" + id + '\'' +
        ", name='" + name + '\'' +
        ", portrait='" + portrait + '\'' +
        ", similarArtists=" + (similarArtists == null ? null : similarArtists.size()) +
        '}';
  }
}
