<?php

class File
{
	private $id;
	private $audioFormat;
	private $audioBitrate;
	
	
	public function __construct($xmlObject)
	{
		// turn the SimpleXMLElement into an array
		$xmlArray = (array)$xmlObject;
		$xmlArray = $xmlArray['@attributes'];
		
		// store data in member variables
		sscanf((string)$xmlArray['format'], '%[^,],%d,%d,%d,%d', $format, $bitrate, $x1, $x2, $x3); // ignoring $x1, $x2 and $x3 because I don't know what these values are
		$this->id = (string)$xmlArray['id'];
		$this->audioFormat = $format;
		$this->audioBitrate = $bitrate;
	}
	
	
	public function getId()
	{
		return $this->id;
	}
	
	
	public function getAudioFormat()
	{
		return $this->audioFormat;
	}
	
	
	public function getAudioBitrate()
	{
		return $this->audioBitrate;
	}
}

?>