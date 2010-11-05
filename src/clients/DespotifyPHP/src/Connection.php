<?php

/**
* Represents a connection to the Despotify gateway
*/
class Connection
{
	private $gateway;
	private $port;
	private $socket;
	
	
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
		$this->socket = socket_create(AF_INET, SOCK_STREAM, SOL_TCP);
		
		if(@socket_connect($this->socket, $this->gateway, $this->port) == false) // cannot connect
		{
			unset($this->socket);
			
			return false;
		}
		else
		{
			return true;
		}
	}
	
	
	/**
	* Disconnect from the Despotify gateway
	*/
	function disconnect()
	{
		socket_write($this->socket, 'quit\n');
		socket_close($this->socket);
		unset($this->socket);
	}
	
	
	/**
	* Check whether there is a connection to the gateway
	*/
	public function isConnected() {
		if(!$this->socket || empty($this->socket))
		{
			return false;
		}
		else
		{
			return true;
		}
	}
	
	
	/**
	* Read $bytes bytes from the socket 
	*/
	public function read($bytes)
	{
		if(DEBUG_MODE)
		{
			debug_print_backtrace();
		}
		
		
		$data = '';
	
		while($bytes > 0)
		{
			$ret = socket_recv($this->socket, $buffer, $bytes, 0);
			if($ret <= 0)
			{
				break;
			}
	
			$bytes -= $ret;
			$data .= $buffer;
		}
	
		return $data;
	}
	
	
	/**
	* Read the header of the data returned by the gateway
	* Headers are formatted as <HTTP-style error code> <payload length> <OK/WARN/ERROR>, so an example would be:
	* 200 2 OK Assigned country below
	* SE
	*
	* @return Payload(length of resulting data) on success, boolean false if WARN or ERROR is returned
	*/
	public function readHeader() {
		$header = socket_read($this->socket, 512, PHP_NORMAL_READ);
		sscanf($header, "%d %d %s %s", $errorCode, $length, $code, $errorString);
		
		if($errorCode != 200) // 200 is the (error) code for OK
		{
			return false;
		}
	
		return $length;
	}
	
	
	/**
	* Write $data to the socket
	* @param data The data to be written to the socket
	* @return The number of bytes written on success, or boolean false on error
	*/
	public function write($data)
	{
		if(DEBUG_MODE)
		{
			debug_print_backtrace();
		}
		
		return socket_write($this->socket, $data);
	}
}


?>