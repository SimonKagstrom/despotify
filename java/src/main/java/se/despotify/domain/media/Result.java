package se.despotify.domain.media;

import se.despotify.domain.Store;
import se.despotify.util.XMLElement;

public class Result {
	private int          version;
	private String       query;
	private String       suggestion;
	private int          totalArtists;
	private int          totalAlbums;
	private int          totalTracks;
	private MediaList<Artist> artists;
	private MediaList<Album>  albums;
	private MediaList<Track>  tracks;
	
	public Result(){
		this.version      = 1;
		this.query        = null;
		this.suggestion   = null;
		this.totalArtists = 0;
		this.totalAlbums  = 0;
		this.totalTracks  = 0;
		this.artists      = new MediaList<Artist>();
		this.albums       = new MediaList<Album>();
		this.tracks       = new MediaList<Track>();
	}
	
	public int getVersion(){
		return this.version;
	}
	
	public String getQuery(){
		return this.query;
	}
	
	public String getSuggestion(){
		return this.suggestion;
	}
	
	public int getTotalArtists(){
		return this.totalArtists;
	}
	
	public int getTotalAlbums(){
		return this.totalAlbums;
	}
	
	public int getTotalTracks(){
		return this.totalTracks;
	}
	
	public MediaList<Artist> getArtists(){
		return this.artists;
	}
	
	public MediaList<Album> getAlbums(){
		return this.albums;
	}
	
	public MediaList<Track> getTracks(){
		return this.tracks;
	}
	
	public static Result fromXMLElement(XMLElement resultElement, Store store){

		Result result = new Result();

		if(resultElement.hasChild("version")){
			result.version = Integer.parseInt(resultElement.getChildText("version"));
		}

    // todo only at search time?
		if(resultElement.hasChild("did-you-mean")){
			result.suggestion = resultElement.getChildText("did-you-mean");
		}
		
		// set result quantities
		if(resultElement.hasChild("total-artists")) {
      result.totalArtists = Integer.parseInt(resultElement.getChildText("total-artists"));
    }
		if(resultElement.hasChild("total-albums")) {
      result.totalAlbums  = Integer.parseInt(resultElement.getChildText("total-albums"));
    }
    if(resultElement.hasChild("total-tracks")){
      result.totalTracks  = Integer.parseInt(resultElement.getChildText("total-tracks"));
    }


		if(resultElement.hasChild("artists")){
			for(XMLElement artistElement : resultElement.getChild("artists").getChildren()){
				result.getArtists().add(Artist.fromXMLElement(artistElement, store));
			}
		}
		if(resultElement.hasChild("albums")){
			for(XMLElement albumElement : resultElement.getChild("albums").getChildren()){
				result.getAlbums().add(Album.fromXMLElement(albumElement, store));
			}
		}
		if(resultElement.hasChild("tracks")){
			for(XMLElement trackElement : resultElement.getChild("tracks").getChildren()){
        Track track = Track.fromXMLElement(trackElement, store);        
        result.getTracks().add(track);
			}
		}

    return result;
		
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
