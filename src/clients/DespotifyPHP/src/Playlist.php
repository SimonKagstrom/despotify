<?php

include_once('global.php');


class Playlist
{
	private $id;
	private $name;
	private $trackIds = array();
	private $tracks = array(); // these are loaded for the first time when they're requested
	private $length;
	private $rid; // don't even know what this is. user id perhaps?
	private $username;
	private $isPublic;
	
	private $connection;
	
	
	public function __construct($xmlOrId, $connection = NULL)
	{
		// Despotify gateway connection
		$this->connection = $connection;
		
		if(is_a($xmlOrId, 'SimpleXMLElement')) // load from XML
		{
			$xmlObject = $xmlOrId;
			$this->playlistFromXMLObject($xmlObject);
		}
		else // load from id
		{
			$id = $xmlOrId;
			$this->playlistFromId($id);
		}
	}
	
	
	private function playlistFromId($id)
	{
		$this->connection->write('playlist ' . $id . "\n");
		
		if(($length = $this->connection->readHeader()) === false) 
		{
			return false;
		}
		
		
		$xmlData = $this->connection->read($length);
		$xmlData = str_replace("\n", '', $xmlData); // remove some oddly place newlines from the XML data
		
		$xmlObject = new SimpleXMLElement($xmlData);
		
		$this->id = $id;
		$this->playlistFromXMLObject($xmlObject);
	}
	
	
	private function playlistFromXMLObject($xmlObject)
	{
		// read and store in member variables
		$this->name = $xmlObject->xpath('next-change/change/ops/name');
		$this->name = (string)$this->name[0][0];
		
		$this->username = $xmlObject->xpath('next-change/change/ops/user');
		$this->username = (string)$this->username[0][0];
		
		$this->trackIds = $xmlObject->xpath('next-change/change/ops/add/items');
		$this->trackIds = explode(',', (string)$this->trackIds[0]);
		
		$this->length = $xmlObject->xpath('next-change/change/time');
		$this->length = (string)$this->length[0];
		
		$this->isPublic = $xmlObject->xpath('next-change/change/ops/pub');
		$this->isPublic = (string)$this->isPublic[0];
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
	
	
	public function getTrackIds()
	{
		return $this->trackIds;
	}
	
	
	public function getTracks()
	{
		if(empty($this->tracks)) // tracks not loaded. load them and then return them
		{
			foreach($this->trackIds as $currentTrackId)
			{
				array_push($this->tracks, new Track($currentTrackId, $this->connection));
			}
		}

		return $this->tracks;
	}
	
	
	public function isPublic()
	{
		return $this->isPublic();
	}
	
	
	public function getUsername()
	{
		return $this->username;
	}
}


?>