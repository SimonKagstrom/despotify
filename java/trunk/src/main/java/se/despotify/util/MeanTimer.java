package se.despotify.util;

import java.io.Serializable;

/**
 * @author kalle
 * @since 2009-jun-25 23:59:03
 */
public class MeanTimer<T> implements Serializable {

  private static final long serialVersionUID = 1l;

  private T metadata;

  private String name;

  private long started;
  private Long stopped;

  private int runs;
  private long totalMillisecondsSpent = 0;

  public void resetHistory() {
    runs = 0;
    totalMillisecondsSpent = 0;
  }

  public MeanTimer(String name) {
    this.name = name;
    start();
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
    runs++;
    totalMillisecondsSpent += stopped - started;
  }

  public void start() {
    started = System.currentTimeMillis();
  }

  public int getRuns() {
    return runs;
  }

  public long getTotalMillisecondsSpent() {
    return totalMillisecondsSpent;
  }

  @Override
  public String toString() {
    double meantime = (double)totalMillisecondsSpent/(double)runs;
    StringBuilder sb = new StringBuilder();
    sb.append("Timer '").append(name).append("' took ");
    appendHumanReadableTime(sb, getMillisecondsSpent());
    sb.append(" last run. Mean time of ").append(runs).append(" runs is ");
    appendHumanReadableTime(sb, meantime);
    return sb.toString();

  }

  public T getMetadata() {
    return metadata;
  }

  public void setMetadata(T metadata) {
    this.metadata = metadata;
  }


  public static void appendHumanReadableTime(StringBuilder sb, double milliseconds) {
    if (milliseconds < 2) {
      sb.append(String.valueOf(milliseconds * 1000));
      sb.append(" nanoseconds");
    } else if (milliseconds < 10000) {
      sb.append(String.valueOf((int) (milliseconds)));
      sb.append(" millseconds");
    } else if (milliseconds < 120000) {
      sb.append(String.valueOf((int) (milliseconds / 1000)));
      sb.append(" seconds");
    } else if (milliseconds < 1000 * 60 * 60 * 2) {
      sb.append(String.valueOf((int) (milliseconds / 60000)));
      sb.append(" minutes");
    } else if (milliseconds < 1000 * 60 * 60 * 49) {
      sb.append(String.valueOf((int) (milliseconds / (1000 * 60 * 60))));
      sb.append(" hours");
    } else {
      sb.append(String.valueOf((int) (milliseconds / (1000 * 60 * 60 * 24))));
      sb.append(" days");
    }

  }


}