package se.despotify.exceptions;

@SuppressWarnings("serial")
public class ProtocolException extends Exception {
  public ProtocolException() {
  }

  public ProtocolException(String message){
		super(message);
	}
}
