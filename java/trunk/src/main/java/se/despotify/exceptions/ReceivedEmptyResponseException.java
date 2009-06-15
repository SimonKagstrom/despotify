package se.despotify.exceptions;

/**
 * @author kalle
 * @since 2009-jun-14 21:38:28
 */
public class ReceivedEmptyResponseException extends DespotifyException{
  public ReceivedEmptyResponseException() {
  }

  public ReceivedEmptyResponseException(String message) {
    super(message);
  }
}
