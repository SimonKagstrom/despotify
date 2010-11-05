<?php

include_once('global.php');
include_once('Track.php');

class Disc
{
	private $id;
	private $name;
	private $number;
	private $tracks = array();
	
	private $connection;
	
	
	public function __construct($xmlObject, $connection)
	{
		$this->connection = $connection;
		
		$this->discFromXMLObject($xmlObject);
	}
	
	
	private function discFromXMLObject($xmlObject)
	{
		// turn the SimpleXMLElement into an array
		$xmlArray = (array)$xmlObject;
		
		$this->name = (string)$xmlArray['name'];
		$this->number = $xmlArray['disc-number'];
		
		foreach($xmlArray['track'] as $trackObject)
		{
			array_push($this->tracks, new Track($trackObject, $this->connection));
		}
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
	
	
	public function getNumber()
	{
		return $this->number;
	}
	
	
	public function getTracks()
	{
		return $this->tracks;
	}
}

?>