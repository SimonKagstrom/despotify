package se.despotify.exceptions;

@SuppressWarnings("serial")
public class ConnectionException extends Exception {
	public ConnectionException(String message){
		super(message);
	}
}
