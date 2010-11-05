<?php

include_once('Connection.php');
include_once('SearchResult.php');
include_once('Playlist.php');
include_once('global.php');



/**
* This class is the interface you use to communicate with the Despotify gateway. You most likely want to use this class...
*/
class Despotify
{
	private $gateway;
	private $port;
	private $socket;
	private $connection;
	
	private $sessionId;
	
	
	
	/**
	* Constructs a Despotify object. The Despotify object is used to communicate with the Despotify gateway.
	* @param gateway The host address of the gateway
	* @param port Listening port of the gateway
	*/
	public function __construct($gateway, $port)
	{
		$this->gateway = $gateway;
		$this->port = $port;
	}
	
	
	/**
	* Attempt to connect to the Despotify gateway
	*/
	public function connect()
	{
		$this->connection = new Connection($this->gateway, $this->port);
		
		return $this->connection->connect();
	}
	
	
	/**
	* Disconnect from the Despotify gateway
	*/
	function disconnect()
	{
		$this->connection->disconnect();
	}
	
	
	/**
	* Tell gateway to attempt to log on to Spotify
	* @param username Spotify premium account username
	* @param password Spotify premium account password
	*
	* @return Session id if successful, boolean false if failed
	*/
	function login($username, $password)
	{
		if($this->isConnected()) // connected
		{
			$this->connection->write(sprintf("login %.20s %.20s\n", $username, $password));
			
			
			$length = $this->connection->readHeader();
			
			if($length === false)
			{
				return false;
			}
			
			// request session id from gateway
			$this->connection->write("id\n");
			
			if(($length = $this->connection->readHeader()) === false)
			{
				return false;
			}
			
			// store the returned session id
			$this->sessionId = $this->connection->read($length);
			
			return $this->sessionId;
		}
		else // not connected
		{
			return false;
		}
	}
	
	
	public function isConnected() {
		return $this->connection->isConnected();
	}
	
	
	/**
	* Check whether you're logged on, i.e. whether the current session is active
	*/
	public function isLoggedIn() {
		if ($this->sessionId != '')
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	
	
	/**
	* Specify session to be used
	* @param sessionId Id of the session to be used
	*/
	public function setSession($sessionId) {
		if($this->connection->isConnected())
		{
			$this->connection->write(sprintf("session %s\n", $sessionId));
			if (($length = $this->connection->readHeader()) === false) 
			{
				return false;
			}
			
			$this->sessionid = $sessionId;
			
			return true;
		}
		else
		{
			return false;
		}
	}
	
	
	/* FUNCTIONS GETTING SPOTIFY DATA */
	
	/**
	* Search Spotify for music
	* 
	* @param string Search string
	*/
	function search($string)
	{
		if ($this->connection->isConnected())
		{
			$this->connection->write('search '.$string."\n");
			
			if(($length = $this->connection->readHeader()) === false)
			{
				return false;
			}
			
			// read the XML response from gateway
			$data = $this->connection->read($length);
			
			// store result in a SearchResult object
			$result = new SearchResult($data);
			
			return $result;
		}
		else
		{
			return false;
		}
	}
	
	
	/**
	* Get the Despotify identifiers of all of the logged on user's playlists
	* @return Array of Despotify ids in case of success. Returns boolean false if not connected to the gateway.
	*/
	function getPlaylistIds()
	{
		if($this->connection->isConnected())
		{
			$playlistId = "0000000000000000000000000000000000"; // 34 zeroes means we want the gateway to send us the identifiers for all playlists
			
			
			$this->connection->write('playlist '.$playlistId."\n");
			
			if(($length = $this->connection->readHeader()) === false) 
			{
				return false;
			}
			
			
			$xmlData = $this->connection->read($length);
			$xmlData = str_replace("\n", '', $xmlData); // remove some oddly place newlines in the XML data
			
			$xml = new SimpleXMLElement($xmlData);
			$playListIdCommaSeparatedString = $xml->xpath("/playlist/next-change/change/ops/add/items");
			$playListIdCommaSeparatedString = (string)$playListIdCommaSeparatedString[0];
			$playListIdArray = explode(",", $playListIdCommaSeparatedString);
				
			return $playListIdArray;
		}
		else
		{
			return false;
		}
	}
	
	
	/**
	* Instantiates and returns a Playlist object
	* @param playlistId The Despotify id of the playlist
	* @return Playlist object on success. Returns boolean false if not connected to the gateway.
	*/
	public function getPlaylist($playlistId)
	{
		$playlistId = trim($playlistId);
		
		if(empty($playlistId))
		{
			return false;
		}
		
		if($this->isConnected())
		{
			$this->connection->write('playlist ' . $playlistId . "\n");
			
			if(($length = $this->connection->readHeader()) === false) 
			{
				return false;
			}
			
			$xmlData = $this->connection->read($length);
			$xmlData = str_replace("\n", '', $xmlData);// to remove some oddly place newlines from the XML
			
			$xml = new SimpleXMLElement($xmlData);
			
			$playlistObject = new Playlist($xml, $this->connection);
			
			return $playlistObject;
		}
		else
		{
			return false;
		}
	}
	
	
	
	/**
	* Convenience function for getting the user's playlists.
	* @returns All user's playlists as Playlist objects
	*/
	public function getPlaylists()
	{
		$playlistIds = $this->getPlaylistIds();
		
		$playlistArray = array(); // to hold the Playlist objects
		
		if(!$playlistIds)
		{
			return false;
		}
		
		foreach($playlistIds as $playlistId)
		{
			array_push($playlistArray, new Playlist($playlistId, $this->connection));
		}
		
		return $playlistArray;
	}
	
	
	/**
	* Instantiates and returns an Artist object
	* @param artistId The Despotify id of the artist
	* @return Artist object on success. Returns boolean false if not connected to the gateway.
	*/
	public function getArtist($artistId)
	{
		$artistId = trim($artistId);
		
		if(empty($artistId))
		{
			return false;
		}
		
		return new Artist($artistId, $this->connection);
	}
	
	
	/**
	* Get the Connection object used by this instance. Useful if you want to create Track, Playlist, etc objects on your own without using the convenience functions in the Despotify class.
	* @return Connection object used by this Despotify instance
	*/
	public function getConnection()
	{
		return $this->connection;
	}
	
	
	
	/**
	* Instantiates and returns a track object
	* @param trackId The Despotify id of the track
	* @return Track object on success. Returns boolean false in case of failure
	*/
	public function getTrack($trackId)
	{
		$trackId = trim($trackId);
		
		if(empty($trackId))
		{
			return false;
		}
		
		return new Track($trackId, $this->connection);
	}
}


?>