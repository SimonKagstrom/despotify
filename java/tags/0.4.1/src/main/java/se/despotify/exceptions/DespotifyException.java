package se.despotify.exceptions;

@SuppressWarnings("serial")
public class DespotifyException extends RuntimeException {
  public DespotifyException() {
  }

  public DespotifyException(String message){
		super(message);
	}
}
