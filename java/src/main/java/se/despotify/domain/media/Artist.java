package se.despotify.domain.media;

import se.despotify.domain.Store;
import se.despotify.util.SpotifyURI;
import se.despotify.util.XMLElement;

public class Artist extends Media implements Visitable {
	private String       name;
	private String       portrait;
	private Float        popularity;
	private MediaList<Artist> similarArtists;

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

  public String getPortrait() {
    return portrait;
  }

  public void setPortrait(String portrait) {
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

  public static Artist fromXMLElement(XMLElement artistElement, Store store){
		Artist artist = store.getArtist(artistElement.getChildText("id"));

		/* Set name. */
		if(artistElement.hasChild("name")){
			artist.name = artistElement.getChildText("name");
		}
		
		/* Set portrait. */
		if(artistElement.hasChild("portrait") && artistElement.getChild("portrait").hasChild("id")){
			artist.portrait = artistElement.getChild("portrait").getChildText("id");
		}
		
		/* Set popularity. */
		if(artistElement.hasChild("popularity")){
			artist.popularity = Float.parseFloat(artistElement.getChildText("popularity"));
		}
		
		/* Set similar artists. */
		if(artistElement.hasChild("similar-artists")){
      if (artist.similarArtists == null) {
        artist.similarArtists = new MediaList<Artist>();
      }
			for(XMLElement similarArtistElement : artistElement.getChild("similar-artists").getChildren()){
        Artist similarArtist = Artist.fromXMLElement(similarArtistElement, store);
        if (!artist.getSimilarArtists().contains(similarArtist)) {
          artist.getSimilarArtists().add(similarArtist);
        }
      }
      // todo remove deleted?
		}
		
		/* TODO bios, genres, years-active, albums, ... */

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
        "id='" + getHexUUID()+ '\'' +
        ", name='" + name + '\'' +
        ", portrait='" + portrait + '\'' +
        ", popularity=" + popularity +
        ", similarArtists=" + similarArtists +
        '}';
  }
}
