package se.despotify.domain.media;

import se.despotify.domain.Store;
import se.despotify.util.Hex;
import se.despotify.util.SpotifyURI;
import se.despotify.util.XMLElement;

import javax.persistence.*;
import java.util.ArrayList;
import java.util.Date;
import java.util.List;

@Entity
public class Track extends RestrictedMedia {

  private static final long serialVersionUID = 1L;

  @Column(length = 1024)
  private String title;
  @ManyToOne(cascade = {CascadeType.MERGE, CascadeType.PERSIST, CascadeType.REFRESH})
  private Artist artist;
  @ManyToOne(cascade = {CascadeType.MERGE, CascadeType.PERSIST, CascadeType.REFRESH})
  private Album album;

  @ManyToOne(cascade = {CascadeType.MERGE, CascadeType.PERSIST, CascadeType.REFRESH})
  private Album composition;

  private Integer year;
  private Integer discNumber;
  private Integer trackNumber;
  private Integer length;

  @ManyToMany(cascade = {CascadeType.MERGE, CascadeType.PERSIST, CascadeType.REFRESH})
  private List<File> files;

  @ManyToOne(cascade = {CascadeType.MERGE, CascadeType.PERSIST, CascadeType.REFRESH})
  private Image cover;

  private Float popularity;

  @ManyToMany(cascade = {CascadeType.MERGE, CascadeType.PERSIST, CascadeType.REFRESH})
  private List<Track> similarTracks;

  /**
   * If this instance of track has been replaced with another track.
   */
  @ManyToMany(cascade = {CascadeType.MERGE, CascadeType.PERSIST, CascadeType.REFRESH})
  private List<Track> redirections;

  @OneToMany(cascade = {CascadeType.MERGE, CascadeType.PERSIST, CascadeType.REFRESH})
  private List<ExternalId> externalIds;

  public Track() {
  }

  public Track(byte[] UUID) {
    this(Hex.toHex(UUID));
  }

  public Track(String hexUUID) {
    this.id = hexUUID;
  }

  public Track(String hexUUID, String title, Artist artist, Album album) {
    this(hexUUID);
    this.title = title;
    this.artist = artist;
    this.album = album;
  }

  public Track(byte[] UUID, String title, Artist artist, Album album) {
    this(UUID);
    this.title = title;
    this.artist = artist;
    this.album = album;
  }

  @Override
  public void accept(Visitor visitor) {
    visitor.visit(this);
  }

  @Override
  public String getSpotifyURI() {
    return "spotify:track:" + SpotifyURI.toURI(getByteUUID());
  }

  @Override
  public String getHttpURL() {
    return "http://open.spotify.com/track/" + SpotifyURI.toURI(getByteUUID());
  }

  public Integer getDiscNumber() {
    return discNumber;
  }

  public void setDiscNumber(Integer discNumber) {
    this.discNumber = discNumber;
  }

  public String getTitle() {
    return title;
  }

  public void setTitle(String title) {
    this.title = title;
  }

  public Artist getArtist() {
    return artist;
  }

  public void setArtist(Artist artist) {
    this.artist = artist;
  }

  public Album getAlbum() {
    return album;
  }

  public void setAlbum(Album album) {
    this.album = album;
  }

  public Integer getYear() {
    return year;
  }

  public void setYear(Integer year) {
    this.year = year;
  }

  public Integer getTrackNumber() {
    return trackNumber;
  }

  public void setTrackNumber(Integer trackNumber) {
    this.trackNumber = trackNumber;
  }

  public Integer getLength() {
    return length;
  }

  public void setLength(Integer length) {
    this.length = length;
  }

  public List<Track> getRedirections() {
    return redirections;
  }

  public void setRedirections(List<Track> redirections) {
    this.redirections = redirections;
  }

  public List<File> getFiles() {
    return files;
  }

  public void setFiles(List<File> files) {
    this.files = files;
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

  public List<ExternalId> getExternalIds() {
    return externalIds;
  }

  public void setExternalIds(List<ExternalId> externalIds) {
    this.externalIds = externalIds;
  }

  public List<Track> getSimilarTracks() {
    return similarTracks;
  }

  public void setSimilarTracks(List<Track> similarTracks) {
    this.similarTracks = similarTracks;
  }

  public Album getComposition() {
    return composition;
  }

  public void setComposition(Album composition) {
    this.composition = composition;
  }


  @Override
  public String toString() {
    return "Track{" +
        "id='" + id + '\'' +
        ", title='" + title + '\'' +
        ", artist=" + (artist == null ? null : artist.getId()) +
        ", album=" + (album == null ? null : album.getId()) +
        ", year=" + year +
        ", discNumber=" + discNumber +
        ", trackNumber=" + trackNumber +
        ", length=" + length +
        ", files=" + files +
        ", cover='" + cover + '\'' +
        ", popularity=" + popularity +
        '}';
  }

}
