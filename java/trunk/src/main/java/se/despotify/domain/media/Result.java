package se.despotify.domain.media;

import se.despotify.domain.Store;
import se.despotify.util.XMLElement;

import java.io.Serializable;
import java.util.ArrayList;
import java.util.List;
import java.util.Date;

public class Result implements Serializable {

  private static final long serialVersionUID = 1L;


  private int version;
  private String query;
  private String suggestion;
  private int totalArtists;
  private int totalAlbums;
  private int totalTracks;
  private List<Artist> artists;
  private List<Album> albums;
  private List<Track> tracks;

  public Result() {
    this.version = 1;
    this.query = null;
    this.suggestion = null;
    this.totalArtists = 0;
    this.totalAlbums = 0;
    this.totalTracks = 0;
    this.artists = new ArrayList<Artist>();
    this.albums = new ArrayList<Album>();
    this.tracks = new ArrayList<Track>();
  }

  public int getVersion() {
    return this.version;
  }

  public String getQuery() {
    return this.query;
  }

  public String getSuggestion() {
    return this.suggestion;
  }

  public int getTotalArtists() {
    return this.totalArtists;
  }

  public int getTotalAlbums() {
    return this.totalAlbums;
  }

  public int getTotalTracks() {
    return this.totalTracks;
  }

  public List<Artist> getArtists() {
    return this.artists;
  }

  public List<Album> getAlbums() {
    return this.albums;
  }

  public List<Track> getTracks() {
    return this.tracks;
  }

  public void setQuery(String query) {
    this.query = query;
  }

  public void setSuggestion(String suggestion) {
    this.suggestion = suggestion;
  }

  public void setTotalArtists(int totalArtists) {
    this.totalArtists = totalArtists;
  }

  public void setTotalAlbums(int totalAlbums) {
    this.totalAlbums = totalAlbums;
  }

  public void setTotalTracks(int totalTracks) {
    this.totalTracks = totalTracks;
  }

  public void setArtists(List<Artist> artists) {
    this.artists = artists;
  }

  public void setAlbums(List<Album> albums) {
    this.albums = albums;
  }

  public void setTracks(List<Track> tracks) {
    this.tracks = tracks;
  }

  @Override
  public boolean equals(Object o) {
    if (this == o) return true;
    if (o == null || getClass() != o.getClass()) return false;

    Result result = (Result) o;

    return query.equals(result.query);

  }

  @Override
  public int hashCode() {
    return query.hashCode();
  }
}
