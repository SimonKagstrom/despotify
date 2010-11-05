<?php

include_once('Artist.php');
include_once('Album.php');
include_once('Track.php');


/**
* Represents the resulting data from a search query
*/
class SearchResult
{
	// superfluous
	private $artistCount;
	private $albumCount;
	private $trackCount;
	
	private $artists = array();
	private $albums = array();
	private $tracks = array();
	
	/*public function __construct($tracks, $artistCount, $albumCount, $trackCount)
	{
		$this->tracks = $tracks;
		$this->artistCount = $artistCount;
		$this->albumCount = $albumCount;
		$this->trackCount = $trackCount;
	}*/
	
	public function __construct($xmlData)
	{
		// object holding the XML data
		$xml = new SimpleXMLElement($xmlData);
		
		// extract and store the data
		$this->artistCount = $xml->xpath('/result/total-artists');
		$this->artistCount = (int)$this->artistCount[0][0];
		$this->albumCount = $xml->xpath('/result/total-albums[1]');
		$this->albumCount = (int)$this->albumCount[0][0];
		$this->trackCount = $xml->xpath('/result/total-tracks[0]');
		$this->trackCount = (int)$this->trackCount[0][0];
		
		// artists
		$artistArray = $xml->xpath('/result/artists/artist');
		foreach($artistArray as $currentArtist)
		{
			array_push($this->artists, new Artist($currentArtist));
		}
		
		
		// albums
		$albumArray = $xml->xpath('/result/albums/album');
		foreach($albumArray as $currentAlbum)
		{
			array_push($this->albums, new Album($currentAlbum));
		}
		
		
		// tracks
		$trackArray = $xml->xpath('/result/tracks/track');
		foreach($trackArray as $currentTrack)
		{
			array_push($this->tracks, new Track($currentTrack));
		}
	}
}

?>