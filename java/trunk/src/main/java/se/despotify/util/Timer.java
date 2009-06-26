package se.despotify.util;

/**
 * @author kalle
 * @since 2009-jun-25 23:59:03
 */
public class Timer<T> {

  private T metadata;

  private String name;

  private long started;
  private Long stopped;

  public Timer(String name) {
    this.name = name;
    started = System.currentTimeMillis();
  }

  public long getMillisecondsSpent() {
    if (stopped == null) {
      System.out.println("Warning! Timer " + name + " was not stopped!");
      return System.currentTimeMillis() - started;
    } else {
      return stopped - started;
    }
  }

  public void stop() {
    stopped = System.currentTimeMillis();
  }

  public void start() {
    started = System.currentTimeMillis();
  }

  public long getStopped() {
    return stopped;
  }

  public void setStopped(long stopped) {
    this.stopped = stopped;
  }

  public void setStarted(long started) {
    this.started = started;
  }

  public long getStarted() {
    return started;
  }

  @Override
  public String toString() {
    return "Timer " + name + " took " + getMillisecondsSpent() + " milliseconds.";
  }

  public T getMetadata() {
    return metadata;
  }

  public void setMetadata(T metadata) {
    this.metadata = metadata;
  }
}
