<?php

include_once('global.php');
include_once('Image.php');

class Artist
{
	private $id;
	private $name;
	private $portraits = array();
	private $popularity;
	private $restrictions = array();
	private $externalIds;
	private $biography;
	private $genres;
	private $albums = array();
	
	private $connection;
	
	
	public function __construct($id, $connection)
	{
		$this->connection = $connection;
		
		if($this->connection->isConnected())
		{
			// make sure it's a valid id
			if(isValidDespotifyId($id))
			{
				$this->connection->write('browseartist '. $id . "\n");
				
				if(($length = $this->connection->readHeader()) === false)
				{
					return false;
				}
				
				$xmlData = $this->connection->read($length);
				//die($xmlData);
				$xmlData = str_replace("\n", '', $xmlData); // remove some oddly place newlines from the XML data
				$xmlObject = new SimpleXMLElement($xmlData, LIBXML_NOCDATA); // LIBXML_NOCDATA converts CDATA into string. if this is not done, 
				
				
				$this->artistFromXMLObject($xmlObject);
			}
			else
			{
				echo 'invalid id: ' . $id . '<br/>';
				return false;
			}
		}
		else
		{
			echo 'Is not connected';
		}
	}
	
	
	private function artistFromXMLObject($xmlObject)
	{
		//var_dump($xmlObject);
		echo "\n\n\n\n\n\n\n\n\n\n\n\n\n\n";
		// turn the SimpleXMLElement object into an array
		$xmlArray = (array)$xmlObject;
		
		// store data in member variables
		$this->id = (string)$xmlArray['id'];
		$this->name = (string)$xmlArray['name'];
		$this->popularity = (string)$xmlArray['popularity'];
		$this->restrictions = $xmlArray['restrictions']; // TODO: figure out what to do with this
		$this->externalIds = (array)$xmlArray['external-ids'];
		$portraitArray = $xmlObject->xpath('bios/bio/portraits/portrait');
		
		foreach($portraitArray as $portraitXML)
		{
			array_push($this->portraits, new Image($portraitXML, $this->connection));
		}
		
		$this->biography = $xmlObject->xpath('/artist/bios/bio/text');
		$this->biography = (string)$this->biography[0];
		
		$this->genres = $xmlObject->xpath('/artist/genres');
		$this->genres = explode(',', (string)$this->genres[0]);
		
		$albumXMLObjects = $xmlObject->xpath('/artist/albums/album');
		foreach($albumXMLObjects as $album)
		{
			array_push($this->albums, new Album($album, $this->connection));
		}
		
		// TODO: läs in album och låtar
	}
	
	
	
	/* GETTERS */
	
	public function getId()
	{
		return $this->id;
	}
	
	
	public function getName()
	{
		return $this->name;
	}
	
	
	public function getPortraits()
	{
		return $this->portraits;
	}
	
	
	public function getPopularity()
	{
		return $this->popularity;
	}
	
	
	public function getRestrictions()
	{
		return $this->restrictions;
	}
	
	
	public function getExternalIds()
	{
		return $this->externalIds;
	}
	
	
	public function getBiography()
	{
		return $this->biography;
	}
	
	
	public function getGenres()
	{
		return $this->genres;
	}
	
	
	public function getAlbums()
	{
		return $this->id;
	}
}

?>