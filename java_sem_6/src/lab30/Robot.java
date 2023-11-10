package lab30;


public class Robot extends Thread {
    public static void stepLeft() {
        System.out.println("Step left");
    }

    public static void stepRight() {
        System.out.println("Step right");
    }

    @Override
    public void run() {
        System.out.println("Thread test " + getName());
    }
}
