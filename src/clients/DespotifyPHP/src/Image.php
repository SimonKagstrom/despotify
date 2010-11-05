<?php

include_once('global.php');

/**
* Represents an image
*/
class Image
{
	private $id;
	private $width;
	private $height;
	private $image;
	
	private $connection;
	
	/*
	* Represents all kinds of images - portraits, cover art...
	*/
	public function __construct($xmlOrId, $connection)
	{
		$this->connection = $connection;
		
		if(is_a($xmlOrId, 'SimpleXMLElement')) // load from XML object
		{
			if(DEBUG_MODE) debug_msg('Creating Image object from XML object');
			$xmlObject = $xmlOrId;
			$this->imageFromXMLObject($xmlObject);
		}
		else // load from id. images currently cannot be loaded from id
		{
			if(DEBUG_MODE) debug_msg('Creating Image object from id');
			$id = $xmlOrId;
			$this->imageFromId($id);
		}
	}
	
	
	/*
	* Populate the object's member variables with data from XML
	*/
	private function imageFromXMLObject($xmlObject)
	{
		$xmlArray = (array)$xmlObject;
		
		$this->id = $xmlArray['id'];
		$this->width = $xmlArray['width'];
		$this->height = $xmlArray['height'];
	}
	
	
	
	private function imageFromId($imageId)
	{
		if($this->connection->isConnected())
		{
			// make sure it's a valid id
			//if(isValidDespotifyId($trackId))
			//{
				
				if(DEBUG_MODE) debug_msg('writing:' .'image ' . $imageId . '\n');

				$this->connection->write('image ' . $imageId . "\n");
				
				
				if(($length = $this->connection->readHeader()) === false)
				{
					return false;
				}
				
				if(DEBUG_MODE) debug_msg('received image data');
				
				$imageData = $this->connection->read($length);
				
				$this->image = $imageData;
				
				
				//$this->imageFromXMLObject($xmlObject);
			/*}
			else
			{
				echo 'invalid id: ' . $imageId . '<br/>';
				return false;
			}
			*/
		}
		else
		{
			return false;
		}
	}
	
	
	public static function getImageData($id, $connection)
	{
		if($connection->isConnected())
		{
			$connection->write('image '. $id . "\n");
			
			if(($length = $connection->readHeader()) === false)
			{
				return false;
			}
			
			$result = $connection->read($length);
		}
		else
		{
			echo 'Not connected';
		}
		
		return $result;
	}
	
	
	/*
	* Requests the image data from the gateway, and stores it internally. Use getImage() to get the loaded image.
	*/
	private function loadImage()
	{
		$this->image = Image::getImageData($this->id, $this->connection);
	}
	
	
	/*
	* @return String representation of the image data. If the image has not yet been loaded, it is loaded.
	*/
	public function getData()
	{
		//load the image, unless it already has been loaded
		if($this->image == '')
		{
			$this->loadImage();
		}
		
		return $this->image;
	}
	
	
	public function getId()
	{
		return $this->id;
	}
	
	
	public function getWidth()
	{
		return $this->width;
	}
	
	
	public function getHeight()
	{
		return $this->height;
	}
}

?>