<?php

include_once('global.php');
include_once('Disc.php');


class Album
{
	private $id;
	private $name;
	private $artistId;
	private $artistName;
	private $coverId = array();
	private $allowedCountries;
	private $allowedCatalogues;
	private $discs = array();
	
	private $connection;
	
	
	public function __construct($xmlOrId, $connection)
	{
		$this->connection = $connection;
		
		if(is_a($xmlOrId, 'SimpleXMLElement')) // load from XML
		{
			$xmlObject = $xmlOrId;
			$this->albumFromXMLObject($xmlObject);
		}
		else
		{
			$id = $xmlOrId;
			$this->albumFromId($id);
		}
	}
	
	
	private function albumFromXMLObject($xmlObject)
	{
		//var_dump($xmlObject);die();
		// turn the SimpleXMLElement into an array
		$xmlArray = (array)$xmlObject;
		
		// store data in member variables
		$this->id = (string)$xmlArray['id'];
		$this->name = (string)$xmlArray['name'];
		$this->artistId = (string)$xmlArray['artist-id'];
		$this->artistName = (string)$xmlArray['artist'];
		$this->coverId = $xmlArray['cover'];
		
		// restrictions
		$this->allowedCountries = $xmlObject->xpath('restrictions/restriction/@allowed');
		$this->allowedCountries = explode(' ', (string)$this->allowedCountries[0]);
		$this->allowedCatalogues = $xmlObject->xpath('restrictions/restriction/@catalogues');
		$this->allowedCatalogues = explode(',', (string)$this->allowedCatalogues[0]);
		
		if($xmlArray['discs'])
		{
			foreach($xmlArray['discs'] as $discXMLObject)
			{
				array_push($this->discs, new Disc($discXMLObject, $this->connection));
			}
		}
		
		debug_msg('test');
		
		if(DEBUG_MODE)
		{
			debug_msg('Loaded album data from XML object');
		}
	}
	
	
	
	private function albumFromId($id)
	{
		$this->connection->write('browsealbum ' . $id . "\n");
		
		if(($length = $this->connection->readHeader()) === false) 
		{
			return false;
		}
		
		
		
		$xmlData = $this->connection->read($length);
		
		
		echo '####\nReceived XML data for album with id '.$id."\n".$xmlData."######";
		//var_dump($xmlData);die();
		$xmlData = str_replace("\n", '', $xmlData); // remove some oddly place newlines in the XML data
		
		$xmlObject = new SimpleXMLElement($xmlData);
		
		$this->id = $id;
		$this->albumFromXMLObject($xmlObject);
	}
	
	
	
	
	/* GETTERS */
	
	public function getName()
	{
		return $this->name;
	}
	
	
	public function getId()
	{
		return $this->id;
	}
	
	
	public function getArtistId()
	{
		return $this->artistId;
	}
	
	
	public function getArtistName()
	{
		return $this->artistName;
	}
	
	
	public function getCoverId()
	{
		return $this->coverId;
	}
	
	
	/**
	* Convenience function for getting cover art
	* @return Image object representing this album's cover art
	*/
	public function getCover()
	{
		if($this->coverId)
		{
			if(DEBUG_MODE)
			{
				debug_msg("About to create Image object to return album's cover");
			}
			
			
			return new Image($this->coverId, $this->connection);
		}
		else
		{
			return false;
		}
	}
	
	
	public function getAllowedCountries()
	{
		return $this->allowedCountries;
	}
	
	
	public function getAllowedCatalogues()
	{
		return $this->allowedCatalogues;
	}
	
	
	/**
	* @return An array with the discs this album consists of. The discs contains the tracks on this album.
	*/
	public function getDiscs()
	{
		return $this->discs;
	}
	
	
	/**
	* Get Spotify HTTP address
	*
	* @return Spotify HTTP address, something like http://open.spotify.com/album/2URijinLBt1ECOe9Vw2NT6
	*/
	public function getHTTPLink()
	{
		return 'http://open.spotify.com/album/' . toSpotifyId($this->id);
	}
}

?>