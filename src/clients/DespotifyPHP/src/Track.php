<?php

include_once('File.php');
include_once('global.php');

/**
* Represents a track
*/
class Track
{
	private $id;
	private $name;
	private $albumId;
	private $albumName;
	private $albumArtistId;
	private $albumArtistName;
	private $artistId;
	private $artistName;
	private $redirect;
	private $length;
	private $year;
	private $trackNumber;
	private $coverId;
	private $popularity;
	private $externalIds;
	private $allowedCountries;
	private $allowedCatalogues;
	private $files = array();
	
	private $connection;
	
	
	
	public function __construct($xmlOrId, $connection)
	{
		// Despotify gateway connection
		$this->connection = $connection;
		
		// faking multiple constructors, kind of
		if(is_a($xmlOrId, 'SimpleXMLElement')) // load from XML
		{
			$xmlObject = $xmlOrId;
			
			$this->trackFromXMLObject($xmlObject);
		}
		else // load from the track's id
		{
			$trackId = $xmlOrId;
			$this->trackFromId($trackId);
		}
	}
	
	
	/*
	* Populate this object's member variables by supplying an id. This id is sent to the gateway to retrieve the track's data.
	*/
	private function trackFromId($trackId)
	{
		// make sure its a 32 byte despotify id, not a 34 byte one
		$trackId = chopDespotifyId($trackId);
		
		if($this->connection->isConnected())
		{
			// make sure it's a valid id
			if(isValidDespotifyId($trackId))
			{
				$this->connection->write('browsetrack '. $trackId . "\n");
				
				if(($length = $this->connection->readHeader()) === false)
				{
					return false;
				}
				
				$xmlResult = $this->connection->read($length);
				
				
				$xmlObject = new SimpleXMLElement($xmlResult);
				
				
				// dirty fix
				if(is_array($xmlObject))
				{
					$xmlObject = $xmlObject[0];
				}
				
				
				$this->trackFromXMLObject($xmlObject);
			}
			else
			{
				echo 'invalid id: ' . $trackId . '<br/>';
				return false;
			}
		}
	}
	
	
	/*
	* Populate this object's member variables by extracting the data from XML.
	* @param xmlObject A SimpleXMLElement object holding track data
	*/
	private function trackFromXMLObject($xmlObject)
	{
		if($xmlObject->xpath('/result/tracks/track')) // because SearchResult's constructor sends a differently structured XML object
		{
			$xmlObject = $xmlObject->xpath('/result/tracks/track');
			$xmlObject = $xmlObject[0];
		}
		
		$xmlArray = (array)$xmlObject;

		
		// store data in member variables
		$this->id = $xmlArray['id'];
		$this->name = $xmlArray['title'];
		$this->albumId = $xmlArray['album-id'];
		$this->albumName = $xmlArray['album'];
		$this->albumArtistId = $xmlArray['album-artist-id'];
		$this->albumArtistName = $xmlArray['album-artist'];
		$this->artistId = $xmlArray['artist-id'];
		$this->artistName = $xmlArray['artist'];
		$this->redirect = $xmlArray['redirect'];
		$this->length = $xmlArray['length'];
		$this->year = $xmlArray['year'];
		$this->trackNumber = $xmlArray['track-number'];
		$this->coverId = $xmlArray['cover'];
		$this->popularity = $xmlArray['popularity'];
		$this->externalIds = $xmlArray['external-ids'];
		
		
		// restrictions
		$this->allowedCountries = $xmlObject->xpath('restrictions/restriction/@allowed');
		$this->allowedCountries = explode(',', (string)$this->allowedCountries[0]);
		$this->allowedCatalogues = $xmlObject->xpath('restrictions/restriction/@catalogues');
		$this->allowedCatalogues = explode(',', (string)$this->allowedCatalogues[0]);
		
		// external ids
		if(!empty($this->externalIds))
		{
			$this->externalIds = $this->externalIds->xpath('/external-ids');
		}

		// files
		//$fileArray = $xmlArray['files'];
		$fileArray = $xmlObject->xpath('files/file');
		if(is_array($fileArray))
		{
			foreach($fileArray as $currentFile)
			{
				array_push($this->files, new File($currentFile));
			}
		}
		
		
		// clean up some weird values
		if(count($this->allowedCountries) == 1 && $this->allowedCountries[0] == '') // empty string
		{
			$this->allowedCountries = NULL;
		}
		
		if(count($this->allowedCatalogues) == 1 && $this->allowedCatalogues[0] == '') // empty string
		{
			$this->allowedCatalogues = NULL;
		}
		
		//}
	}
	
	
	
	
	/* GETTERS */
	
	/**
	* Get Spotify HTTP address
	*
	* @return Spotify HTTP address, something like http://open.spotify.com/track/2URijinLBt1ECOe9Vw2NT6
	*/
	public function getHTTPLink()
	{
		return 'http://open.spotify.com/track/' . toSpotifyId($this->id);
	}
	
	
	/*
	* Get Spotify address
	*
	* @return Address conforming to the Spotify URI scheme, something like spotify:track:2URijinLBt1ECOe9Vw2NT6
	*/
	public function getSpotifyURI()
	{
		return 'spotify:track:' . toSpotifyId($this->id);
	}
	
	
	public function getId()
	{
		return $this->id;
	}
	
	
	public function getName()
	{
		return $this->name;
	}
	
	
	public function getAlbumId()
	{
		return $this->albumId;
	}
	
	
	public function getAlbumName()
	{
		return $this->albumName;
	}
	
	
	public function getAlbumArtistId()
	{
		return $this->albumArtistId;
	}
	
	
	public function getAlbumArtistName()
	{
		return $this->albumArtistName;
	}
	
	
	public function getArtistName()
	{
		return $this->artistName;
	}
	
	
	public function getArtistId()
	{
		return $this->artistId;
	}
	
	
	public function getRedirect()
	{
		return $this->redirect;
	}
	
	
	public function getLength()
	{
		return $this->length();
	}
	
	
	public function getYear()
	{
		return $this->year;
	}
	
	
	public function getTrackNumber()
	{
		return $this->trackNumber;
	}
	
	
	public function getCoverId()
	{
		return $this->coverId;
	}
	
	
	public function getPopularity()
	{
		return $this->popularity;
	}
	
	
	public function getExternalIds()
	{
		return $this->externalIds;
	}
	
	
	public function getAllowedCountries()
	{
		return $this->allowedCountries;
	}
	
	
	public function getAllowedCatalogues()
	{
		return $this->allowedCatalogues;
	}
	
	
	public function getFiles()
	{
		return $this->files;
	}
}

?>